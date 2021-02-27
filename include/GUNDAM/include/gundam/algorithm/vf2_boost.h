#ifndef _VF2BOOST_H
#define _VF2BOOST_H

#include <iterator>

//#include "graph.h"
#include "vf2.h"
namespace GUNDAM {
namespace _vf2_boost {
enum EdgeState { kIn, kOut };

template <class GraphType, class Fn, class VertexPtr1>
inline bool ForEachVertexIf(const GraphType &graph, Fn f,
                            const VertexPtr1 &vertex_a_ptr) {
  for (auto vertex_iter = graph.VertexCBegin(vertex_a_ptr->label());
       !vertex_iter.IsDone(); vertex_iter++) {
    typename GraphType::VertexConstPtr vertex_ptr = vertex_iter;
    if (!f(vertex_ptr)) return false;
  }
  return true;
}
template <enum EdgeState edge_state, class VertexPtr, class Fn, class EdgePtr1>
inline bool ForEachEdgeIf(const VertexPtr &vertex_ptr, Fn f,
                          const EdgePtr1 &edge_a_ptr) {
  for (auto edge_iter = (edge_state == EdgeState::kIn)
                            ? vertex_ptr->InEdgeCBegin(edge_a_ptr->label())
                            : vertex_ptr->OutEdgeCBegin(edge_a_ptr->label());
       !edge_iter.IsDone(); edge_iter++) {
    typename VertexPtr::GraphType::EdgeConstPtr edge_ptr = edge_iter;
    if (!f(edge_ptr)) return false;
  }
  return true;
}
// Init Candidate Set
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph>
inline bool InitCandidateSet(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    std::map<typename QueryGraph::VertexConstPtr,
             std::vector<typename TargetGraph::VertexConstPtr>>
        &candidate_set) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  for (auto query_vertex_iter = query_graph.VertexCBegin();
       !query_vertex_iter.IsDone(); query_vertex_iter++) {
    QueryVertexPtr query_vertex_ptr = query_vertex_iter;
    auto &l = candidate_set[query_vertex_iter];
    for (auto target_vertex_iter =
             target_graph.VertexCBegin(query_vertex_ptr->label());
         !target_vertex_iter.IsDone(); target_vertex_iter++) {
      TargetVertexPtr target_vertex_ptr = target_vertex_iter;
      int flag = 0;
      for (auto edge_label_it = query_vertex_ptr->OutEdgeLabelBegin();
           !edge_label_it.IsDone(); edge_label_it++) {
        auto query_out_count = query_vertex_ptr->CountOutEdge(
            *edge_label_it /*edge_label_it->label()*/);
        auto target_out_count = target_vertex_ptr->CountOutEdge(
            *edge_label_it /*edge_label_it->label()*/);
        if (query_out_count > target_out_count) {
          flag = 1;
          break;
        }
      }
      for (auto edge_label_it = query_vertex_ptr->InEdgeLabelBegin();
           !edge_label_it.IsDone(); edge_label_it++) {
        auto query_in_count = query_vertex_ptr->CountInEdge(
            *edge_label_it /*edge_label_it->label()*/);
        auto target_in_count = target_vertex_ptr->CountInEdge(
            *edge_label_it /*edge_label_it->label()*/);
        if (query_in_count > target_in_count) {
          flag = 1;
          break;
        }
      }
      if (!flag) {
        l.emplace_back(target_vertex_ptr);
      }
    }
    if (l.empty()) return false;
  }
  for (auto &it : candidate_set) {
    sort(it.second.begin(), it.second.end());
  }
  return true;
}
// Check target_id is in C(query_vertex_ptr)
template <class QueryVertexPtr, class TargetVertexPtr, class TargetVertexID>
inline bool CheckIsInCandidateSet(
    QueryVertexPtr query_vertex_ptr,
    const std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    const TargetVertexID &target_id) {
  if (candidate_set.find(query_vertex_ptr) == candidate_set.end()) return false;
  for (const auto &v : candidate_set.find(query_vertex_ptr)->second) {
    // std::cout << "id = " << v->id() << std::endl;
    if (v->id() == target_id) {
      return true;
    }
  }
  return false;
}

//
template <class QueryVertexPtr, class TargetVertexPtr>
inline QueryVertexPtr DetermineMatchOrder(
    const std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    const std::map<QueryVertexPtr, TargetVertexPtr> &match_state) {
  std::set<QueryVertexPtr> next_query_set;
  for (const auto &match_pair : match_state) {
    const auto &query_vertex_ptr = match_pair.first;

    for (auto edge_iter = query_vertex_ptr->OutEdgeCBegin();
         !edge_iter.IsDone(); edge_iter++) {
      auto query_opp_vertex_ptr = edge_iter->const_dst_ptr();
      if (match_state.count(query_opp_vertex_ptr) == 0) {
        next_query_set.emplace(query_opp_vertex_ptr);
      }
    }

    for (auto edge_iter = query_vertex_ptr->InEdgeCBegin(); !edge_iter.IsDone();
         edge_iter++) {
      auto query_opp_vertex_ptr = edge_iter->const_src_ptr();
      if (match_state.count(query_opp_vertex_ptr) == 0) {
        next_query_set.emplace(query_opp_vertex_ptr);
      }
    }
  }

  if (next_query_set.empty()) {
    for (const auto &candidate_pair : candidate_set) {
      const auto &query_vertex_ptr = candidate_pair.first;
      if (match_state.count(query_vertex_ptr) == 0) {
        next_query_set.emplace(query_vertex_ptr);
      }
    }
  }

  QueryVertexPtr res;
  size_t min = SIZE_MAX;
  for (const auto &query_vertex_ptr : next_query_set) {
    size_t candidate_count =
        candidate_set.find(query_vertex_ptr)->second.size();
    if (candidate_count < min) {
      res = query_vertex_ptr;
      min = candidate_count;
    }
  }
  // assert(min >= 0 && !res.IsNull());
  return res;
}

template <enum EdgeState edge_state, class QueryGraph, class TargetGraph,
          class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap,
          class TargetVertexSet>
inline void UpdateCandidateSetOneDirection(
    QueryVertexPtr query_vertex_ptr, TargetVertexPtr target_vertex_ptr,
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    const MatchStateMap &match_state, const TargetVertexSet &target_matched) {
  std::map<typename QueryGraph::VertexType::LabelType,
           std::set<TargetVertexPtr>>
      temp_adj_vertex;
  for (auto label_it = ((edge_state == EdgeState::kIn)
                            ? query_vertex_ptr->InEdgeLabelBegin()
                            : query_vertex_ptr->OutEdgeLabelBegin());
       !label_it.IsDone(); label_it++) {
    for (auto it = ((edge_state == EdgeState::kIn)
                        ? target_vertex_ptr->InEdgeCBegin(
                              *label_it /*label_it->label()*/)
                        : target_vertex_ptr->OutEdgeCBegin(
                              *label_it /*label_it->label()*/));
         !it.IsDone(); it++) {
      TargetVertexPtr temp_target_ptr = (edge_state == EdgeState::kIn)
                                            ? it->const_src_ptr()
                                            : it->const_dst_ptr();
      // if (target_matched.count(temp_target_ptr)) continue;
      temp_adj_vertex[temp_target_ptr->label()].insert(temp_target_ptr);
    }
    for (auto vertex_it = ((edge_state == EdgeState::kIn)
                               ? query_vertex_ptr->InVertexCBegin(
                                     *label_it /*label_it->label()*/)
                               : query_vertex_ptr->OutVertexCBegin(
                                     *label_it /*label_it->label()*/));
         !vertex_it.IsDone(); vertex_it++) {
      QueryVertexPtr temp_vertex_ptr = vertex_it;
      std::vector<TargetVertexPtr> res_candidate;
      if (match_state.count(temp_vertex_ptr)) continue;
      std::set_intersection(temp_adj_vertex[vertex_it->label()].begin(),
                            temp_adj_vertex[vertex_it->label()].end(),
                            candidate_set[temp_vertex_ptr].begin(),
                            candidate_set[temp_vertex_ptr].end(),
                            inserter(res_candidate, res_candidate.begin()));
      candidate_set[temp_vertex_ptr] = res_candidate;
    }
  }
}
template <class QueryGraph, class TargetGraph, class QueryVertexPtr,
          class TargetVertexPtr, class MatchStateMap, class TargetVertexSet>
inline void UpdateCandidateSet(
    QueryVertexPtr query_vertex_ptr, TargetVertexPtr target_vertex_ptr,
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    const MatchStateMap &match_state, const TargetVertexSet &target_matched) {
  UpdateCandidateSetOneDirection<EdgeState::kIn, QueryGraph, TargetGraph>(
      query_vertex_ptr, target_vertex_ptr, candidate_set, match_state,
      target_matched);
  UpdateCandidateSetOneDirection<EdgeState::kOut, QueryGraph, TargetGraph>(
      query_vertex_ptr, target_vertex_ptr, candidate_set, match_state,
      target_matched);
}
template <enum EdgeState edge_state, class QueryGraph, class TargetGraph,
          class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap,
          class IsCliqueMap>
inline bool JoinableCheck(const QueryVertexPtr &query_vertex_ptr,
                          const TargetVertexPtr &target_vertex_ptr,
                          const MatchStateMap &match_state,
                          const IsCliqueMap &is_clique) {
  using QueryEdgePtr = typename QueryGraph::EdgeConstPtr;
  using TargetEdgePtr = typename TargetGraph::EdgeConstPtr;
  using QueryEdgeMapPair = std::pair<typename QueryGraph::EdgeType::LabelType,
                                     typename QueryGraph::VertexConstPtr>;
  using TargetEdgeMapPair = std::pair<typename TargetGraph::EdgeType::LabelType,
                                      typename TargetGraph::VertexConstPtr>;
  std::set<typename TargetGraph::EdgeType::IDType> used_edge;
  std::map<QueryEdgeMapPair, int> equal_vertex_map;
  for (auto query_edge_iter =
           ((edge_state == EdgeState::kIn) ? query_vertex_ptr->InEdgeCBegin()
                                           : query_vertex_ptr->OutEdgeCBegin());
       !query_edge_iter.IsDone(); query_edge_iter++) {
    QueryEdgePtr query_edge_ptr = query_edge_iter;
    QueryVertexPtr query_opp_vertex_ptr = (edge_state == EdgeState::kIn)
                                              ? query_edge_ptr->const_src_ptr()
                                              : query_edge_ptr->const_dst_ptr();
    auto match_iter = match_state.find(query_opp_vertex_ptr);
    if (match_iter == match_state.end()) continue;

    TargetVertexPtr query_opp_match_vertex_ptr = match_iter->second;
    if (query_opp_match_vertex_ptr == target_vertex_ptr) {
      equal_vertex_map[std::make_pair(query_edge_ptr->label(),
                                      query_opp_vertex_ptr)]++;
      continue;
    }
    bool find_target_flag = false;
    for (auto target_edge_iter =
             ((edge_state == EdgeState::kIn)
                  ? target_vertex_ptr->InEdgeCBegin(query_edge_ptr->label())
                  : target_vertex_ptr->OutEdgeCBegin(query_edge_ptr->label()));
         !target_edge_iter.IsDone(); target_edge_iter++) {
      if (used_edge.count(target_edge_iter->id())) continue;
      TargetEdgePtr target_edge_ptr = target_edge_iter;
      TargetVertexPtr target_opp_vertex_ptr =
          (edge_state == EdgeState::kIn) ? target_edge_iter->const_src_ptr()
                                         : target_edge_iter->const_dst_ptr();
      if (target_opp_vertex_ptr != query_opp_match_vertex_ptr) continue;
      find_target_flag = true;
      used_edge.insert(target_edge_ptr->id());
      break;
    }
    if (!find_target_flag) return false;
  }
  for (const auto &it : equal_vertex_map) {
    auto find_it = (is_clique.find(target_vertex_ptr->id()));
    if (find_it == is_clique.end()) return false;
    auto label_map_it = find_it->second.find(it.first.first);
    if (label_map_it == find_it->second.end()) return false;
    if (it.second > label_map_it->second) return false;
  }
  return true;
}
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexPtr, class TargetVertexPtr,
          class MatchStateMap, class TargetVertexSet, class UsedTimeMap,
          class IsCliqueMap>
inline bool IsJoinable(QueryVertexPtr query_vertex_ptr,
                       TargetVertexPtr target_vertex_ptr,
                       const MatchStateMap &match_state,
                       const TargetVertexSet &target_matched,
                       const UsedTimeMap &vertex_used_time,
                       const IsCliqueMap &is_clique) {
  if (match_semantics == MatchSemantics::kIsomorphism) {
    auto it = target_matched.find(target_vertex_ptr);
    if (it != target_matched.end() &&
        it->second == vertex_used_time.find(target_vertex_ptr)->second) {
      return false;
    }
  }
  if (!JoinableCheck<EdgeState::kIn, QueryGraph, TargetGraph>(
          query_vertex_ptr, target_vertex_ptr, match_state, is_clique)) {
    return false;
  }
  if (!JoinableCheck<EdgeState::kOut, QueryGraph, TargetGraph>(
          query_vertex_ptr, target_vertex_ptr, match_state, is_clique)) {
    return false;
  }
  return true;
}
template <class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap,
          class TargetVertexSet>
inline void UpdateState(QueryVertexPtr query_vertex_ptr,
                        TargetVertexPtr target_vertex_ptr,
                        MatchStateMap &match_state,
                        TargetVertexSet &target_matched) {
  match_state.emplace(query_vertex_ptr, target_vertex_ptr);
  if (target_matched.count(target_vertex_ptr)) {
    target_matched.find(target_vertex_ptr)->second++;
  } else {
    target_matched.emplace(target_vertex_ptr, 1);
  }
}
template <class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap,
          class TargetVertexSet>
inline void RestoreState(QueryVertexPtr query_vertex_ptr,
                         TargetVertexPtr target_vertex_ptr,
                         MatchStateMap &match_state,
                         TargetVertexSet &target_matched) {
  match_state.erase(query_vertex_ptr);
  if (target_matched.find(target_vertex_ptr)->second == 1)
    target_matched.erase(target_vertex_ptr);
  else
    target_matched.find(target_vertex_ptr)->second--;
}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexPtr, class TargetVertexPtr,
          class MatchCallback, class UsedtimeMap, class IsCliqueMap>
bool _VF2(
    const std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
    std::map<TargetVertexPtr, int> &target_matched,
    const UsedtimeMap &vertex_used_time, const IsCliqueMap &is_clique,
    size_t &result_count, MatchCallback user_callback) {
  // for (auto &it : match_state) {
  //  std::cout << it.first->id() << " " << it.second->id() << std::endl;
  //}
  // std::cout << std::endl;
  if (match_state.size() == candidate_set.size()) {
    result_count++;
    return user_callback(match_state);
  }

  QueryVertexPtr next_query_vertex_ptr =
      DetermineMatchOrder(candidate_set, match_state);
  // std::cout << "next size = "
  //          << candidate_set.find(next_query_vertex_ptr)->second.size()
  //         << std::endl;
  for (const TargetVertexPtr &next_target_vertex_ptr :
       candidate_set.find(next_query_vertex_ptr)->second) {
    // std::cout << "pair = " << next_query_vertex_ptr->id() << " "
    //           << next_target_vertex_ptr->id() << std::endl;
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            next_query_vertex_ptr, next_target_vertex_ptr, match_state,
            target_matched, vertex_used_time, is_clique)) {
      std::map<QueryVertexPtr, std::vector<TargetVertexPtr>>
          temp_candidate_set = candidate_set;
      UpdateState(next_query_vertex_ptr, next_target_vertex_ptr, match_state,
                  target_matched);
      UpdateCandidateSet<QueryGraph, TargetGraph>(
          next_query_vertex_ptr, next_target_vertex_ptr, temp_candidate_set,
          match_state, target_matched);
      if (!_VF2<match_semantics, QueryGraph, TargetGraph>(
              temp_candidate_set, match_state, target_matched, vertex_used_time,
              is_clique, result_count, user_callback)) {
        return false;
      }
      RestoreState(next_query_vertex_ptr, next_target_vertex_ptr, match_state,
                   target_matched);
    }
  }
  return true;
}
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class MatchCallback, class UsedtimeMap,
          class IsCliqueMap>
