#ifndef _GUNDAM_ALGORITHM_DPISO_H
#define _GUNDAM_ALGORITHM_DPISO_H
#include <omp.h>

#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <type_traits>
#include <vector>
#include <limits>

#include "assert.h"
#include "gundam/type_getter/edge_handle.h"
#include "gundam/type_getter/vertex_handle.h"
#include "match_helper.h"
#include "match_semantics.h"

namespace GUNDAM {

namespace _dp_iso {
enum EdgeState { kIn, kOut };
// if query.CountEdge()>=large_query_edge, using fail set
// small query not using fail set
constexpr size_t large_query_edge = 6;
constexpr size_t adj_vertex_limit = 200000;

template <typename GraphType>
size_t CountInVertex(
    typename VertexHandle<GraphType>::type &vertex_handle,
    const typename GraphType::EdgeType::LabelType &edge_label) {
  if constexpr (GraphType::vertex_has_edge_label_index) {
    return vertex_handle->CountInVertex(edge_label);
  } else {
    std::set<typename VertexHandle<GraphType>::type> vertex_set;
    for (auto edge_it = vertex_handle->InEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_it->label() != edge_label) {
        continue;
      }
      vertex_set.emplace(edge_it->src_handle());
    }
    return vertex_set.size();
  }
  return 0;
}

template <typename GraphType>
size_t CountOutVertex(
    typename VertexHandle<GraphType>::type &vertex_handle,
    const typename GraphType::EdgeType::LabelType &edge_label) {
  if constexpr (GraphType::vertex_has_edge_label_index) {
    return vertex_handle->CountOutVertex(edge_label);
  } else {
    std::set<typename VertexHandle<GraphType>::type> vertex_set;
    for (auto edge_it = vertex_handle->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_it->label() != edge_label) {
        continue;
      }
      vertex_set.emplace(edge_it->dst_handle());
    }
    return vertex_set.size();
  }
  return 0;
}

template <typename GraphType>
size_t CountInEdge(typename VertexHandle<GraphType>::type &vertex_handle,
                   const typename GraphType::EdgeType::LabelType &edge_label) {
  if constexpr (GraphType::vertex_has_edge_label_index) {
    return vertex_handle->CountInEdge(edge_label);
  } else {
    size_t counter = 0;
    for (auto edge_it = vertex_handle->InEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_it->label() == edge_label) {
        counter++;
      }
    }
    return counter;
  }
  return 0;
}

template <typename GraphType>
size_t CountOutEdge(typename VertexHandle<GraphType>::type &vertex_handle,
                    const typename GraphType::EdgeType::LabelType &edge_label) {
  if constexpr (GraphType::vertex_has_edge_label_index) {
    return vertex_handle->CountOutEdge(edge_label);
  } else {
    size_t counter = 0;
    for (auto edge_it = vertex_handle->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_it->label() == edge_label) {
        counter++;
      }
    }
    return counter;
  }
  return 0;
}

template <typename GraphType>
size_t CountInEdge(
    typename VertexHandle<GraphType>::type &vertex_handle,
    const typename GraphType::EdgeType::LabelType &edge_label,
    const typename VertexHandle<GraphType>::type &src_vertex_handle) {
  if constexpr (GraphType::vertex_has_edge_label_index) {
    return vertex_handle->CountInEdge(edge_label, src_vertex_handle);
  } else {
    size_t counter = 0;
    for (auto edge_it = vertex_handle->InEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_it->label() == edge_label &&
          edge_it->src_handle() == src_vertex_handle) {
        counter++;
      }
    }
    return counter;
  }
  return 0;
}

template <typename GraphType>
size_t CountOutEdge(
    typename VertexHandle<GraphType>::type &vertex_handle,
    const typename GraphType::EdgeType::LabelType &edge_label,
    const typename VertexHandle<GraphType>::type &dst_vertex_handle) {
  if constexpr (GraphType::vertex_has_edge_label_index) {
    return vertex_handle->CountOutEdge(edge_label, dst_vertex_handle);
  } else {
    size_t counter = 0;
    for (auto edge_it = vertex_handle->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_it->label() == edge_label &&
          edge_it->dst_handle() == dst_vertex_handle) {
        counter++;
      }
    }
    return counter;
  }
  return 0;
}

template <typename QueryVertexHandle, typename TargetVertexHandle>
inline bool DegreeFiltering(QueryVertexHandle &query_vertex_handle,
                            TargetVertexHandle &target_vertex_handle) {
  if (query_vertex_handle->CountInEdge() > target_vertex_handle->CountInEdge())
    return false;
  if (query_vertex_handle->CountOutEdge() >
      target_vertex_handle->CountOutEdge())
    return false;
  if (query_vertex_handle->CountInVertex() >
      target_vertex_handle->CountInVertex())
    return false;
  if (query_vertex_handle->CountOutVertex() >
      target_vertex_handle->CountOutVertex())
    return false;
  return true;
}

template <enum EdgeState edge_state, typename QueryGraph, typename TargetGraph>
inline bool NeighborLabelFrequencyFilteringSingleDirection(
    typename VertexHandle<QueryGraph>::type &query_vertex_handle,
    typename VertexHandle<TargetGraph>::type &target_vertex_handle) {
  for (auto edge_label_it =
           (edge_state == EdgeState::kIn ? query_vertex_handle->InEdgeBegin()
                                         : query_vertex_handle->OutEdgeBegin());
       !edge_label_it.IsDone(); edge_label_it++) {
    auto query_count = (edge_state == EdgeState::kIn
                            ? _dp_iso::CountInVertex<QueryGraph>(
                                  query_vertex_handle, edge_label_it->label())
                            : _dp_iso::CountOutVertex<QueryGraph>(
                                  query_vertex_handle, edge_label_it->label()));
    //  ? query_vertex_handle-> CountInVertex(edge_label_it->label())
    //  : query_vertex_handle->CountOutVertex(edge_label_it->label()));
    auto target_count =
        (edge_state == EdgeState::kIn
             ? _dp_iso::CountInVertex<TargetGraph>(target_vertex_handle,
                                                   edge_label_it->label())
             : _dp_iso::CountOutVertex<TargetGraph>(target_vertex_handle,
                                                    edge_label_it->label()));
    //  ? target_vertex_handle-> CountInVertex(edge_label_it->label())
    //  : target_vertex_handle->CountOutVertex(edge_label_it->label()));
    assert(query_count >= 0 && target_count >= 0);
    if (query_count > target_count) {
      return false;
    }
    query_count = (edge_state == EdgeState::kIn
                       ? _dp_iso::CountInEdge<QueryGraph>(
                             query_vertex_handle, edge_label_it->label())
                       : _dp_iso::CountOutEdge<QueryGraph>(
                             query_vertex_handle, edge_label_it->label()));
    //  ? query_vertex_handle-> CountInEdge(edge_label_it->label())
    //  : query_vertex_handle->CountOutEdge(edge_label_it->label()));
    target_count = (edge_state == EdgeState::kIn
                        ? _dp_iso::CountInEdge<TargetGraph>(
                              target_vertex_handle, edge_label_it->label())
                        : _dp_iso::CountOutEdge<TargetGraph>(
                              target_vertex_handle, edge_label_it->label()));
    //  ? target_vertex_handle-> CountInEdge(edge_label_it->label())
    //  : target_vertex_handle->CountOutEdge(edge_label_it->label()));
    assert(query_count >= 0 && target_count >= 0);
    if (query_count > target_count) {
      return false;
    }
  }
  return true;
}

template <typename QueryGraph, typename TargetGraph>
inline bool NeighborLabelFrequencyFiltering(
    typename VertexHandle<QueryGraph>::type &query_vertex_handle,
    typename VertexHandle<TargetGraph>::type &target_vertex_handle) {
  if (!NeighborLabelFrequencyFilteringSingleDirection<EdgeState::kIn,
                                                      QueryGraph, TargetGraph>(
          query_vertex_handle, target_vertex_handle)) {
    return false;
  }
  if (!NeighborLabelFrequencyFilteringSingleDirection<EdgeState::kOut,
                                                      QueryGraph, TargetGraph>(
          query_vertex_handle, target_vertex_handle)) {
    return false;
  }
  return true;
}

