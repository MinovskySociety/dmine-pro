#ifndef _GUNDAM_ALGORITHM_BFS_H
#define _GUNDAM_ALGORITHM_BFS_H

#include <queue>
#include <set>
#include <type_traits>

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/type_getter/edge_handle.h"

namespace GUNDAM {

// legal prune_callback forms:
//    prune_callback(next_vertex_handle)
//    prune_callback(next_vertex_handle, next_edge_handle)
//    prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx)
//    prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx, current_distance)
//
// legal callback forms:
//    user_callback(current_vertex_handle)
//    user_callback(current_vertex_handle, current_bfs_idx)
//    user_callback(current_vertex_handle, current_bfs_idx, current_distance)
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
                || // prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType)> >
                || // prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx, current_distance)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType,
                                               typename  GraphType::VertexCounterType)> >, bool> = false>
inline size_t Bfs(GraphType&  graph,
  const std::set<typename VertexHandle<GraphType>::type>& src_vertex_handle_set,
           UserCallBackType  user_callback,
          PruneCallBackType prune_callback) {
  using VertexCounterType = typename  GraphType::VertexCounterType;
  using  VertexHandleType = typename VertexHandle<GraphType>::type;
  using    EdgeHandleType = typename   EdgeHandle<GraphType>::type;
  static_assert(
       // user_callback(vertex_handle)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType)> >
    || // user_callback(vertex_handle, bfs_idx)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType, 
                                   VertexCounterType)> >
    || // user_callback(vertex_handle, bfs_idx, distance)
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType, 
                                   VertexCounterType, 
                                   VertexCounterType)> >, 
      "illegal callback type, only allows one of user_callback(vertex_handle), user_callback(vertex_handle, bfs_idx) and user_callback(vertex_handle, bfs_idx, distance)");

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
    || // prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType,
                                  VertexCounterType)> >
    || // prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx, current_distance)
      std::is_convertible_v<
                PruneCallBackType, 
                std::function<bool(VertexHandleType, 
                                     EdgeHandleType,
                                  VertexCounterType,
                                  VertexCounterType)> >,
      "illegal callback type, only allows one of prune_callback(vertex_handle), prune_callback(vertex_handle, edge_handle), prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx), prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx, current_distance)");

  VertexCounterType bfs_idx = 0;
  std::queue<std::pair<VertexHandleType, 
                       VertexCounterType>> vertex_handle_queue;
  std:: set <VertexHandleType> visited;
  for (const auto& src_vertex_handle 
                 : src_vertex_handle_set){
    vertex_handle_queue.emplace(src_vertex_handle, 0);
    visited.emplace(src_vertex_handle);
  }
  while (!vertex_handle_queue.empty()){
    auto [current_vertex_handle, 
          current_distance] = vertex_handle_queue.front();
    vertex_handle_queue.pop();
    bool ret = false;
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType)> >){
      ret = user_callback(current_vertex_handle);
    }
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType,
                                   VertexCounterType)> >){
      ret = user_callback(current_vertex_handle,
                          bfs_idx);
    }
    if constexpr (
      std::is_convertible_v<
                UserCallBackType, 
                std::function<bool(VertexHandleType,
                                   VertexCounterType,
                                   VertexCounterType)> >){
      ret = user_callback(current_vertex_handle,
                          bfs_idx,
                          current_distance);
    }
    bfs_idx++;
    if (!ret){
      // meets stopping condition, stop the matching process
      return bfs_idx;
    }
    for (auto out_edge_it = current_vertex_handle->OutEdgeBegin();
             !out_edge_it.IsDone();
              out_edge_it++) {
      if (visited.find(out_edge_it->dst_handle()) != visited.end()){
        // already visited
        continue;
      }
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
                                   out_edge_it, bfs_idx);
      }
      if constexpr (
        std::is_convertible_v<
                  PruneCallBackType, 
                  std::function<bool(VertexHandleType, 
                                        EdgeHandleType,
                                    VertexCounterType,
                                    VertexCounterType)> >){
        prune_ret = prune_callback(out_edge_it->dst_handle(),
                                   out_edge_it, bfs_idx, current_distance + 1);
      }
      if (prune_ret){
        // this vertex is pruned, does not be considered
        continue;
      }
      visited.emplace(out_edge_it->dst_handle());
      vertex_handle_queue.emplace(out_edge_it->dst_handle(), current_distance + 1);
    }
    if constexpr (bidirectional){
      for (auto in_edge_it = current_vertex_handle->InEdgeBegin();
               !in_edge_it.IsDone();
                in_edge_it++) {
        if (visited.find(in_edge_it->src_handle()) != visited.end()){
          // already visited
          continue;
        }
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
                                     in_edge_it, bfs_idx);
        }
        if constexpr (
          std::is_convertible_v<
                    PruneCallBackType, 
                    std::function<bool(VertexHandleType, 
                                         EdgeHandleType,
                                      VertexCounterType,
                                      VertexCounterType)> >){
          prune_ret = prune_callback(in_edge_it->src_handle(),
                                     in_edge_it, bfs_idx, current_distance + 1);
        }

        if (prune_ret){
          // this vertex is pruned, does not be considered
          continue;
        }
        visited.emplace(in_edge_it->src_handle());
        vertex_handle_queue.emplace(in_edge_it->src_handle(), current_distance + 1);
      }
    }
  }
  return bfs_idx;
}


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
                || // prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType)> >
                || // prune_callback(next_vertex_handle, next_edge_handle, current_bfs_idx, current_distance)
                   std::is_convertible_v<
                            PruneCallBackType, 
                            std::function<bool(typename VertexHandle<GraphType>::type, 
                                               typename   EdgeHandle<GraphType>::type,
                                               typename  GraphType::VertexCounterType,
                                               typename  GraphType::VertexCounterType)> >, bool> = false>
