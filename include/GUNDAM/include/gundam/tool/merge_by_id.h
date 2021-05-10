#ifndef _GUNDAM_TOOL_MERGE_BY_ID_H
#define _GUNDAM_TOOL_MERGE_BY_ID_H

#include "gundam/match/match.h"

namespace GUNDAM {  

// merge graph1 into graph0 by id
template <typename GraphType0,
          typename GraphType1>
inline bool MergeById(GraphType0& graph0,
                      GraphType1& graph1) {

  GUNDAM::Match<GraphType1, 
                GraphType0> match(graph1, graph0, "same_id_map");

  // merge vertexes 
  for (auto graph1_vertex_it = graph1.VertexBegin();
           !graph1_vertex_it.IsDone();
            graph1_vertex_it++){
    auto graph0_vertex_handle = match.MapTo(graph1_vertex_it);
    if (graph0_vertex_handle){
      // graph0_vertex_handle is not null
      // graph0_vertex_handle has already
      // contained in graph1
      continue;
    }
    auto [vertex_handle, add_vertex_ret] 
        = graph0.AddVertex(graph1_vertex_it->id(),
                           graph1_vertex_it->label());
    if (!add_vertex_ret){
      // is not added successfully
      return false;
    }
    for (auto attr_it = graph1_vertex_it->AttributeBegin();
             !attr_it.IsDone(); attr_it++){
      auto [attr_handle, attr_ret] 
        = vertex_handle->AddAttribute(attr_it->key(), 
                                      attr_it->value_type(), 
                                      attr_it->value_str());
      assert(!attr_handle.IsNull());
      assert(attr_ret);
    }
  }

  GUNDAM::Match<GraphType1, 
                GraphType0> complete_match(graph1, graph0, "same_id_map");

  assert(complete_match.size() == graph1.CountVertex());

  // merge edges
  for (auto graph1_vertex_it = graph1.VertexBegin();
           !graph1_vertex_it.IsDone();
            graph1_vertex_it++){
    for (auto out_edge_it = graph1_vertex_it->OutEdgeBegin();
             !out_edge_it.IsDone(); 
              out_edge_it++){
      auto [edge_handle, add_edge_ret] 
        = graph0.AddEdge(out_edge_it->src_handle()->id(),
                         out_edge_it->dst_handle()->id(),
                         out_edge_it->label(),
                         out_edge_it->id());
      if (!add_edge_ret){
        // already existed
        continue;
      }
      assert(edge_handle);
      for (auto attr_it = out_edge_it->AttributeBegin();
               !attr_it.IsDone(); 
                attr_it++){
        auto [attr_handle, attr_ret] 
            = edge_handle->AddAttribute(attr_it->key(), 
                                        attr_it->value_type(), 
                                        attr_it->value_str());
        assert(!attr_handle.IsNull());
        assert(attr_ret);
      }
    }
  }
  return true;
}

}

#endif // _GUNDAM_TOOL_MERGE_BY_ID_H