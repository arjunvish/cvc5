/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2023 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Rewrite database proof reconstructor
 */

#include "rewriter/rewrite_db_proof_cons.h"

#include "expr/node_algorithm.h"
#include "options/proof_options.h"
#include "rewriter/rewrite_db_term_process.h"
#include "smt/env.h"
#include "theory/arith/arith_poly_norm.h"
#include "theory/builtin/proof_checker.h"
#include "theory/rewriter.h"

using namespace cvc5::internal::kind;

namespace cvc5::internal {
namespace rewriter {

RewriteDbProofCons::RewriteDbProofCons(Env& env, RewriteDb* db)
    : EnvObj(env),
      d_notify(*this),
      d_trrc(env),
      d_db(db),
      d_eval(nullptr),
      d_currRecLimit(0),
      d_currStepLimit(0),
      d_currFixedPointId(DslPfRule::FAIL),
      d_statTotalInputs(
          statisticsRegistry().registerInt("RewriteDbProofCons::totalInputs")),
      d_statTotalAttempts(statisticsRegistry().registerInt(
          "RewriteDbProofCons::totalAttempts")),
      d_statTotalInputSuccess(statisticsRegistry().registerInt(
          "RewriteDbProofCons::totalInputSuccess"))
{
  NodeManager* nm = NodeManager::currentNM();
  d_true = nm->mkConst(true);
  d_false = nm->mkConst(false);
  /*
  // initialize the query cache
  const std::unordered_set<Node>& fvs = db->getAllFreeVariables();
  std::vector<Node> qcvars(fvs.begin(), fvs.end());
  d_qcache.initialize(qcvars);
  */
}

bool RewriteDbProofCons::prove(CDProof* cdp,
                               Node a,
                               Node b,
                               theory::TheoryId tid,
                               MethodId mid,
                               int64_t recLimit,
                               int64_t stepLimit)
{
  // clear the proof caches? use attributes instead?
  d_pcache.clear();
  // clear the evaluate cache?
  d_evalCache.clear();
  Trace("rpc") << "RewriteDbProofCons::prove: " << a << " == " << b
               << std::endl;
  Trace("rpc-debug") << "- prove basic" << std::endl;
  // first, try with the basic utility
  if (d_trrc.prove(cdp, a, b, tid, mid))
  {
    Trace("rpc") << "...success (basic)" << std::endl;
    return true;
  }
  ++d_statTotalInputs;
  Trace("rpc-debug") << "- convert to internal" << std::endl;
  DslPfRule id;
  Node eq = a.eqNode(b);
  Node eqi = d_rdnc.convert(eq);
  if (!proveInternalBase(eqi, id))
  {
    Trace("rpc-debug") << "- prove internal" << std::endl;
    // add one to recursion limit, since it is decremented whenever we initiate
    // the getMatches routine.
    d_currRecLimit = recLimit + 1;
    d_currStepLimit = stepLimit;
    // Otherwise, we call the main prove internal method, which recurisvely
    // tries to find a matched conclusion whose conditions can be proven
    id = proveInternal(eqi);
    Trace("rpc-debug") << "- finished prove internal" << std::endl;
  }
  bool success = (id != DslPfRule::FAIL);
  // if a proof was provided, fill it in
  if (success && cdp != nullptr)
  {
    ++d_statTotalInputSuccess;
    Trace("rpc-debug") << "- ensure proof" << std::endl;
    // if it changed encoding, account for this
    if (eq != eqi)
    {
      cdp->addStep(eq, PfRule::ENCODE_PRED_TRANSFORM, {eqi}, {eq});
    }
    ensureProofInternal(cdp, eqi);
    AlwaysAssert(cdp->hasStep(eqi)) << eqi;
    Trace("rpc-debug") << "- finish ensure proof" << std::endl;
  }
  Trace("rpc") << "..." << (success ? "success" : "fail") << std::endl;
  return success;
}

DslPfRule RewriteDbProofCons::proveInternal(Node eqi)
{
  d_currProving.insert(eqi);
  ++d_statTotalAttempts;
  // eqi should not hold trivially and should not be cached
  Assert(d_currRecLimit > 0);
  Trace("rpc-debug2") << "proveInternal " << eqi << std::endl;
  // Otherwise, call the get matches routine. This will call notifyMatch below
  // for each matching rewrite rule conclusion in the database
  // decrease the recursion depth
  Assert(eqi.getKind() == EQUAL);
  // first, try congruence if possible, which does not count towards recursion
  // limit.
  DslPfRule retId = proveInternalViaStrategy(eqi);
  d_currProving.erase(eqi);
  return retId;
}

DslPfRule RewriteDbProofCons::proveInternalViaStrategy(Node eqi)
{
  Assert(eqi.getKind() == EQUAL);
  if (proveWithRule(DslPfRule::CONG, eqi, {}, {}, false, false, true))
  {
    Trace("rpc-debug2") << "...proved via congruence" << std::endl;
    return DslPfRule::CONG;
  }
  if (proveWithRule(DslPfRule::CONG_EVAL, eqi, {}, {}, false, false, true))
  {
    Trace("rpc-debug2") << "...proved via congruence + evaluation" << std::endl;
    return DslPfRule::CONG_EVAL;
  }
  // if arithmetic, maybe holds by arithmetic normalization?
  if (proveWithRule(
          DslPfRule::ARITH_POLY_NORM, eqi, {}, {}, false, false, true))
  {
    return DslPfRule::ARITH_POLY_NORM;
  }
  Trace("rpc-debug2") << "...not proved via builtin tactic" << std::endl;
  d_currRecLimit--;
  Node prevTarget = d_target;
  d_target = eqi;
  d_db->getMatches(eqi[0], &d_notify);
  d_target = prevTarget;
  d_currRecLimit++;
  // if we cached it during the above call, we succeeded
  std::unordered_map<Node, ProvenInfo>::iterator it = d_pcache.find(eqi);
  if (it != d_pcache.end())
  {
    // Assert(it->second.d_id != DslPfRule::FAIL)
    //    << "unexpected failure for " << eqi;
    return it->second.d_id;
  }
  // if target is (= (= t1 t2) true), maybe try showing (= t1 t2); otherwise
  // try showing (= target true)
  DslPfRule eqTrueId =
      eqi[1] == d_true ? DslPfRule::TRUE_INTRO : DslPfRule::TRUE_ELIM;
  if (proveWithRule(eqTrueId, eqi, {}, {}, false, false, true))
  {
    Trace("rpc-debug2") << "...proved via " << eqTrueId << std::endl;
    return eqTrueId;
  }
  Trace("rpc-fail") << "FAIL: cannot prove " << eqi[0] << " == " << eqi[1]
                    << std::endl;
  // store failure, and its maximum depth
  ProvenInfo& pi = d_pcache[eqi];
  pi.d_id = DslPfRule::FAIL;
  pi.d_failMaxDepth = d_currRecLimit;
  return DslPfRule::FAIL;
}

bool RewriteDbProofCons::notifyMatch(Node s,
                                     Node n,
                                     std::vector<Node>& vars,
                                     std::vector<Node>& subs)
{
  // if we reach our step limit, do not continue trying
  if (d_currStepLimit == 0)
  {
    return false;
  }
  d_currStepLimit--;
  Trace("rpc-debug2") << "[steps remaining: " << d_currStepLimit << "]"
                      << std::endl;
  Trace("rpc-debug2") << "notifyMatch: " << s << " from " << n << " via "
                      << vars << " -> " << subs << std::endl;
  Assert(d_target.getKind() == EQUAL);
  Assert(s.getType().isComparableTo(n.getType()));
  Assert(vars.size() == subs.size());
  if (d_currFixedPointId != DslPfRule::FAIL)
  {
    Trace("rpc-debug2") << "Part of fixed point for rule " << d_currFixedPointId
                        << std::endl;
    const RewriteProofRule& rpr = d_db->getRule(d_currFixedPointId);
    // get the conclusion
    Node target = rpr.getConclusion();
    // apply substitution, which may notice vars may be out of order wrt rule
    // var list
    target = expr::narySubstitute(target, vars, subs);
    // We now prove with the given rule. this should only fail if there are
    // conditions on the rule which fail. Notice we never allow recursion here.
    // We also don't permit inflection matching (which regardless should not
    // apply).
    if (proveWithRule(
            d_currFixedPointId, target, vars, subs, false, false, false))
    {
      // successfully proved, store in temporary variable
      d_currFixedPointConc = target;
      d_currFixedPointSubs = subs;
    }
    // regardless, no further matches, due to semantics of fixed point which
    // limits to first match
    return false;
  }
  Assert(d_target[0] == s);
  bool recurse = d_currRecLimit > 0;
  // get the rule identifiers for the conclusion
  const std::vector<DslPfRule>& ids = d_db->getRuleIdsForHead(n);
  Assert(!ids.empty());
  // check each rule instance, succeed if one proves
  for (DslPfRule id : ids)
  {
    // try to prove target with the current rule, using inflection matching
    // and fixed point semantics
    if (proveWithRule(id, d_target, vars, subs, true, true, recurse))
    {
      // if successful, we do not want to be notified of further matches
      // and return false here.
      return false;
    }
    // notice that we do not cache a failure here since we only know that the
    // current rule was not able to prove the current target
  }
  // want to keep getting notify calls
  return true;
}

bool RewriteDbProofCons::proveWithRule(DslPfRule id,
                                       Node target,
                                       const std::vector<Node>& vars,
                                       const std::vector<Node>& subs,
                                       bool doInflectMatch,
                                       bool doFixedPoint,
                                       bool doRecurse)
{
  Assert(!target.isNull() && target.getKind() == EQUAL);
  Trace("rpc-debug2") << "Check rule " << id << std::endl;
  std::vector<Node> vcs;
  Node transEq;
  ProvenInfo pic;
  if (id == DslPfRule::CONG)
  {
    size_t nchild = target[0].getNumChildren();
    if (nchild == 0 || nchild != target[1].getNumChildren()
        || target[0].getOperator() != target[1].getOperator())
    {
      // cannot show congruence between different operators
      return false;
    }
    pic.d_id = id;
    for (size_t i = 0; i < nchild; i++)
    {
      if (!target[0][i].getType().isComparableTo(target[1][i].getType()))
      {
        // type error on children (required for certain polymorphic operators)
        return false;
      }
      Node eq = target[0][i].eqNode(target[1][i]);
      vcs.push_back(eq);
      pic.d_vars.push_back(eq);
    }
  }
  else if (id == DslPfRule::CONG_EVAL)
  {
    size_t nchild = target[0].getNumChildren();
    // evaluate the right hand side
    Node r2 = doEvaluate(target[1]);
    if (nchild == 0 || r2.isNull())
    {
      return false;
    }
    Node r = rewrite(target[0]);
    if (r != r2)
    {
      return false;
    }
    pic.d_id = id;
    // if all children rewrite to a constant, try proving equalities
    // on those children
    std::vector<Node> rchildren;
    for (size_t i = 0; i < nchild; i++)
    {
      Node rr = rewrite(target[0][i]);
      if (!rr.isConst())
      {
        return false;
      }
      Node eq = target[0][i].eqNode(rr);
      vcs.push_back(eq);
      pic.d_vars.push_back(eq);
      rchildren.push_back(rr);
    }
    // must check if it truly evaluates. This can fail if the evaluator does
    // not support constant folding for the operator in question, which is the
    // case e.g. for operators that return regular expressions, datatypes,
    // sequences, sets.
    if (target[0].getMetaKind() == metakind::PARAMETERIZED)
    {
      rchildren.insert(rchildren.begin(), target[0].getOperator());
    }
    NodeManager* nm = NodeManager::currentNM();
    Node tappc = nm->mkNode(target[0].getKind(), rchildren);
    if (doEvaluate(tappc) != r2)
    {
      return false;
    }
  }
  else if (id == DslPfRule::TRUE_ELIM)
  {
    if (target[1] == d_true)
    {
      // don't do for equals true, avoids unbounded recursion
      return false;
    }
    pic.d_id = id;
    Node eq = target.eqNode(d_true);
    vcs.push_back(eq);
    pic.d_vars.push_back(eq);
  }
  else if (id == DslPfRule::TRUE_INTRO)
  {
    if (target[1] != d_true || target[0].getKind() != EQUAL)
    {
      // only works for (= (= t1 t2) true)
      return false;
    }
    pic.d_id = id;
    Node eq = target[0];
    vcs.push_back(eq);
    pic.d_vars.push_back(eq);
  }
  else if (id == DslPfRule::ARITH_POLY_NORM)
  {
    if (!theory::arith::PolyNorm::isArithPolyNorm(target[0], target[1]))
    {
      return false;
    }
    pic.d_id = id;
  }
  else
  {
    const RewriteProofRule& rpr = d_db->getRule(id);
    // does it conclusion match what we are trying to show?
    Node conc = rpr.getConclusion();
    Assert(conc.getKind() == EQUAL && target.getKind() == EQUAL);
    // get rule conclusion, which may incorporate fixed point semantics when
    // doFixedPoint is true. This stores the rule for the conclusion in pic,
    // which is either id or DslPfRule::TRANS.
    Node stgt = getRuleConclusion(rpr, vars, subs, pic, doFixedPoint);
    Trace("rpc-debug2") << "            RHS: " << conc[1] << std::endl;
    Trace("rpc-debug2") << "Substituted RHS: " << stgt << std::endl;
    Trace("rpc-debug2") << "     Target RHS: " << target[1] << std::endl;
    // check if conclusion is null
    if (stgt.isNull())
    {
      // this is likely due to not finding a null terminator for a gradual
      // type term
      Trace("rpc-debug2") << "...fail (no construct conclusion)" << std::endl;
      return false;
    }
    // inflection substitution, used if conclusion does not exactly match
    std::unordered_map<Node, std::pair<Node, Node>> isubs;
    if (stgt != target[1])
    {
      if (!doInflectMatch)
      {
        Trace("rpc-debug2") << "...fail (no inflection)" << std::endl;
        return false;
      }
      // The conclusion term may actually change type. Note that we must rewrite
      // the terms, since they may involve operators with abstract type that
      // evaluate to terms with concrete types.
      if (!rewrite(stgt).getType().isComparableTo(rewrite(target[1]).getType()))
      {
        Trace("rpc-debug2") << "...fail (types)" << std::endl;
        return false;
      }
      // the missing transitivity link is a subgoal to prove
      transEq = stgt.eqNode(target[1]);
      vcs.push_back(transEq);
      Trace("rpc-debug2") << "  Try transitive with " << transEq << std::endl;
    }
    // do its conditions hold?
    // Get the conditions, substituted { vars -> subs } and with side conditions
    // evaluated.
    if (!rpr.getObligations(vars, subs, vcs))
    {
      // cannot get conditions, likely due to failed side condition
      Trace("rpc-debug2") << "...fail (obligations)" << std::endl;
      return false;
    }
  }
  // First, check which premises are non-trivial, and if there is a trivial
  // failure. Those that are non-trivial are added to condToProve.
  std::vector<Node> condToProve;
  for (const Node& cond : vcs)
  {
    Assert(cond.getKind() == kind::EQUAL);
    // substitute to get the condition-to-prove
    DslPfRule cid;
    // check whether condition is already known to hold or not hold
    if (proveInternalBase(cond, cid))
    {
      if (cid == DslPfRule::FAIL)
      {
        // does not hold, we fail
        Trace("rpc-debug2") << "...fail (simple condition failure for " << cond
                            << ")" << std::endl;
        return false;
      }
      // already holds, continue
      continue;
    }
    if (!doRecurse)
    {
      // we can't apply recursion, return false
      Trace("rpc-debug2") << "...fail (recursion limit)" << std::endl;
      return false;
    }
    // save, to check below
    condToProve.push_back(cond);
  }
  // if no trivial failures, go back and try to recursively prove
  for (const Node& cond : condToProve)
  {
    Trace("rpc-infer-sc") << "Check condition: " << cond << std::endl;
    // recursively check if the condition holds
    DslPfRule cid = proveInternal(cond);
    if (cid == DslPfRule::FAIL)
    {
      // print reason for failure
      Trace("rpc-infer-debug")
          << "required: " << cond << " for " << id << std::endl;
      return false;
    }
  }
  // successfully found instance of rule
  if (TraceIsOn("rpc-infer"))
  {
    Trace("rpc-infer") << "INFER " << target << " by " << id
                       << (transEq.isNull() ? "" : " + TRANS") << std::endl;
  }
  // cache the success
  ProvenInfo* pi = &d_pcache[target];
  if (!transEq.isNull())
  {
    Trace("rpc-debug2") << "..." << target << " proved by TRANS" << std::endl;
    Node transEqStart = target[0].eqNode(transEq[0]);
    // proves both
    pi->d_id = DslPfRule::TRANS;
    pi->d_vars.push_back(transEqStart);
    pi->d_vars.push_back(transEq);
    Trace("rpc-debug2") << "...original equality was " << transEqStart
                        << std::endl;
    // we also prove the original
    pi = &d_pcache[transEqStart];
  }
  pi->d_id = pic.d_id;
  if (pic.isInternalRule())
  {
    pi->d_vars = pic.d_vars;
  }
  else
  {
    pi->d_vars = vars;
    pi->d_subs = subs;
  }
  Trace("rpc-debug2") << "...target proved by " << d_pcache[target].d_id
                      << std::endl;
  // success
  return true;
}

bool RewriteDbProofCons::proveInternalBase(Node eqi, DslPfRule& idb)
{
  Trace("rpc-debug2") << "Prove internal base: " << eqi << std::endl;
  Assert(eqi.getKind() == kind::EQUAL);
  // if we are currently trying to prove this, fail
  if (d_currProving.find(eqi) != d_currProving.end())
  {
    Trace("rpc-debug2") << "...fail (already proving)" << std::endl;
    idb = DslPfRule::FAIL;
    return true;
  }
  // already cached?
  std::unordered_map<Node, ProvenInfo>::iterator it = d_pcache.find(eqi);
  if (it != d_pcache.end())
  {
    if (it->second.d_id != DslPfRule::FAIL)
    {
      // proof exists, return
      idb = it->second.d_id;
      return true;
    }
    if (d_currRecLimit <= it->second.d_failMaxDepth)
    {
      idb = it->second.d_id;
      return true;
    }
    Trace("rpc-debug2") << "...fail (already fail)" << std::endl;
    // Will not succeed below, since we know we've already tried. Hence, we
    // are in a situation where we have yet to succeed to prove eqi for some
    // depth, but we are currently trying at a higher maximum depth.
    return false;
  }
  // symmetry or reflexivity, applied potentially to non-Booleans?
  // if (CDProof::isSame(eqi[0], eqi[1]))
  if (eqi[0] == eqi[1])
  {
    ProvenInfo& pi = d_pcache[eqi];
    idb = DslPfRule::REFL;
    pi.d_id = idb;
    return true;
  }
  // non-well-typed equalities cannot be proven
  // also, variables cannot be rewritten
  if (eqi.getTypeOrNull().isNull() || eqi[0].isVar())
  {
    Trace("rpc-debug2") << "...fail ("
                        << (eqi[0].isVar() ? "variable" : "ill-typed") << ")"
                        << std::endl;
    ProvenInfo& pi = d_pcache[eqi];
    idb = DslPfRule::FAIL;
    pi.d_failMaxDepth = 0;
    pi.d_id = idb;
    return true;
  }
  // evaluate the two sides of the equality, without help of the rewriter
  Node ev[2];
  bool evalSuccess = true;
  for (size_t i = 0; i < 2; i++)
  {
    ev[i] = doEvaluate(eqi[i]);
    if (ev[i].isNull())
    {
      // does not evaluate
      // If it rewrites to false, then it is obviously infeasible. Notice that
      // rewriting is more expensive than evaluation, so we do it as a second
      // resort.
      Node lhs = i == 1 ? ev[0] : eqi[0];
      Node eq = lhs.eqNode(eqi[1]);
      if (eq.getTypeOrNull().isNull())
      {
        ProvenInfo& pi = d_pcache[eqi];
        idb = DslPfRule::FAIL;
        pi.d_failMaxDepth = 0;
        pi.d_id = idb;
        return true;
      }
      Node eqr = rewrite(eq);
      if (eqr.isConst())
      {
        // definitely not true
        if (!eqr.getConst<bool>())
        {
          ProvenInfo& pi = d_pcache[eqi];
          Trace("rpc-debug2") << "Infeasible due to rewriting: " << eqi[0]
                              << " == " << eqi[1] << std::endl;
          idb = DslPfRule::FAIL;
          pi.d_failMaxDepth = 0;
          pi.d_id = idb;
          return true;
        }
        // NOTE: if does not rewrite to true, it still could be true, hence we
        // fail
      }
      evalSuccess = false;
      break;
    }
  }
  if (evalSuccess)
  {
    ProvenInfo& pi = d_pcache[eqi];
    // we can evaluate both sides, check to see if the values are the same
    if (ev[0] == ev[1])
    {
      idb = DslPfRule::EVAL;
    }
    else
    {
      idb = DslPfRule::FAIL;
      // failure relies on nothing, depth is 0
      pi.d_failMaxDepth = 0;
    }
    // cache it
    pi.d_id = idb;
    return true;
  }
  if (eqi[0].isConst())
  {
    Trace("rpc-debug2") << "...fail (constant head)" << std::endl;
    ProvenInfo& pi = d_pcache[eqi];
    idb = DslPfRule::FAIL;
    pi.d_failMaxDepth = 0;
    pi.d_id = idb;
    return true;
  }
  /*
  // see if a != b is satisfiable
  Node query = eqi.notNode();
  if (d_qcache.addTerm(query))
  {
    Trace("rpc-debug2") << "Infeasible due to query cache: " << eqi[0]
                        << " == " << eqi[1] << std::endl;
    ProvenInfo& pi = d_pcache[eqi];
    idb = DslPfRule::FAIL;
    pi.d_failMaxDepth = 0;
    pi.d_id = idb;
    return true;
  }
  */
  // otherwise, we fail to either prove or disprove the equality
  return false;
}

bool RewriteDbProofCons::ensureProofInternal(CDProof* cdp, Node eqi)
{
  // TODO: use single internal cdp to improve subproof sharing?
  NodeManager* nm = NodeManager::currentNM();
  std::unordered_map<TNode, bool> visited;
  std::unordered_map<TNode, std::vector<Node>> premises;
  std::unordered_map<TNode, std::vector<Node>> pfArgs;
  std::unordered_map<TNode, bool>::iterator it;
  std::unordered_map<Node, ProvenInfo>::iterator itd;
  std::vector<Node>::iterator itv;
  std::vector<TNode> visit;
  TNode cur;
  visit.push_back(eqi);
  do
  {
    cur = visit.back();
    visit.pop_back();
    it = visited.find(cur);
    itd = d_pcache.find(cur);
    Assert(cur.getKind() == kind::EQUAL);
    if (it == visited.end())
    {
      Trace("rpc-debug") << "Ensure proof for " << cur << std::endl;
      visit.push_back(cur);
      // may already have a proof rule from a previous call
      if (cdp->hasStep(cur))
      {
        visited[cur] = true;
        Trace("rpc-debug") << "...already proven" << std::endl;
      }
      else
      {
        Assert(itd != d_pcache.end());
        Assert(itd->second.d_id != DslPfRule::FAIL);
        Trace("rpc-debug") << "...proved via " << itd->second.d_id << std::endl;
        if (itd->second.d_id == DslPfRule::REFL)
        {
          // trivial proof
          Assert(cur[0] == cur[1]);
          cdp->addStep(cur, PfRule::REFL, {}, {cur[0]});
        }
        else if (itd->second.d_id == DslPfRule::EVAL)
        {
          visited[cur] = true;
          // NOTE: this could just evaluate the equality itself
          Assert(cur.getKind() == kind::EQUAL);
          std::vector<Node> transc;
          for (unsigned i = 0; i < 2; i++)
          {
            Node curv = doEvaluate(cur[i]);
            if (curv == cur[i])
            {
              continue;
            }
            Node eq = cur[i].eqNode(curv);
            // flip orientation for second child
            transc.push_back(i == 1 ? curv.eqNode(cur[i]) : eq);
            // trivial evaluation, add evaluation method id
            cdp->addStep(eq, PfRule::EVALUATE, {}, {cur[i]});
          }
          if (transc.size() == 2)
          {
            // do transitivity if both sides evaluate
            cdp->addStep(cur, PfRule::TRANS, transc, {});
          }
        }
        else
        {
          visited[cur] = false;
          std::vector<Node>& ps = premises[cur];
          if (isInternalDslPfRule(itd->second.d_id))
          {
            // premises are the steps, stored in d_vars
            ps.insert(premises[cur].end(),
                      itd->second.d_vars.begin(),
                      itd->second.d_vars.end());
            if (itd->second.d_id == DslPfRule::CONG
                || itd->second.d_id == DslPfRule::CONG_EVAL)
            {
              pfArgs[cur].push_back(
                  ProofRuleChecker::mkKindNode(cur[0].getKind()));
              if (cur[0].getMetaKind() == kind::metakind::PARAMETERIZED)
              {
                pfArgs[cur].push_back(cur[0].getOperator());
              }
            }
          }
          else
          {
            const RewriteProofRule& rpr = d_db->getRule(itd->second.d_id);
            // add the DSL proof rule we used
            pfArgs[cur].push_back(nm->mkConstInt(
                Rational(static_cast<uint32_t>(itd->second.d_id))));
            // compute premises based on the used substitution
            // build the substitution context
            const std::vector<Node>& vs = rpr.getVarList();
            Assert(itd->second.d_vars.size() == vs.size());
            std::vector<Node> rsubs;
            // must order the variables to match order of rewrite rule
            for (const Node& v : vs)
            {
              itv = std::find(
                  itd->second.d_vars.begin(), itd->second.d_vars.end(), v);
              size_t d = std::distance(itd->second.d_vars.begin(), itv);
              Assert(d < itd->second.d_subs.size());
              rsubs.push_back(itd->second.d_subs[d]);
            }
            // get the conditions, store into premises of cur.
            if (!rpr.getObligations(vs, rsubs, ps))
            {
              Assert(false);
              // failed a side condition?
              return false;
            }
            pfArgs[cur].insert(pfArgs[cur].end(), rsubs.begin(), rsubs.end());
          }
          // recurse on premises
          visit.insert(visit.end(), ps.begin(), ps.end());
        }
      }
    }
    else if (!it->second)
    {
      // Now, add the proof rule. We do this after its children proofs already
      // exist.
      visited[cur] = true;
      Assert(premises.find(cur) != premises.end());
      std::vector<Node>& ps = premises[cur];
      // get the conclusion
      Node conc;
      if (itd->second.d_id == DslPfRule::TRANS)
      {
        conc = ps[0][0].eqNode(ps.back()[1]);
        cdp->addStep(conc, PfRule::TRANS, ps, {});
      }
      else if (itd->second.d_id == DslPfRule::CONG)
      {
        cdp->addStep(cur, PfRule::CONG, ps, pfArgs[cur]);
      }
      else if (itd->second.d_id == DslPfRule::CONG_EVAL)
      {
        // congruence + evaluation, given we are trying to prove
        //   (f t1 ... tn) == c
        // This tactic checks if t1 ... tn rewrite to constants c1 ... cn.
        // If so, we try to show subgoals
        //   t1 == c1 ... tn == cn
        // The final proof is a congruence step + evaluation:
        //   (f t1 ... tn) == (f c1 ... cn) == c.
        Node lhs = cur[0];
        std::vector<Node> lhsTgtc;
        if (cur[0].getMetaKind() == metakind::PARAMETERIZED)
        {
          lhsTgtc.push_back(cur[0].getOperator());
        }
        for (const Node& eq : itd->second.d_vars)
        {
          Assert(eq.getKind() == EQUAL);
          lhsTgtc.push_back(eq[1]);
        }
        Node lhsTgt = nm->mkNode(cur[0].getKind(), lhsTgtc);
        Node rhs = doEvaluate(cur[1]);
        Assert(!rhs.isNull());
        Node eq1 = lhs.eqNode(lhsTgt);
        Node eq2 = lhsTgt.eqNode(rhs);
        std::vector<Node> transChildren = {eq1, eq2};
        cdp->addStep(eq1, PfRule::CONG, ps, pfArgs[cur]);
        cdp->addStep(eq2, PfRule::EVALUATE, {}, {lhsTgt});
        if (rhs != cur[1])
        {
          cdp->addStep(cur[1].eqNode(rhs), PfRule::EVALUATE, {}, {cur[1]});
          transChildren.push_back(rhs.eqNode(cur[1]));
        }
        cdp->addStep(cur, PfRule::TRANS, transChildren, {});
      }
      else if (itd->second.d_id == DslPfRule::TRUE_ELIM)
      {
        conc = ps[0][0];
        cdp->addStep(conc, PfRule::TRUE_ELIM, ps, {});
      }
      else if (itd->second.d_id == DslPfRule::TRUE_INTRO)
      {
        conc = ps[0].eqNode(d_true);
        cdp->addStep(conc, PfRule::TRUE_INTRO, ps, {});
      }
      else if (itd->second.d_id == DslPfRule::ARITH_POLY_NORM)
      {
        cdp->addStep(cur, PfRule::ARITH_POLY_NORM, {}, {cur});
      }
      else
      {
        Assert(pfArgs.find(cur) != pfArgs.end());
        const RewriteProofRule& rpr = d_db->getRule(itd->second.d_id);
        const std::vector<Node>& args = pfArgs[cur];
        std::vector<Node> subs(args.begin() + 1, args.end());
        conc = rpr.getConclusionFor(subs);
        Trace("rpc-debug") << "Finalize proof for " << cur << std::endl;
        Trace("rpc-debug") << "Proved: " << cur << std::endl;
        Trace("rpc-debug") << "From: " << conc << std::endl;
        cdp->addStep(conc, PfRule::DSL_REWRITE, ps, args);
      }
    }
  } while (!visit.empty());
  return true;
}

Node RewriteDbProofCons::doEvaluate(Node n)
{
  std::unordered_map<Node, Node>::iterator itv = d_evalCache.find(n);
  if (itv != d_evalCache.end())
  {
    return itv->second;
  }
  Node nev = d_eval.eval(n, {}, {});
  d_evalCache[n] = nev;
  return nev;
}

Node RewriteDbProofCons::getRuleConclusion(const RewriteProofRule& rpr,
                                           const std::vector<Node>& vars,
                                           const std::vector<Node>& subs,
                                           ProvenInfo& pi,
                                           bool doFixedPoint)
{
  pi.d_id = rpr.getId();
  Node conc = rpr.getConclusion();
  // if fixed point, we continue applying
  if (doFixedPoint && rpr.isFixedPoint())
  {
    Assert(d_currFixedPointId == DslPfRule::FAIL);
    Assert(d_currFixedPointConc.isNull());
    d_currFixedPointId = rpr.getId();
    // check if stgt also rewrites with the same rule?
    bool continueFixedPoint;
    std::vector<Node> steps;
    std::vector<std::vector<Node>> stepsSubs;
    // start from the source, match again to start the chain. Notice this is
    // required for uniformity since we want to successfully cache the first
    // step, independent of the target.
    Node ssrc = expr::narySubstitute(conc[0], vars, subs);
    Node stgt = ssrc;
    do
    {
      continueFixedPoint = false;
      rpr.getMatches(stgt, &d_notify);
      if (!d_currFixedPointConc.isNull())
      {
        // currently avoid accidental loops: arbitrarily bound to 1000
        continueFixedPoint = steps.size() <= 1000;
        Assert(d_currFixedPointConc.getKind() == EQUAL);
        steps.push_back(d_currFixedPointConc[1]);
        stepsSubs.emplace_back(d_currFixedPointSubs.begin(),
                               d_currFixedPointSubs.end());
        stgt = d_currFixedPointConc[1];
      }
      d_currFixedPointConc = Node::null();
    } while (continueFixedPoint);

    std::vector<Node> transEq;
    Node prev = ssrc;
    Node context = rpr.getContext();
    Node placeholder = context[0][0];
    Node body = context[1];
    Node currConc = body;
    Node currContext = placeholder;
    for (size_t i = 0, size = steps.size(); i < size; i++)
    {
      const std::vector<Node>& stepSubs = stepsSubs[i];
      Node step = steps[i];
      Node source = expr::narySubstitute(conc[0], vars, stepSubs);
      Node target = expr::narySubstitute(body, vars, stepSubs);
      target = target.substitute(TNode(placeholder), TNode(step));
      cacheProofSubPlaceholder(currContext, placeholder, source, target);

      ProvenInfo dpi;
      dpi.d_id = pi.d_id;
      dpi.d_vars = vars;
      dpi.d_subs = stepSubs;
      d_pcache[source.eqNode(target)] = dpi;

      currConc = expr::narySubstitute(currConc, vars, stepSubs);
      currContext = currConc;
      Node prevConc = currConc;
      if (i < size - 1)
      {
        currConc = currConc.substitute(TNode(placeholder), TNode(body));
      }
      Node stepConc = prevConc.substitute(TNode(placeholder), TNode(step));
      transEq.push_back(prev.eqNode(stepConc));
      prev = stepConc;
    }

    d_currFixedPointId = DslPfRule::FAIL;
    // add the transistivity rule here if needed
    if (transEq.size() >= 2)
    {
      pi.d_id = DslPfRule::TRANS;
      // store transEq in d_vars
      pi.d_vars = transEq;
      // return the end of the chain, which will be used for constrained
      // matching
      return transEq.back()[1];
    }
  }

  Node res = conc[1];
  if (rpr.isFixedPoint())
  {
    Node context = rpr.getContext();
    res = context[1].substitute(TNode(context[0][0]), TNode(conc[1]));
  }
  return expr::narySubstitute(res, vars, subs);
}

void RewriteDbProofCons::cacheProofSubPlaceholder(TNode context,
                                                  TNode placeholder,
                                                  TNode source,
                                                  TNode target)
{
  std::vector<TNode> toVisit = {context};
  std::unordered_map<TNode, TNode> parent;
  std::vector<Node> congs;
  parent[context] = TNode::null();
  while (!toVisit.empty())
  {
    TNode curr = toVisit.back();
    toVisit.pop_back();

    if (curr == placeholder)
    {
      while (parent[curr] != Node::null())
      {
        Node lhs = parent[curr].substitute(placeholder, source);
        Node rhs = parent[curr].substitute(placeholder, target);
        congs.emplace_back(lhs.eqNode(rhs));
        curr = parent[curr];
      }
      break;
    }

    for (TNode n : curr)
    {
      if (parent.find(n) != parent.end())
      {
        continue;
      }

      toVisit.emplace_back(n);
      parent[n] = curr;
    }
  }

  for (const Node& cong : congs)
  {
    ProvenInfo cpi;
    cpi.d_id = DslPfRule::CONG;
    for (size_t i = 0, size = cong[0].getNumChildren(); i < size; i++)
    {
      TNode lhs = cong[0][i];
      TNode rhs = cong[1][i];
      if (lhs == rhs)
      {
        ProvenInfo pi;
        pi.d_id = DslPfRule::REFL;
        d_pcache[lhs.eqNode(rhs)] = pi;
      }
      cpi.d_vars.emplace_back(lhs.eqNode(rhs));
    }
    d_pcache[cong] = cpi;
  }
}

}  // namespace rewriter
}  // namespace cvc5::internal