inline size_t Bfs(GraphType&  graph,
  const typename VertexHandle<GraphType>::type& src_vertex_handle,
           UserCallBackType  user_callback,
          PruneCallBackType prune_callback) {
  const std::set<typename VertexHandle<GraphType>::type> src_vertex_handle_set = {src_vertex_handle};
  return Bfs<bidirectional>(graph, src_vertex_handle_set, user_callback,
                                                         prune_callback);
}

template <bool bidirectional = false,
          typename        GraphType,
          typename UserCallBackType>
inline size_t Bfs(GraphType&  graph,
  const std::set<typename VertexHandle<GraphType>::type>& src_vertex_handle_set,
           UserCallBackType  user_callback,
           int distance_limit = -1) {
  using VertexCounterType = typename  GraphType::VertexCounterType;
  using  VertexHandleType = typename VertexHandle<GraphType>::type;
  using    EdgeHandleType = typename   EdgeHandle<GraphType>::type;
  auto prune_distance_callback = [&distance_limit](
         VertexHandleType  vertex_handle,
           EdgeHandleType    edge_handle,
        VertexCounterType current_idx,
        VertexCounterType current_distance){
    if (distance_limit == -1){
      // distance limit is not set, prune nothing
      return false;
    }
    assert(current_distance <= distance_limit + 1);
    if (current_distance > distance_limit) {
      // reach the distance limit, prune this vertex
      return true;
    }
    // does not reach the limit, do not prune
    return false;
  };
  return Bfs<bidirectional>(graph,
                src_vertex_handle_set, 
                user_callback, 
      prune_distance_callback); 
}

template<bool bidirectional = false,
         typename GraphType,
         typename UserCallBackType>
inline size_t Bfs(GraphType& graph,
        const typename VertexHandle<GraphType>::type& src_vertex_handle,
           UserCallBackType user_callback,
           int distance_limit = -1) {
  using VertexHandleType = typename VertexHandle<GraphType>::type;
  std::set<VertexHandleType> src_vertex_handle_set 
                          = {src_vertex_handle};
  return Bfs<bidirectional>(graph, src_vertex_handle_set, user_callback, distance_limit);
}

template<bool bidirectional = false,
         typename GraphType>
inline size_t Bfs(GraphType& graph,
        const typename VertexHandle<GraphType>::type& src_vertex_handle,
           int distance_limit = -1) {

  using VertexHandleType = typename VertexHandle<GraphType>::type;
  auto do_nothing_callback = [](VertexHandleType vertex_handle, 
                                typename GraphType::VertexCounterType current_distance){
    // do nothing, continue matching
    return true;
  };
  return Bfs<bidirectional>(graph, src_vertex_handle, do_nothing_callback, distance_limit);
}

}  // namespace GUNDAM

#endif // _BFS_H