template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph>
inline bool InitCandidateSet(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using CandidateContainer =
      std::vector<typename VertexHandle<TargetGraph>::type>;
  candidate_set.clear();
  for (auto query_vertex_iter = query_graph.VertexBegin();
       !query_vertex_iter.IsDone(); query_vertex_iter++) {
    QueryVertexHandle query_vertex_handle = query_vertex_iter;
    CandidateContainer query_vertex_candidate;
    if constexpr (TargetGraph::graph_has_vertex_label_index) {
      // TargetGraph has graph.VertexBegin(vertex_label) method
      for (auto target_vertex_iter =
               target_graph.VertexBegin(query_vertex_handle->label());
           !target_vertex_iter.IsDone(); target_vertex_iter++) {
        TargetVertexHandle target_vertex_handle = target_vertex_iter;
        if (!DegreeFiltering(query_vertex_handle, target_vertex_handle)) {
          continue;
        }
        if (!NeighborLabelFrequencyFiltering<QueryGraph, TargetGraph>(
                query_vertex_handle, target_vertex_handle)) {
          continue;
        }
        query_vertex_candidate.emplace_back(target_vertex_handle);
      }
    } else {
      // TargetGraph does not have graph.VertexBegin(vertex_label) method
      for (auto target_vertex_iter = target_graph.VertexBegin();
           !target_vertex_iter.IsDone(); target_vertex_iter++) {
        if (target_vertex_iter->label() != query_vertex_handle->label()) {
          continue;
        }
        TargetVertexHandle target_vertex_handle = target_vertex_iter;
        if (!DegreeFiltering(query_vertex_handle, target_vertex_handle)) {
          continue;
        }
        if (!NeighborLabelFrequencyFiltering<QueryGraph, TargetGraph>(
                query_vertex_handle, target_vertex_handle)) {
          continue;
        }
        query_vertex_candidate.emplace_back(target_vertex_handle);
      }
    }
    if (query_vertex_candidate.empty()) return false;
    assert(candidate_set.find(query_vertex_handle) == candidate_set.end());
    candidate_set.emplace(query_vertex_handle,
                          std::move(query_vertex_candidate));
  }
  for (auto &[query_vertex_handle, query_vertex_candidate] : candidate_set) {
    std::stable_sort(query_vertex_candidate.begin(),
                     query_vertex_candidate.end());
  }
  return true;
}

template <enum EdgeState edge_state, typename QueryVertexHandle,
          typename TargetVertexHandle>
inline void GetAdjNotMatchedVertexOneDirection(
    QueryVertexHandle &query_vertex_handle,
    std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> &candidate_set,
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    std::set<QueryVertexHandle> &next_query_set) {
  for (auto edge_iter =
           (edge_state == EdgeState::kIn ? query_vertex_handle->InEdgeBegin()
                                         : query_vertex_handle->OutEdgeBegin());
       !edge_iter.IsDone(); edge_iter++) {
    QueryVertexHandle query_opp_vertex_handle = edge_state == EdgeState::kIn
                                                    ? edge_iter->src_handle()
                                                    : edge_iter->dst_handle();
    if (match_state.count(query_opp_vertex_handle) == 0 &&
        candidate_set.count(query_opp_vertex_handle)) {
      next_query_set.emplace(query_opp_vertex_handle);
    }
  }
  return;
}

template <typename QueryVertexHandle, typename TargetVertexHandle>
inline void GetAdjNotMatchedVertex(
    QueryVertexHandle query_vertex_handle,
    std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> &candidate_set,
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    std::set<QueryVertexHandle> &next_query_set) {
  GetAdjNotMatchedVertexOneDirection<EdgeState::kOut>(
      query_vertex_handle, candidate_set, match_state, next_query_set);
  GetAdjNotMatchedVertexOneDirection<EdgeState::kIn>(
      query_vertex_handle, candidate_set, match_state, next_query_set);
}
template <typename QueryVertexHandle, typename TargetVertexHandle>
inline QueryVertexHandle NextMatchVertex(
    std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> &candidate_set,
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state) {
  std::set<QueryVertexHandle> next_query_set;
  for (auto &[query_vertex_handle, target_vertex_handle] : match_state) {
    auto temp_query_vertex_handle = query_vertex_handle;
    GetAdjNotMatchedVertex(query_vertex_handle, candidate_set, match_state,
                           next_query_set);
  }
  if (next_query_set.empty()) {
    for (auto &[query_vertex_handle, query_vertex_candidate] : candidate_set) {
      if (match_state.count(query_vertex_handle) == 0) {
        next_query_set.emplace(query_vertex_handle);
      }
    }
  }

  if (next_query_set.empty()) {
    return QueryVertexHandle();
  }

  QueryVertexHandle res = QueryVertexHandle();
  assert(!res);
  size_t min = std::numeric_limits<size_t>::max();
  for (auto &query_vertex_handle : next_query_set) {
    assert(candidate_set.count(query_vertex_handle) > 0);
    size_t candidate_count =
        candidate_set.find(query_vertex_handle)->second.size();
    if (candidate_count < min) {
      res = query_vertex_handle;
      min = candidate_count;
    }
  }
  assert(min >= 0 && min < std::numeric_limits<size_t>::max());
  assert(res);
  return res;
}

template <enum EdgeState edge_state, typename QueryGraph, typename TargetGraph>
inline bool JoinableCheck(
    typename VertexHandle<QueryGraph>::type query_vertex_handle,
    typename VertexHandle<TargetGraph>::type target_vertex_handle,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using QueryEdgeHandle = typename EdgeHandle<QueryGraph>::type;
  using TargetEdgeHandle = typename EdgeHandle<TargetGraph>::type;
  std::set<typename TargetGraph::EdgeType::IDType> used_edge;
  for (auto query_edge_iter = ((edge_state == EdgeState::kIn)
                                   ? query_vertex_handle->InEdgeBegin()
                                   : query_vertex_handle->OutEdgeBegin());
       !query_edge_iter.IsDone(); query_edge_iter++) {
    QueryEdgeHandle query_edge_handle = query_edge_iter;
    QueryVertexHandle query_opp_vertex_handle =
        (edge_state == EdgeState::kIn) ? query_edge_handle->src_handle()
                                       : query_edge_handle->dst_handle();
    auto match_iter = match_state.find(query_opp_vertex_handle);
    if (match_iter == match_state.end()) continue;

    TargetVertexHandle query_opp_match_vertex_handle = match_iter->second;

    bool find_target_flag = false;
    if constexpr (TargetGraph::vertex_has_edge_label_index) {
      // vertex has vertex_handle->In/Out EdgeBegin(edge_label, vertex_handle)
      // method
      for (auto target_edge_iter = ((edge_state == EdgeState::kIn)
                                        ? target_vertex_handle->InEdgeBegin(
                                              query_edge_handle->label(),
                                              query_opp_match_vertex_handle)
                                        : target_vertex_handle->OutEdgeBegin(
                                              query_edge_handle->label(),
                                              query_opp_match_vertex_handle));
           !target_edge_iter.IsDone(); target_edge_iter++) {
        if (used_edge.count(target_edge_iter->id())) continue;
        find_target_flag = true;
        used_edge.insert(target_edge_iter->id());
        break;
      }
    } else {
      // vertex does not have vertex_handle->In/Out EdgeBegin(edge_label,
      // vertex_handle) method
      for (auto target_edge_iter = (edge_state == EdgeState::kIn)
                                       ? target_vertex_handle->InEdgeBegin()
                                       : target_vertex_handle->OutEdgeBegin();
           !target_edge_iter.IsDone(); target_edge_iter++) {
        if (target_edge_iter->label() != query_edge_handle->label()) {
          continue;
        }

        auto opp_vertex_handle = (edge_state == EdgeState::kIn)
                                     ? target_edge_iter->src_handle()
                                     : target_edge_iter->dst_handle();
        if (query_opp_match_vertex_handle != opp_vertex_handle) {
          continue;
        }

        if (used_edge.count(target_edge_iter->id())) continue;
        find_target_flag = true;
        used_edge.insert(target_edge_iter->id());
        break;
      }
    }
    if (!find_target_flag) return false;
  }
  return true;
}

template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph>
inline bool IsJoinable(
    typename VertexHandle<QueryGraph>::type query_vertex_handle,
    typename VertexHandle<TargetGraph>::type target_vertex_handle,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    std::set<typename VertexHandle<TargetGraph>::type> &target_matched) {
  if (match_semantics == MatchSemantics::kIsomorphism &&
      target_matched.count(target_vertex_handle) > 0) {
    return false;
  }
  if (!JoinableCheck<EdgeState::kIn, QueryGraph, TargetGraph>(
          query_vertex_handle, target_vertex_handle, match_state)) {
    return false;
  }
  if (!JoinableCheck<EdgeState::kOut, QueryGraph, TargetGraph>(
          query_vertex_handle, target_vertex_handle, match_state)) {
    return false;
  }
  return true;
}

template <typename QueryVertexHandle, typename TargetVertexHandle>
inline void UpdateState(
    QueryVertexHandle query_vertex_handle,
    TargetVertexHandle target_vertex_handle,
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    std::set<TargetVertexHandle> &target_matched) {
  assert(match_state.count(query_vertex_handle) == 0);
  match_state.emplace(query_vertex_handle, target_vertex_handle);
  target_matched.emplace(target_vertex_handle);
  return;
}

