#ifndef _GUNDAM_TOOL_RANDOM_WALK_H
#define _GUNDAM_TOOL_RANDOM_WALK_H

#include <queue>
#include <set>
#include <type_traits>

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/type_getter/edge_handle.h"

#include "gundam/algorithm/bfs.h"

namespace GUNDAM{

// legal prune_callback forms:
//    prune_callback(next_vertex_handle)
//    prune_callback(next_vertex_handle, next_edge_handle)
//    prune_callback(next_vertex_handle, next_edge_handle, current_random_walk_idx)
//    prune_callback(next_vertex_handle, next_edge_handle, current_random_walk_idx, current_depth)
//
// legal callback forms:
//    user_callback(current_vertex_handle)
//    user_callback(current_vertex_handle, current_random_walk_idx)
//
//    return how many steps has been walked, should be smaller than the size of
//    graph that has been actually visited
template <bool bidirectional = false,
          typename         GraphType,
          typename  UserCallBackType,
          typename PruneCallBackType,
          typename std::enable_if_t< 
                   // prune_callback(next_vertex_handle)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type)> >
                || // prune_callback(next_vertex_handle, next_edge_handle)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type)> >
                || // prune_callback(next_vertex_handle, next_edge_handle, current_random_walk_idx)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType)> >
                || // prune_callback(next_vertex_handle, next_edge_handle, current_random_walk_idx, current_depth)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType,
                                               typename  GraphType::VertexCounterType)> >, bool> = false>
inline size_t RandomWalk(GraphType&  graph,
   typename VertexHandle<GraphType>::type src_vertex_handle,
                  UserCallBackType  user_callback,
                 PruneCallBackType prune_callback) {
  using VertexCounterType = typename  GraphType::VertexCounterType;
  using  VertexHandleType = typename VertexHandle<GraphType>::type;
  using    EdgeHandleType = typename   EdgeHandle<GraphType>::type;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  static_assert(
       // user_callback(vertex_handle)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType)> >
    || // user_callback(vertex_handle, random_walk_idx)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType, 
                                  VertexCounterType)> >, 
      "illegal user_callback type, only allows one of user_callback(vertex_handle) and user_callback(vertex_handle, random_walk_idx)");

  static_assert(
       // prune_callback(next_vertex_handle)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType)> >
    || // prune_callback(next_vertex_handle, next_edge_handle)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType)> >
    || // prune_callback(next_vertex_handle, next_edge_handle, current_random_walk_idx)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType,
                                  VertexCounterType)> >,
      "illegal prune_callback type, only allows one of prune_callback(vertex_handle), prune_callback(vertex_handle, edge_handle) and prune_callback(next_vertex_handle, next_edge_handle, current_random_walk_idx)");

  VertexCounterType random_walk_idx = 0;
  VertexHandleType current_vertex_handle
                     = src_vertex_handle;
  // walk in the data graph randomly until reach the ending condition
  while (true){
    bool user_callback_ret = false;
    if constexpr (
        // user_callback(vertex_handle)
        std::is_convertible_v<
                  UserCallBackType, 
                  std::function<bool(VertexHandleType)> >){
      user_callback_ret = user_callback(current_vertex_handle);
    }
    if constexpr (
        // user_callback(vertex_handle, random_walk_idx)
        std::is_convertible_v<
                  UserCallBackType, 
                  std::function<bool(VertexHandleType, 
                                    VertexCounterType)> >){
      user_callback_ret = user_callback(current_vertex_handle,
                                        random_walk_idx);
    }

    random_walk_idx++;
    if (!user_callback_ret){
      // end condition has been reached, end random walking
      break;
    }

    std::vector<EdgeIDType> qualified_edge_id_set;

    if constexpr (bidirectional){
      // find the qualified in edge
      for (auto in_edge_it = current_vertex_handle->InEdgeBegin();
               !in_edge_it.IsDone(); 
                in_edge_it++) {
        bool prune_ret = false;
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType)> >){
          prune_ret = prune_callback(in_edge_it->src_handle());
        }
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType, 
                                         EdgeHandleType)> >){
          prune_ret = prune_callback(in_edge_it->src_handle(),
                                     in_edge_it);
        }
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType, 
                                         EdgeHandleType,
                                      VertexCounterType)> >){
          prune_ret = prune_callback(in_edge_it->src_handle(),
                                     in_edge_it, random_walk_idx);
        }
        if (prune_ret){
          // this edge is pruned
          continue;
        }
        qualified_edge_id_set.emplace_back(in_edge_it->id());
      }
    }
    // find the qualified out edge
    for (auto out_edge_it = current_vertex_handle->OutEdgeBegin();
             !out_edge_it.IsDone(); 
              out_edge_it++) {
      bool prune_ret = false;
      if constexpr (
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType)> >){
        prune_ret = prune_callback(out_edge_it->dst_handle());
      }
      if constexpr (
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType, 
                                        EdgeHandleType)> >){
        prune_ret = prune_callback(out_edge_it->dst_handle(),
                                   out_edge_it);
      }
      if constexpr (
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType, 
                                        EdgeHandleType,
                                    VertexCounterType)> >){
        prune_ret = prune_callback(out_edge_it->dst_handle(),
                                   out_edge_it, random_walk_idx);
      }
      if (prune_ret){
        // this edge is pruned
        continue;
      }
      qualified_edge_id_set.emplace_back(out_edge_it->id());
    }

    if (qualified_edge_id_set.empty()){
      // does not find legal edge, end random walking
      break;
    }

    const EdgeIDType kSelectedEdgeId
        = qualified_edge_id_set[std::rand() % qualified_edge_id_set.size()];

    if constexpr (bidirectional){
      bool found_in_in_edge = false;
      // to find the whether the selected is an in edge
      for (auto in_edge_it = current_vertex_handle->InEdgeBegin();
               !in_edge_it.IsDone();
                in_edge_it++) {
        if (in_edge_it->id() != kSelectedEdgeId) {
          // this edge is not selected
          continue;
        }
        // this edge is selected
        current_vertex_handle = in_edge_it->src_handle();
        found_in_in_edge = true;
        break;
      }
      if (found_in_in_edge){
        continue;
      }
    }
    // to find the whether the selected is an in edge
    for (auto out_edge_it = current_vertex_handle->OutEdgeBegin();
             !out_edge_it.IsDone();
              out_edge_it++) {
      if (out_edge_it->id() != kSelectedEdgeId) {
        // this edge is not selected
        continue;
      }
      // this edge is selected
      current_vertex_handle = out_edge_it->dst_handle();
      break;
    }
  }
  return random_walk_idx;
}

