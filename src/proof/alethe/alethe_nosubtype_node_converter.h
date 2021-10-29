/******************************************************************************
 * Top contributors (to current version):
 *   Haniel Barbosa
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Alethe node conversion to remove subtyping
 */

#include "cvc5_private.h"

#ifndef CVC4__PROOF__ALETHE__ALETHE_NOSUBTYPE_NODE_CONVERTER_H
#define CVC4__PROOF__ALETHE__ALETHE_NOSUBTYPE_NODE_CONVERTER_H

#include "expr/node.h"
#include "expr/node_converter.h"

namespace cvc5 {
namespace proof {

/**
 * This is a helper class for the Alethe post-processor that converts nodes into
 * form that Alethe expects.
 */
class AletheNoSubtypeNodeConverter : public NodeConverter
{
 public:
  AletheNoSubtypeNodeConverter() {}
  ~AletheNoSubtypeNodeConverter() {}
  /** Convert by removing attributes of quantifiers. */
  Node postConvert(Node n) override;
};

}  // namespace proof
}  // namespace cvc5

#endif