template <enum EdgeState edge_state, typename QueryGraph, typename TargetGraph>
inline void UpdateCandidateSetOneDirection(
    typename VertexHandle<QueryGraph>::type query_vertex_handle,
    typename VertexHandle<TargetGraph>::type target_vertex_handle,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    std::set<typename VertexHandle<TargetGraph>::type> &target_matched) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using CandidateContainerType =
      std::vector<typename VertexHandle<TargetGraph>::type>;
  std::set<typename QueryGraph::EdgeType::LabelType> used_edge_label;

  for (auto label_it = ((edge_state == EdgeState::kIn)
                            ? query_vertex_handle->InEdgeBegin()
                            : query_vertex_handle->OutEdgeBegin());
       !label_it.IsDone(); label_it++) {
    QueryVertexHandle temp_handle = (edge_state == EdgeState::kIn)
                                        ? label_it->src_handle()
                                        : label_it->dst_handle();
    if (match_state.count(temp_handle) > 0) continue;
    if (candidate_set.count(temp_handle) == 0) continue;
    //枚举label
    if (used_edge_label.count(label_it->label()) > 0) continue;
    used_edge_label.insert(label_it->label());
    std::map<typename TargetGraph::VertexType::LabelType,
             std::vector<TargetVertexHandle>>
        temp_adj_vertex;
    size_t adj_count = (edge_state == EdgeState::kIn)
                           ? _dp_iso::CountInVertex<TargetGraph>(
                                 target_vertex_handle, label_it->label())
                           : _dp_iso::CountOutVertex<TargetGraph>(
                                 target_vertex_handle, label_it->label());
    // ? target_vertex_handle->CountInVertex(label_it->label())
    // : target_vertex_handle->CountOutVertex(label_it->label());
    if (adj_count > adj_vertex_limit) {
      continue;
    }
    if constexpr (TargetGraph::vertex_has_edge_label_index) {
      // vertex has vertex_handle->In/Out VertexBegin(edge_label)
      // method
      for (auto vertex_it =
               ((edge_state == EdgeState::kIn)
                    ? target_vertex_handle->InVertexBegin(label_it->label())
                    : target_vertex_handle->OutVertexBegin(label_it->label()));
           !vertex_it.IsDone(); vertex_it++) {
        TargetVertexHandle temp_target_handle = vertex_it;
        auto temp_target_handle_it =
            temp_adj_vertex.find(temp_target_handle->label());
        if (temp_target_handle_it == temp_adj_vertex.end()) {
          CandidateContainerType target_handle_candidate{temp_target_handle};
          temp_adj_vertex.emplace_hint(temp_target_handle_it,
                                       temp_target_handle->label(),
                                       std::move(target_handle_candidate));
          continue;
        }
        temp_target_handle_it->second.emplace_back(temp_target_handle);
      }
    } else {
      std::set<TargetVertexHandle> visited_target_handle_set;
      // vertex does not have vertex_handle->In/Out
      // VertexBegin(edge_label) method
      for (auto edge_it = ((edge_state == EdgeState::kIn)
                               ? target_vertex_handle->InEdgeBegin()
                               : target_vertex_handle->OutEdgeBegin());
           !edge_it.IsDone(); edge_it++) {
        if (edge_it->label() != label_it->label()) {
          continue;
        }
        TargetVertexHandle temp_target_handle =
            (edge_state == EdgeState::kIn) ? edge_it->src_handle()   //  in edge
                                           : edge_it->dst_handle();  // out edge
        // there might be two edges with same label point to the same vertex
        auto [visited_target_handle_set_it, visited_target_handle_set_ret] =
            visited_target_handle_set.emplace(temp_target_handle);
        if (!visited_target_handle_set_ret) {
          // this vertex has already been visited
          continue;
        }
        auto temp_target_handle_it =
            temp_adj_vertex.find(temp_target_handle->label());
        if (temp_target_handle_it == temp_adj_vertex.end()) {
          CandidateContainerType target_handle_candidate{temp_target_handle};
          temp_adj_vertex.emplace_hint(temp_target_handle_it,
                                       temp_target_handle->label(),
                                       std::move(target_handle_candidate));
          continue;
        }
        temp_target_handle_it->second.emplace_back(temp_target_handle);
      }
    }
    for (auto &[vertex_label, vertex_label_candidate] : temp_adj_vertex) {
      std::sort(vertex_label_candidate.begin(), vertex_label_candidate.end());
      auto erase_it = std::unique(vertex_label_candidate.begin(),
                                  vertex_label_candidate.end());
      vertex_label_candidate.erase(erase_it, vertex_label_candidate.end());
    }
    std::set<QueryVertexHandle> used_vertex;
    for (auto vertex_it = ((edge_state == EdgeState::kIn)
                               ? query_vertex_handle->InEdgeBegin()
                               : query_vertex_handle->OutEdgeBegin());
         !vertex_it.IsDone(); vertex_it++) {
      //枚举Vertex
      if (vertex_it->label() != label_it->label()) continue;

      QueryVertexHandle temp_vertex_handle = (edge_state == EdgeState::kIn)
                                                 ? vertex_it->src_handle()
                                                 : vertex_it->dst_handle();
      if (used_vertex.count(temp_vertex_handle) > 0) continue;
      if (candidate_set.count(temp_vertex_handle) == 0) continue;
      used_vertex.insert(temp_vertex_handle);
      std::vector<TargetVertexHandle> res_candidate;
      if (match_state.count(temp_vertex_handle) > 0) continue;
      if (temp_adj_vertex.count(temp_vertex_handle->label()) == 0) {
        // intersection result is empty
        candidate_set.find(temp_vertex_handle)->second.clear();
      } else {
        std::set_intersection(
            temp_adj_vertex.find(temp_vertex_handle->label())->second.begin(),
            temp_adj_vertex.find(temp_vertex_handle->label())->second.end(),
            candidate_set.find(temp_vertex_handle)->second.begin(),
            candidate_set.find(temp_vertex_handle)->second.end(),
            inserter(res_candidate, res_candidate.begin()));
        std::swap(candidate_set.find(temp_vertex_handle)->second,
                  res_candidate);
      }
    }
  }
  return;
}

template <typename QueryGraph, typename TargetGraph>
inline void UpdateCandidateSet(
    typename VertexHandle<QueryGraph>::type query_vertex_handle,
    typename VertexHandle<TargetGraph>::type target_vertex_handle,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    std::set<typename VertexHandle<TargetGraph>::type> &target_matched) {
  UpdateCandidateSetOneDirection<EdgeState::kIn, QueryGraph, TargetGraph>(
      query_vertex_handle, target_vertex_handle, candidate_set, match_state,
      target_matched);
  UpdateCandidateSetOneDirection<EdgeState::kOut, QueryGraph, TargetGraph>(
      query_vertex_handle, target_vertex_handle, candidate_set, match_state,
      target_matched);
  return;
}

template <typename QueryVertexHandle, typename TargetVertexHandle>
inline void RestoreState(
    QueryVertexHandle query_vertex_handle,
    TargetVertexHandle target_vertex_handle,
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    std::set<TargetVertexHandle> &target_matched) {
  assert(match_state.count(query_vertex_handle) > 0 &&
         target_matched.count(target_vertex_handle) > 0);
  match_state.erase(query_vertex_handle);
  target_matched.erase(target_vertex_handle);
  return;
}

template <typename QueryVertexHandle, typename TargetVertexHandle,
          typename ResultContainer>
inline bool MatchCallbackSaveSupp(
    std::map<QueryVertexHandle, TargetVertexHandle> &m,
    QueryVertexHandle *query_vertex_handle, ResultContainer *r) {
  if (r) {
    r->insert(m[*query_vertex_handle]);
  }
  return true;
}

template <typename QueryVertexHandle, typename TargetVertexHandle,
          typename SuppContainer>
inline bool PruneSuppCallback(
    std::map<QueryVertexHandle, TargetVertexHandle> &m,
    QueryVertexHandle *query_vertex_handle, SuppContainer *r) {
  if (r) {
    auto it = m.find(*query_vertex_handle);
    if (it == m.end()) {
      return false;
    }
    TargetVertexHandle target_vertex_handle = it->second;
    if (r->count(target_vertex_handle)) {
      return true;
    }
  }
  return false;
}

template <typename QueryVertexHandle, typename TargetVertexHandle>
inline bool PruneCallbackEmpty(
    std::map<QueryVertexHandle, TargetVertexHandle> &m) {
  return false;
}