template <bool bidirectional = false,
          typename GraphType>
inline size_t RandomWalk(GraphType&  graph,
   typename VertexHandle<GraphType>::type src_vertex_handle,
                  size_t depth_limit,
                  size_t  size_limit,
  std::function<bool(typename VertexHandle<GraphType>::type)> user_callback) {

  using VertexCounterType = typename  GraphType::VertexCounterType;
  using VertexHandleType  = typename VertexHandle<GraphType>::type;
  using   EdgeHandleType  = typename   EdgeHandle<GraphType>::type;

  std::set<VertexHandleType> vertexes_in_k_hop;

  auto preserve_vertexes_in_k_hop_prune_callback 
            = [&vertexes_in_k_hop,
               &depth_limit](VertexHandleType vertex_handle,
                               EdgeHandleType   edge_handle,
                            VertexCounterType bfs_idx,
                            VertexCounterType bfs_depth){
    assert(bfs_depth <= depth_limit + 1);
    if (bfs_depth > depth_limit) {
      // reach the distance limit, prune this vertex
      return true;
    }
    // does not reach the limit, do not prune
    return false;
  };

  auto preserve_vertexes_in_k_hop_user_callback 
            = [&vertexes_in_k_hop](VertexHandleType vertex_handle){
    auto [vertexes_in_k_hop_it,
          vertexes_in_k_hop_ret] 
        = vertexes_in_k_hop.emplace(vertex_handle);
    assert(vertexes_in_k_hop_ret);
    return true;
  };

  const auto kKHopVertexSize = Bfs<bidirectional>(graph, src_vertex_handle,
                                  preserve_vertexes_in_k_hop_user_callback, 
                                  preserve_vertexes_in_k_hop_prune_callback);

  assert(vertexes_in_k_hop.size() == kKHopVertexSize);

  auto prune_callback = [&vertexes_in_k_hop](
         VertexHandleType vertex_handle){
    if (vertexes_in_k_hop.find(vertex_handle) 
     == vertexes_in_k_hop.end()){
      // this vertex is not contained in k_hop
      // does not need to be visited
      return true;
    }
    return false;
  };

  std::set<VertexHandleType> visited_vertex;

  const size_t kSizeLimit = size_limit < vertexes_in_k_hop.size() ?
                            size_limit : vertexes_in_k_hop.size();

  auto random_walk_callback= [&user_callback,
                              &visited_vertex,
                              &kSizeLimit](
              VertexHandleType vertex_handle) {
    assert(visited_vertex.size() < kSizeLimit);
    visited_vertex.emplace(vertex_handle);
    bool user_callback_ret = user_callback(vertex_handle);
    return (visited_vertex.size() < kSizeLimit)
         && user_callback_ret;
  };

  return RandomWalk<bidirectional>(graph,
                       src_vertex_handle,
                    random_walk_callback,
                          prune_callback);
}

}; // namespace GUNDAM

#endif // _GUNDAM_TOOL_RANDOM_WALK_H