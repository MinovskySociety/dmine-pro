#ifndef _GUNDAM_TOOL_K_HOP_H
#define _GUNDAM_TOOL_K_HOP_H

#include "gundam/algorithm/bfs.h"

#include "gundam/type_getter/vertex_handle.h"

namespace GUNDAM {

template<typename GraphType>
GraphType KHop(GraphType& graph,
         const std::set<typename GUNDAM::VertexHandle<GraphType>::type>& src_handle_set,
               size_t k){

  using VertexHandleType = typename GUNDAM::VertexHandle<GraphType>::type;

  GraphType k_hop;

  auto hop_callback = [&k_hop
                      #ifndef NDEBUG
                      ,&k
                      #endif
                      ](
        VertexHandleType vertex_handle,
        uint32_t bfs_idx,
        uint32_t distance
      ){
    assert(distance <= k);

    auto [new_vertex_handle, ret] = k_hop.AddVertex(vertex_handle->id(),
                                                    vertex_handle->label());
    assert(new_vertex_handle);

    // add attribute to new_vertex
    for (auto attr_it = vertex_handle->AttributeBegin();
             !attr_it.IsDone();
              attr_it++){
      auto [attr_handle, ret] = new_vertex_handle->AddAttribute(
                                      attr_it->key(),
                                      attr_it->value_type(),
                                      attr_it->value_str());
    }

    for (auto out_edge_it = vertex_handle->OutEdgeBegin();
             !out_edge_it.IsDone(); 
              out_edge_it++) {
      auto k_hop_vertex_handle 
         = k_hop.FindVertex(out_edge_it->dst_handle()->id());
      if (!k_hop_vertex_handle) {
        // the dst vertex does not exist in the k_hop, 
        // this edge does not need to be added into k_hop
        continue;
      }
      // this vertex should already be contained in sub graph
      auto [k_hop_edge_handle, ret] 
          = k_hop.AddEdge(out_edge_it->src_handle()->id(),
                          out_edge_it->dst_handle()->id(),
                          out_edge_it->label(),
                          out_edge_it->id());
      assert(k_hop_edge_handle);
      for (auto attr_it = out_edge_it->AttributeBegin();
               !attr_it.IsDone();
                attr_it++){
        auto [attr_handle, ret] = k_hop_edge_handle->AddAttribute(
                                        attr_it->key(),
                                        attr_it->value_type(),
                                        attr_it->value_str());
      }
    }

    for (auto in_edge_it = vertex_handle->InEdgeBegin();
             !in_edge_it.IsDone(); 
              in_edge_it++) {
      auto k_hop_vertex_handle 
         = k_hop.FindVertex(in_edge_it->src_handle()->id());
      if (!k_hop_vertex_handle) {
        // the dst vertex does not exist in the k_hop, 
        // this edge does not need to be added into k_hop
        continue;
      }
      // this vertex should already be contained in sub graph
      auto [k_hop_edge_handle, ret] 
          = k_hop.AddEdge(in_edge_it->src_handle()->id(),
                          in_edge_it->dst_handle()->id(),
                          in_edge_it->label(),
                          in_edge_it->id());
      assert(k_hop_edge_handle);
      for (auto attr_it = in_edge_it->AttributeBegin();
               !attr_it.IsDone();
                attr_it++){
        auto [attr_handle, ret] = k_hop_edge_handle->AddAttribute(
                                        attr_it->key(),
                                        attr_it->value_type(),
                                        attr_it->value_str());
      }
    }
    return true;
  };

  GUNDAM::Bfs<true>(graph, src_handle_set, hop_callback, k);

  return k_hop;
}

}; // namespace GUNDAM

#endif // _GUNDAM_TOOL_K_HOP_H