template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, typename MatchCallback, typename PruneCallback>
bool _DPISO(std::map<typename VertexHandle<QueryGraph>::type,
                     std::vector<typename VertexHandle<TargetGraph>::type>>
                &candidate_set,
            std::map<typename VertexHandle<QueryGraph>::type,
                     typename VertexHandle<TargetGraph>::type> &match_state,
            std::set<typename VertexHandle<TargetGraph>::type> &target_matched,
            MatchCallback user_callback, PruneCallback prune_callback,
            time_t begin_time, double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  if (query_limit_time > 0 &&
      ((std::time(NULL) - begin_time)) > query_limit_time) {
    return false;
  }
  if constexpr (!std::is_null_pointer_v<PruneCallback>) {
    if (prune_callback(match_state)) {
      return true;
    }
  }

  if (match_state.size() == candidate_set.size()) {
    if constexpr (!std::is_null_pointer_v<MatchCallback>) {
      return user_callback(match_state);
    } else {
      // user callback is nullptr,so go on search
      return true;
    }
  }
  assert(match_state.size() < candidate_set.size());
  QueryVertexHandle next_query_vertex_handle =
      NextMatchVertex(candidate_set, match_state);
  assert(next_query_vertex_handle);
  assert(match_state.count(next_query_vertex_handle) == 0);
  for (TargetVertexHandle &next_target_vertex_handle :
       candidate_set.find(next_query_vertex_handle)->second) {
    if constexpr (!std::is_null_pointer_v<PruneCallback>) {
      if (prune_callback(match_state)) {
        return true;
      }
    }
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            next_query_vertex_handle, next_target_vertex_handle, match_state,
            target_matched)) {
      std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
          temp_candidate_set{candidate_set};
      UpdateState(next_query_vertex_handle, next_target_vertex_handle,
                  match_state, target_matched);
      UpdateCandidateSet<QueryGraph, TargetGraph>(
          next_query_vertex_handle, next_target_vertex_handle,
          temp_candidate_set, match_state, target_matched);
      if (!_DPISO<match_semantics, QueryGraph, TargetGraph>(
              temp_candidate_set, match_state, target_matched, user_callback,
              prune_callback, begin_time, query_limit_time)) {
        return false;
      }
      RestoreState(next_query_vertex_handle, next_target_vertex_handle,
                   match_state, target_matched);
    }
  }
  return true;
}

template <enum EdgeState edge_state, class QueryVertexHandle,
          class TargetVertexHandle>
void UpdateParentOneDirection(
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    QueryVertexHandle update_query_vertex_handle,
    std::map<QueryVertexHandle, std::vector<QueryVertexHandle>> &parent) {
  for (auto edge_it = (edge_state == EdgeState::kIn
                           ? update_query_vertex_handle->InEdgeBegin()
                           : update_query_vertex_handle->OutEdgeBegin());
       !edge_it.IsDone(); edge_it++) {
    QueryVertexHandle update_query_adj_handle = edge_state == EdgeState::kIn
                                                    ? edge_it->src_handle()
                                                    : edge_it->dst_handle();
    if (match_state.find(update_query_adj_handle) == match_state.end())
      continue;
    assert(parent.find(update_query_vertex_handle) != parent.end());
    auto &query_parent_set = parent.find(update_query_vertex_handle)->second;
    auto find_it = parent.find(update_query_adj_handle);
    if (find_it != parent.end()) {
      for (auto &it : find_it->second) {
        query_parent_set.push_back(it);
      }
    }
  }
  return;
}

template <class QueryVertexHandle, class TargetVertexHandle>
void UpdateParent(
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    QueryVertexHandle update_query_vertex_handle,
    std::map<QueryVertexHandle, std::vector<QueryVertexHandle>> &parent) {
  std::vector<QueryVertexHandle> update_query_vertex_parent{
      update_query_vertex_handle};
  assert(parent.count(update_query_vertex_handle) == 0);
  parent.emplace(update_query_vertex_handle,
                 std::move(update_query_vertex_parent));
  UpdateParentOneDirection<EdgeState::kIn>(match_state,
                                           update_query_vertex_handle, parent);
  UpdateParentOneDirection<EdgeState::kOut>(match_state,
                                            update_query_vertex_handle, parent);
  auto &l = parent.find(update_query_vertex_handle)->second;
  std::sort(l.begin(), l.end());
  auto erase_it = std::unique(l.begin(), l.end());
  l.erase(erase_it, l.end());
  return;
}

// using Fail Set
template <enum MatchSemantics match_semantics, class QueryGraph,
          typename TargetGraph, class MatchCallback, class PruneCallback>
bool _DPISO(
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    std::set<typename VertexHandle<TargetGraph>::type> &target_matched,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<QueryGraph>::type>> &parent,
    std::vector<typename VertexHandle<QueryGraph>::type> &fail_set,
    MatchCallback user_callback, PruneCallback prune_callback,
    time_t begin_time, double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  if (query_limit_time > 0 &&
      ((std::time(NULL) - begin_time)) > query_limit_time) {
    return false;
  }
  if constexpr (!std::is_null_pointer_v<PruneCallback>) {
    if (prune_callback(match_state)) {
      return true;
    }
  }
  if (match_state.size() == candidate_set.size()) {
    // find match ,so fail set is empty
    fail_set.clear();
    if constexpr (!std::is_null_pointer_v<MatchCallback>) {
      return user_callback(match_state);
    } else {
      // user callback is nullptr,so go on search
      return true;
    }
  }
  assert(match_state.size() < candidate_set.size());
  QueryVertexHandle next_query_vertex_handle =
      NextMatchVertex(candidate_set, match_state);
  assert(next_query_vertex_handle);
  assert(match_state.count(next_query_vertex_handle) == 0);
  // cal this vertex's  parent
  UpdateParent(match_state, next_query_vertex_handle, parent);
  std::vector<QueryVertexHandle> this_state_fail_set;
  bool find_fail_set_flag = false;
  if (candidate_set.find(next_query_vertex_handle)->second.size() == 0) {
    // C(u) is empty ,so fail set = anc(u)
    this_state_fail_set = parent.find(next_query_vertex_handle)->second;
  }
  for (TargetVertexHandle &next_target_vertex_handle :
       candidate_set.find(next_query_vertex_handle)->second) {
    if constexpr (!std::is_null_pointer_v<PruneCallback>) {
      if (prune_callback(match_state)) {
        return true;
      }
    }
    if (find_fail_set_flag && !this_state_fail_set.empty() &&
        !std::binary_search(this_state_fail_set.begin(),
                            this_state_fail_set.end(),
                            next_query_vertex_handle)) {
      // find fail set , u is not in fail set and fail set is not empty!
      // so not expand
      std::swap(fail_set, this_state_fail_set);
      return true;
    }
    if (!find_fail_set_flag &&
        match_semantics == MatchSemantics::kIsomorphism &&
        target_matched.count(next_target_vertex_handle) > 0) {
      // find u' satisfy that match_state[u']=next_target_vertex_handle
      // next_state_fail_set = anc[u] union anc[u']
      std::vector<QueryVertexHandle> next_state_fail_set;
      for (auto &[query_ptr, target_ptr] : match_state) {
        if (target_ptr == next_target_vertex_handle) {
          std::vector<QueryVertexHandle> temp_next_state_fail_set;
          std::set_union(parent.find(next_query_vertex_handle)->second.begin(),
                         parent.find(next_query_vertex_handle)->second.end(),
                         parent.find(query_ptr)->second.begin(),
                         parent.find(query_ptr)->second.end(),
                         inserter(temp_next_state_fail_set,
                                  temp_next_state_fail_set.begin()));
          std::swap(next_state_fail_set, temp_next_state_fail_set);
          break;
        }
      }
      // update this_state_fail_set
      // anc[u] must contain u ,so union fail set
      std::vector<QueryVertexHandle> temp_this_state_fail_set;
      std::set_union(
          next_state_fail_set.begin(), next_state_fail_set.end(),
          this_state_fail_set.begin(), this_state_fail_set.end(),
          inserter(temp_this_state_fail_set, temp_this_state_fail_set.begin()));
      std::swap(this_state_fail_set, temp_this_state_fail_set);
    }
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            next_query_vertex_handle, next_target_vertex_handle, match_state,
            target_matched)) {
      std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
          temp_candidate_set{candidate_set};
      UpdateState(next_query_vertex_handle, next_target_vertex_handle,
                  match_state, target_matched);
      UpdateCandidateSet<QueryGraph, TargetGraph>(
          next_query_vertex_handle, next_target_vertex_handle,
          temp_candidate_set, match_state, target_matched);
      std::vector<QueryVertexHandle> next_state_fail_set;
      if (!_DPISO<match_semantics, QueryGraph, TargetGraph>(
              temp_candidate_set, match_state, target_matched, parent,
              next_state_fail_set, user_callback, prune_callback, begin_time,
              query_limit_time)) {
        return false;
      }

      RestoreState(next_query_vertex_handle, next_target_vertex_handle,
                   match_state, target_matched);

      if (next_state_fail_set.empty()) {
        // if ont child node's fail set is empty
        // so this state's fail set is empty
        find_fail_set_flag = true;
        this_state_fail_set.clear();
      } else if (!std::binary_search(next_state_fail_set.begin(),
                                     next_state_fail_set.end(),
                                     next_query_vertex_handle)) {
        // if one child node's fail set not contain next_query_vertex_handle
        // so this state's fail set is next_state_fail_set
        find_fail_set_flag = true;
        this_state_fail_set = next_state_fail_set;
      } else if (!find_fail_set_flag) {
        std::vector<QueryVertexHandle> temp_this_state_fail_set;
        std::set_union(next_state_fail_set.begin(), next_state_fail_set.end(),
                       this_state_fail_set.begin(), this_state_fail_set.end(),
                       inserter(temp_this_state_fail_set,
                                temp_this_state_fail_set.begin()));
        std::swap(temp_this_state_fail_set, this_state_fail_set);
      }
    }
  }
  std::swap(fail_set, this_state_fail_set);
  return true;
}

