/*********************                                                        */
/*! \file theory_arrays_rewriter.h
 ** \verbatim
 ** Original author: dejan
 ** Major contributors: barrett, mdeters
 ** Minor contributors (to current version): none
 ** This file is part of the CVC4 prototype.
 ** Copyright (c) 2009, 2010, 2011  The Analysis of Computer Systems Group (ACSys)
 ** Courant Institute of Mathematical Sciences
 ** New York University
 ** See the file COPYING in the top-level source directory for licensing
 ** information.\endverbatim
 **
 ** \brief [[ Add one-line brief description here ]]
 **
 ** [[ Add lengthier description here ]]
 ** \todo document this file
 **/

#include "cvc4_private.h"

#ifndef __CVC4__THEORY__ARRAYS__THEORY_ARRAYS_REWRITER_H
#define __CVC4__THEORY__ARRAYS__THEORY_ARRAYS_REWRITER_H

#include "theory/rewriter.h"

namespace CVC4 {
namespace theory {
namespace arrays {

class TheoryArraysRewriter {

  static Node normalizeConstant(TNode node) {
    TNode store = node[0];
    TNode index = node[1];
    TNode value = node[2];

    std::vector<TNode> indices;
    std::vector<TNode> elements;

    // Normal form for nested stores is just ordering by index - but also need
    // to check if we are writing to default value

    // Go through nested stores looking for where to insert index
    // Also check whether we are replacing an existing store
    TNode replacedValue;
    Integer depth = 1;
    Integer valCount = 1;
    while (store.getKind() == kind::STORE) {
      if (index == store[1]) {
        replacedValue = store[2];
        store = store[0];
        break;
      }
      else if (!(index < store[1])) {
        break;
      }
      if (value == store[2]) {
        valCount += 1;
      }
      depth += 1;
      indices.push_back(store[1]);
      elements.push_back(store[2]);
      store = store[0];
    }
    Node n = store;

    // Get the default value at the bottom of the nested stores
    while (store.getKind() == kind::STORE) {
      if (value == store[2]) {
        valCount += 1;
      }
      depth += 1;
      store = store[0];
    }
    Assert(store.getKind() == kind::STORE_ALL);
    ArrayStoreAll storeAll = store.getConst<ArrayStoreAll>();
    Node defaultValue = Node::fromExpr(storeAll.getExpr());
    NodeManager* nm = NodeManager::currentNM();

    // Check if we are writing to default value - if so the store
    // to index can be ignored
    if (value == defaultValue) {
      if (replacedValue.isNull()) {
        // Quick exit - if writing to default value and nothing was
        // replaced, we can just return node[0]
        return node[0];
      }
    }
    else {
      n = nm->mkNode(kind::STORE, n, index, value);
    }

    // Build the rest of the store after inserting/deleting
    while (!indices.empty()) {
      n = nm->mkNode(kind::STORE, n, indices.back(), elements.back());
      indices.pop_back();
      elements.pop_back();
    }

    Cardinality indexCard = index.getType().getCardinality();
    if (indexCard.isInfinite()) {
      return n;
    }

    // When index sort is finite, we have to check whether there is any value
    // that is written to more than the default value.  If so, it must become
    // the new default value

    TNode mostFrequentValue;
    Integer mostFrequentValueCount = 0;
    bool recompute = false;
    if (node[0].getKind() == kind::STORE) {
      // TODO: look up most frequent value and count
      if (!replacedValue.isNull() && mostFrequentValue == replacedValue) {
        recompute = true;
      }
    }

    // Compute the most frequently written value for n
    if (valCount > mostFrequentValueCount ||
        (valCount == mostFrequentValueCount && value < mostFrequentValue)) {
      mostFrequentValue = value;
      mostFrequentValueCount = valCount;
      recompute = false;
    }

    // Need to make sure the default value count is larger, or the same and the default value is expression-order-less-than nextValue
    int compare;// = indexCard.compare(mostFrequentValueCount + depth);
    // Assert result of compare is not unknown
    if (compare > 0 ||
        (compare == 0 && (defaultValue < mostFrequentValue))) {
      return n;
    }

    // Bad case: have to recompute value counts and/or possibly switch out
    // default value
    store = n;
    std::hash_set<TNode, TNodeHashFunction> indexSet;
    std::hash_map<TNode, unsigned, TNodeHashFunction> elementsMap;
    std::hash_map<TNode, unsigned, TNodeHashFunction>::iterator it;
    unsigned count;
    unsigned max = 0;
    TNode maxValue;
    while (store.getKind() == kind::STORE) {
      indices.push_back(store[1]);
      indexSet.insert(store[1]);
      elements.push_back(store[2]);
      it = elementsMap.find(store[2]);
      if (it != elementsMap.end()) {
        (*it).second = (*it).second + 1;
        count = (*it).second;
      }
      else {
        elementsMap[store[2]] = 1;
        count = 1;
      }
      if (count > max ||
          (count == max && store[2] < maxValue)) {
        max = count;
        maxValue = store[2];
      }
      store = store[0];
    }

    Assert(depth == indices.size());
    //compare = indexCard.compare(max + depth);
    // Assert result of compare is not unknown
    if (compare > 0 ||
        (compare == 0 && (defaultValue < maxValue))) {
      Assert(recompute);
      return n;
    }

    // Out of luck: have to swap out default value
    std::vector<Node> newIndices;
    // Enumerate values from index type into newIndices and sort

    //n = storeAll(maxValue)
    while (!newIndices.empty() || !indices.empty()) {
      if (!newIndices.empty() && (indices.empty() || newIndices.back() < indices.back())) {
        n = nm->mkNode(kind::STORE, n, newIndices.back(), defaultValue);
        newIndices.pop_back();
      }
      else if (newIndices.empty() || indices.back() < newIndices.back()) {
        if (elements.back() != maxValue) {
          n = nm->mkNode(kind::STORE, n, indices.back(), elements.back());
        }
        indices.pop_back();
        elements.pop_back();
      }
    }
    return n;
  }

public:

