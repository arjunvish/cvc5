/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds, Andres Noetzli, Aina Niemetz
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

#include "smt/smt_solver.h"

#include "options/base_options.h"
#include "options/main_options.h"
#include "options/smt_options.h"
#include "prop/prop_engine.h"
#include "smt/assertions.h"
#include "smt/env.h"
#include "smt/logic_exception.h"
#include "smt/preprocessor.h"
#include "smt/solver_engine.h"
#include "smt/solver_engine_state.h"
#include "smt/solver_engine_stats.h"
#include "theory/logic_info.h"
#include "theory/theory_engine.h"
#include "theory/theory_traits.h"

using namespace std;

namespace cvc5::internal {
namespace smt {

SmtSolver::SmtSolver(Env& env,
                     AbstractValues& abs,
                     Assertions& asserts,
                     SolverEngineStatistics& stats)
    : EnvObj(env),
      d_pp(env, abs, stats),
      d_asserts(asserts),
      d_stats(stats),
      d_theoryEngine(nullptr),
      d_propEngine(nullptr)
{
}

SmtSolver::~SmtSolver() {}

void SmtSolver::finishInit()
{
  // We have mutual dependency here, so we add the prop engine to the theory
  // engine later (it is non-essential there)
  d_theoryEngine.reset(new TheoryEngine(d_env));

  // Add the theories
  for (theory::TheoryId id = theory::THEORY_FIRST; id < theory::THEORY_LAST;
       ++id)
  {
    theory::TheoryConstructor::addTheory(d_theoryEngine.get(), id);
  }
  // Add the proof checkers for each theory
  ProofNodeManager* pnm = d_env.getProofNodeManager();
  if (pnm)
  {
    // reset the rule checkers
    pnm->getChecker()->reset();
    // add rule checkers from the theory engine
    d_theoryEngine->initializeProofChecker(pnm->getChecker());
  }
  Trace("smt-debug") << "Making prop engine..." << std::endl;
  /* force destruction of referenced PropEngine to enforce that statistics
   * are unregistered by the obsolete PropEngine object before registered
   * again by the new PropEngine object */
  d_propEngine.reset(nullptr);
  d_propEngine.reset(new prop::PropEngine(d_env, d_theoryEngine.get()));

  Trace("smt-debug") << "Setting up theory engine..." << std::endl;
  d_theoryEngine->setPropEngine(getPropEngine());
  Trace("smt-debug") << "Finishing init for theory engine..." << std::endl;
  d_theoryEngine->finishInit();
  d_propEngine->finishInit();

  d_pp.finishInit(d_theoryEngine.get(), d_propEngine.get());
}

void SmtSolver::resetAssertions()
{
  /* Create new PropEngine.
   * First force destruction of referenced PropEngine to enforce that
   * statistics are unregistered by the obsolete PropEngine object before
   * registered again by the new PropEngine object */
  d_propEngine.reset(nullptr);
  d_propEngine.reset(new prop::PropEngine(d_env, d_theoryEngine.get()));
  d_theoryEngine->setPropEngine(getPropEngine());
  // Notice that we do not reset TheoryEngine, nor does it require calling
  // finishInit again. In particular, TheoryEngine::finishInit does not
  // depend on knowing the associated PropEngine.
  d_propEngine->finishInit();
  // must reset the preprocessor as well
  d_pp.finishInit(d_theoryEngine.get(), d_propEngine.get());
}

void SmtSolver::interrupt()
{
  if (d_propEngine != nullptr)
  {
    d_propEngine->interrupt();
  }
  if (d_theoryEngine != nullptr)
  {
    d_theoryEngine->interrupt();
  }
}
Result SmtSolver::checkSatisfiability(const std::vector<Node>& assumptions)
{
  return checkSatisfiability(d_asserts, assumptions);
}

Result SmtSolver::checkSatisfiability(Assertions& as,
                                      const std::vector<Node>& assumptions)
{
  Result result;
  try
  {
    // then, initialize the assertions
    as.setAssumptions(assumptions);

    // make the check, where notice smt engine should be fully inited by now

    Trace("smt") << "SmtSolver::check()" << endl;

    ResourceManager* rm = d_env.getResourceManager();
    if (rm->out())
    {
      UnknownExplanation why = rm->outOfResources()
                                   ? UnknownExplanation::RESOURCEOUT
                                   : UnknownExplanation::TIMEOUT;
      result = Result(Result::UNKNOWN, why);
    }
    else
    {
      rm->beginCall();

      // Preprocess the assertions
      Trace("smt") << "SmtSolver::check(): preprocess" << endl;
      preprocess(as);
      Trace("smt") << "SmtSolver::check(): done preprocess" << endl;

      // Make sure the prop layer has all of the assertions
      Trace("smt") << "SmtSolver::check(): assert to internal" << endl;
      assertToInternal(as);
      Trace("smt") << "SmtSolver::check(): done assert to internal" << endl;

      d_env.verbose(2) << "solving..." << std::endl;
      Trace("smt") << "SmtSolver::check(): running check" << endl;
      result = checkSatInternal();
      Trace("smt") << "SmtSolver::check(): result " << result << std::endl;

      rm->endCall();
      Trace("limit") << "SmtSolver::check(): cumulative millis "
                     << rm->getTimeUsage() << ", resources "
                     << rm->getResourceUsage() << endl;
    }
  }
  catch (const LogicException& e)
  {
    // The exception may have been throw during solving, backtrack to reset the
    // decision level to the level expected after this method finishes
    getPropEngine()->resetTrail();
    throw;
  }

  return result;
}

Result SmtSolver::checkSatInternal()
{
  // call the prop engine to check sat
  Result result = d_propEngine->checkSat();
  // handle options-specific modifications to result
  if ((options().smt.solveRealAsInt || options().smt.solveIntAsBV > 0)
      && result.getStatus() == Result::UNSAT)
  {
    result = Result(Result::UNKNOWN, UnknownExplanation::UNKNOWN_REASON);
  }
  // handle preprocessing-specific modifications to result
  if (options().quantifiers.globalNegate)
  {
    Trace("smt") << "SmtSolver::process global negate " << result << std::endl;
    if (result.getStatus() == Result::UNSAT)
    {
      result = Result(Result::SAT);
    }
    else if (result.getStatus() == Result::SAT)
    {
      // Only can answer unsat if the theory is satisfaction complete. This
      // includes linear arithmetic and bitvectors, which are the primary
      // targets for the global negate option. Other logics are possible
      // here but not considered.
      LogicInfo logic = logicInfo();
      if ((logic.isPure(theory::THEORY_ARITH) && logic.isLinear())
          || logic.isPure(theory::THEORY_BV))
      {
        result = Result(Result::UNSAT);
      }
      else
      {
        result = Result(Result::UNKNOWN, UnknownExplanation::UNKNOWN_REASON);
      }
    }
    Trace("smt") << "SmtSolver::global negate returned " << result << std::endl;
  }
  return result;
}

void SmtSolver::processAssertions(Assertions& as)
{
  // preprocess
  preprocess(as);
  // assert to internal
  assertToInternal(as);
}

void SmtSolver::preprocess(Assertions& as)
{
  TimerStat::CodeTimer paTimer(d_stats.d_processAssertionsTime);
  d_env.getResourceManager()->spendResource(Resource::PreprocessStep);

  // process the assertions with the preprocessor
  d_pp.process(as);

  // end: INVARIANT to maintain: no reordering of assertions or
  // introducing new ones

  preprocessing::AssertionPipeline& ap = as.getAssertionPipeline();
  const std::vector<Node>& assertions = ap.ref();
  // It is important to distinguish the input assertions from the skolem
  // definitions, as the decision justification heuristic treates the latter
  // specially. Note that we don't pass the preprocess learned literals
  // d_pp.getLearnedLiterals() here, since they may not exactly correspond
  // to the actual preprocessed learned literals, as the input may have
  // undergone further preprocessing.
  preprocessing::IteSkolemMap& ism = ap.getIteSkolemMap();
  // if we can deep restart, we always remember the preprocessed formulas,
  // which are the basis for the next check-sat.
  if (trackPreprocessedAssertions())
  {
    // incompatible with global negation
    Assert(!options().quantifiers.globalNegate);
    theory::SubstitutionMap& sm = d_env.getTopLevelSubstitutions().get();
    // note that if a skolem is eliminated in preprocessing, we remove it
    // from the preprocessed skolem map
    std::vector<size_t> elimSkolems;
    for (const std::pair<const size_t, Node>& k : d_ppSkolemMap)
    {
      if (sm.hasSubstitution(k.second))
      {
        Trace("deep-restart-ism")
            << "SKOLEM:" << k.second << " was eliminated during preprocessing"
            << std::endl;
        elimSkolems.push_back(k.first);
        continue;
      }
      Trace("deep-restart-ism") << "SKOLEM:" << k.second << " is skolem for "
                                << assertions[k.first] << std::endl;
    }
    for (size_t i : elimSkolems)
    {
      ism.erase(i);
    }
    // remember the assertions and Skolem mapping
    d_ppAssertions = assertions;
    d_ppSkolemMap = ism;
  }
}

void SmtSolver::assertToInternal(Assertions& as)
{
  // get the assertions
  preprocessing::AssertionPipeline& ap = as.getAssertionPipeline();
  const std::vector<Node>& assertions = ap.ref();
  preprocessing::IteSkolemMap& ism = ap.getIteSkolemMap();
  // assert to prop engine, which will convert to CNF
  d_env.verbose(2) << "converting to CNF..." << endl;
  d_propEngine->assertInputFormulas(assertions, ism);
  // clear the current assertions
  as.clearCurrent();
}

const std::vector<Node>& SmtSolver::getPreprocessedAssertions() const
{
  return d_ppAssertions;
}

const std::unordered_map<size_t, Node>& SmtSolver::getPreprocessedSkolemMap()
    const
{
  return d_ppSkolemMap;
}

void SmtSolver::deepRestart(Assertions& asr, const std::vector<Node>& zll)
{
  Assert(trackPreprocessedAssertions());
  Assert(!zll.empty());
  Trace("deep-restart") << "Have " << zll.size()
                        << " zero level learned literals" << std::endl;

  preprocessing::AssertionPipeline& apr = asr.getAssertionPipeline();
  // Copy the preprocessed assertions and skolem map information directly
  for (const Node& a : d_ppAssertions)
  {
    apr.push_back(a);
  }
  preprocessing::IteSkolemMap& ismr = apr.getIteSkolemMap();
  for (const std::pair<const size_t, Node>& k : d_ppSkolemMap)
  {
    // carry the entire skolem map, which should align with the order of
    // assertions passed into the new assertions pipeline
    ismr[k.first] = k.second;
  }

  if (isOutputOn(OutputTag::DEEP_RESTART))
  {
    output(OutputTag::DEEP_RESTART) << "(deep-restart (";
    bool firstTime = true;
    for (TNode lit : zll)
    {
      output(OutputTag::DEEP_RESTART) << (firstTime ? "" : " ") << lit;
      firstTime = false;
    }
    output(OutputTag::DEEP_RESTART) << "))" << std::endl;
  }
  for (TNode lit : zll)
  {
    Trace("deep-restart-lit") << "Restart learned lit: " << lit << std::endl;
    apr.push_back(lit);
    if (Configuration::isAssertionBuild())
    {
      Assert(d_allLearnedLits.find(lit) == d_allLearnedLits.end())
          << "Relearned: " << lit << std::endl;
      d_allLearnedLits.insert(lit);
    }
  }
  Trace("deep-restart") << "Finished compute deep restart" << std::endl;

  // we now finish init to reconstruct prop engine and theory engine
  finishInit();
}

bool SmtSolver::trackPreprocessedAssertions() const
{
  return options().smt.deepRestartMode != options::DeepRestartMode::NONE
         || options().smt.produceProofs;
}

TheoryEngine* SmtSolver::getTheoryEngine() { return d_theoryEngine.get(); }

prop::PropEngine* SmtSolver::getPropEngine() { return d_propEngine.get(); }

theory::QuantifiersEngine* SmtSolver::getQuantifiersEngine()
{
  Assert(d_theoryEngine != nullptr);
  return d_theoryEngine->getQuantifiersEngine();
}

Preprocessor* SmtSolver::getPreprocessor() { return &d_pp; }

void SmtSolver::notifyPushPre()
{
  // must preprocess the assertions and push them to the SAT solver, to make
  // the state accurate prior to pushing
  processAssertions(d_asserts);
}

void SmtSolver::notifyPushPost()
{
  TimerStat::CodeTimer pushPopTimer(d_stats.d_pushPopTime);
  Assert(d_propEngine != nullptr);
  d_propEngine->push();
}

void SmtSolver::notifyPopPre()
{
  TimerStat::CodeTimer pushPopTimer(d_stats.d_pushPopTime);
  Assert(d_propEngine != nullptr);
  d_propEngine->pop();
}

void SmtSolver::notifyPostSolve()
{
  Assert(d_propEngine != nullptr);
  d_propEngine->resetTrail();

  Assert(d_theoryEngine != nullptr);
  d_theoryEngine->postsolve();
}

}  // namespace smt
}  // namespace cvc5::internal
