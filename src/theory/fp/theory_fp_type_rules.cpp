/******************************************************************************
 * Top contributors (to current version):
 *   Aina Niemetz, Martin Brain, Tim King
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2022 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Type rules for the theory of floating-point arithmetic.
 */

#include "theory/fp/theory_fp_type_rules.h"

// This is only needed for checking that components are only applied to leaves.
#include "theory/fp/theory_fp_utils.h"
#include "theory/theory.h"
#include "util/cardinality.h"
#include "util/floatingpoint.h"
#include "util/roundingmode.h"

namespace cvc5::internal {
namespace theory {
namespace fp {

#define TRACE(FUNCTION)                                                \
  Trace("fp-type") << FUNCTION "::computeType(" << check << "): " << n \
                   << std::endl

TypeNode FloatingPointConstantTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointConstantTypeRule::computeType(NodeManager* nodeManager,
                                                    TNode n,
                                                    bool check,
                                                    std::ostream* errOut)
{
  TRACE("FloatingPointConstantTypeRule");

  const FloatingPoint& f = n.getConst<FloatingPoint>();

  if (check)
  {
    if (!(validExponentSize(f.getSize().exponentWidth())))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "constant with invalid exponent size");
      return TypeNode::null();
    }
    if (!(validSignificandSize(f.getSize().significandWidth())))
    {
      throw TypeCheckingExceptionPrivate(
          n, "constant with invalid significand size");
      return TypeNode::null();
    }
  }
  return nodeManager->mkFloatingPointType(f.getSize());
}

TypeNode RoundingModeConstantTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode RoundingModeConstantTypeRule::computeType(NodeManager* nodeManager,
                                                   TNode n,
                                                   bool check,
                                                   std::ostream* errOut)
{
  TRACE("RoundingModeConstantTypeRule");

  // Nothing to check!
  return nodeManager->roundingModeType();
}

TypeNode FloatingPointFPTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointFPTypeRule::computeType(NodeManager* nodeManager,
                                              TNode n,
                                              bool check,
                                              std::ostream* errOut)
{
  TRACE("FloatingPointFPTypeRule");

  TypeNode signType = n[0].getType(check);
  TypeNode exponentType = n[1].getType(check);
  TypeNode significandType = n[2].getType(check);

  if (!signType.isBitVector() || !exponentType.isBitVector()
      || !significandType.isBitVector())
  {
    throw TypeCheckingExceptionPrivate(n,
                                       "arguments to fp must be bit vectors");
    return TypeNode::null();
  }

  uint32_t signBits = signType.getBitVectorSize();
  uint32_t exponentBits = exponentType.getBitVectorSize();
  uint32_t significandBits = significandType.getBitVectorSize();

  if (check)
  {
    if (signBits != 1)
    {
      throw TypeCheckingExceptionPrivate(
          n, "sign bit vector in fp must be 1 bit long");
      return TypeNode::null();
    }
    else if (!(validExponentSize(exponentBits)))
    {
      throw TypeCheckingExceptionPrivate(
          n, "exponent bit vector in fp is an invalid size");
      return TypeNode::null();
    }
    else if (!(validSignificandSize(significandBits)))
    {
      throw TypeCheckingExceptionPrivate(
          n, "significand bit vector in fp is an invalid size");
      return TypeNode::null();
    }
  }

  // The +1 is for the implicit hidden bit
  return nodeManager->mkFloatingPointType(exponentBits, significandBits + 1);
}

TypeNode FloatingPointTestTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointTestTypeRule::computeType(NodeManager* nodeManager,
                                                TNode n,
                                                bool check,
                                                std::ostream* errOut)
{
  TRACE("FloatingPointTestTypeRule");

  if (check)
  {
    TypeNode firstOperand = n[0].getType(check);

    if (!firstOperand.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(
          n, "floating-point test applied to a non floating-point sort");
      return TypeNode::null();
    }

    size_t children = n.getNumChildren();
    for (size_t i = 1; i < children; ++i)
    {
      if (!(n[i].getType(check) == firstOperand))
      {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point test applied to mixed sorts");
        return TypeNode::null();
      }
    }
  }

  return nodeManager->booleanType();
}

TypeNode FloatingPointOperationTypeRule::preComputeType(NodeManager* nm,
                                                        TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointOperationTypeRule::computeType(NodeManager* nodeManager,
                                                     TNode n,
                                                     bool check,
                                                     std::ostream* errOut)
{
  TRACE("FloatingPointOperationTypeRule");

  TypeNode firstOperand = n[0].getType(check);

  if (check)
  {
    if (!firstOperand.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(
          n, "floating-point operation applied to a non floating-point sort");
      return TypeNode::null();
    }

    size_t children = n.getNumChildren();
    for (size_t i = 1; i < children; ++i)
    {
      if (!(n[i].getType(check) == firstOperand))
      {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point test applied to mixed sorts");
        return TypeNode::null();
      }
    }
  }

  return firstOperand;
}

TypeNode FloatingPointRoundingOperationTypeRule::preComputeType(NodeManager* nm,
                                                                TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointRoundingOperationTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointRoundingOperationTypeRule");

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }
  }

