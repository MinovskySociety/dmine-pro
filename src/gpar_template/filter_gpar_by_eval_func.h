#ifndef _GPAR_FILTER_BY_EVAL_H
#define _GPAR_FILTER_BY_EVAL_H
#include <algorithm>
#include <cmath>

#include "gpar_new.h"

namespace gmine_new {

template <class GPARList>
void FilterGPARByEvalFunc(GPARList &gpar_list) {
  using GPAR = typename GPARList::value_type;
  using Pattern = typename GPAR::value_type;
  using VertexIDType = typename GPAR::VertexIDType;
  using VertexLabelType = typename GPAR::VertexLabelType;
  using EdgeIDType = typename GPAR::EdgeIDType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
}
}  // namespace gmine_new

#endif