template <class QueryVertexHandle, class TargetVertexHandle>
bool UpdateCandidateCallbackEmpty(
    std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set) {
  return true;
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class CandidateSetContainer>
bool UpdateCandidateCallback(
    CandidateSetContainer &candidate_set,
    QueryVertexHandle *query_vertex_handle,
    std::vector<TargetVertexHandle> &possible_candidate) {
  if (possible_candidate.empty()) {
    return true;
  }
  std::vector<TargetVertexHandle> res_candidate;
  auto &query_vertex_candidate = candidate_set[*query_vertex_handle];
  std::set_intersection(query_vertex_candidate.begin(),
                        query_vertex_candidate.end(),
                        possible_candidate.begin(), possible_candidate.end(),
                        inserter(res_candidate, res_candidate.begin()));
  if (res_candidate.empty()) return false;
  query_vertex_candidate = res_candidate;
  return true;
}

template <class QueryVertexHandle, class TargetVertexHandle>
inline bool MatchCallbackLimit(
    std::map<QueryVertexHandle, TargetVertexHandle> &, int *max_result) {
  if (max_result && *max_result > 0) {
    (*max_result)--;
    if (*max_result == 0) return false;
  }
  return true;
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class ResultContainer>
inline bool MatchCallbackSaveResult1(
    std::map<QueryVertexHandle, TargetVertexHandle> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    r->emplace_back(m);
  }

  return MatchCallbackLimit(m, max_result);
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class ResultContainer>
inline bool MatchCallbackSaveResult2(
    std::map<QueryVertexHandle, TargetVertexHandle> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    using ResultType = typename ResultContainer::value_type;
    ResultType tmp;
    for (auto &p : m) {
      tmp.emplace_back(p);
    }
    r->emplace_back(std::move(tmp));
  }

  return MatchCallbackLimit(m, max_result);
}

template <class A, class B>
inline void CopyMap(std::map<A, B> &c, std::map<A, B> &m) {
  c = m;
  return;
};

template <class A, class B>
inline void CopyMap(std::list<std::pair<A, B>> &c, std::map<A, B> &m) {
  for (auto &p : m) {
    c.emplace_back(p);
  }
  return;
};

template <class A, class B>
inline void CopyMap(std::vector<std::pair<A, B>> &c, std::map<A, B> &m) {
  for (auto &p : m) {
    c.emplace_back(p);
  }
  return;
};

template <class A, class B>
inline void CopyMap(std::map<A, B> &c, const std::map<A, B> &m) {
  c = m;
  return;
};

template <class A, class B>
inline void CopyMap(std::list<std::pair<A, B>> &c, const std::map<A, B> &m) {
  for (auto &p : m) {
    c.emplace_back(p);
  }
  return;
};

template <class A, class B>
inline void CopyMap(std::vector<std::pair<A, B>> &c, const std::map<A, B> &m) {
  for (auto &p : m) {
    c.emplace_back(p);
  }
  return;
};

template <class QueryVertexHandle, class TargetVertexHandle,
          class ResultContainer>
inline bool MatchCallbackSaveResult(
    std::map<QueryVertexHandle, TargetVertexHandle> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    using ResultType = typename ResultContainer::value_type;

    ResultType tmp;
    CopyMap(tmp, m);
    r->emplace_back(std::move(tmp));
  }

  return MatchCallbackLimit(m, max_result);
}

namespace _DAGDP {

template <class QueryGraph, class InDegreeContainer>
inline void BFS(QueryGraph &query_graph, InDegreeContainer &in_degree) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using QueryEdgeIDType = typename QueryGraph::EdgeType::IDType;
  std::set<QueryVertexHandle> used_vertex;
  std::set<QueryEdgeIDType> used_edge;
  // bfs get indegree (build dag but not really get new graph)
  for (auto it = query_graph.VertexBegin(); !it.IsDone(); it++) {
    QueryVertexHandle query_vertex_handle = it;
    if (used_vertex.count(query_vertex_handle)) continue;
    std::queue<QueryVertexHandle> bfs_queue;
    in_degree[query_vertex_handle] = 0;
    used_vertex.insert(query_vertex_handle);
    bfs_queue.push(query_vertex_handle);
    while (!bfs_queue.empty()) {
      QueryVertexHandle now_vertex_handle = bfs_queue.front();
      bfs_queue.pop();
      for (auto edge_it = now_vertex_handle->OutEdgeBegin(); !edge_it.IsDone();
           edge_it++) {
        if (used_edge.count(edge_it->id())) continue;
        QueryVertexHandle next_vertex_handle = edge_it->dst_handle();
        used_edge.insert(edge_it->id());
        in_degree[next_vertex_handle]++;
        if (!used_vertex.count(next_vertex_handle)) {
          bfs_queue.push(next_vertex_handle);
          used_vertex.insert(next_vertex_handle);
        }
      }
      for (auto edge_it = now_vertex_handle->InEdgeBegin(); !edge_it.IsDone();
           edge_it++) {
        if (used_edge.count(edge_it->id())) continue;
        QueryVertexHandle next_vertex_handle = edge_it->src_handle();
        used_edge.insert(edge_it->id());
        in_degree[next_vertex_handle]++;
        if (!used_vertex.count(next_vertex_handle)) {
          bfs_queue.push(next_vertex_handle);
          used_vertex.insert(next_vertex_handle);
        }
      }
    }
  }
  return;
}

template <class QueryGraph, class InDegreeContainer, class TopuSeqContainer>
inline void TopuSort(QueryGraph &query_graph, InDegreeContainer &in_degree,
                     TopuSeqContainer &topu_seq) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using QueryEdgeIDType = typename QueryGraph::EdgeType::IDType;
  std::queue<QueryVertexHandle> topu_sort_queue;
  std::set<QueryEdgeIDType> used_edge;
  // topu sort
  for (auto it = query_graph.VertexBegin(); !it.IsDone(); it++) {
    QueryVertexHandle query_vertex_handle = it;
    if (in_degree[query_vertex_handle] == 0) {
      topu_sort_queue.push(query_vertex_handle);
    }
  }
  while (!topu_sort_queue.empty()) {
    QueryVertexHandle query_vertex_handle = topu_sort_queue.front();
    topu_sort_queue.pop();
    topu_seq.emplace_back(query_vertex_handle);
    for (auto edge_it = query_vertex_handle->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (used_edge.count(edge_it->id())) continue;
      QueryVertexHandle next_vertex_handle = edge_it->dst_handle();
      in_degree[next_vertex_handle]--;
      used_edge.insert(edge_it->id());
      if (in_degree[next_vertex_handle] == 0) {
        topu_sort_queue.push(next_vertex_handle);
      }
    }
    for (auto edge_it = query_vertex_handle->InEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      if (used_edge.count(edge_it->id())) continue;
      QueryVertexHandle next_vertex_handle = edge_it->src_handle();
      in_degree[next_vertex_handle]--;
      used_edge.insert(edge_it->id());
      if (in_degree[next_vertex_handle] == 0) {
        topu_sort_queue.push(next_vertex_handle);
      }
    }
  }
  return;
}

template <class QueryGraph, class TopuSeqContainer>
inline void GetTopuSeq(QueryGraph &query_graph, TopuSeqContainer &topu_seq) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using QueryEdgeIDType = typename QueryGraph::EdgeType::IDType;
  std::map<QueryVertexHandle, int> in_degree;
  BFS(query_graph, in_degree);
  assert(in_degree.size() == query_graph.CountVertex());
  TopuSort(query_graph, in_degree, topu_seq);
}

template <class QueryGraph, class TargetGraph, class EdgeCountContainer>
inline bool EdgeCheck(
    typename VertexHandle<QueryGraph>::type &query_vertex_handle,
    typename VertexHandle<QueryGraph>::type &temp_query_vertex_handle,
    typename VertexHandle<TargetGraph>::type &target_vertex_handle,
    typename VertexHandle<TargetGraph>::type &temp_target_vertex_handle,
    EdgeCountContainer &out_edge_count, EdgeCountContainer &in_edge_count) {
  auto it = out_edge_count.find(temp_query_vertex_handle);
  if (it != out_edge_count.end()) {
    for (auto &edge_count : it->second) {
      // if (target_vertex_handle->CountOutEdge(
      //         edge_count.first, temp_target_vertex_handle) <
      //         edge_count.second)
      if (_dp_iso::CountOutEdge<TargetGraph>(
              target_vertex_handle, edge_count.first,
              temp_target_vertex_handle) < edge_count.second)
        return false;
    }
  }
  it = in_edge_count.find(temp_query_vertex_handle);
  if (it != in_edge_count.end()) {
    for (auto &edge_count : it->second) {
      // if (target_vertex_handle->CountInEdge(
      //         edge_count.first, temp_target_vertex_handle) <
      //         edge_count.second)
      if (_dp_iso::CountInEdge<TargetGraph>(
              target_vertex_handle, edge_count.first,
              temp_target_vertex_handle) < edge_count.second)
        return false;
    }
  }
  return true;
}
template <bool is_out_direction, typename QueryGraph, typename TargetGraph,
          class CandidateSetContainer, class EdgeCountContainer>