  TypeNode firstOperand = n[1].getType(check);

  if (check)
  {
    if (!firstOperand.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(
          n, "floating-point operation applied to a non floating-point sort");
      return TypeNode::null();
    }

    size_t children = n.getNumChildren();
    for (size_t i = 2; i < children; ++i)
    {
      if (!(n[i].getType(check) == firstOperand))
      {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point operation applied to mixed sorts");
        return TypeNode::null();
      }
    }
  }

  return firstOperand;
}

TypeNode FloatingPointPartialOperationTypeRule::preComputeType(NodeManager* nm,
                                                               TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointPartialOperationTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointOperationTypeRule");
  AlwaysAssert(n.getNumChildren() > 0);

  TypeNode firstOperand = n[0].getType(check);

  if (check)
  {
    if (!firstOperand.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(
          n, "floating-point operation applied to a non floating-point sort");
      return TypeNode::null();
    }

    const size_t children = n.getNumChildren();
    for (size_t i = 1; i < children - 1; ++i)
    {
      if (n[i].getType(check) != firstOperand)
      {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point partial operation applied to mixed sorts");
        return TypeNode::null();
      }
    }

    TypeNode UFValueType = n[children - 1].getType(check);

    if (!(UFValueType.isBitVector()) || !(UFValueType.getBitVectorSize() == 1))
    {
      throw TypeCheckingExceptionPrivate(
          n,
          "floating-point partial operation final argument must be a "
          "bit-vector of length 1");
      return TypeNode::null();
    }
  }

  return firstOperand;
}

TypeNode FloatingPointParametricOpTypeRule::preComputeType(NodeManager* nm,
                                                           TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointParametricOpTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointParametricOpTypeRule");

  return nodeManager->builtinOperatorType();
}

TypeNode FloatingPointToFPIEEEBitVectorTypeRule::preComputeType(NodeManager* nm,
                                                                TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToFPIEEEBitVectorTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointToFPIEEEBitVectorTypeRule");
  AlwaysAssert(n.getNumChildren() == 1);

  FloatingPointToFPIEEEBitVector info =
      n.getOperator().getConst<FloatingPointToFPIEEEBitVector>();

  if (check)
  {
    TypeNode operandType = n[0].getType(check);

    if (!(operandType.isBitVector()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to floating-point from "
                                         "bit vector used with sort other "
                                         "than bit vector");
      return TypeNode::null();
    }
    else if (!(operandType.getBitVectorSize()
               == info.getSize().exponentWidth()
                      + info.getSize().significandWidth()))
    {
      throw TypeCheckingExceptionPrivate(
          n,
          "conversion to floating-point from bit vector used with bit vector "
          "length that does not match floating point parameters");
      return TypeNode::null();
    }
  }

  return nodeManager->mkFloatingPointType(info.getSize());
}

