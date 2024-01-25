/******************************************************************************
 * Top contributors (to current version):
 *   Andres Noetzli, Andrew Reynolds, Morgan Deters
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2023 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * AssertionPipeline stores a list of assertions modified by
 * preprocessing passes.
 */

#include "cvc5_private.h"

#ifndef CVC5__PREPROCESSING__ASSERTION_PIPELINE_H
#define CVC5__PREPROCESSING__ASSERTION_PIPELINE_H

#include <vector>

#include "expr/node.h"
#include "proof/lazy_proof.h"
#include "proof/trust_node.h"
#include "smt/env_obj.h"

namespace cvc5::internal {

class ProofGenerator;
namespace smt {
class PreprocessProofGenerator;
}

namespace preprocessing {

using IteSkolemMap = std::unordered_map<size_t, Node>;

/**
 * Assertion Pipeline stores a list of assertions modified by preprocessing
 * passes. It is assumed that all assertions after d_realAssertionsEnd were
 * generated by ITE removal. Hence, d_iteSkolemMap maps into only these.
 */
class AssertionPipeline : protected EnvObj
{
 public:
  AssertionPipeline(Env& env);

  size_t size() const { return d_nodes.size(); }

  void resize(size_t n) { d_nodes.resize(n); }

  /**
   * Clear the list of assertions and assumptions.
   */
  void clear();

  const Node& operator[](size_t i) const { return d_nodes[i]; }

  /**
   * Adds an assertion/assumption to be preprocessed.
   *
   * @param n The assertion/assumption
   * @param isInput If true, n is an input formula (an assumption in the main
   * body of the overall proof).
   * @param pg The proof generator who can provide a proof of n. The proof
   * generator is not required and is ignored if isInput is true.
   */
  void push_back(Node n,
                 bool isInput = false,
                 ProofGenerator* pg = nullptr);
  /** Same as above, with TrustNode */
  void pushBackTrusted(TrustNode trn);

  /**
   * Get the constant reference to the underlying assertions. It is only
   * possible to modify these via the replace methods below.
   */
  const std::vector<Node>& ref() const { return d_nodes; }

  std::vector<Node>::const_iterator begin() const { return d_nodes.cbegin(); }
  std::vector<Node>::const_iterator end() const { return d_nodes.cend(); }

  /*
   * Replaces assertion i with node n and records the dependency between the
   * original assertion and its replacement.
   *
   * @param i The position of the assertion to replace.
   * @param n The replacement assertion.
   * @param pg The proof generator who can provide a proof of d_nodes[i] == n,
   * where d_nodes[i] is the assertion at position i prior to this call.
   */
  void replace(size_t i, Node n, ProofGenerator* pg = nullptr);
  /**
   * Same as above, with TrustNode trn, which is of kind REWRITE and proves
   * d_nodes[i] = n for some n.
   */
  void replaceTrusted(size_t i, TrustNode trn);

  IteSkolemMap& getIteSkolemMap() { return d_iteSkolemMap; }
  const IteSkolemMap& getIteSkolemMap() const { return d_iteSkolemMap; }

  /**
   * Returns true if substitutions must be stored as assertions. This is for
   * example the case when we do incremental solving.
   */
  bool storeSubstsInAsserts() { return d_storeSubstsInAsserts; }

  /**
   * Enables storing substitutions as assertions.
   */
  void enableStoreSubstsInAsserts();

  /**
   * Disables storing substitutions as assertions.
   */
  void disableStoreSubstsInAsserts();

  /**
   * Adds a substitution node of the form (= lhs rhs) to the assertions.
   * This conjoins n to assertions at a distinguished index given by
   * d_substsIndex.
   *
   * @param n The substitution node
   * @param pg The proof generator that can provide a proof of n.
   */
  void addSubstitutionNode(Node n, ProofGenerator* pg = nullptr);

  /**
   * Checks whether the assertion at a given index represents substitutions.
   *
   * @param i The index in question
   */
  bool isSubstsIndex(size_t i) const;
  /** Is in conflict? True if this pipeline contains the false assertion */
  bool isInConflict() const { return d_conflict; }
  /** Is refutation unsound? */
  bool isRefutationUnsound() const { return d_isRefutationUnsound; }
  /** Is model unsound? */
  bool isModelUnsound() const { return d_isModelUnsound; }
  /** Is negated? */
  bool isNegated() const { return d_isNegated; }
  /** mark refutation unsound */
  void markRefutationUnsound();
  /** mark model unsound */
  void markModelUnsound();
  /** mark negated */
  void markNegated();
  //------------------------------------ for proofs
  /**
   * Enable proofs for this assertions pipeline. This must be called
   * explicitly since we construct the assertions pipeline before we know
   * whether proofs are enabled.
   *
   * @param pppg The preprocess proof generator of the proof manager.
   */
  void enableProofs(smt::PreprocessProofGenerator* pppg);
  /** Is proof enabled? */
  bool isProofEnabled() const;
  //------------------------------------ end for proofs
 private:
  /** Set that we are in conflict */
  void markConflict();
  /** Boolean constants */
  Node d_true;
  Node d_false;
  /** The list of current assertions */
  std::vector<Node> d_nodes;

  /**
   * Map from skolem variables to index in d_assertions containing
   * corresponding introduced Boolean ite
   */
  IteSkolemMap d_iteSkolemMap;

  /** Size of d_nodes when preprocessing starts */
  size_t d_realAssertionsEnd;

  /**
   * If true, we store the substitutions as assertions. This is necessary when
   * doing incremental solving because we cannot apply them to existing
   * assertions while preprocessing new assertions.
   */
  bool d_storeSubstsInAsserts;

  /**
   * The index of the assertions that holds the substitutions.
   *
   * TODO(#2473): replace by separate vector of substitution assertions.
   */
  std::unordered_set<size_t> d_substsIndices;

  /** Index of the first assumption */
  size_t d_assumptionsStart;
  /** The number of assumptions */
  size_t d_numAssumptions;
  /** The proof generator, if one is provided */
  smt::PreprocessProofGenerator* d_pppg;
  /** Are we in conflict? */
  bool d_conflict;
  /** Is refutation unsound? */
  bool d_isRefutationUnsound;
  /** Is model unsound? */
  bool d_isModelUnsound;
  /** Is negated? */
  bool d_isNegated;
  /**
   * Maintains proofs for eliminating top-level AND from inputs to this class.
   */
  std::unique_ptr<LazyCDProof> d_andElimEpg;
}; /* class AssertionPipeline */

}  // namespace preprocessing
}  // namespace cvc5::internal

#endif /* CVC5__PREPROCESSING__ASSERTION_PIPELINE_H */
