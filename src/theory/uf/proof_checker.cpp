/*********************                                                        */
/*! \file proof_checker.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Implementation of equality proof checker
 **/

#include "theory/uf/proof_checker.h"

//#include "theory/builtin/proof_checker.h"

using namespace CVC4::kind;

namespace CVC4 {
namespace theory {
namespace eq {

Node EqProofRuleChecker::check(PfRule id,
                               const std::vector<Node>& children,
                               const std::vector<Node>& args)
{
  // compute what was proven
  if (id == PfRule::REFL)
  {
    Assert(children.empty());
    Assert(args.size() == 1);
    return args[0].eqNode(args[0]);
  }
  else if (id == PfRule::SYMM)
  {
    Assert(children.size() == 1);
    Assert(args.empty());
    Node eqp = children[0];
    if (eqp.isNull() || eqp.getKind() != EQUAL)
    {
      return Node::null();
    }
    return eqp[1].eqNode(eqp[0]);
  }
  else if (id == PfRule::TRANS)
  {
    Assert(children.size() > 0);
    Assert(args.empty());
    Node first;
    Node curr;
    for (size_t i = 0, nchild = children.size(); i < nchild; i++)
    {
      Node eqp = children[i];
      if (eqp.isNull() || eqp.getKind() != EQUAL)
      {
        return Node::null();
      }
      if (first.isNull())
      {
        first = eqp[0];
      }
      else if (eqp[0] != curr)
      {
        return Node::null();
      }
      curr = eqp[1];
    }
    return first.eqNode(curr);
  }
  else if (id == PfRule::SPLIT)
  {
    Assert(children.empty());
    Assert(args.size() == 1);
    return NodeManager::currentNM()->mkNode(OR, args[0], args[0].notNode());
  }
  // no rule
  return Node::null();
}

}  // namespace eq
}  // namespace theory
}  // namespace CVC4
