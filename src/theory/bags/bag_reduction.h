/******************************************************************************
 * Top contributors (to current version):
 *   Mudathir Mohamed
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2022 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * bag reduction.
 */

#ifndef CVC5__BAG_REDUCTION_H
#define CVC5__BAG_REDUCTION_H

#include <vector>

#include "cvc5_private.h"
#include "smt/env_obj.h"
#include "theory/bags/inference_manager.h"

namespace cvc5::internal {
namespace theory {
namespace bags {

/**
 * class for bag reductions
 */
class BagReduction
{
 public:
  BagReduction();
  ~BagReduction();

  /**
   * @param node a term of the form (bag.fold f t A) where
   *        f: (-> T1 T2 T2) is a binary operation
   *        t: T2 is the initial value
   *        A: (Bag T1) is a bag
   * @param asserts a list of assertions generated by this reduction
   * @return the reduction term (combine n) with asserts:
   * - (forall ((i Int))
   *    (let ((iMinusOne (- i 1)))
   *      (let ((uf_i (uf i)))
   *        (=>
   *          (and (>= i 1) (<= i n))
   *          (and
   *            (= (combine i) (f uf_i (combine iMinusOne)))
   *            (=
   *              (unionDisjoint i)
   *              (bag.union_disjoint
   *                (bag uf_i 1)
   *                (unionDisjoint iMinusOne))))))))
   * - (= (combine 0) t)
   * - (= (unionDisjoint 0) (as bag.empty (Bag T1)))
   * - (= A (unionDisjoint n))
   * - (>= n 0))
   * where
   * n: Int is the cardinality of bag A
   * uf:Int -> T1 is an uninterpreted function that represents elements of A
   * combine: Int -> T2 is an uninterpreted function
   * unionDisjoint: Int -> (Bag T1) is an uninterpreted function
   */
  static Node reduceFoldOperator(Node node, std::vector<Node>& asserts);

  /**
   * @param node a term of the form (bag.card A) where A: (Bag T) is a bag
   * @param asserts a list of assertions generated by this reduction
   * @return the reduction term (cardinality n) with asserts:
   * - (forall ((i Int))
   *    (let ((uf_i (uf i)))
   *     (let ((count_uf_i (bag.count uf_i A)))
   *      (=>
   *       (and (>= i 1) (<= i n))
   *       (and
   *        (= (cardinality i) (+ count_uf_i (cardinality (- i 1))))
   *        (=
   *         (unionDisjoint i)
   *         (bag.union_disjoint (bag uf_i count_uf_i) (unionDisjoint (- i 1))))
   *        (forall ((j Int))
   *                (or (not (and (< i j) (<= j n)))
   *                    (not (= (uf i) (uf j))))))))))
   * - (= (cardinality 0) 0)
   * - (= (unionDisjoint 0) (as bag.empty (Bag String)))
   * - (= A (unionDisjoint n))
   * - (>= n 0)
   *
   * where
   *   n: number of distinct elements in A
   *   uf:Int -> T is an uninterpreted function that represents distinct
   *             elements of A
   *   cardinality: Int -> Int is an uninterpreted function
   *   unionDisjoint: Int -> (Bag T1) is an uninterpreted function
   */
  static Node reduceCardOperator(Node node, std::vector<Node>& asserts);
  /**
   * @param node of the form ((_ table.aggr n1 ... nk) f initial A))
   * @return reduction term that uses map, fold, and group operators
   * as follows:
   * (bag.map
   *   (lambda ((B Table)) (bag.fold f initial B))
   *   ((_ table.group n1 ... nk) A))
   */
  static Node reduceAggregateOperator(Node node);
  /**
   * @param n has the form ((table.project n1 ... nk) A) where A has type
   *          (Bag T)
   * @return (bag.map (lambda ((t T)) ((_ tuple.project n1 ... nk) t)) A)
   */
  static Node reduceProjectOperator(Node n);
};

}  // namespace bags
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__BAG_REDUCTION_H */
