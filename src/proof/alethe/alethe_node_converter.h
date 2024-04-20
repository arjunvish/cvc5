/******************************************************************************
 * Top contributors (to current version):
 *   Haniel Barbosa
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2024 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Alethe node conversion
 */

#include "cvc5_private.h"

#ifndef CVC5__PROOF__ALETHE__ALETHE_NODE_CONVERTER_H
#define CVC5__PROOF__ALETHE__ALETHE_NODE_CONVERTER_H

#include "expr/node.h"
#include "expr/node_converter.h"
#include "proof/alf/alf_node_converter.h"

namespace cvc5::internal {
namespace proof {

/**
 * This is a helper class for the Alethe post-processor that converts nodes into
 * the form that Alethe expects.
 */
class AletheNodeConverter : public BaseAlfNodeConverter
{
 public:

  AletheNodeConverter(NodeManager* nm, bool defineSkolems = false)
    : BaseAlfNodeConverter(nm), d_defineSkolems(defineSkolems)
  {
  }
  ~AletheNodeConverter() {}

  /** convert at post-order traversal */
  Node postConvert(Node n) override;

  Node maybeConvert(Node n);

  std::string d_error;

  std::map<Node, Node> d_convToOriginalAssumption;

  std::map<Node, Node> d_skolems;

  Node getOperatorOfTerm(Node n, bool reqCast = false) override
  {
    return Node::null();
  };
  Node typeAsNode(TypeNode tni) override { return Node::null(); };

  Node mkInternalSymbol(const std::string& name,
                        TypeNode tn,
                        bool useRawSym = true) override;

  Node mkInternalApp(const std::string& name,
                     const std::vector<Node>& args,
                     TypeNode ret,
                     bool useRawSym = true) override
  {
    return Node::null();
  };

 private:
  bool d_defineSkolems;
  std::map<Node, Node> d_skolemsAux;

  /**
   * As above but uses the s-expression type.
   */
  Node mkInternalSymbol(const std::string& name);

  /** Maps from internally generated symbols to the built nodes. */
  std::map<std::pair<TypeNode, std::string>, Node> d_symbolsMap;
};

}  // namespace proof
}  // namespace cvc5::internal

#endif
