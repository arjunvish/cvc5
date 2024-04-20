/******************************************************************************
 * Top contributors (to current version):
 *   Aina Niemetz, Gereon Kremer
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2024 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Black box testing of the guards of the C++ API functions.
 */

#include "test_api.h"

namespace cvc5::internal {

namespace test {

class TestApiBlackGrammar : public TestApi
{
};

TEST_F(TestApiBlackGrammar, toString)
{
  d_solver->setOption("sygus", "true");
  Sort boolean = d_tm.getBooleanSort();
  Term start = d_tm.mkVar(boolean);
  Grammar g;
  g = d_solver->mkGrammar({}, {start});
  g.addRule(start, d_tm.mkBoolean(false));

  {
    std::stringstream ss;
    ss << g;
    ASSERT_EQ(ss.str(), g.toString());
  }
}

TEST_F(TestApiBlackGrammar, addRule)
{
  d_solver->setOption("sygus", "true");
  Sort boolean = d_tm.getBooleanSort();
  Sort integer = d_tm.getIntegerSort();

  Term nullTerm;
  Term start = d_tm.mkVar(boolean);
  Term nts = d_tm.mkVar(boolean);

  Grammar g = d_solver->mkGrammar({}, {start});

  ASSERT_NO_THROW(g.addRule(start, d_tm.mkBoolean(false)));

  ASSERT_THROW(g.addRule(nullTerm, d_tm.mkBoolean(false)), CVC5ApiException);
  ASSERT_THROW(g.addRule(start, nullTerm), CVC5ApiException);
  ASSERT_THROW(g.addRule(nts, d_tm.mkBoolean(false)), CVC5ApiException);
  ASSERT_THROW(g.addRule(start, d_tm.mkInteger(0)), CVC5ApiException);

  d_solver->synthFun("f", {}, boolean, g);

  ASSERT_THROW(g.addRule(start, d_tm.mkBoolean(false)), CVC5ApiException);
}

TEST_F(TestApiBlackGrammar, addRules)
{
  d_solver->setOption("sygus", "true");
  Sort boolean = d_tm.getBooleanSort();
  Sort integer = d_tm.getIntegerSort();

  Term nullTerm;
  Term start = d_tm.mkVar(boolean);
  Term nts = d_tm.mkVar(boolean);

  Grammar g = d_solver->mkGrammar({}, {start});

  ASSERT_NO_THROW(g.addRules(start, {d_tm.mkBoolean(false)}));

  ASSERT_THROW(g.addRules(nullTerm, {d_tm.mkBoolean(false)}), CVC5ApiException);
  ASSERT_THROW(g.addRules(start, {nullTerm}), CVC5ApiException);
  ASSERT_THROW(g.addRules(nts, {d_tm.mkBoolean(false)}), CVC5ApiException);
  ASSERT_THROW(g.addRules(start, {d_tm.mkInteger(0)}), CVC5ApiException);

  d_solver->synthFun("f", {}, boolean, g);

  ASSERT_THROW(g.addRules(start, {d_tm.mkBoolean(false)}), CVC5ApiException);
}

TEST_F(TestApiBlackGrammar, addAnyConstant)
{
  d_solver->setOption("sygus", "true");
  Sort boolean = d_tm.getBooleanSort();

  Term nullTerm;
  Term start = d_tm.mkVar(boolean);
  Term nts = d_tm.mkVar(boolean);

  Grammar g = d_solver->mkGrammar({}, {start});

  ASSERT_NO_THROW(g.addAnyConstant(start));
  ASSERT_NO_THROW(g.addAnyConstant(start));

  ASSERT_THROW(g.addAnyConstant(nullTerm), CVC5ApiException);
  ASSERT_THROW(g.addAnyConstant(nts), CVC5ApiException);

  d_solver->synthFun("f", {}, boolean, g);

  ASSERT_THROW(g.addAnyConstant(start), CVC5ApiException);
}

TEST_F(TestApiBlackGrammar, addAnyVariable)
{
  d_solver->setOption("sygus", "true");
  Sort boolean = d_tm.getBooleanSort();

  Term nullTerm;
  Term x = d_tm.mkVar(boolean);
  Term start = d_tm.mkVar(boolean);
  Term nts = d_tm.mkVar(boolean);

  Grammar g1 = d_solver->mkGrammar({x}, {start});
  Grammar g2 = d_solver->mkGrammar({}, {start});

  ASSERT_NO_THROW(g1.addAnyVariable(start));
  ASSERT_NO_THROW(g1.addAnyVariable(start));
  ASSERT_NO_THROW(g2.addAnyVariable(start));

  ASSERT_THROW(g1.addAnyVariable(nullTerm), CVC5ApiException);
  ASSERT_THROW(g1.addAnyVariable(nts), CVC5ApiException);

  d_solver->synthFun("f", {}, boolean, g1);

  ASSERT_THROW(g1.addAnyVariable(start), CVC5ApiException);
}
}  // namespace test
}  // namespace cvc5::internal