TypeNode FloatingPointToFPFloatingPointTypeRule::preComputeType(NodeManager* nm,
                                                                TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToFPFloatingPointTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointToFPFloatingPointTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  FloatingPointToFPFloatingPoint info =
      n.getOperator().getConst<FloatingPointToFPFloatingPoint>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isFloatingPoint()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to floating-point from "
                                         "floating-point used with sort "
                                         "other than floating-point");
      return TypeNode::null();
    }
  }

  return nodeManager->mkFloatingPointType(info.getSize());
}

TypeNode FloatingPointToFPRealTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToFPRealTypeRule::computeType(NodeManager* nodeManager,
                                                    TNode n,
                                                    bool check,
                                                    std::ostream* errOut)
{
  TRACE("FloatingPointToFPRealTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  FloatingPointToFPReal info =
      n.getOperator().getConst<FloatingPointToFPReal>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isReal()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to floating-point from "
                                         "real used with sort other than "
                                         "real");
      return TypeNode::null();
    }
  }

  return nodeManager->mkFloatingPointType(info.getSize());
}

TypeNode FloatingPointToFPSignedBitVectorTypeRule::preComputeType(
    NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToFPSignedBitVectorTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointToFPSignedBitVectorTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  FloatingPointToFPSignedBitVector info =
      n.getOperator().getConst<FloatingPointToFPSignedBitVector>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isBitVector()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to floating-point from "
                                         "signed bit vector used with sort "
                                         "other than bit vector");
      return TypeNode::null();
    }
  }

  return nodeManager->mkFloatingPointType(info.getSize());
}

TypeNode FloatingPointToFPUnsignedBitVectorTypeRule::preComputeType(
    NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToFPUnsignedBitVectorTypeRule::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointToFPUnsignedBitVectorTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  FloatingPointToFPUnsignedBitVector info =
      n.getOperator().getConst<FloatingPointToFPUnsignedBitVector>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isBitVector()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to floating-point from "
                                         "unsigned bit vector used with sort "
                                         "other than bit vector");
      return TypeNode::null();
    }
  }

  return nodeManager->mkFloatingPointType(info.getSize());
}

TypeNode FloatingPointToUBVTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToUBVTypeRule::computeType(NodeManager* nodeManager,
                                                 TNode n,
                                                 bool check,
                                                 std::ostream* errOut)
{
  TRACE("FloatingPointToUBVTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  FloatingPointToUBV info = n.getOperator().getConst<FloatingPointToUBV>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isFloatingPoint()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to unsigned bit vector "
                                         "used with a sort other than "
                                         "floating-point");
      return TypeNode::null();
    }
  }

  return nodeManager->mkBitVectorType(info.d_bv_size);
}

TypeNode FloatingPointToSBVTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToSBVTypeRule::computeType(NodeManager* nodeManager,
                                                 TNode n,
                                                 bool check,
                                                 std::ostream* errOut)
{
  TRACE("FloatingPointToSBVTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  FloatingPointToSBV info = n.getOperator().getConst<FloatingPointToSBV>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isFloatingPoint()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to signed bit vector "
                                         "used with a sort other than "
                                         "floating-point");
      return TypeNode::null();
    }
  }

  return nodeManager->mkBitVectorType(info.d_bv_size);
}

