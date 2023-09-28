/******************************************************************************
 * Top contributors (to current version):
 *   Tim King, Dejan Jovanovic, Haniel Barbosa
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2023 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * [[ Add one-line brief description here ]]
 *
 * [[ Add lengthier description here ]]
 * \todo document this file
 */

#include "theory/booleans/theory_bool_rewriter.h"

#include <algorithm>
#include <unordered_set>

#include "expr/algorithm/flatten.h"
#include "expr/node_value.h"
#include "util/cardinality.h"

namespace cvc5::internal {
namespace theory {
namespace booleans {

RewriteResponse TheoryBoolRewriter::postRewrite(TNode node) {
  return preRewrite(node);
}

/**
 * flattenNode looks for children of same kind, and if found merges
 * them into the parent.
 *
 * It simultaneously handles a couple of other optimizations:
 * - trivialNode - if found during exploration, return that node itself
 *    (like in case of OR, if "true" is found, makes sense to replace
 *     whole formula with "true")
 * - skipNode - as name suggests, skip them
 *    (like in case of OR, you may want to skip any "false" nodes found)
 *
 * Use a null node if you want to ignore any of the optimizations.
 */
RewriteResponse flattenNode(TNode n, TNode trivialNode, TNode skipNode)
{
  // first flatten
  Node nf = expr::algorithm::flatten(n);
  if (nf != n)
  {
    return RewriteResponse(REWRITE_AGAIN, nf);
  }
  // then do duplicate elimination and trivial node finding
  std::vector<TNode> childList;
  std::unordered_set<TNode> visited;
  visited.insert(skipNode);
  bool hadDups = false;
  for (TNode nn : n)
  {
    if (nn == trivialNode)
    {
      return RewriteResponse(REWRITE_DONE, trivialNode);
    }
    if (visited.find(nn) != visited.end())
    {
      hadDups = true;
      continue;
    }
    visited.insert(nn);
    childList.push_back(nn);
  }
  if (hadDups)
  {
    if (childList.empty())
    {
      return RewriteResponse(REWRITE_DONE, skipNode);
    }
    if (childList.size() == 1)
    {
      return RewriteResponse(REWRITE_AGAIN, childList[0]);
    }
    Node nn = NodeManager::currentNM()->mkNode(n.getKind(), childList);
    return RewriteResponse(REWRITE_AGAIN, nn);
  }
  return RewriteResponse(REWRITE_DONE, n);
}

// Equality parity returns
// * 0 if no relation between a and b is found
// * 1 if a == b
// * 2 if a == not(b)
// * 3 or b == not(a)
inline int equalityParity(TNode a, TNode b){
  if(a == b){
    return 1;
  }else if(a.getKind() == kind::NOT && a[0] == b){
    return 2;
  }else if(b.getKind() == kind::NOT && b[0] == a){
    return 3;
  }else{
    return 0;
  }
}

inline Node makeNegation(TNode n){
  bool even = false;
  while(n.getKind() == kind::NOT){
    n = n[0];
    even = !even;
  }
  if(even){
    return n;
  } else {
    if(n.isConst()){
      return NodeManager::currentNM()->mkConst(!n.getConst<bool>());
    }else{
      return n.notNode();
    }
  }
}

RewriteResponse TheoryBoolRewriter::preRewrite(TNode n) {
  NodeManager* nodeManager = NodeManager::currentNM();
  Node tt = nodeManager->mkConst(true);
  Node ff = nodeManager->mkConst(false);

  switch(n.getKind()) {
  case kind::NOT: {
    if (n[0] == tt) return RewriteResponse(REWRITE_DONE, ff);
    if (n[0] == ff) return RewriteResponse(REWRITE_DONE, tt);
    if (n[0].getKind() == kind::NOT) return RewriteResponse(REWRITE_AGAIN, n[0][0]);
    break;
  }
  case kind::OR: {
    bool done = true;
    TNode::iterator i = n.begin(), iend = n.end();
    for(; i != iend; ++i) {
      if (*i == tt) return RewriteResponse(REWRITE_DONE, tt);
      if (*i == ff) done = false;
      if ((*i).getKind() == kind::OR) done = false;
    }
    if (!done) {
      return flattenNode(n, /* trivialNode = */ tt, /* skipNode = */ ff);
    }
    // x v ... v x --> x
    unsigned ind, size;
    for (ind = 0, size = n.getNumChildren(); ind < size - 1; ++ind)
    {
      if (n[ind] != n[ind+1])
      {
        break;
      }
    }
    if (ind == size - 1)
    {
      return RewriteResponse(REWRITE_AGAIN, n[0]);
    }
    break;
  }
  case kind::AND: {
    bool done = true;
    TNode::iterator i = n.begin(), iend = n.end();
    for(; i != iend; ++i) {
      if (*i == ff) return RewriteResponse(REWRITE_DONE, ff);
      if (*i == tt) done = false;
      if ((*i).getKind() == kind::AND) done = false;
    }
    if (!done) {
      RewriteResponse ret = flattenNode(n, /* trivialNode = */ ff, /* skipNode = */ tt);
      Trace("bool-flatten") << n << ": " << ret.d_node << std::endl;
      return ret;
    }
    // x ^ ... ^ x --> x
    unsigned ind, size;
    for (ind = 0, size = n.getNumChildren(); ind < size - 1; ++ind)
    {
      if (n[ind] != n[ind+1])
      {
        break;
      }
    }
    if (ind == size - 1)
    {
      return RewriteResponse(REWRITE_AGAIN, n[0]);
    }
    break;
  }
  case kind::IMPLIES: {
    if (n[0] == ff || n[1] == tt) return RewriteResponse(REWRITE_DONE, tt);
    if (n[0] == tt && n[1] == ff) return RewriteResponse(REWRITE_DONE, ff);
    if (n[0] == tt) return RewriteResponse(REWRITE_AGAIN, n[1]);
    if (n[1] == ff) return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
    break;
  }
  case kind::EQUAL: {
    // rewrite simple cases of IFF
    if(n[0] == tt) {
      // IFF true x
      return RewriteResponse(REWRITE_AGAIN, n[1]);
    } else if(n[1] == tt) {
      // IFF x true
      return RewriteResponse(REWRITE_AGAIN, n[0]);
    } else if(n[0] == ff) {
      // IFF false x
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[1]));
    } else if(n[1] == ff) {
      // IFF x false
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
    } else if(n[0] == n[1]) {
      // IFF x x
      return RewriteResponse(REWRITE_DONE, tt);
    } else if(n[0].getKind() == kind::NOT && n[0][0] == n[1]) {
      // IFF (NOT x) x
      return RewriteResponse(REWRITE_DONE, ff);
    } else if(n[1].getKind() == kind::NOT && n[1][0] == n[0]) {
      // IFF x (NOT x)
      return RewriteResponse(REWRITE_DONE, ff);
    } else if(n[0].getKind() == kind::EQUAL && n[1].getKind() == kind::EQUAL) {
      // a : (= i x)
      // i : (= j k)
      // x : (= y z)

      // assume wlog k, z are constants and j is the same symbol as y
      // (iff (= j k) (= j z))
      // if k = z
      //  then (iff (= j k) (= j k)) => true
      // else
      //  (iff (= j k) (= j z)) <=> b
      //  b : (and (not (= j k)) (not (= j z)))
      //  (= j k) (= j z) | a b
      //  f       f       | t t
      //  f       t       | f f
      //  t       f       | f f
      //  t       t       | * f
      // * j cannot equal both k and z in a theory model
      TNode t,c;
      if (n[0][0].isConst()) {
        c = n[0][0];
        t = n[0][1];
      }
      else if (n[0][1].isConst()) {
        c = n[0][1];
        t = n[0][0];
      }
      bool matchesForm = false;
      bool constantsEqual = false;
      if (!c.isNull()) {
        if (n[1][0] == t && n[1][1].isConst()) {
          matchesForm = true;
          constantsEqual = (n[1][1] == c);
        }
        else if (n[1][1] == t && n[1][0].isConst()) {
          matchesForm = true;
          constantsEqual = (n[1][0] == c);
        }
      }
      if(matchesForm){
        if(constantsEqual){
          return RewriteResponse(REWRITE_DONE, tt);
        }else{
          if (t.getType().isCardinalityLessThan(2))
          {
            return RewriteResponse(REWRITE_DONE, ff);
          }
          else
          {
            Node neitherEquality = (makeNegation(n[0])).andNode(makeNegation(n[1]));
            return RewriteResponse(REWRITE_AGAIN, neitherEquality);
          }
        }
      }
    }
    // sort
    if (n[0].getId() > n[1].getId())
    {
      return RewriteResponse(REWRITE_AGAIN,
                             nodeManager->mkNode(kind::EQUAL, n[1], n[0]));
    }
    return RewriteResponse(REWRITE_DONE, n);
  }
  case kind::XOR: {
    // rewrite simple cases of XOR
    if(n[0] == tt) {
      // XOR true x
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[1]));
    } else if(n[1] == tt) {
      // XOR x true
      return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
    } else if(n[0] == ff) {
      // XOR false x
      return RewriteResponse(REWRITE_AGAIN, n[1]);
    } else if(n[1] == ff) {
      // XOR x false
      return RewriteResponse(REWRITE_AGAIN, n[0]);
    } else if(n[0] == n[1]) {
      // XOR x x
      return RewriteResponse(REWRITE_DONE, ff);
    } else if(n[0].getKind() == kind::NOT && n[0][0] == n[1]) {
      // XOR (NOT x) x
      return RewriteResponse(REWRITE_DONE, tt);
    } else if(n[1].getKind() == kind::NOT && n[1][0] == n[0]) {
      // XOR x (NOT x)
      return RewriteResponse(REWRITE_DONE, tt);
    }
    break;
  }
  case kind::ITE: {
    // non-Boolean-valued ITEs should have been removed in place of
    // a variable
    // rewrite simple cases of ITE
    if (n[0].isConst()) {
      if (n[0] == tt) {
        // ITE true x y
        Trace("bool-ite") << "TheoryBoolRewriter::preRewrite_ITE: n[0] ==tt "
                          << n << ": " << n[1] << std::endl;
        return RewriteResponse(REWRITE_AGAIN, n[1]);
      } else {
        Assert(n[0] == ff);
        // ITE false x y
        Trace("bool-ite") << "TheoryBoolRewriter::preRewrite_ITE: n[0] ==ff "
                          << n << ": " << n[1] << std::endl;
        return RewriteResponse(REWRITE_AGAIN, n[2]);
      }
    } else if (n[1].isConst()) {
      if (n[1] == tt && n[2] == ff) {
        Trace("bool-ite")
            << "TheoryBoolRewriter::preRewrite_ITE: n[1] ==tt && n[2] == ff "
            << n << ": " << n[0] << std::endl;
        return RewriteResponse(REWRITE_AGAIN, n[0]);
      }
      else if (n[1] == ff && n[2] == tt) {
        Trace("bool-ite")
            << "TheoryBoolRewriter::preRewrite_ITE: n[1] ==ff && n[2] == tt "
            << n << ": " << n[0].notNode() << std::endl;
        return RewriteResponse(REWRITE_AGAIN, makeNegation(n[0]));
      }
    }

    if (n[0].getKind() == kind::NOT)
    {
      // ite(not(c), x, y) ---> ite(c, y, x)
      return RewriteResponse(
          REWRITE_AGAIN, nodeManager->mkNode(kind::ITE, n[0][0], n[2], n[1]));
    }

    int parityTmp;
    if ((parityTmp = equalityParity(n[1], n[2])) != 0) {
      Node resp = (parityTmp == 1) ? (Node)n[1] : n[0].eqNode(n[1]);
      Trace("bool-ite")
          << "TheoryBoolRewriter::preRewrite_ITE:  equalityParity n[1], n[2] "
          << parityTmp << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(!n[1].isConst() && (parityTmp = equalityParity(n[0], n[1])) != 0){
      // (parityTmp == 1) if n[0] == n[1]
      // otherwise, n[0] == not(n[1]) or not(n[0]) == n[1]

      // if n[1] is constant this can loop, this is possible in prewrite
      Node resp = n[0].iteNode( (parityTmp == 1) ? tt : ff, n[2]);
      Trace("bool-ite")
          << "TheoryBoolRewriter::preRewrite_ITE: equalityParity n[0], n[1] "
          << parityTmp << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(!n[2].isConst() && (parityTmp = equalityParity(n[0], n[2])) != 0){
      // (parityTmp == 1) if n[0] == n[2]
      // otherwise, n[0] == not(n[2]) or not(n[0]) == n[2]
      Node resp = n[0].iteNode(n[1], (parityTmp == 1) ? ff : tt);
      Trace("bool-ite")
          << "TheoryBoolRewriter::preRewrite_ITE: equalityParity n[0], n[2] "
          << parityTmp << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(n[1].getKind() == kind::ITE &&
              (parityTmp = equalityParity(n[0], n[1][0])) != 0){
      // (parityTmp == 1) then n : (ite c (ite c x y) z)
      // (parityTmp > 1)  then n : (ite c (ite (not c) x y) z) or
      // n: (ite (not c) (ite c x y) z)
      Node resp = n[0].iteNode((parityTmp == 1) ? n[1][1] : n[1][2], n[2]);
      Trace("bool-ite")
          << "TheoryBoolRewriter::preRewrite: equalityParity n[0], n[1][0] "
          << parityTmp << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    } else if(n[2].getKind() == kind::ITE &&
              (parityTmp = equalityParity(n[0], n[2][0])) != 0){
      // (parityTmp == 1) then n : (ite c x (ite c y z))
      // (parityTmp > 1)  then n : (ite c x (ite (not c) y z)) or
      // n: (ite (not c) x (ite c y z))
      Node resp = n[0].iteNode(n[1], (parityTmp == 1) ? n[2][2] : n[2][1]);
      Trace("bool-ite")
          << "TheoryBoolRewriter::preRewrite: equalityParity n[0], n[2][0] "
          << parityTmp << " " << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    }

    // Rewrites for ITEs with a constant branch. These rewrites are applied
    // after the parity rewrites above because they may simplify ITEs such as
    // `(ite c c true)` to `(ite c true true)`. As a result, we avoid
    // introducing an unnecessary conjunction/disjunction here.
    if (n[1].isConst() && (n[1] == tt || n[1] == ff))
    {
      // ITE C true y --> C v y
      // ITE C false y --> ~C ^ y
      Node resp =
          n[1] == tt ? n[0].orNode(n[2]) : (n[0].negate()).andNode(n[2]);
      Trace("bool-ite") << "TheoryBoolRewriter::preRewrite_ITE: n[1] const "
                        << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    }
    else if (n[2].isConst() && (n[2] == tt || n[2] == ff))
    {
      // ITE C x true --> ~C v x
      // ITE C x false --> C ^ x
      Node resp =
          n[2] == tt ? (n[0].negate()).orNode(n[1]) : n[0].andNode(n[1]);
      Trace("bool-ite") << "TheoryBoolRewriter::preRewrite_ITE: n[2] const "
                        << n << ": " << resp << std::endl;
      return RewriteResponse(REWRITE_AGAIN, resp);
    }

    break;
  }
  default:
    return RewriteResponse(REWRITE_DONE, n);
  }
  return RewriteResponse(REWRITE_DONE, n);
}

}  // namespace booleans
}  // namespace theory
}  // namespace cvc5::internal
