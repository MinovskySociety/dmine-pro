#ifndef _GUNDAM_TOOL_SAME_PATTERN_H
#define _GUNDAM_TOOL_SAME_PATTERN_H

#include "gundam/algorithm/dp_iso_using_match.h"

namespace GUNDAM {

template <typename GraphType0,
          typename GraphType1>
inline bool SamePattern(GraphType0& graph_0,
                        GraphType1& graph_1
                        #ifndef NDEBUG
                        , bool double_check = true
                        #endif
                        ) {
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  if ((graph_0.CountVertex()
    != graph_1.CountVertex())
    ||(graph_0.CountEdge()
    != graph_1.CountEdge())) {
    return false;
  }

  auto ret = GUNDAM::DpisoUsingMatch(graph_0, graph_1, 1);

  if (ret == 0){
    #ifndef NDEBUG
    if (double_check) {
      assert(!SamePattern(graph_1, graph_0, false));
    }
    #endif // NDEBUG
    return false;
  }
  #ifndef NDEBUG
  if (double_check) {
    assert(SamePattern(graph_1, graph_0, false));
  }
  #endif // NDEBUG
  return true;
}

}  // namespace GUNDAM

#endif // _GUNDAM_TOOL_SAME_PATTERN_H