inline int VF2_Recursive(const QueryGraph &query_graph,
                         const TargetGraph &target_graph,
                         const UsedtimeMap &vertex_used_time,
                         const IsCliqueMap &is_clique,
                         MatchCallback user_callback) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;

  std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> candidate_set;
  if (!_vf2_boost::InitCandidateSet<match_semantics>(query_graph, target_graph,
                                                     candidate_set)) {
    return 0;
  }
  std::map<QueryVertexPtr, TargetVertexPtr> match_state;
  std::map<TargetVertexPtr, int> target_matched;
  size_t result_count = 0;
  _VF2<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched, vertex_used_time, is_clique,
      result_count, user_callback);

  return static_cast<int>(result_count);
}

}  // namespace _vf2_boost

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class UsedtimeMap,
          class MatchCallback, class IsCliqueMap>
inline int VF2_Boost(const QueryGraph &query_graph,
                     const TargetGraph &target_graph,
                     const UsedtimeMap &vertex_used_time,
                     const IsCliqueMap &is_clique,
                     MatchCallback user_callback) {
  return _vf2_boost::VF2_Recursive<match_semantics>(
      query_graph, target_graph, vertex_used_time, is_clique, user_callback);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class UsedTimeMap,
          class IsCliqueMap>
inline int VF2_Boost(const QueryGraph &query_graph,
                     const TargetGraph &target_graph,
                     const UsedTimeMap &vertex_used_time,
                     const IsCliqueMap &is_clique, int max_result) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;

  return VF2_Boost<match_semantics>(
      query_graph, target_graph, vertex_used_time, is_clique,
      std::bind(_vf2::MatchCallbackLimit<QueryVertexPtr, TargetVertexPtr>,
                std::placeholders::_1, &max_result));
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class ResultContainer,
          class UsedTimeMap, class IsCliqueMap>
inline int VF2_Boost(const QueryGraph &query_graph,
                     const TargetGraph &target_graph,
                     const UsedTimeMap &vertex_used_time,
                     const IsCliqueMap &is_clique, int max_result,
                     ResultContainer &match_result) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;

  match_result.clear();

  return VF2_Boost<match_semantics>(
      query_graph, target_graph, vertex_used_time, is_clique,
      std::bind(_vf2::MatchCallbackSaveResult<QueryVertexPtr, TargetVertexPtr,
                                              ResultContainer>,
                std::placeholders::_1, &max_result, &match_result));
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class ResultContainer,
          class UsedTimeMap, class IsCliqueMap>
inline int VF2_Boost(const QueryGraph &query_graph,
                     const TargetGraph &target_graph,
                     const UsedTimeMap &vertex_used_time,
                     const IsCliqueMap &is_clique,
                     const typename QueryGraph::VertexType::IDType query_id,
                     const typename TargetGraph::VertexType::IDType target_id,
                     int max_result, ResultContainer &match_result) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  match_result.clear();
  std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> candidate_set;
  if (!_vf2_boost::InitCandidateSet<match_semantics>(query_graph, target_graph,
                                                     candidate_set))
    return 0;
  // for (auto &it : candidate_set) {
  // std::cout << it.first->id() << " " << it.second.size() << std::endl;
  //}
  // std::cout << "cout end!" << std::endl;
  QueryVertexPtr query_ptr = query_graph.FindConstVertex(query_id);
  TargetVertexPtr target_ptr = target_graph.FindConstVertex(target_id);
  if (!_vf2_boost::CheckIsInCandidateSet(query_ptr, candidate_set, target_id)) {
    return 0;
  }
  // std::cout << "check end!" << std::endl;
  std::map<QueryVertexPtr, TargetVertexPtr> match_state;
  std::map<TargetVertexPtr, int> target_matched;

  _vf2_boost::UpdateState(query_ptr, target_ptr, match_state, target_matched);
  // std::cout << "update end!" << std::endl;
  _vf2_boost::UpdateCandidateSet<QueryGraph, TargetGraph>(
      query_ptr, target_ptr, candidate_set, match_state, target_matched);
  // std::cout << "update1 end!" << std::endl;
  size_t result_count = 0;
  return _vf2_boost::_VF2<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched, vertex_used_time, is_clique,
      result_count,
      std::bind(_vf2::MatchCallbackSaveResult<QueryVertexPtr, TargetVertexPtr,
                                              ResultContainer>,
                std::placeholders::_1, &max_result, &match_result));
}  // namespace GUNDAM
}  // namespace GUNDAM
#endif