TypeNode FloatingPointToUBVTotalTypeRule::preComputeType(NodeManager* nm,
                                                         TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToUBVTotalTypeRule::computeType(NodeManager* nodeManager,
                                                      TNode n,
                                                      bool check,
                                                      std::ostream* errOut)
{
  TRACE("FloatingPointToUBVTotalTypeRule");
  AlwaysAssert(n.getNumChildren() == 3);

  FloatingPointToUBVTotal info =
      n.getOperator().getConst<FloatingPointToUBVTotal>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isFloatingPoint()))
    {
      throw TypeCheckingExceptionPrivate(
          n,
          "conversion to unsigned bit vector total"
          "used with a sort other than "
          "floating-point");
      return TypeNode::null();
    }

    TypeNode defaultValueType = n[2].getType(check);

    if (!(defaultValueType.isBitVector())
        || !(defaultValueType.getBitVectorSize() == info))
    {
      throw TypeCheckingExceptionPrivate(
          n,
          "conversion to unsigned bit vector total"
          "needs a bit vector of the same length"
          "as last argument");
      return TypeNode::null();
    }
  }

  return nodeManager->mkBitVectorType(info.d_bv_size);
}

TypeNode FloatingPointToSBVTotalTypeRule::preComputeType(NodeManager* nm,
                                                         TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToSBVTotalTypeRule::computeType(NodeManager* nodeManager,
                                                      TNode n,
                                                      bool check,
                                                      std::ostream* errOut)
{
  TRACE("FloatingPointToSBVTotalTypeRule");
  AlwaysAssert(n.getNumChildren() == 3);

  FloatingPointToSBVTotal info =
      n.getOperator().getConst<FloatingPointToSBVTotal>();

  if (check)
  {
    TypeNode roundingModeType = n[0].getType(check);

    if (!roundingModeType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "first argument must be a rounding mode");
      return TypeNode::null();
    }

    TypeNode operandType = n[1].getType(check);

    if (!(operandType.isFloatingPoint()))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to signed bit vector "
                                         "used with a sort other than "
                                         "floating-point");
      return TypeNode::null();
    }

    TypeNode defaultValueType = n[2].getType(check);

    if (!(defaultValueType.isBitVector())
        || !(defaultValueType.getBitVectorSize() == info))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "conversion to signed bit vector total"
                                         "needs a bit vector of the same length"
                                         "as last argument");
      return TypeNode::null();
    }
  }

  return nodeManager->mkBitVectorType(info.d_bv_size);
}

TypeNode FloatingPointToRealTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToRealTypeRule::computeType(NodeManager* nodeManager,
                                                  TNode n,
                                                  bool check,
                                                  std::ostream* errOut)
{
  TRACE("FloatingPointToRealTypeRule");
  AlwaysAssert(n.getNumChildren() == 1);

  if (check)
  {
    TypeNode operandType = n[0].getType(check);

    if (!operandType.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(
          n, "floating-point to real applied to a non floating-point sort");
      return TypeNode::null();
    }
  }

  return nodeManager->realType();
}

TypeNode FloatingPointToRealTotalTypeRule::preComputeType(NodeManager* nm,
                                                          TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointToRealTotalTypeRule::computeType(NodeManager* nodeManager,
                                                       TNode n,
                                                       bool check,
                                                       std::ostream* errOut)
{
  TRACE("FloatingPointToRealTotalTypeRule");
  AlwaysAssert(n.getNumChildren() == 2);

  if (check)
  {
    TypeNode operandType = n[0].getType(check);

    if (!operandType.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(
          n,
          "floating-point to real total applied to a non floating-point sort");
      return TypeNode::null();
    }

    TypeNode defaultValueType = n[1].getType(check);

    if (!defaultValueType.isReal())
    {
      throw TypeCheckingExceptionPrivate(
          n, "floating-point to real total needs a real second argument");
      return TypeNode::null();
    }
  }

  return nodeManager->realType();
}

TypeNode FloatingPointComponentBit::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointComponentBit::computeType(NodeManager* nodeManager,
                                                TNode n,
                                                bool check,
                                                std::ostream* errOut)
{
  TRACE("FloatingPointComponentBit");

  if (check)
  {
    TypeNode operandType = n[0].getType(check);

    if (!operandType.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "floating-point bit component "
                                         "applied to a non floating-point "
                                         "sort");
      return TypeNode::null();
    }
    if (!(Theory::isLeafOf(n[0], THEORY_FP)
          || n[0].getKind() == kind::FLOATINGPOINT_TO_FP_FROM_REAL))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "floating-point bit component "
                                         "applied to a non leaf / to_fp leaf "
                                         "node");
      return TypeNode::null();
    }
  }

  return nodeManager->mkBitVectorType(1);
}

