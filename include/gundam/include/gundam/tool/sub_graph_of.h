#ifndef _GUNDAM_SUB_GRAPH_OF_H
#define _GUNDAM_SUB_GRAPH_OF_H

#include "gundam/match/match.h"
#include "gundam/algorithm/dp_iso_using_match.h"

namespace GUNDAM {

// return true if these two graphs are same 
template <typename GraphType0,
          typename GraphType1>
inline bool SubGraphOf(GraphType0& graph_0,
                       GraphType1& graph_1,                       
                 Match<GraphType0,
                       GraphType1>& match_graph_0_to_graph_1) {
  // just begin bfs at a random vertex and find whether
  // it can reach all vertexes
  if ((graph_0.CountVertex() > graph_1.CountVertex())
    ||(graph_0.CountEdge()   > graph_1.CountEdge())) {
    return false;
  }

  auto ret = GUNDAM::DpisoUsingMatch(graph_0, graph_1, match_graph_0_to_graph_1, 1);
  assert(ret == 0 || ret == 1);
  if (ret == 0){
    return false;
  }
  return true;
}

template <typename GraphType0,
          typename GraphType1>
inline bool SubGraphOf(GraphType0& graph_0,
                       GraphType1& graph_1) {
  Match<GraphType0, GraphType1> match_graph_0_to_graph_1;
  return SubGraphOf(graph_0, graph_1, match_graph_0_to_graph_1);
}

}  // namespace GUNDAM

#endif // _GUNDAM_SUB_GRAPH_OF_H