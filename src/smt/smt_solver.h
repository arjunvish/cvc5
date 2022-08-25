/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds, Aina Niemetz, Morgan Deters
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2022 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * The solver for SMT queries in an SolverEngine.
 */

#include "cvc5_private.h"

#ifndef CVC5__SMT__SMT_SOLVER_H
#define CVC5__SMT__SMT_SOLVER_H

#include <vector>

#include "expr/node.h"
#include "smt/assertions.h"
#include "smt/env_obj.h"
#include "smt/preprocessor.h"
#include "theory/logic_info.h"
#include "util/result.h"

namespace cvc5::internal {

class SolverEngine;
class Env;
class TheoryEngine;
class ResourceManager;
class ProofNodeManager;

namespace prop {
class PropEngine;
}

namespace theory {
class QuantifiersEngine;
}

namespace smt {

class SolverEngineState;
struct SolverEngineStatistics;

/**
 * A solver for SMT queries.
 *
 * This class manages the initialization of the theory engine and propositional
 * engines and implements the method for checking satisfiability of the current
 * set of assertions.
 *
 * Notice that this class is only conceptually responsible for running
 * check-sat commands and an interface for sending formulas to the underlying
 * classes. It does not implement any query techniques beyond getting the result
 * (unsat/sat/unknown) of check-sat calls. More detailed information (e.g.
 * models) can be queries using other classes that examine the state of the
 * TheoryEngine directly, which can be accessed via getTheoryEngine.
 */
class SmtSolver : protected EnvObj
{
 public:
  SmtSolver(Env& env,
            AbstractValues& abs,
            Assertions& asserts,
            SolverEngineStatistics& stats);
  ~SmtSolver();
  /**
   * Create theory engine, prop engine based on the environment.
   */
  void finishInit();
  /** Reset all assertions, global declarations, etc.  */
  void resetAssertions();
  /**
   * Interrupt a running query.  This can be called from another thread
   * or from a signal handler.  Throws a ModalException if the SmtSolver
   * isn't currently in a query.
   */
  void interrupt();
  /**
   * Check satisfiability for the given assumptions and the current assertions.
   *
   * @param assumptions The assumptions for this check-sat call, which are
   * temporary assertions.
   */
  Result checkSatisfiability(const std::vector<Node>& assumptions);
  /**
   * Process the assertions that have been asserted in as. This moves the set of
   * assertions that have been buffered into as, preprocesses them, pushes them
   * into the SMT solver, and clears the buffer.
   */
  void processAssertions(Assertions& as);
  /**
   * Get the list of preprocessed assertions. Only valid if
   * trackPreprocessedAssertions is true.
   */
  const std::vector<Node>& getPreprocessedAssertions() const;
  /**
   * Get the skolem map corresponding to the preprocessed assertions. Only valid
   * if trackPreprocessedAssertions is true.
   */
  const std::unordered_map<size_t, Node>& getPreprocessedSkolemMap() const;
  /**
   * Perform a deep restart.
   *
   * This constructs a fresh copy of the theory engine and prop engine, and
   * populates the given assertions for the next call to checkSatisfiability.
   * In particular, we add the preprocessed assertions from the previous
   * call to checkSatisfiability, as well as those in zll.
   *
   * @param as The assertions to populate
   * @param zll The zero-level literals we learned on the previous call to
   * checkSatisfiability.
   */
  void deepRestart(Assertions& as, const std::vector<Node>& zll);
  // --------------------------------------- callbacks from the context manager
  /**
   * Notify push pre, which is called just before the user context of the state
   * pushes. This processes all pending assertions.
   */
  void notifyPushPre();
  /**
   * Notify push post, which is called just after the user context of the state
   * pushes. This performs a push on the underlying prop engine.
   */
  void notifyPushPost();
  /**
   * Notify pop pre, which is called just before the user context of the state
   * pops. This performs a pop on the underlying prop engine.
   */
  void notifyPopPre();
  /**
   * Notify post solve, which is called once per check-sat query. It is
   * triggered when the first d_state.doPendingPops() is issued after the
   * check-sat. This calls the postsolve method of the underlying TheoryEngine.
   */
  void notifyPostSolve();
  // ----------------------------------- end callbacks from the context manager
  //------------------------------------------ access methods
  /** Get a pointer to the TheoryEngine owned by this solver. */
  TheoryEngine* getTheoryEngine();
  /** Get a pointer to the PropEngine owned by this solver. */
  prop::PropEngine* getPropEngine();
  /** Get a pointer to the QuantifiersEngine owned by this solver. */
  theory::QuantifiersEngine* getQuantifiersEngine();
  /** Get a pointer to the preprocessor */
  Preprocessor* getPreprocessor();
  //------------------------------------------ end access methods

 private:
  /**
   * Check satisfiability for the given assertions object and assumptions.
   */
  Result checkSatisfiability(Assertions& as,
                             const std::vector<Node>& assumptions);
  /**
   * Preprocess the assertions. This calls the preprocessor on the assertions
   * and sets d_ppAssertions / d_ppSkolemMap if necessary.
   */
  void preprocess(Assertions& as);
  /**
   * Push the assertions to the prop engine. Assumes that as has been
   * preprocessed. This pushes the assertions in as into the prop engine of
   * this solver and subsequently clears as.
   */
  void assertToInternal(Assertions& as);
  /**
   * Check satisfiability based on the current state of the prop engine.
   * This assumes we have pushed the necessary assertions to it. It post
   * processes the results based on the options.
   */
  Result checkSatInternal();
  /** Whether we track information necessary for deep restarts */
  bool trackPreprocessedAssertions() const;
  /** The preprocessor of this SMT solver */
  Preprocessor d_pp;
  /** The assertions of the parent solver engine */
  Assertions& d_asserts;
  /** Reference to the statistics of SolverEngine */
  SolverEngineStatistics& d_stats;
  /** The theory engine */
  std::unique_ptr<TheoryEngine> d_theoryEngine;
  /** The propositional engine */
  std::unique_ptr<prop::PropEngine> d_propEngine;
  //------------------------------------------ Bookkeeping for deep restarts
  /** The exact list of preprocessed assertions we sent to the PropEngine */
  std::vector<Node> d_ppAssertions;
  /** The skolem map associated with d_ppAssertions */
  std::unordered_map<size_t, Node> d_ppSkolemMap;
  /** All learned literals, used for debugging */
  std::unordered_set<Node> d_allLearnedLits;
};

}  // namespace smt
}  // namespace cvc5::internal

#endif /* CVC5__SMT__SMT_SOLVER_H */
