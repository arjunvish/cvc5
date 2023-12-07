/******************************************************************************
 * Top contributors (to current version):
 *   Alex Ozdemir
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2023 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Multivariate root finding. Implements "FindZero" from [OKTB23].
 *
 * [OKTB23]: https://doi.org/10.1007/978-3-031-37703-7_8
 */

#include "cvc5_private.h"

#ifdef CVC5_USE_COCOA

#ifndef CVC5__THEORY__FF__MULTI_ROOTS_H
#define CVC5__THEORY__FF__MULTI_ROOTS_H

#include <CoCoA/BigInt.H>
#include <CoCoA/ideal.H>
#include <CoCoA/ring.H>

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "expr/node.h"

namespace cvc5::internal {
namespace theory {
namespace ff {

/**
 * Find a common zero for all poynomials in this ideal. Figure 5 from [OKTB23].
 */
std::vector<CoCoA::RingElem> findZero(const CoCoA::ideal& ideal);

/**
 * Enumerates **assignment**s: monic, degree-one, univariate polynomials.
 */
class AssignmentEnumerator
{
 public:
  virtual ~AssignmentEnumerator();
  /**
   * Return the next assignment, or an empty option.
   */
  virtual std::optional<CoCoA::RingElem> next() = 0;
  /**
   * get the name of this enumerator
   */
  virtual std::string name() = 0;
};

/**
 * Guess a prescribed set of values for a variable.
 */
class ListEnumerator : public AssignmentEnumerator
{
 public:
  ListEnumerator(const std::vector<CoCoA::RingElem>&& options);
  ~ListEnumerator() override;
  std::optional<CoCoA::RingElem> next() override;
  std::string name() override;

 private:
  std::vector<CoCoA::RingElem> d_remainingOptions;
};

/**
 * Return a list enumerator whose elements are (real) factors of this
 * polynomial.
 */
std::unique_ptr<ListEnumerator> factorEnumerator(
    CoCoA::RingElem univariatePoly);

/**
 * Guess all values for all variables, in a round robin. Only works for a prime
 * field (order p):
 *
 * * v0: 0
 * * v1: 0
 * * ...
 * * vN: 0
 * * v0: 1
 * * v1: 1
 * * ...
 * * vN: 1
 * * .....
 * * v0: p-1
 * * v1: p-1
 * * ...
 * * vN: p-1
 */
class RoundRobinEnumerator : public AssignmentEnumerator
{
 public:
  RoundRobinEnumerator(const std::vector<CoCoA::RingElem>& vars,
                       const CoCoA::ring& ring);
  ~RoundRobinEnumerator() override;
  std::optional<CoCoA::RingElem> next() override;
  std::string name() override;

 private:
  const std::vector<CoCoA::RingElem> d_vars;
  const CoCoA::ring d_ring;
  CoCoA::BigInt d_idx;
  CoCoA::BigInt d_maxIdx;
};

/**
 * Is this ideal the whole ring?
 *
 * That is, does this ideal contain all polynomials, including all non-zero
 * constant polynomials (which have no zeros).
 *
 * This is a sound, but incomplete test for there being no common zeros (the
 * system being UNSAT).
 *
 * For an example of incompleteness, the ideal generated by just X^2+1 is not
 * the whole ring, but has no zeros in the field of order three.
 */
bool isUnsat(const CoCoA::ideal& ideal);

/**
 * Given a univariate linear polynomial, extract the assignment that satisfies
 * it.
 *
 * An assignment is a variable number and a coefficient ring value.
 */
std::pair<size_t, CoCoA::RingElem> extractAssignment(
    const CoCoA::RingElem& elem);

/**
 * Which variables are assigned? Represented as strings.
 */
std::unordered_set<std::string> assignedVars(const CoCoA::ideal& ideal);

/**
 * Are all variables assigned?
 */
bool allVarsAssigned(const CoCoA::ideal& ideal);

/**
 * Apply a branching rule (Figure 6 of [OKTB23]). Returns an assignment
 * enumerator.
 *
 * Cases:
 * * if the basis has a univariate, super-linear poly, enumerate its roots
 * * if the variety has dimension 0, construct a minimal poly an enumerate its
 *   roots
 * * Otherwise, do round-robin guessing
 */
std::unique_ptr<AssignmentEnumerator> applyRule(const CoCoA::ideal& ideal);

}  // namespace ff
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__THEORY__FF__MULTI_ROOTS_H */

#endif /* CVC5_USE_COCOA */
