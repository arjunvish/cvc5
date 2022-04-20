/******************************************************************************
 * Top contributors (to current version):
 *   Hanna Lachnitt
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2022 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * The module for printing Alethe proof nodes.
 */

#include "proof/alethe/alethe_printer.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

#include "options/expr_options.h"
#include "proof/alethe/alethe_proof_rule.h"

namespace cvc5::internal {

namespace proof {

AletheLetBinding::AletheLetBinding(uint32_t thresh) : LetBinding(thresh) {}

Node AletheLetBinding::convert(Node n, const std::string& prefix)
{
  if (d_letMap.empty())
  {
    return n;
  }
  NodeManager* nm = NodeManager::currentNM();
  std::unordered_map<TNode, Node> visited;
  std::unordered_map<TNode, Node>::iterator it;
  std::vector<TNode> visit;
  TNode cur;
  visit.push_back(n);
  do
  {
    cur = visit.back();
    visit.pop_back();
    it = visited.find(cur);

    if (it == visited.end())
    {
      uint32_t id = getId(cur);
      // do not letify id 0
      if (id > 0)
      {
        Trace("alethe-printer") << "Node " << cur << " has id " << id << "\n";
        // if cur has already been declared, make the let variable
        if (d_declared.find(cur) != d_declared.end())
        {
          std::stringstream ss;
          ss << prefix << id;
          visited[cur] = nm->mkBoundVar(ss.str(), cur.getType());
          continue;
        }
        // otherwise declare it and continue visiting
        d_declared.insert(cur);
      }
      if (cur.isClosure())
      {
        // we do not convert beneath quantifiers, so we need to finish the
        // traversal here. However if id > 0, then we need to declare cur's
        // variable.
        if (id == 0)
        {
          visited[cur] = cur;
          continue;
        }
        std::stringstream ss;
        ss << "(! ";
        options::ioutils::applyOutputLang(ss, Language::LANG_SMTLIB_V2_6);
        cur.toStream(ss, -1, 0);
        ss << " :named @p_" << id << ")";
        visited[cur] = nm->mkRawSymbol(ss.str(), cur.getType());
        continue;
      }
      visited[cur] = Node::null();
      visit.push_back(cur);
      // we insert in reverse order to guarantee that first (left-to-right)
      // occurrence, if more than one equal children of id > 0, is the one that
      // is declared rather than replaced
      visit.insert(visit.end(), cur.rbegin(), cur.rend());
    }
    else if (it->second.isNull())
    {
      Node ret = cur;
      bool childChanged = false;
      uint32_t id;
      std::vector<Node> children;
      if (cur.getMetaKind() == kind::metakind::PARAMETERIZED)
      {
        children.push_back(cur.getOperator());
      }
      for (const Node& cn : cur)
      {
        it = visited.find(cn);
        Assert(it != visited.end());
        Assert(!it->second.isNull());
        children.push_back(it->second);
        // if cn changed and if it has id > 0 and is *not* a variable, then
        // necessarily it is being declared, and from now on when it occurs it
        // must be mapped to its variable
        if (cn != it->second)
        {
          childChanged = true;
          id = getId(cn);
          if (id > 0)
          {
            std::stringstream ss;
            ss << prefix << id;
            visited[cn] = nm->mkBoundVar(ss.str(), cn.getType());
          }
        }

      }
      if (childChanged)
      {
        ret = nm->mkNode(cur.getKind(), children);
      }
      id = getId(cur);
      // if cur has id bigger than 0, it should be letified. If we are here
      // is because it's being declared and this is the first time it's
      // re-visited. So we turn it into a declaration (! ret :named @p_{id})
      if (id > 0)
      {
        std::stringstream ss;
        ss << "(! ";
        options::ioutils::applyOutputLang(ss, Language::LANG_SMTLIB_V2_6);
        ret.toStream(ss, -1, 0);
        ss << " :named @p_" << id << ")";
        visited[cur] = nm->mkRawSymbol(ss.str(), ret.getType());
        continue;
      }
      visited[cur] = ret;
    }
  } while (!visit.empty());
  Assert(visited.find(n) != visited.end());
  Assert(!visited.find(n)->second.isNull());
  return visited[n];
}

LetUpdaterPfCallback::LetUpdaterPfCallback(AletheLetBinding& lbind)
    : d_lbind(lbind)
{
}

LetUpdaterPfCallback::~LetUpdaterPfCallback() {}

bool LetUpdaterPfCallback::shouldUpdate(std::shared_ptr<ProofNode> pn,
                                        const std::vector<Node>& fa,
                                        bool& continueUpdate)
{
  // We do this here so we do not go into update, as we are never updating the
  // proof node, not even do we pass a node manager to the proof node updater.
  const std::vector<Node>& args = pn->getArguments();
  // Letification done on the converted terms and potentially on arguments
  AlwaysAssert(args.size() > 2)
      << "res: " << pn->getResult() << "\nid: " << getAletheRule(args[0]);
  for (size_t i = 2, size = args.size(); i < size; ++i)
  {
    Trace("alethe-printer") << "Process " << args[i] << "\n";
    d_lbind.process(args[i]);
  }

  return false;
}

bool LetUpdaterPfCallback::update(Node res,
                                  PfRule id,
                                  const std::vector<Node>& children,
                                  const std::vector<Node>& args,
                                  CDProof* cdp,
                                  bool& continueUpdate)
{
  return false;
}

AletheProofPrinter::AletheProofPrinter()
    : d_lbind(options::defaultDagThresh() ? options::defaultDagThresh() + 1
                                          : 0),
      d_cb(new LetUpdaterPfCallback(d_lbind))
{
}

void AletheProofPrinter::printTerm(std::ostream& out, TNode n)
{
  out << d_lbind.convert(n, "@p_");
}

void AletheProofPrinter::print(std::ostream& out,
                               std::shared_ptr<ProofNode> pfn)
{
  Trace("alethe-printer") << "- Print proof in Alethe format. " << std::endl;
  std::shared_ptr<ProofNode> innerPf = pfn->getChildren()[0];
  AlwaysAssert(innerPf);
  // Traverse the proof node to letify the (converted) conclusions of proof
  // steps. TODO This traversal will collect the skolems to de defined.
  ProofNodeUpdater updater(nullptr, *(d_cb.get()), false, false);
  Trace("alethe-printer") << "- letify.\n";
  updater.process(innerPf);

  if (options::defaultDagThresh())
  {
    std::vector<Node> letList;
    d_lbind.letify(letList);
    for (TNode n : letList)
    {
      Trace("alethe-printer")
          << "Term " << n << " has id " << d_lbind.getId(n) << "\n";
    }
  }

  Trace("alethe-printer") << "- Print assumptions.\n";
  std::unordered_map<Node, std::string> assumptions;
  const std::vector<Node>& args = pfn->getArguments();
  // Special handling for the first scope
  // Print assumptions and add them to the list but do not print anchor.
  for (size_t i = 3, size = args.size(); i < size; i++)
  {
    // assumptions are always being declared
    out << "(assume a" << i - 3 << " ";
    printTerm(out, args[i]);
    out << ")\n";
    assumptions[args[i]] = "a" + std::to_string(i - 3);
  }
  // Then, print the rest of the proof node
  uint32_t start_t = 1;
  std::unordered_map<std::shared_ptr<ProofNode>, std::string> steps = {};
  printInternal(out, pfn->getChildren()[0], assumptions, steps, "", start_t);
}

std::string AletheProofPrinter::printInternal(
    std::ostream& out,
    std::shared_ptr<ProofNode> pfn,
    std::unordered_map<Node, std::string>& assumptions,
    std::unordered_map<std::shared_ptr<ProofNode>, std::string>& steps,
    std::string current_prefix,
    uint32_t& current_step_id)
{
  int step_id = current_step_id;
  const std::vector<Node>& args = pfn->getArguments();

  // If the proof node is untranslated a problem might have occured during
  // postprocessing
  if (args.size() < 3 || pfn->getRule() != PfRule::ALETHE_RULE)
  {
    Trace("alethe-printer")
        << "... printing failed! Encountered untranslated Node. "
        << pfn->getResult() << " " << pfn->getRule() << " "
        << " / " << args << std::endl;
    return "";
  }

  // Get the alethe proof rule
  AletheRule arule = getAletheRule(args[0]);

  // Assumptions are printed at the anchor and therefore have to be in the list
  // of assumptions when an assume is reached.
  if (arule == AletheRule::ASSUME)
  {
    Trace("alethe-printer")
        << "... reached assumption " << pfn->getResult() << " " << arule << " "
        << " / " << args << " " << std::endl;

    auto it = assumptions.find(args[2]);
    Assert(it != assumptions.end())
        << "Assumption has not been printed yet! " << args[2] << "/"
        << assumptions << std::endl;
    Trace("alethe-printer")
        << "... found assumption in list " << it->second << ": " << args[2]
        << "/" << assumptions << std::endl;
    return it->second;
  }

  // If the current step is already printed return its id
  auto it = steps.find(pfn);
  if (it != steps.end())
  {
    Trace("alethe-printer")
        << "... step is already printed " << it->second << " "
        << pfn->getResult() << " " << arule << " / " << args << std::endl;
    return it->second;
  }
  std::vector<std::string> current_assumptions;
  std::unordered_map<Node, std::string> assumptions_before_subproof =
      assumptions;
  std::unordered_map<std::shared_ptr<ProofNode>, std::string>
      steps_before_subproof = steps;

  // In case the rule is an anchor it is printed before its children.
  if (arule == AletheRule::ANCHOR_SUBPROOF || arule == AletheRule::ANCHOR_BIND)
  {
    // Print anchor
    std::string current_t =
        current_prefix + "t" + std::to_string(current_step_id);
    Trace("alethe-printer")
        << "... print anchor " << pfn->getResult() << " " << arule << " "
        << " / " << args << std::endl;
    out << "(anchor :step " << current_t;

    // Append index of anchor to prefix so that all steps in the subproof use it
    current_prefix.append("t" + std::to_string(current_step_id) + ".");

    // Reset the current step id s.t. the numbering inside the subproof starts
    // with 1
    current_step_id = 1;

    // If the subproof is a bind the arguments need to be printed as
    // assignments, i.e. args=[(= v0 v1)] is printed as (:= (v0 Int) v1).
    //
    // Note that since these are variables there is no need to letify.
    if (arule == AletheRule::ANCHOR_BIND)
    {
      out << " :args (";
      for (size_t j = 3, size = args.size(); j < size; j++)
      {
        out << "(:= (" << args[j][0] << " " << args[j][0].getType() << ") "
            << args[j][1] << ")" << (j != args.size() - 1 ? " " : "");
      }
      out << ")";
    }
    // In all other cases there are no arguments
    out << ")\n";

    // If the subproof is a genuine subproof the arguments are printed as
    // assumptions. To be able to discharge the assumptions afterwards we need
    // to store them.
    if (arule == AletheRule::ANCHOR_SUBPROOF)
    {
      for (size_t i = 3, size = args.size(); i < size; i++)
      {
        std::string assumption_name =
            current_prefix + "a" + std::to_string(i - 3);
        Trace("alethe-printer")
            << "... print assumption " << args[i] << std::endl;
        out << "(assume " << assumption_name << " ";
        printTerm(out, args[i]);
        out << ")\n";
        assumptions[args[i]] = assumption_name;
        current_assumptions.push_back(assumption_name);
      }
    }
  }

  // Print children
  std::vector<std::string> child_prefixes;
  const std::vector<std::shared_ptr<ProofNode>>& children = pfn->getChildren();
  for (const std::shared_ptr<ProofNode>& child : children)
  {
    std::string child_prefix = printInternal(
        out, child, assumptions, steps, current_prefix, current_step_id);
    child_prefixes.push_back(child_prefix);
  }

  // If the rule is a subproof a final subproof step needs to be printed
  if (arule == AletheRule::ANCHOR_SUBPROOF || arule == AletheRule::ANCHOR_BIND)
  {
    Trace("alethe-printer") << "... print anchor node " << pfn->getResult()
                            << " " << arule << " / " << args << std::endl;

    current_prefix.pop_back();
    out << "(step " << current_prefix << " ";
    printTerm(out, args[2]);
    out << " :rule " << arule;

    // Reset steps array to the steps before the subproof since steps inside the
    // subproof cannot be accessed anymore
    steps = steps_before_subproof;
    assumptions = assumptions_before_subproof;

    // Add to map of printed steps
    steps[pfn] = current_prefix;
    Trace("alethe-printer") << "... add to steps " << pfn->getArguments()[2]
                            << " " << current_prefix << std::endl;

    // Reset step id to the number before the subproof + 1
    current_step_id = step_id + 1;

    // Discharge assumptions in the case of subproof
    // The assumptions of this level have been stored in current_assumptions
    if (arule == AletheRule::ANCHOR_SUBPROOF)
    {
      out << " :discharge (";
      for (size_t j = 0, size = current_assumptions.size(); j < size; j++)
      {
        out << current_assumptions[j] << (j != current_assumptions.size() - 1 ? " " : "");
      }
      out << ")";
    }
    out << ")\n";
    return current_prefix;
  }

  // Otherwise, the step is a normal step
  // Print current step
  std::string current_t =
      current_prefix + "t" + std::to_string(current_step_id);
  Trace("alethe-printer") << "... print node " << current_t << " "
                          << pfn->getResult() << " " << arule << " / " << args
                          << std::endl;

  // Add to map of printed steps
  steps[pfn] = current_t;
  Trace("alethe-printer") << "... add to steps " << pfn->getArguments()[2]
                          << " " << current_t << std::endl;
  current_step_id++;

  out << "(step " << current_t << " ";
  printTerm(out, args[2]);
  out << " :rule " << arule;
  if (args.size() > 3)
  {
    out << " :args (";
    for (size_t i = 3, size = args.size(); i < size; i++)
    {
      if (arule == AletheRule::FORALL_INST)
      {
        out << "(:= " << args[i][0] << " ";
        printTerm(out, args[i][1]);
        out << ")";
      }
      else
      {
        printTerm(out, args[i]);
      }
      if (i != args.size() - 1)
      {
        out << " ";
      }
    }
    out << ")";
  }
  if (pfn->getChildren().size() >= 1)
  {
    out << " :premises (";
    for (size_t i = 0, size = child_prefixes.size(); i < size; i++)
    {
      out << child_prefixes[i] << (i != size - 1? " " : "");
    }
    out << "))\n";
  }
  else
  {
    out << ")\n";
  }
  return current_t;
}

}  // namespace proof

}  // namespace cvc5::internal
