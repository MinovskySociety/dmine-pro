#ifndef _GUNDAM_TOOL_VERTEX_ID_CONTINUED_H
#define _GUNDAM_TOOL_VERTEX_ID_CONTINUED_H

#include <set>

#include "assert.h"

namespace GUNDAM {

template <typename GraphType>
bool VertexIDContinued(const GraphType& graph){
  using VertexIDType = typename GraphType::VertexType::IDType;
  std::set<VertexIDType> vertex_id_set;
  for (auto vertex_cit = graph.VertexBegin();
           !vertex_cit.IsDone();
            vertex_cit++){
    auto [ret_it, ret] = vertex_id_set.emplace(vertex_cit->id());
    // should added successfully
    assert(ret);
  }
  assert(vertex_id_set.size() == graph.CountVertex());
  VertexIDType current_id = *(vertex_id_set.begin());
  for (const auto& vertex_id : vertex_id_set){
    if (current_id != vertex_id){
      // is not connected
      return false;
    }
    current_id++;
  }
  return true;
}

};

#endif // _GUNDAM_TOOL_VERTEX_ID_CONTINUED_H