inline bool CheckIsLegal(
    typename VertexHandle<QueryGraph>::type query_vertex_handle,
    typename VertexHandle<TargetGraph>::type target_vertex_handle,
    std::set<typename VertexHandle<QueryGraph>::type> &used_vertex,
    EdgeCountContainer &out_edge_count, EdgeCountContainer &in_edge_count,
    CandidateSetContainer &candidate_set) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  for (auto edge_it = is_out_direction ? query_vertex_handle->OutEdgeBegin()
                                       : query_vertex_handle->InEdgeBegin();
       !edge_it.IsDone(); edge_it++) {
    QueryVertexHandle next_vertex_handle =
        is_out_direction ? edge_it->dst_handle() : edge_it->src_handle();
    if (!used_vertex.count(next_vertex_handle)) continue;
    auto &l1 = candidate_set.find(next_vertex_handle)->second;
    bool find_flag = false;
    if constexpr (TargetGraph::vertex_has_edge_label_index) {
      // vertex has vertex_handle->In/Out VertexBegin(edge_label)
      // method
      for (auto target_edge_it =
               is_out_direction
                   ? target_vertex_handle->OutVertexBegin(edge_it->label())
                   : target_vertex_handle->InVertexBegin(edge_it->label());
           !target_edge_it.IsDone(); target_edge_it++) {
        TargetVertexHandle temp_target_vertex_handle = target_edge_it;
        if (!EdgeCheck<QueryGraph, TargetGraph>(
                query_vertex_handle, next_vertex_handle, target_vertex_handle,
                temp_target_vertex_handle, out_edge_count, in_edge_count)) {
          continue;
        }
        auto it1 =
            std::lower_bound(l1.begin(), l1.end(), temp_target_vertex_handle);
        if (it1 != l1.end() && (*it1) == temp_target_vertex_handle) {
          find_flag = true;
          break;
        }
      }
    } else {
      // vertex does not have vertex_handle->In/Out VertexBegin(edge_label)
      // method
      for (auto target_edge_it = is_out_direction
                                     ? target_vertex_handle->OutEdgeBegin()
                                     : target_vertex_handle->InEdgeBegin();
           !target_edge_it.IsDone(); target_edge_it++) {
        if (target_edge_it->label() != edge_it->label()) {
          continue;
        }
        TargetVertexHandle temp_target_vertex_handle =
            is_out_direction ? target_edge_it->dst_handle()   // is out edge
                             : target_edge_it->src_handle();  // is  in edge
        if (!EdgeCheck<QueryGraph, TargetGraph>(
                query_vertex_handle, next_vertex_handle, target_vertex_handle,
                temp_target_vertex_handle, out_edge_count, in_edge_count)) {
          continue;
        }
        auto it1 =
            std::lower_bound(l1.begin(), l1.end(), temp_target_vertex_handle);
        if (it1 != l1.end() && (*it1) == temp_target_vertex_handle) {
          find_flag = true;
          break;
        }
      }
    }
    if (!find_flag) return false;
  }
  return true;
}

}  // namespace _DAGDP
template <typename QueryGraph, typename TargetGraph,
          class CandidateSetContainer>
inline bool DAGDP(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::vector<typename VertexHandle<QueryGraph>::type> &topu_seq,
    CandidateSetContainer &candidate_set) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using QueryEdgeLabelType = typename QueryGraph::EdgeType::LabelType;
  std::set<QueryVertexHandle> used_vertex;
  CandidateSetContainer temp_candidate_set;
  for (auto &vertex_handle : topu_seq) {
    used_vertex.insert(vertex_handle);
    auto &l = candidate_set.find(vertex_handle)->second;
    auto &l1 = temp_candidate_set[vertex_handle];
    std::map<QueryVertexHandle, std::map<QueryEdgeLabelType, int>>
        in_edge_count, out_edge_count;
    for (auto edge_it = vertex_handle->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      QueryVertexHandle dst_handle = edge_it->dst_handle();
      if (!used_vertex.count(dst_handle)) {
        continue;
      }
      out_edge_count[dst_handle][edge_it->label()]++;
    }
    for (auto edge_it = vertex_handle->InEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      QueryVertexHandle dst_handle = edge_it->src_handle();
      if (!used_vertex.count(dst_handle)) {
        continue;
      }
      in_edge_count[dst_handle][edge_it->label()]++;
    }
    for (auto &candidate_it : l) {
      bool candidate_flag = true;
      candidate_flag = _DAGDP::CheckIsLegal<true, QueryGraph, TargetGraph>(
          vertex_handle, candidate_it, used_vertex, out_edge_count,
          in_edge_count, candidate_set);
      if (!candidate_flag) {
        continue;
      }
      candidate_flag = _DAGDP::CheckIsLegal<false, QueryGraph, TargetGraph>(
          vertex_handle, candidate_it, used_vertex, out_edge_count,
          in_edge_count, candidate_set);
      if (candidate_flag) {
        l1.emplace_back(candidate_it);
      }
    }
    l = l1;
    if (l.empty()) return false;
  }
  return true;
}

template <typename QueryGraph, typename TargetGraph,
          class CandidateSetContainer>
inline bool RefineCandidateSet(QueryGraph &query_graph,
                               TargetGraph &target_graph,
                               CandidateSetContainer &candidate_set) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  std::vector<QueryVertexHandle> topu_seq;
  _DAGDP::GetTopuSeq(query_graph, topu_seq);
  if (!DAGDP(query_graph, target_graph, topu_seq, candidate_set)) return false;
  constexpr int loop_num = 2;
  for (int i = 1; i <= loop_num; i++) {
    std::reverse(topu_seq.begin(), topu_seq.end());
    if (!DAGDP(query_graph, target_graph, topu_seq, candidate_set))
      return false;
  };
  return true;
}
template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, typename QueryVertexHandle,
          typename TargetVertexHandle>
