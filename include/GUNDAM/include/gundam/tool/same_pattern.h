#ifndef _SAME_PATTERN_H
#define _SAME_PATTERN_H

#include "gundam/algorithm/dp_iso_using_match.h"

namespace GUNDAM {

template <typename GraphType0,
          typename GraphType1>
inline bool SamePattern(GraphType0& graph_0,
                        GraphType1& graph_1) {
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  const auto& graph_0_const_ref = graph_0;
  const auto& graph_1_const_ref = graph_1;

  if ((graph_0_const_ref.CountVertex()
    != graph_1_const_ref.CountVertex())
    ||(graph_0_const_ref.CountEdge()
    != graph_1_const_ref.CountEdge())) {
    return false;
  }

  auto ret = GUNDAM::DpisoUsingMatch(graph_0_const_ref,
                                     graph_1_const_ref, 1);

  if (ret == 0){
    return false;
  }
  return true;
}

}  // namespace GUNDAM

#endif // _SAME_PATTERN_H