  static RewriteResponse postRewrite(TNode node) {
    Trace("arrays-postrewrite") << "Arrays::postRewrite start " << node << std::endl;
    switch (node.getKind()) {
      case kind::SELECT: {
        TNode store = node[0];
        TNode index = node[1];
        Node n;
        bool val;
        while (store.getKind() == kind::STORE) {
          if (index == store[1]) {
            val = true;
          }
          else if (index.isConst() && store[1].isConst()) {
            val = false;
          }
          else {
            n = Rewriter::rewrite(store[1].eqNode(index));
            if (n.getKind() != kind::CONST_BOOLEAN) {
              break;
            }
            val = n.getConst<bool>();
          }
          if (val) {
            // select(store(a,i,v),j) = v if i = j
            Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << store[2] << std::endl;
            return RewriteResponse(REWRITE_DONE, store[2]);
          }
          // select(store(a,i,v),j) = select(a,j) if i /= j
          store = store[0];
        }
        if (store.getKind() == kind::STORE_ALL) {
          // select(store_all(v),i) = v
          ArrayStoreAll storeAll = store.getConst<ArrayStoreAll>();
          n = Node::fromExpr(storeAll.getExpr());
          Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << n << std::endl;
          Assert(n.isConst());
          return RewriteResponse(REWRITE_DONE, n);
        }
        else if (store != node[0]) {
          n = NodeManager::currentNM()->mkNode(kind::SELECT, store, index);
          Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << n << std::endl;
          return RewriteResponse(REWRITE_DONE, n);
        }
        break;
      }
      case kind::STORE: {
        TNode store = node[0];
        TNode value = node[2];
        // store(a,i,select(a,i)) = a
        if (value.getKind() == kind::SELECT &&
            value[0] == store &&
            value[1] == node[1]) {
          Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << store << std::endl;
          return RewriteResponse(REWRITE_DONE, store);
        }
        TNode index = node[1];
        if (store.isConst() && index.isConst() && value.isConst()) {
          // normalize constant
          Node n = normalizeConstant(node);
          Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << n << std::endl;
          return RewriteResponse(REWRITE_DONE, n);
        }
        if (store.getKind() == kind::STORE) {
          // store(store(a,i,v),j,w)
          bool val;
          if (index == store[1]) {
            val = true;
          }
          else if (index.isConst() && store[1].isConst()) {
            val = false;
          }
          else {
            Node eqRewritten = Rewriter::rewrite(store[1].eqNode(index));
            if (eqRewritten.getKind() != kind::CONST_BOOLEAN) {
              Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << node << std::endl;
              return RewriteResponse(REWRITE_DONE, node);
            }
            val = eqRewritten.getConst<bool>();
          }
          NodeManager* nm = NodeManager::currentNM();
          if (val) {
            // store(store(a,i,v),i,w) = store(a,i,w)
            Node result = nm->mkNode(kind::STORE, store[0], index, value);
            Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << result << std::endl;
            return RewriteResponse(REWRITE_DONE, result);
          }
          else if (index < store[1]) {
            // store(store(a,i,v),j,w) = store(store(a,j,w),i,v)
            //    IF i != j and j comes before i in the ordering
            std::vector<TNode> indices;
            std::vector<TNode> elements;
            indices.push_back(store[1]);
            elements.push_back(store[2]);
            store = store[0];
            Node n;
            while (store.getKind() == kind::STORE) {
              if (index == store[1]) {
                val = true;
              }
              else if (index.isConst() && store[1].isConst()) {
                val = false;
              }
              else {
                n = Rewriter::rewrite(store[1].eqNode(index));
                if (n.getKind() != kind::CONST_BOOLEAN) {
                  break;
                }
                val = n.getConst<bool>();
              }
              if (val) {
                store = store[0];
                break;
              }
              else if (!(index < store[1])) {
                break;
              }
              indices.push_back(store[1]);
              elements.push_back(store[2]);
              store = store[0];
            }
            n = nm->mkNode(kind::STORE, store, index, value);
            while (!indices.empty()) {
              n = nm->mkNode(kind::STORE, n, indices.back(), elements.back());
              indices.pop_back();
              elements.pop_back();
            }
            Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << n << std::endl;
            return RewriteResponse(REWRITE_DONE, n);
          }
        }
        break;
      }
      case kind::EQUAL:
      case kind::IFF: {
        if(node[0] == node[1]) {
          Trace("arrays-postrewrite") << "Arrays::postRewrite returning true" << std::endl;
          return RewriteResponse(REWRITE_DONE, NodeManager::currentNM()->mkConst(true));
        }
        if (node[0] > node[1]) {
          Node newNode = NodeManager::currentNM()->mkNode(node.getKind(), node[1], node[0]);
          Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << newNode << std::endl;
          return RewriteResponse(REWRITE_DONE, newNode);
        }
        break;
      }
      default:
        break;
    }
    Trace("arrays-postrewrite") << "Arrays::postRewrite returning " << node << std::endl;
    return RewriteResponse(REWRITE_DONE, node);
  }

