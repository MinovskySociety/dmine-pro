#ifndef _GUNDAM_TOOL_EDGE_ID_CONTINUED_H
#define _GUNDAM_TOOL_EDGE_ID_CONTINUED_H

#include <set>

#include "assert.h"

namespace GUNDAM {

template <typename GraphType>
bool EdgeIDContinued(const GraphType& graph){
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  std::set<EdgeIDType> edge_id_set;
  for (auto vertex_cit = graph.VertexBegin();
           !vertex_cit.IsDone();
            vertex_cit++){
    for (auto edge_cit = vertex_cit->OutEdgeBegin();
             !edge_cit.IsDone();
              edge_cit++){
      auto [ret_it, ret] = edge_id_set.emplace(edge_cit->id());
      // should added successfully
      assert(ret);
    }
  }
  assert(edge_id_set.size() == graph.CountEdge());
  EdgeIDType current_id = *(edge_id_set.begin());
  for (const auto& edge_id : edge_id_set){
    if (current_id != edge_id){
      // is not connected
      return false;
    }
    current_id++;
  }
  return true;
}

};

#endif // _GUNDAM_TOOL_EDGE_ID_CONTINUED_H