bool CheckMatchIsLegal(
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state) {
  std::set<TargetVertexHandle> target_matched;
  for (auto &[query_ptr, target_ptr] : match_state) {
    target_matched.insert(target_ptr);
  }
  if (match_semantics == MatchSemantics::kIsomorphism &&
      target_matched.size() != match_state.size()) {
    return false;
  }
  for (auto &[query_ptr, target_ptr] : match_state) {
    if (!_dp_iso::JoinableCheck<EdgeState::kIn, QueryGraph, TargetGraph>(
            query_ptr, target_ptr, match_state)) {
      return false;
    }
    if (!_dp_iso::JoinableCheck<EdgeState::kOut, QueryGraph, TargetGraph>(
            query_ptr, target_ptr, match_state)) {
      return false;
    }
  }
  return true;
}
template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, class MatchCallback, class PruneCallback>
inline int DPISO_Recursive(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200.0) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  std::set<TargetVertexHandle> target_matched;
  if (!CheckMatchIsLegal<match_semantics, QueryGraph, TargetGraph>(
          match_state)) {
    // partial match is not legal.
    return false;
  }
  for (auto &[query_ptr, target_ptr] : match_state) {
    target_matched.insert(target_ptr);
  }
  for (auto &[query_ptr, target_ptr] : match_state) {
    UpdateCandidateSet<QueryGraph, TargetGraph>(
        query_ptr, target_ptr, candidate_set, match_state, target_matched);
  }
  size_t result_count = 0;
  // add lock to user_callback and prune_callback
  omp_lock_t user_callback_lock;
  omp_init_lock(&user_callback_lock);
  bool user_callback_has_return_false = false;
  auto par_user_callback =
      [&result_count, &user_callback, &user_callback_lock,
       &user_callback_has_return_false](auto &match_state) {
        omp_set_lock(&user_callback_lock);
        bool ret_val = false;
        if (!user_callback_has_return_false) {
          ret_val = user_callback(match_state);
          if (!ret_val) {
            user_callback_has_return_false = true;
          }
          result_count++;
        }
        omp_unset_lock(&user_callback_lock);
        return ret_val;
      };
  omp_lock_t prune_callback_lock;
  omp_init_lock(&prune_callback_lock);
  auto par_prune_callback =
      [&prune_callback_lock, &prune_callback,
       //  &user_callback_lock,
       &user_callback_has_return_false](auto &match_state) {
        // it might be unnecessary to set the lock here
        // user_callback_lock is read-only in this callback
        // and can only be set from false to true
        // omp_set_lock(&user_callback_lock);
        if (user_callback_has_return_false) {
          return true;
        }
        // omp_unset_lock(&user_callback_lock);
        omp_set_lock(&prune_callback_lock);
        bool ret_val = false;
        if constexpr (!std::is_null_pointer_v<decltype(prune_callback)>) {
          ret_val = prune_callback(match_state);
        }
        omp_unset_lock(&prune_callback_lock);
        return ret_val;
      };
  QueryVertexHandle next_query_ptr =
      _dp_iso::NextMatchVertex(candidate_set, match_state);
  assert(match_state.count(next_query_ptr) == 0);
  if (!next_query_ptr) {
    if (query_graph.CountEdge() >= large_query_edge) {
      std::map<QueryVertexHandle, std::vector<QueryVertexHandle>> parent;
      for (auto &[query_ptr, target_ptr] : match_state) {
        _dp_iso::UpdateParent(match_state, query_ptr, parent);
      }
      std::vector<QueryVertexHandle> fail_set;
      _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
          candidate_set, match_state, target_matched, parent, fail_set,
          par_user_callback, par_prune_callback, std::time(NULL),
          query_limit_time);
    } else {
      _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
          candidate_set, match_state, target_matched, par_user_callback,
          par_prune_callback, std::time(NULL), query_limit_time);
    }
  } else {
    // partition next ptr's candiate
    auto &match_ptr_candidate = candidate_set.find(next_query_ptr)->second;
#pragma omp parallel
#pragma omp single
    {
      auto begin_time = std::time(NULL);
      for (int i = 0; i < match_ptr_candidate.size(); i++) {
#pragma omp task
        {
          // it might be unnecessary to set the lock here
          // user_callback_lock is read-only in this callback
          // and can only be set from false to true
          // omp_set_lock(&user_callback_lock);
          if (!user_callback_has_return_false) {
            auto &match_target_ptr = match_ptr_candidate[i];
            if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
                    next_query_ptr, match_target_ptr, match_state,
                    target_matched)) {
              auto temp_match_state = match_state;
              auto temp_target_matched = target_matched;
              auto temp_candidate_set = candidate_set;
              _dp_iso::UpdateState(next_query_ptr, match_target_ptr,
                                   temp_match_state, temp_target_matched);
              _dp_iso::UpdateCandidateSet<QueryGraph, TargetGraph>(
                  next_query_ptr, match_target_ptr, temp_candidate_set,
                  temp_match_state, temp_target_matched);
              if (query_graph.CountEdge() >= large_query_edge) {
                std::map<QueryVertexHandle, std::vector<QueryVertexHandle>>
                    parent;
                for (auto &[query_ptr, target_ptr] : temp_match_state) {
                  _dp_iso::UpdateParent(temp_match_state, query_ptr, parent);
                }
                std::vector<QueryVertexHandle> fail_set;
                if (!_dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
                        temp_candidate_set, temp_match_state,
                        temp_target_matched, parent, fail_set,
                        par_user_callback, par_prune_callback, begin_time,
                        query_limit_time)) {
                  user_callback_has_return_false = true;
                }
              } else {
                if (!_dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
                        temp_candidate_set, temp_match_state,
                        temp_target_matched, par_user_callback,
                        par_prune_callback, begin_time, query_limit_time)) {
                  user_callback_has_return_false = true;
                }
              }
            }
          }
          // omp_unset_lock(&user_callback_lock);
        }
      }
#pragma omp taskwait
    }
  }
  return static_cast<int>(result_count);
}
template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, class MatchCallback, class PruneCallback,
          class UpdateCandidateCallback>
inline int DPISO_Recursive(QueryGraph &query_graph, TargetGraph &target_graph,
                           MatchCallback user_callback,
                           PruneCallback prune_callback,
                           UpdateCandidateCallback update_candidate_callback,
                           double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph,
                                         candidate_set)) {
    return 0;
  }
  if (!RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  if constexpr (!std::is_null_pointer_v<UpdateCandidateCallback>) {
    if (!update_candidate_callback(candidate_set)) {
      return 0;
    }
  }
  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
  return DPISO_Recursive<match_semantics>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}
template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, class MatchCallback, class PruneCallback>
inline int DPISO_Recursive(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  std::map<typename QueryGraph::VertexConstPtr,
           typename VertexHandle<TargetGraph>::type>
      match_state;
  return DPISO_Recursive<match_semantics, QueryGraph, TargetGraph>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, class MatchCallback, class PruneCallback,
          class UpdateCandidateCallback>
inline int DPISO_Recursive(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    UpdateCandidateCallback update_candidate_callback,
    double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph,
                                         candidate_set)) {
    return 0;
  }
  for (auto &[query_ptr, target_ptr] : match_state) {
    candidate_set[query_ptr].clear();
    candidate_set[query_ptr].emplace_back(target_ptr);
  }
  if (!RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  if constexpr (!std::is_null_pointer_v<UpdateCandidateCallback>) {
    if (!update_candidate_callback(candidate_set)) {
      return 0;
    }
  }
  return DPISO_Recursive<match_semantics>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics, typename QueryGraph,
          typename TargetGraph, class MatchCallback>
inline int DPISO_Recursive(QueryGraph &query_graph, TargetGraph &target_graph,
                           MatchCallback user_callback,
                           double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  return DPISO_Recursive<match_semantics>(query_graph, target_graph,
                                          user_callback, nullptr, nullptr,
                                          query_limit_time);
}
}  // namespace _dp_iso

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback>
inline int DPISO(QueryGraph &query_graph, TargetGraph &target_graph,
                 MatchCallback user_callback, double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, user_callback, query_limit_time);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph>
inline int DPISO(QueryGraph &query_graph, TargetGraph &target_graph,
                 int max_result, double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  return DPISO<match_semantics>(
      query_graph, target_graph,
      std::bind(
          _dp_iso::MatchCallbackLimit<QueryVertexHandle, TargetVertexHandle>,
          std::placeholders::_1, &max_result),
      query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class ResultContainer>
inline int DPISO(QueryGraph &query_graph, TargetGraph &target_graph,
                 int max_result, ResultContainer &match_result,
                 double query_limit_time = 1200) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  match_result.clear();
  return DPISO<match_semantics>(
      query_graph, target_graph,
      std::bind(
          _dp_iso::MatchCallbackSaveResult<QueryVertexHandle,
                                           TargetVertexHandle, ResultContainer>,
          std::placeholders::_1, &max_result, &match_result),
      query_limit_time);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, class TargetGraph>
inline int DPISO(
    QueryGraph &query_graph, TargetGraph &target_graph,
    typename VertexHandle<QueryGraph>::type cal_supp_vertex_handle,
    std::vector<typename VertexHandle<TargetGraph>::type> &possible_supp,
    std::vector<typename VertexHandle<TargetGraph>::type> &supp,
    double single_query_limit_time = 100) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  if (!_dp_iso::InitCandidateSet<match_semantics>(query_graph, target_graph,
                                                  candidate_set)) {
    return 0;
  }
  if (!_dp_iso::UpdateCandidateCallback<QueryVertexHandle, TargetVertexHandle>(
          candidate_set, &cal_supp_vertex_handle, possible_supp)) {
    return 0;
  }
  if (!_dp_iso::RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }

  for (auto &target_handle :
       candidate_set.find(cal_supp_vertex_handle)->second) {
    time_t begin = std::time(NULL);
    int max_result = 1;
    size_t result_count = 0;
    std::map<QueryVertexHandle, TargetVertexHandle> match_state;
    std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        temp_candidate_set{candidate_set};
    auto prune_callback = nullptr;
    auto user_callback = std::bind(
        _dp_iso::MatchCallbackLimit<QueryVertexHandle, TargetVertexHandle>,
        std::placeholders::_1, &max_result);
    std::set<TargetVertexHandle> target_matched;
    _dp_iso::UpdateState(cal_supp_vertex_handle, target_handle, match_state,
                         target_matched);
    auto t_begin = std::time(NULL);
    _dp_iso::UpdateCandidateSet<QueryGraph, TargetGraph>(
        cal_supp_vertex_handle, target_handle, temp_candidate_set, match_state,
        target_matched);
    auto t_end = std::time(NULL);
    if (query_graph.CountEdge() >= _dp_iso::large_query_edge) {
      std::map<QueryVertexHandle, std::vector<QueryVertexHandle>> parent;
      _dp_iso::UpdateParent(match_state, cal_supp_vertex_handle, parent);
      std::vector<QueryVertexHandle> fail_set;

      _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
          temp_candidate_set, match_state, target_matched, parent, fail_set,
          user_callback, prune_callback, std::time(NULL),
          single_query_limit_time);
    } else {
      _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
          temp_candidate_set, match_state, target_matched, user_callback,
          prune_callback, std::time(NULL), single_query_limit_time);
    }

    if (max_result == 0) {
      supp.emplace_back(target_handle);
    }
    time_t end = std::time(NULL);
  }
  return static_cast<int>(supp.size());
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback,
          class PruneCallBack, class UpdateInitCandidateCallback>
