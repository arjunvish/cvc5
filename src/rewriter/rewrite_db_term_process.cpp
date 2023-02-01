/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2023 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Rewrite database term processor
 */

#include "rewriter/rewrite_db_term_process.h"

#include "expr/attribute.h"
#include "expr/nary_term_util.h"
#include "util/string.h"
#include "theory/builtin/generic_op.h"

using namespace cvc5::internal::kind;

namespace cvc5::internal {
namespace rewriter {

Node RewriteDbNodeConverter::postConvert(Node n)
{
  Kind k = n.getKind();
  TypeNode tn = n.getType();
  if (k == CONST_STRING)
  {
    NodeManager* nm = NodeManager::currentNM();
    // "ABC" is (str.++ "A" "B" "C")
    const std::vector<unsigned>& vec = n.getConst<String>().getVec();
    if (vec.size() <= 1)
    {
      return n;
    }
    std::vector<unsigned> v(vec.begin(), vec.end());
    std::vector<Node> children;
    for (unsigned i = 0, size = v.size(); i < size; i++)
    {
      std::vector<unsigned> tmp;
      tmp.push_back(v[i]);
      children.push_back(nm->mkConst(String(tmp)));
    }
    return nm->mkNode(STRING_CONCAT, children);
  }
  // convert indexed operators to symbolic
  if (GenericOp::isIndexedOperatorKind(k))
  {
    NodeManager* nm = NodeManager::currentNM();
    std::vector<Node> indices = GenericOp::getIndicesForOperator(k, n.getOperator());
    indices.insert(indices.begin(), nm->mkConst(GenericOp(k)));
    indices.insert(indices.end(), n.begin(), n.end());
    return nm->mkNode(APPLY_INDEXED_SYMBOLIC, indices);
  }
  
  return n;
}

}  // namespace rewriter
}  // namespace cvc5::internal