TypeNode FloatingPointComponentExponent::preComputeType(NodeManager* nm,
                                                        TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointComponentExponent::computeType(NodeManager* nodeManager,
                                                     TNode n,
                                                     bool check,
                                                     std::ostream* errOut)
{
  TRACE("FloatingPointComponentExponent");

  TypeNode operandType = n[0].getType(check);

  if (check)
  {
    if (!operandType.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "floating-point exponent component "
                                         "applied to a non floating-point "
                                         "sort");
      return TypeNode::null();
    }
    if (!(Theory::isLeafOf(n[0], THEORY_FP)
          || n[0].getKind() == kind::FLOATINGPOINT_TO_FP_FROM_REAL))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "floating-point exponent component "
                                         "applied to a non leaf / to_fp "
                                         "node");
      return TypeNode::null();
    }
  }

  /* Need to create some symfpu objects as the size of bit-vector
   * that is needed for this component is dependent on the encoding
   * used (i.e. whether subnormals are forcibly normalised or not).
   * Here we use types from floatingpoint.h which are the literal
   * back-end but it should't make a difference. */
  FloatingPointSize fps = operandType.getConst<FloatingPointSize>();
  uint32_t bw = FloatingPoint::getUnpackedExponentWidth(fps);
  return nodeManager->mkBitVectorType(bw);
}

TypeNode FloatingPointComponentSignificand::preComputeType(NodeManager* nm,
                                                           TNode n)
{
  return TypeNode::null();
}
TypeNode FloatingPointComponentSignificand::computeType(
    NodeManager* nodeManager, TNode n, bool check, std::ostream* errOut)
{
  TRACE("FloatingPointComponentSignificand");

  TypeNode operandType = n[0].getType(check);

  if (check)
  {
    if (!operandType.isFloatingPoint())
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "floating-point significand "
                                         "component applied to a non "
                                         "floating-point sort");
      return TypeNode::null();
    }
    if (!(Theory::isLeafOf(n[0], THEORY_FP)
          || n[0].getKind() == kind::FLOATINGPOINT_TO_FP_FROM_REAL))
    {
      throw TypeCheckingExceptionPrivate(n,
                                         "floating-point significand "
                                         "component applied to a non leaf / "
                                         "to_fp node");
      return TypeNode::null();
    }
  }

  /* As before we need to use some of sympfu. */
  FloatingPointSize fps = operandType.getConst<FloatingPointSize>();
  uint32_t bw = FloatingPoint::getUnpackedSignificandWidth(fps);
  return nodeManager->mkBitVectorType(bw);
}

TypeNode RoundingModeBitBlast::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}
TypeNode RoundingModeBitBlast::computeType(NodeManager* nodeManager,
                                           TNode n,
                                           bool check,
                                           std::ostream* errOut)
{
  TRACE("RoundingModeBitBlast");

  if (check)
  {
    TypeNode operandType = n[0].getType(check);

    if (!operandType.isRoundingMode())
    {
      throw TypeCheckingExceptionPrivate(
          n, "rounding mode bit-blast applied to a non rounding-mode sort");
      return TypeNode::null();
    }
    if (!Theory::isLeafOf(n[0], THEORY_FP))
    {
      throw TypeCheckingExceptionPrivate(
          n, "rounding mode bit-blast applied to a non leaf node");
      return TypeNode::null();
    }
  }

  return nodeManager->mkBitVectorType(CVC5_NUM_ROUNDING_MODES);
}

Cardinality CardinalityComputer::computeCardinality(TypeNode type)
{
  return fp::utils::getCardinality(type);
}

}  // namespace fp
}  // namespace theory
}  // namespace cvc5::internal
