/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Rewrite database
 */

#include "theory/rewrite_term_util.h"

#include "expr/attribute.h"
#include "theory/bv/theory_bv_utils.h"
#include "theory/strings/word.h"
#include "util/bitvector.h"
#include "util/rational.h"
#include "util/regexp.h"
#include "util/string.h"

using namespace cvc5::kind;

namespace cvc5 {
namespace theory {

struct IsListTag
{
};
using IsListAttr = expr::Attribute<IsListTag, bool>;

void markListVar(TNode fv)
{
  Assert(fv.getKind() == BOUND_VARIABLE);
  fv.setAttribute(IsListAttr(), true);
}

bool isListVar(TNode fv) { return fv.hasAttribute(IsListAttr()); }

bool containsListVar(TNode n)
{
  std::unordered_set<TNode> visited;
  std::unordered_set<TNode>::iterator it;
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
      visited.insert(cur);
      if (isListVar(cur))
      {
        return true;
      }
      visit.insert(visit.end(), cur.begin(), cur.end());
    }
  } while (!visit.empty());
  return false;
}

Node getNullTerminator(Kind k, TypeNode tn)
{
  NodeManager* nm = NodeManager::currentNM();
  Node nullTerm;
  switch (k)
  {
    case OR: nullTerm = nm->mkConst(false); break;
    case AND:
    case SEP_STAR: nullTerm = nm->mkConst(true); break;
    case PLUS: nullTerm = nm->mkConst(Rational(0)); break;
    case MULT:
    case NONLINEAR_MULT: nullTerm = nm->mkConst(Rational(1)); break;
    case STRING_CONCAT:
      // handles strings and sequences
      nullTerm = theory::strings::Word::mkEmptyWord(tn);
      break;
    case REGEXP_CONCAT:
      // the language containing only the empty string
      nullTerm = nm->mkNode(STRING_TO_REGEXP, nm->mkConst(String("")));
      break;
    case BITVECTOR_AND:
      nullTerm = theory::bv::utils::mkOnes(tn.getBitVectorSize());
      break;
    case BITVECTOR_OR:
    case BITVECTOR_ADD:
    case BITVECTOR_XOR:
      nullTerm = theory::bv::utils::mkZero(tn.getBitVectorSize());
      break;
    case BITVECTOR_MULT:
      nullTerm = theory::bv::utils::mkOne(tn.getBitVectorSize());
      break;
    case BITVECTOR_CONCAT:
    {
      // the null terminator of bitvector concat is a dummy variable of
      // bit-vector type with zero width, regardless of the type of the overall
      // concat. FIXME
    }
    break;
    default:
      // not handled as null-terminated
      break;
  }
  return nullTerm;
}

Node listSubstitute(Node src,
                    std::vector<Node>& vars,
                    std::vector<std::vector<Node> >& subs)
{
  // assumes all variables are list variables
  NodeManager* nm = NodeManager::currentNM();
  std::unordered_map<TNode, Node> visited;
  std::unordered_map<TNode, Node>::iterator it;
  std::vector<TNode> visit;
  std::vector<Node>::iterator itv;
  TNode cur;
  visit.push_back(src);
  do
  {
    cur = visit.back();
    it = visited.find(cur);
    if (it == visited.end())
    {
      visited[cur] = Node::null();
      visit.insert(visit.end(), cur.begin(), cur.end());
      continue;
    }
    visit.pop_back();
    if (it->second.isNull())
    {
      Node ret = cur;
      bool childChanged = false;
      std::vector<Node> children;
      for (const Node& cn : cur)
      {
        // if it is variable to replace, insert the list
        itv = std::find(vars.begin(), vars.end(), cur);
        if (itv != vars.end())
        {
          childChanged = true;
          size_t d = std::distance(vars.begin(), itv);
          Assert(d < subs.size());
          std::vector<Node>& sd = subs[d];
          children.insert(children.end(), sd.begin(), sd.end());
          continue;
        }
        it = visited.find(cn);
        Assert(it != visited.end());
        Assert(!it->second.isNull());
        childChanged = childChanged || cn != it->second;
        children.push_back(it->second);
      }
      if (childChanged)
      {
        if (children.size() != cur.getNumChildren())
        {
          // n-ary operators cannot be parameterized
          Assert(cur.getMetaKind() != metakind::PARAMETERIZED);
          ret = children.empty()
                    ? getNullTerminator(cur.getKind(), cur.getType())
                    : (children.size() == 1
                           ? children[0]
                           : nm->mkNode(cur.getKind(), children));
        }
        else
        {
          if (cur.getMetaKind() == metakind::PARAMETERIZED)
          {
            children.insert(children.begin(), cur.getOperator());
          }
          ret = nm->mkNode(cur.getKind(), children);
        }
      }
      visited[cur] = ret;
    }

  } while (!visit.empty());
  Assert(visited.find(src) != visited.end());
  Assert(!visited.find(src)->second.isNull());
  return visited[src];
}

bool listMatch(Node n1,
               Node n2,
               std::unordered_map<Node, std::vector<Node> >& subs)
{
  // TODO
  return false;
}

}  // namespace theory
}  // namespace cvc5
