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
  // terms with a child that is being declared
  std::unordered_set<TNode> hasDeclaredChild;
  // For a term being declared, its position relative to the list of children
  // of the parent of this term, its parent, and its declaration value. These
  // are necessary to properly declare letified terms occurring for the first
  // time once conversions start
  std::unordered_map<TNode, size_t> declaredPosition;
  std::unordered_map<TNode, TNode> parentOf;
  std::unordered_map<TNode, Node> declaredValue;
  // visiting utils
  std::unordered_map<TNode, Node> visited;
  std::unordered_map<TNode, Node>::iterator it;
  std::vector<TNode> visit;
  TNode cur;
  // start with input
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
        Trace("alethe-printer-share")
            << "Node " << cur << " has id " << id << "\n";
        // if cur has previously been declared, just use the let variable.
        if (d_declared.find(cur) != d_declared.end())
        {
          // create the let variable for cur
          std::stringstream ss;
          ss << prefix << id;
          visited[cur] = nm->mkBoundVar(ss.str(), cur.getType());
          Trace("alethe-printer-share")
              << "\tdeclared, use var " << visited[cur] << "\n";
          continue;
        }
        // If the input of this method is letified and it has not yet been
        // declared, we will need to declare its post-visit result. So we do
        // nothing at this point other than book-keep. The information is
        // necessary to guarentee that this occurence, its first in the overall
        // term, is ultimately used as a declaration rather than as just the
        // letified variable. For this we find the parent of this first
        // occurrence of cur and the position in its children in which cur
        // occurs. The declaration will be created when cur is post-visited and
        // used when the parent of this occurrence of cur is post-visited.
        if (cur != n)
        {
          // The parent of cur will have been set when it was visited
          Assert(parentOf.find(cur) != parentOf.end());
          Node parent = parentOf[cur];
          auto itPos = std::find(parent.begin(), parent.end(), cur);
          Assert(itPos != parent.end());
          declaredPosition[cur] = itPos - parent.begin();
          Trace("alethe-printer-share")
              << "\tset for its parent " << parent << " mark position "
              << itPos - parent.begin() << "\n";
        }
        // Mark that future occurrences are just the variable
        d_declared.insert(cur);
      }
      if (cur.isClosure())
      {
        // We do not convert beneath quantifiers, so we need to finish the
        // traversal here. However if id > 0, then we need to declare cur's
        // variable. Since cur is not post-visited the declaration is of cur
        // itself.
        if (id == 0)
        {
          visited[cur] = cur;
          continue;
        }
        std::stringstream ss;
        ss << "(! ";
        options::ioutils::applyOutputLanguage(ss, Language::LANG_SMTLIB_V2_6);
        options::ioutils::applyDagThresh(ss, 0);
        cur.toStream(ss);
        ss << " :named " << prefix << id << ")";
        Node letVar = nm->mkRawSymbol(ss.str(), cur.getType());
        visited[cur] = letVar;
        declaredValue[cur] = letVar;
        continue;
      }
      visited[cur] = Node::null();
      visit.push_back(cur);
      // We now check if any of the children of cur is being declared, in which
      // case we associate cur as the parent of declared children, as will as
      // that cur has declared children.
      //
      // We also use this loop to add the children to be visited. Note we add
      // them in reverse order, since we must do post-order traversal (last
      // added to the list are first visited, thus this entails left-to-right
      // traversal of children)
      for (size_t i = 0, size = cur.getNumChildren(); i < size; ++i)
      {
        visit.push_back(cur[size - i - 1]);
        id = getId(cur[i]);
        if (id > 0 && d_declared.find(cur[i]) == d_declared.end())
        {
          parentOf[cur[i]] = cur;
          hasDeclaredChild.insert(cur);
        }
      }
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
      // if cur is a parent has declared child, then for each position we must
      // check if that position is of a child being declared and whose declared
      // position is that one. In this case we use not the value in visited but
      // rather the value in declaredValue
      bool checkDeclaredChild = hasDeclaredChild.count(cur);
      if (checkDeclaredChild)
      {
        Trace("alethe-printer-share")
            << "Post-visiting node " << cur << " with declared child\n";
      }
      for (size_t i = 0, size = cur.getNumChildren(); i < size; ++i)
      {
        bool useVisited = true;
        // cur has a declared child and if cur[i] is declared and in this
        // position, then we use its declared value rather than visited[cur[i]].
        if (checkDeclaredChild)
        {
          const auto& itDeclPos = declaredPosition.find(cur[i]);
          useVisited =
              itDeclPos == declaredPosition.end() || itDeclPos->second != i;
        }
        Assert(useVisited || getId(cur[i]) > 0)
            << "With input " << n << " we got child " << cur[i]
            << " to use declared value but its id is 0\n";
        it = useVisited ? visited.find(cur[i]) : declaredValue.find(cur[i]);
        Assert(it != visited.end())
            << "With input " << n << " did not find for term " << cur
            << " its child " << cur[i] << " in map with useVisited "
            << useVisited << "\n";
        Assert(!it->second.isNull());
        childChanged = childChanged || cur[i] != it->second;
        children.push_back(it->second);
      }
      if (childChanged)
      {
        ret = nm->mkNode(cur.getKind(), children);
      }
      id = getId(cur);
      // if cur has id bigger than 0, then we are declaring its conversion to
      // ret. We save the declaration in declaredValue and set the value in
      // visited to be the let variable, since next occurrences should use that.
      // The use of the declared value will be controlled by the parent. If cur
      // is n, since there is no parent, then we use directly the declared
      // value.
      if (id > 0)
      {
        std::stringstream ss, ssVar;
        ss << "(! ";
        options::ioutils::applyOutputLanguage(ss, Language::LANG_SMTLIB_V2_6);
        options::ioutils::applyDagThresh(ss, 0);
        ret.toStream(ss);
        ssVar << prefix << id;
        ss << " :named " << ssVar.str() << ")";
        Node declaration = nm->mkRawSymbol(ss.str(), ret.getType());
        declaredValue[cur] = declaration;
        visited[cur] =
            cur == n ? declaration : nm->mkBoundVar(ssVar.str(), cur.getType());
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
    // We do not go *below* cl, since the clause itself cannot be shared (goes
    // against the Alethe specification)
    // TODO strengthen this to guarantee this var is indeed the "cl" one
    if (args[i].getKind() == kind::SEXPR
        && args[i][0].getKind() == kind::BOUND_VARIABLE)
    {
      for (const auto& arg : args[i])
      {
        d_lbind.process(arg);
      }
      continue;
    }
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

AletheProofPrinter::AletheProofPrinter(Env& env)
    : EnvObj(env),
      d_lbind(options::ioutils::getDagThresh(std::cout)
                  ? options::ioutils::getDagThresh(std::cout) + 1
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
  ProofNodeUpdater updater(d_env, *(d_cb.get()), false, false);
  Trace("alethe-printer") << "- letify.\n";
  updater.process(innerPf);

  if (options::ioutils::getDagThresh(std::cout))
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
        out << current_assumptions[j]
            << (j != current_assumptions.size() - 1 ? " " : "");
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