inline int DPISO(QueryGraph &query_graph, TargetGraph &target_graph,
                 MatchCallback match_callback, PruneCallBack prune_callback,
                 UpdateInitCandidateCallback update_initcandidate_callback,
                 double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, match_callback, prune_callback,
      update_initcandidate_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback,
          class PruneCallback>
inline int DPISO(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback,
          class PruneCallback>
inline int DPISO(QueryGraph &query_graph, TargetGraph &target_graph,
                 std::map<typename VertexHandle<QueryGraph>::type,
                          std::vector<typename VertexHandle<TargetGraph>::type>>
                     &candidate_set,
                 MatchCallback user_callback, PruneCallback prune_callback,
                 double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_limit_time, target_graph, candidate_set, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback,
          class PruneCallback, class UpdateCandidateCallback>
inline int DPISO(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<typename VertexHandle<QueryGraph>::type,
             typename VertexHandle<TargetGraph>::type> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    UpdateCandidateCallback update_candidate_callback,
    double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, match_state, user_callback, prune_callback,
      update_candidate_callback, query_limit_time);
}

template <class CandidateSetContainer, class Pivot>
inline bool SuppUpdateCallBack(CandidateSetContainer &candidate_set,
                               Pivot &supp_list) {
  for (auto it = candidate_set.begin(); it != candidate_set.end();) {
    if (std::find(std::begin(supp_list), std::end(supp_list), it->first) ==
        std::end(supp_list)) {
      it = candidate_set.erase(it);
    } else {
      it++;
    }
  }
  return true;
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback,
          class SuppContainer>
inline int DPISO(QueryGraph &query_graph, TargetGraph &target_graph,
                 SuppContainer &supp_list, MatchCallback user_callback,
                 double single_query_time = 100) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using MatchMap = std::map<QueryVertexHandle, TargetVertexHandle>;
  using MatchResult = std::vector<MatchMap>;
  using CandidateSetContainerType =
      std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>;

  auto update_callback =
      std::bind(SuppUpdateCallBack<CandidateSetContainerType, SuppContainer>,
                std::placeholders::_1, std::ref(supp_list));

  MatchResult supp_match;
  int max_result = -1;
  auto match_callback = std::bind(
      _dp_iso::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                       MatchResult>,
      std::placeholders::_1, &max_result, &supp_match);
  DPISO<match_semantics>(query_graph, target_graph, match_callback, nullptr,
                         update_callback, double(1200));
  MatchResult match_result;
  CandidateSetContainerType candidate_set;
  if (!_dp_iso::InitCandidateSet<match_semantics>(query_graph, target_graph,
                                                  candidate_set)) {
    return 0;
  }
  if (!_dp_iso::RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  for (auto &single_match : supp_match) {
    bool flag = 0;
    for (auto &it : single_match) {
      if (!std::binary_search(std::begin(candidate_set[it.first]),
                              std::end(candidate_set[it.first]), it.second)) {
        flag = 1;
        break;
      }
    }
    if (flag) continue;
    int max_result = 1;
    auto match_callback = std::bind(
        _dp_iso::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                         MatchResult>,
        std::placeholders::_1, &max_result, &match_result);
    auto temp_candidate_set = candidate_set;
    DPISO<match_semantics>(query_graph, target_graph, temp_candidate_set,
                           single_match, match_callback, nullptr,
                           single_query_time);
  }
  for (auto &it : match_result) {
    user_callback(it);
  }
  return static_cast<int>(match_result.size());
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchCallback>
inline int IncreamentDPISO(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::vector<typename VertexHandle<TargetGraph>::type> &delta_target_graph,
    MatchCallback match_callback, double query_limit_time = -1) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using CandidateSet =
      std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>;
  CandidateSet candidate_set;
  if (!GUNDAM::_dp_iso::InitCandidateSet<match_semantics>(
          query_graph, target_graph, candidate_set)) {
    return 0;
  }
  if (!_dp_iso::RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  std::sort(delta_target_graph.begin(), delta_target_graph.end());
  std::vector<QueryVertexHandle> has_delta_target_graph_pattern_vertex;
  for (auto &[query_ptr, target_list] : candidate_set) {
    bool find_new_vertex_flag = false;
    for (auto &target_ptr : target_list) {
      if (std::binary_search(delta_target_graph.begin(),
                             delta_target_graph.end(), target_ptr)) {
        find_new_vertex_flag = true;
        break;
      }
    }
    if (!find_new_vertex_flag) {
      continue;
    }
    has_delta_target_graph_pattern_vertex.emplace_back(query_ptr);
  }
  std::sort(has_delta_target_graph_pattern_vertex.begin(),
            has_delta_target_graph_pattern_vertex.end());
  int total_mask = (1 << (has_delta_target_graph_pattern_vertex.size()));
  for (int mask = 1; mask < total_mask; mask++) {
    std::vector<QueryVertexHandle> this_mask_vertex;
    for (int bit_pos = 0;
         bit_pos < has_delta_target_graph_pattern_vertex.size(); bit_pos++) {
      if (mask & (1 << bit_pos)) {
        this_mask_vertex.emplace_back(
            has_delta_target_graph_pattern_vertex[bit_pos]);
      }
    }
    CandidateSet copy_candidate_set{candidate_set};
    for (auto &[query_ptr, target_list] : copy_candidate_set) {
      if (std::binary_search(this_mask_vertex.begin(), this_mask_vertex.end(),
                             query_ptr)) {
        std::vector<TargetVertexHandle> this_vertex_target_list;
        for (auto &target_ptr : target_list) {
          if (std::binary_search(delta_target_graph.begin(),
                                 delta_target_graph.end(), target_ptr)) {
            this_vertex_target_list.emplace_back(target_ptr);
          }
        }
        std::swap(this_vertex_target_list, target_list);
      } else {
        std::vector<TargetVertexHandle> this_vertex_target_list;
        for (auto &target_ptr : target_list) {
          if (!std::binary_search(delta_target_graph.begin(),
                                  delta_target_graph.end(), target_ptr)) {
            this_vertex_target_list.emplace_back(target_ptr);
          }
        }
        std::swap(this_vertex_target_list, target_list);
      }
    }
    std::map<QueryVertexHandle, TargetVertexHandle> match_state;
    GUNDAM::DPISO<match_semantics>(query_graph, target_graph,
                                   copy_candidate_set, match_state,
                                   match_callback, nullptr, query_limit_time);
  }
  return 1;
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class MatchSet>
inline int DPISO_UsingMatchSet(QueryGraph &query_graph,
                               TargetGraph &target_graph, MatchSet &match_set) {
  using Match = typename MatchSet::MatchType;
  auto user_callback = [&match_set](auto &match_state) {
    Match match;
    for (auto &single_match : match_state) {
      match.AddMap(single_match.first, single_match.second);
    }
    match_set.AddMatch(match);
    return true;
  };
  return DPISO<match_semantics>(query_graph, target_graph, user_callback,
                                (double)-1);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          typename QueryGraph, typename TargetGraph, class Match,
          class MatchSet>
inline int DPISO_UsingPatricalMatchAndMatchSet(QueryGraph &query_graph,
                                               TargetGraph &target_graph,
                                               Match &partical_match,
                                               MatchSet &match_set) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using MatchMap = std::map<QueryVertexHandle, TargetVertexHandle>;
  using MatchContainer = std::vector<MatchMap>;
  using CandidateSetContainer =
      std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>;
  CandidateSetContainer candidate_set;
  if (!_dp_iso::InitCandidateSet<match_semantics>(query_graph, target_graph,
                                                  candidate_set)) {
    return 0;
  }
  if (!_dp_iso::RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  MatchMap match_state;
  MatchContainer match_result;
  std::set<TargetVertexHandle> target_matched;
  for (auto vertex_it = query_graph.VertexBegin(); !vertex_it.IsDone();
       vertex_it++) {
    QueryVertexHandle vertex_handle = vertex_it;
    if (partical_match.HasMap(vertex_handle)) {
      TargetVertexHandle match_vertex_handle =
          partical_match.MapTo(vertex_handle);
      match_state.insert(std::make_pair(vertex_handle, match_vertex_handle));
      target_matched.insert(match_vertex_handle);
    }
  }
  int max_result = -1;
  size_t result_count = 0;
  auto user_callback = std::bind(
      _dp_iso::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                       MatchContainer>,
      std::placeholders::_1, &max_result, &match_result);
  if (query_graph.CountEdge() < _dp_iso::large_query_edge) {
    _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, user_callback, nullptr,
        std::time(NULL), -1);
  } else {
    using FailSetContainer = std::vector<QueryVertexHandle>;
    using ParentContainer =
        std::map<QueryVertexHandle, std::vector<QueryVertexHandle>>;
    ParentContainer parent;
    FailSetContainer fail_set;
    for (auto &single_match : match_state) {
      _dp_iso::UpdateParent(match_state, single_match.first, parent);
    }
    _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, parent, fail_set,
        user_callback, nullptr, std::time(NULL), -1.0);
  }

  for (auto &single_match : match_result) {
    Match match;
    for (auto &match_pair : single_match) {
      match.AddMap(match_pair.first, match_pair.second);
    }
    match_set.AddMatch(match);
  }
  return result_count;
}
}  // namespace GUNDAM
#endif