  static inline RewriteResponse preRewrite(TNode node) {
    Trace("arrays-prerewrite") << "Arrays::preRewrite start " << node << std::endl;
    switch (node.getKind()) {
      case kind::SELECT: {
        TNode store = node[0];
        TNode index = node[1];
        Node n;
        bool val;
        while (store.getKind() == kind::STORE) {
          if (index == store[1]) {
            val = true;
          }
          else if (index.isConst() && store[1].isConst()) {
            val = false;
          }
          else {
            n = Rewriter::rewrite(store[1].eqNode(index));
            if (n.getKind() != kind::CONST_BOOLEAN) {
              break;
            }
            val = n.getConst<bool>();
          }
          if (val) {
            // select(store(a,i,v),j) = v if i = j
            Trace("arrays-prerewrite") << "Arrays::preRewrite returning " << store[2] << std::endl;
            return RewriteResponse(REWRITE_AGAIN, store[2]);
          }
          // select(store(a,i,v),j) = select(a,j) if i /= j
          store = store[0];
        }
        if (store.getKind() == kind::STORE_ALL) {
          // select(store_all(v),i) = v
          ArrayStoreAll storeAll = store.getConst<ArrayStoreAll>();
          n = Node::fromExpr(storeAll.getExpr());
          Trace("arrays-prerewrite") << "Arrays::preRewrite returning " << n << std::endl;
          Assert(n.isConst());
          return RewriteResponse(REWRITE_DONE, n);
        }
        else if (store != node[0]) {
          n = NodeManager::currentNM()->mkNode(kind::SELECT, store, index);
          Trace("arrays-prerewrite") << "Arrays::preRewrite returning " << n << std::endl;
          return RewriteResponse(REWRITE_DONE, n);
        }
        break;
      }
      case kind::STORE: {
        TNode store = node[0];
        TNode value = node[2];
        // store(a,i,select(a,i)) = a
        if (value.getKind() == kind::SELECT &&
            value[0] == store &&
            value[1] == node[1]) {
          Trace("arrays-prerewrite") << "Arrays::preRewrite returning " << store << std::endl;
          return RewriteResponse(REWRITE_AGAIN, store);
        }
        if (store.getKind() == kind::STORE) {
          // store(store(a,i,v),j,w)
          TNode index = node[1];
          bool val;
          if (index == store[1]) {
            val = true;
          }
          else if (index.isConst() && store[1].isConst()) {
            val = false;
          }
          else {
            Node eqRewritten = Rewriter::rewrite(store[1].eqNode(index));
            if (eqRewritten.getKind() != kind::CONST_BOOLEAN) {
              break;
            }
            val = eqRewritten.getConst<bool>();
          }
          NodeManager* nm = NodeManager::currentNM();
          if (val) {
            // store(store(a,i,v),i,w) = store(a,i,w)
            Node newNode = nm->mkNode(kind::STORE, store[0], index, value);
            Trace("arrays-prerewrite") << "Arrays::preRewrite returning " << newNode << std::endl;
            return RewriteResponse(REWRITE_DONE, newNode);
          }
        }
        break;
      }
      case kind::EQUAL:
      case kind::IFF: {
        if(node[0] == node[1]) {
          Trace("arrays-prerewrite") << "Arrays::preRewrite returning true" << std::endl;
          return RewriteResponse(REWRITE_DONE, NodeManager::currentNM()->mkConst(true));
        }
        break;
      }
      default:
        break;
    }

    Trace("arrays-prerewrite") << "Arrays::preRewrite returning " << node << std::endl;
    return RewriteResponse(REWRITE_DONE, node);
  }

  static inline void init() {}
  static inline void shutdown() {}

};/* class TheoryArraysRewriter */

}/* CVC4::theory::arrays namespace */
}/* CVC4::theory namespace */
}/* CVC4 namespace */

#endif /* __CVC4__THEORY__ARRAYS__THEORY_ARRAYS_REWRITER_H */
