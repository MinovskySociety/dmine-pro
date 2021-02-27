#ifndef _DPISO_H
#define _DPISO_H
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

#include "match_helper.h"
#include "vf2.h"
namespace GUNDAM {
namespace _dp_iso {
enum EdgeState { kIn, kOut };
// if query.CountEdge()>=large_query_edge, using fail set
// small query not using fail set
constexpr size_t large_query_edge = 6;
constexpr size_t adj_vertex_limit = 200000;
template <class QueryVertexPtr, class TargetVertexPtr>
inline bool DegreeFiltering(const QueryVertexPtr query_vertex_ptr,
                            const TargetVertexPtr target_vertex_ptr) {
  if (query_vertex_ptr->CountInEdge() > target_vertex_ptr->CountInEdge())
    return false;
  if (query_vertex_ptr->CountOutEdge() > target_vertex_ptr->CountOutEdge())
    return false;
  if (query_vertex_ptr->CountInVertex() > target_vertex_ptr->CountInVertex())
    return false;
  if (query_vertex_ptr->CountOutVertex() > target_vertex_ptr->CountOutVertex())
    return false;
  return true;
}
template <enum EdgeState edge_state, class QueryVertexPtr,
          class TargetVertexPtr>
inline bool NeighborLabelFrequencyFilteringSingleDirection(
    const QueryVertexPtr query_vertex_ptr,
    const TargetVertexPtr target_vertex_ptr) {
  for (auto edge_label_it =
           (edge_state == EdgeState::kIn ? query_vertex_ptr->InEdgeCBegin()
                                         : query_vertex_ptr->OutEdgeCBegin());
       !edge_label_it.IsDone(); edge_label_it++) {
    auto query_count =
        (edge_state == EdgeState::kIn
             ? query_vertex_ptr->CountInVertex(edge_label_it->label())
             : query_vertex_ptr->CountOutVertex(edge_label_it->label()));
    auto target_count =
        (edge_state == EdgeState::kIn
             ? target_vertex_ptr->CountInVertex(edge_label_it->label())
             : target_vertex_ptr->CountOutVertex(edge_label_it->label()));
    if (query_count > target_count) {
      return false;
    }
    query_count =
        (edge_state == EdgeState::kIn
             ? query_vertex_ptr->CountInEdge(edge_label_it->label())
             : query_vertex_ptr->CountOutEdge(edge_label_it->label()));
    target_count =
        (edge_state == EdgeState::kIn
             ? target_vertex_ptr->CountInEdge(edge_label_it->label())
             : target_vertex_ptr->CountOutEdge(edge_label_it->label()));
    if (query_count > target_count) {
      return false;
    }
  }
  return true;
}
template <class QueryVertexPtr, class TargetVertexPtr>
inline bool NeighborLabelFrequencyFiltering(
    const QueryVertexPtr query_vertex_ptr,
    const TargetVertexPtr target_vertex_ptr) {
  if (!NeighborLabelFrequencyFilteringSingleDirection<EdgeState::kIn>(
          query_vertex_ptr, target_vertex_ptr)) {
    return false;
  }
  if (!NeighborLabelFrequencyFilteringSingleDirection<EdgeState::kOut>(
          query_vertex_ptr, target_vertex_ptr)) {
    return false;
  }
  return true;
}
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class CandidateSetContainer>
inline bool InitCandidateSet(const QueryGraph &query_graph,
                             const TargetGraph &target_graph,
                             CandidateSetContainer &candidate_set) {
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
      if (!DegreeFiltering(query_vertex_ptr, target_vertex_ptr)) {
        continue;
      }
      if (!NeighborLabelFrequencyFiltering(query_vertex_ptr,
                                           target_vertex_ptr)) {
        continue;
      }
      l.emplace_back(target_vertex_ptr);
    }
    if (l.empty()) return false;
  }
  for (auto &it : candidate_set) {
    sort(it.second.begin(), it.second.end());
  }
  return true;
}
template <enum EdgeState edge_state, class QueryVertexPtr,
          class TargetVertexPtr, class CandidateSetContainer>
inline void GetAdjNotMatchedVertexSingleDirection(
    const QueryVertexPtr &query_vertex_ptr,
    const CandidateSetContainer &candidate_set,
    const std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
    std::set<QueryVertexPtr> &next_query_set) {
  for (auto edge_iter =
           (edge_state == EdgeState::kIn ? query_vertex_ptr->InEdgeCBegin()
                                         : query_vertex_ptr->OutEdgeCBegin());
       !edge_iter.IsDone(); edge_iter++) {
    const QueryVertexPtr &query_opp_vertex_ptr =
        edge_state == EdgeState::kIn ? edge_iter->const_src_ptr()
                                     : edge_iter->const_dst_ptr();
    if (match_state.count(query_opp_vertex_ptr) == 0 &&
        candidate_set.count(query_opp_vertex_ptr)) {
      next_query_set.insert(query_opp_vertex_ptr);
    }
  }
}
template <class QueryVertexPtr, class TargetVertexPtr,
          class CandidateSetContainer>
inline void GetAdjNotMatchedVertex(
    const QueryVertexPtr &query_vertex_ptr,
    const CandidateSetContainer &candidate_set,
    const std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
    std::set<QueryVertexPtr> &next_query_set) {
  GetAdjNotMatchedVertexSingleDirection<EdgeState::kOut>(
      query_vertex_ptr, candidate_set, match_state, next_query_set);
  GetAdjNotMatchedVertexSingleDirection<EdgeState::kIn>(
      query_vertex_ptr, candidate_set, match_state, next_query_set);
}
template <class QueryVertexPtr, class TargetVertexPtr,
          class CandidateSetContainer>
inline QueryVertexPtr NextMatchVertex(
    const CandidateSetContainer &candidate_set,
    const std::map<QueryVertexPtr, TargetVertexPtr> &match_state) {
  std::set<QueryVertexPtr> next_query_set;
  for (const auto &match_pair : match_state) {
    GetAdjNotMatchedVertex(match_pair.first, candidate_set, match_state,
                           next_query_set);
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
          class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap>
inline bool JoinableCheck(const QueryVertexPtr &query_vertex_ptr,
                          const TargetVertexPtr &target_vertex_ptr,
                          const MatchStateMap &match_state) {
  using QueryEdgePtr = typename QueryGraph::EdgeConstPtr;
  using TargetEdgePtr = typename TargetGraph::EdgeConstPtr;
  using QueryEdgeMapPair = std::pair<typename QueryGraph::EdgeType::LabelType,
                                     typename QueryGraph::VertexConstPtr>;
  using TargetEdgeMapPair = std::pair<typename TargetGraph::EdgeType::LabelType,
                                      typename TargetGraph::VertexConstPtr>;
  std::set<typename TargetGraph::EdgeType::IDType> used_edge;
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

    bool find_target_flag = false;
    for (auto target_edge_iter =
             ((edge_state == EdgeState::kIn)
                  ? target_vertex_ptr->InEdgeCBegin(query_edge_ptr->label(),
                                                    query_opp_match_vertex_ptr)
                  : target_vertex_ptr->OutEdgeCBegin(
                        query_edge_ptr->label(), query_opp_match_vertex_ptr));
         !target_edge_iter.IsDone(); target_edge_iter++) {
      if (used_edge.count(target_edge_iter->id())) continue;
      find_target_flag = true;
      used_edge.insert(target_edge_iter->id());
      break;
    }
    if (!find_target_flag) return false;
  }
  return true;
}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexPtr, class TargetVertexPtr,
          class MatchStateMap, class TargetVertexSet>
inline bool IsJoinable(QueryVertexPtr query_vertex_ptr,
                       TargetVertexPtr target_vertex_ptr,
                       const MatchStateMap &match_state,
                       const TargetVertexSet &target_matched) {
  if (match_semantics == MatchSemantics::kIsomorphism &&
      target_matched.count(target_vertex_ptr) > 0) {
    return false;
  }
  if (!JoinableCheck<EdgeState::kIn, QueryGraph, TargetGraph>(
          query_vertex_ptr, target_vertex_ptr, match_state)) {
    return false;
  }
  if (!JoinableCheck<EdgeState::kOut, QueryGraph, TargetGraph>(
          query_vertex_ptr, target_vertex_ptr, match_state)) {
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
  match_state.insert(std::make_pair(query_vertex_ptr, target_vertex_ptr));
  target_matched.insert(target_vertex_ptr);
}
template <enum EdgeState edge_state, class QueryGraph, class TargetGraph,
          class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap,
          class TargetVertexSet, class CandidateSetContainer>
inline void UpdateCandidateSetOneDirection(
    QueryVertexPtr query_vertex_ptr, TargetVertexPtr target_vertex_ptr,
    CandidateSetContainer &candidate_set, const MatchStateMap &match_state,
    const TargetVertexSet &target_matched) {
  std::map<typename TargetGraph::VertexType::LabelType,
           std::vector<TargetVertexPtr>>
      temp_adj_vertex;
  std::set<typename QueryGraph::EdgeType::LabelType> used_edge_label;

  for (auto label_it =
           ((edge_state == EdgeState::kIn) ? query_vertex_ptr->InEdgeCBegin()
                                           : query_vertex_ptr->OutEdgeCBegin());
       !label_it.IsDone(); label_it++) {
    QueryVertexPtr temp_ptr = (edge_state == EdgeState::kIn)
                                  ? label_it->const_src_ptr()
                                  : label_it->const_dst_ptr();
    if (match_state.count(temp_ptr)) continue;
    if (!candidate_set.count(temp_ptr)) continue;
    //枚举label
    if (used_edge_label.count(label_it->label())) continue;
    used_edge_label.insert(label_it->label());
    temp_adj_vertex.clear();
    size_t adj_count =
        (edge_state == EdgeState::kIn)
            ? target_vertex_ptr->CountInVertex(label_it->label())
            : target_vertex_ptr->CountOutVertex(label_it->label());
    if (adj_count > adj_vertex_limit) {
      continue;
    }
    for (auto it =
             ((edge_state == EdgeState::kIn)
                  ? target_vertex_ptr->InVertexCBegin(label_it->label())
                  : target_vertex_ptr->OutVertexCBegin(label_it->label()));
         !it.IsDone(); it++) {
      TargetVertexPtr temp_target_ptr = it;
      temp_adj_vertex[temp_target_ptr->label()].push_back(temp_target_ptr);
    }
    for (auto &it : temp_adj_vertex) {
      std::sort(it.second.begin(), it.second.end());
      auto erase_it = std::unique(it.second.begin(), it.second.end());
      it.second.erase(erase_it, it.second.end());
    }
    std::set<QueryVertexPtr> used_vertex;
    for (auto vertex_it = ((edge_state == EdgeState::kIn)
                               ? query_vertex_ptr->InEdgeCBegin()
                               : query_vertex_ptr->OutEdgeCBegin());
         !vertex_it.IsDone(); vertex_it++) {
      //枚举Vertex
      if (vertex_it->label() != label_it->label()) continue;

      QueryVertexPtr temp_vertex_ptr = (edge_state == EdgeState::kIn)
                                           ? vertex_it->const_src_ptr()
                                           : vertex_it->const_dst_ptr();
      if (used_vertex.count(temp_vertex_ptr)) continue;
      if (!candidate_set.count(temp_vertex_ptr)) continue;
      used_vertex.insert(temp_vertex_ptr);
      std::vector<TargetVertexPtr> res_candidate;
      if (match_state.count(temp_vertex_ptr)) continue;
      std::set_intersection(temp_adj_vertex[temp_vertex_ptr->label()].begin(),
                            temp_adj_vertex[temp_vertex_ptr->label()].end(),
                            candidate_set[temp_vertex_ptr].begin(),
                            candidate_set[temp_vertex_ptr].end(),
                            inserter(res_candidate, res_candidate.begin()));
      candidate_set[temp_vertex_ptr] = res_candidate;
    }
  }
}
template <class QueryGraph, class TargetGraph, class QueryVertexPtr,
          class TargetVertexPtr, class MatchStateMap, class TargetVertexSet,
          class CandidateSetContainer>
inline void UpdateCandidateSet(QueryVertexPtr query_vertex_ptr,
                               TargetVertexPtr target_vertex_ptr,
                               CandidateSetContainer &candidate_set,
                               const MatchStateMap &match_state,
                               const TargetVertexSet &target_matched) {
  // test liantong data:
  UpdateCandidateSetOneDirection<EdgeState::kIn, QueryGraph, TargetGraph>(
      query_vertex_ptr, target_vertex_ptr, candidate_set, match_state,
      target_matched);
  UpdateCandidateSetOneDirection<EdgeState::kOut, QueryGraph, TargetGraph>(
      query_vertex_ptr, target_vertex_ptr, candidate_set, match_state,
      target_matched);
}
template <class QueryVertexPtr, class TargetVertexPtr, class MatchStateMap,
          class TargetVertexSet>
inline void RestoreState(QueryVertexPtr query_vertex_ptr,
                         TargetVertexPtr target_vertex_ptr,
                         MatchStateMap &match_state,
                         TargetVertexSet &target_matched) {
  match_state.erase(query_vertex_ptr);
  target_matched.erase(target_vertex_ptr);
}
template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool MatchCallbackSaveSupp(std::map<QueryVertexPtr, TargetVertexPtr> &m,
                                  QueryVertexPtr *query_vertex_ptr,
                                  ResultContainer *r) {
  if (r) {
    r->insert(m[*query_vertex_ptr]);
  }
  return true;
}
template <class QueryVertexPtr, class TargetVertexPtr, class SuppContainer>
inline bool PruneSuppCallback(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m,
    QueryVertexPtr *query_vertex_ptr, SuppContainer *r) {
  if (r) {
    auto it = m.find(*query_vertex_ptr);
    if (it == m.end()) {
      return false;
    }
    TargetVertexPtr target_vertex_ptr = it->second;
    if (r->count(target_vertex_ptr)) {
      return true;
    }
  }
  return false;
}
template <class QueryVertexPtr, class TargetVertexPtr>
inline bool PruneCallbackEmpty(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m) {
  return false;
}
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class CandidateSetContainer, class QueryVertexPtr,
          class TargetVertexPtr, class MatchCallback, class PruneCallback>
bool _DPISO(const CandidateSetContainer &candidate_set,
            std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
            std::set<TargetVertexPtr> &target_matched, size_t &result_count,
            MatchCallback user_callback, PruneCallback prune_callback,
            time_t begin_time, double query_limit_time = 1200) {
  if (query_limit_time > 0 &&
      ((clock() - begin_time) / CLOCKS_PER_SEC) > query_limit_time) {
    return false;
  }
  // for (auto &it : match_state) {
  //  std::cout << it.first->id() << " " << it.second->id() << std::endl;
  //}
  // std::cout << std::endl;
  if (prune_callback(match_state)) {
    return true;
  }
  if (match_state.size() == candidate_set.size()) {
    result_count++;
    return user_callback(match_state);
  }
  QueryVertexPtr next_query_vertex_ptr =
      NextMatchVertex(candidate_set, match_state);
  for (const TargetVertexPtr &next_target_vertex_ptr :
       candidate_set.find(next_query_vertex_ptr)->second) {
    if (prune_callback(match_state)) {
      return true;
    }
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            next_query_vertex_ptr, next_target_vertex_ptr, match_state,
            target_matched)) {
      std::map<QueryVertexPtr, std::vector<TargetVertexPtr>>
          temp_candidate_set = candidate_set;
      UpdateState(next_query_vertex_ptr, next_target_vertex_ptr, match_state,
                  target_matched);
      auto t_begin = clock();
      UpdateCandidateSet<QueryGraph, TargetGraph>(
          next_query_vertex_ptr, next_target_vertex_ptr, temp_candidate_set,
          match_state, target_matched);
      auto t_end = clock();
      // std::cout << "update1 time = " << (1.0 * t_end - t_begin) /
      // CLOCKS_PER_SEC
      //          << std::endl;
      if (!_DPISO<match_semantics, QueryGraph, TargetGraph>(
              temp_candidate_set, match_state, target_matched, result_count,
              user_callback, prune_callback, begin_time, query_limit_time)) {
        return false;
      }
      RestoreState(next_query_vertex_ptr, next_target_vertex_ptr, match_state,
                   target_matched);
    }
  }

  return true;
}
template <enum EdgeState edge_state, class QueryVertexPtr,
          class TargetVertexPtr>
void UpdateParentSingleDirection(
    const std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
    const QueryVertexPtr update_query_vertex_ptr,
    std::map<QueryVertexPtr, std::vector<QueryVertexPtr>> &parent) {
  for (auto edge_it = (edge_state == EdgeState::kIn
                           ? update_query_vertex_ptr->InEdgeCBegin()
                           : update_query_vertex_ptr->OutEdgeCBegin());
       !edge_it.IsDone(); edge_it++) {
    QueryVertexPtr update_query_adj_ptr = edge_state == EdgeState::kIn
                                              ? edge_it->const_src_ptr()
                                              : edge_it->const_dst_ptr();
    if (match_state.find(update_query_adj_ptr) == match_state.end()) continue;
    auto &query_parent_set = parent.find(update_query_vertex_ptr)->second;
    auto find_it = parent.find(update_query_adj_ptr);
    if (find_it != parent.end()) {
      for (const auto &it : find_it->second) {
        query_parent_set.push_back(it);
      }
    }
  }
}
template <class QueryVertexPtr, class TargetVertexPtr>
void UpdateParent(
    const std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
    const QueryVertexPtr update_query_vertex_ptr,
    std::map<QueryVertexPtr, std::vector<QueryVertexPtr>> &parent) {
  parent[update_query_vertex_ptr].push_back(update_query_vertex_ptr);
  UpdateParentSingleDirection<EdgeState::kIn>(match_state,
                                              update_query_vertex_ptr, parent);
  UpdateParentSingleDirection<EdgeState::kOut>(match_state,
                                               update_query_vertex_ptr, parent);
  auto &l = parent.find(update_query_vertex_ptr)->second;
  std::sort(l.begin(), l.end());
  auto erase_it = std::unique(l.begin(), l.end());
  l.erase(erase_it, l.end());
}
int call = 0;
// using Fail Set
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class CandidateSetContainer, class QueryVertexPtr,
          class TargetVertexPtr, class MatchCallback, class PruneCallback>
bool _DPISO(const CandidateSetContainer &candidate_set,
            std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
            std::set<TargetVertexPtr> &target_matched,
            std::map<QueryVertexPtr, std::vector<QueryVertexPtr>> &parent,
            std::vector<QueryVertexPtr> &fail_set, size_t &result_count,
            MatchCallback user_callback, PruneCallback prune_callback,
            time_t begin_time, double query_limit_time = 1200) {
  if (query_limit_time > 0 &&
      ((clock() - begin_time) / CLOCKS_PER_SEC) > query_limit_time) {
    return false;
  }
  // call++;
  // for (auto &it : match_state) {
  //  std::cout << it.first->id() << " " << it.second->id() << std::endl;
  //}
  // std::cout << std::endl;
  if (match_state.size() == candidate_set.size()) {
    // find match ,so fail set is empty
    fail_set.clear();
    result_count++;
    return user_callback(match_state);
  }
  if (prune_callback(match_state)) {
    return true;
  }
  QueryVertexPtr next_query_vertex_ptr =
      NextMatchVertex(candidate_set, match_state);
  // cal this vertex's  parent
  UpdateParent(match_state, next_query_vertex_ptr, parent);
  std::vector<QueryVertexPtr> this_state_fail_set;
  bool find_fail_set_flag = false;
  if (candidate_set.find(next_query_vertex_ptr)->second.size() == 0) {
    // C(u) is empty ,so fail set = anc(u)
    this_state_fail_set = parent.find(next_query_vertex_ptr)->second;
  }
  for (const TargetVertexPtr &next_target_vertex_ptr :
       candidate_set.find(next_query_vertex_ptr)->second) {
    if (prune_callback(match_state)) {
      return true;
    }
    if (find_fail_set_flag && !this_state_fail_set.empty() &&
        !std::binary_search(this_state_fail_set.begin(),
                            this_state_fail_set.end(), next_query_vertex_ptr)) {
      // find fail set , u is not in fail set and fail set is not empty!
      // so not expand
      std::swap(fail_set, this_state_fail_set);
      return true;
    }
    if (!find_fail_set_flag &&
        match_semantics == MatchSemantics::kIsomorphism &&
        target_matched.count(next_target_vertex_ptr) > 0) {
      // find u' satisfy that match_state[u']=next_target_vertex_ptr
      // next_state_fail_set = anc[u] union anc[u']
      std::vector<QueryVertexPtr> next_state_fail_set;
      for (const auto &it : match_state) {
        if (it.second == next_target_vertex_ptr) {
          std::vector<QueryVertexPtr> temp_next_state_fail_set;
          std::set_union(std::begin(parent.find(next_query_vertex_ptr)->second),
                         std::end(parent.find(next_query_vertex_ptr)->second),
                         std::begin(parent.find(it.first)->second),
                         std::end(parent.find(it.first)->second),
                         inserter(temp_next_state_fail_set,
                                  temp_next_state_fail_set.begin()));
          std::swap(next_state_fail_set, temp_next_state_fail_set);
          break;
        }
      }
      // update this_state_fail_set
      // anc[u] must contain u ,so union fail set
      std::vector<QueryVertexPtr> temp_this_state_fail_set;
      std::set_union(
          std::begin(next_state_fail_set), std::end(next_state_fail_set),
          std::begin(this_state_fail_set), std::end(this_state_fail_set),
          inserter(temp_this_state_fail_set, temp_this_state_fail_set.begin()));
      std::swap(this_state_fail_set, temp_this_state_fail_set);
    }
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            next_query_vertex_ptr, next_target_vertex_ptr, match_state,
            target_matched)) {
      std::map<QueryVertexPtr, std::vector<TargetVertexPtr>>
          temp_candidate_set = candidate_set;
      UpdateState(next_query_vertex_ptr, next_target_vertex_ptr, match_state,
                  target_matched);
      UpdateCandidateSet<QueryGraph, TargetGraph>(
          next_query_vertex_ptr, next_target_vertex_ptr, temp_candidate_set,
          match_state, target_matched);
      std::vector<QueryVertexPtr> next_state_fail_set;
      if (!_DPISO<match_semantics, QueryGraph, TargetGraph>(
              temp_candidate_set, match_state, target_matched, parent,
              next_state_fail_set, result_count, user_callback, prune_callback,
              begin_time, query_limit_time)) {
        return false;
      }

      RestoreState(next_query_vertex_ptr, next_target_vertex_ptr, match_state,
                   target_matched);

      if (next_state_fail_set.empty()) {
        // if ont child node's fail set is empty
        // so this state's fail set is empty
        find_fail_set_flag = true;
        this_state_fail_set.clear();
      } else if (!std::binary_search(next_state_fail_set.begin(),
                                     next_state_fail_set.end(),
                                     next_query_vertex_ptr)) {
        // if one child node's fail set not contain next_query_vertex_ptr
        // so this state's fail set is next_state_fail_set
        find_fail_set_flag = true;
        this_state_fail_set = next_state_fail_set;
      } else if (!find_fail_set_flag) {
        std::vector<QueryVertexPtr> temp_this_state_fail_set;
        std::set_union(
            std::begin(next_state_fail_set), std::end(next_state_fail_set),
            std::begin(this_state_fail_set), std::end(this_state_fail_set),
            inserter(temp_this_state_fail_set,
                     temp_this_state_fail_set.begin()));
        std::swap(temp_this_state_fail_set, this_state_fail_set);
      }
    }
  }
  std::swap(fail_set, this_state_fail_set);
  return true;
}
template <class QueryVertexPtr, class TargetVertexPtr>
bool UpdateCandidateCallbackEmpty(
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set) {
  return true;
}
template <class QueryVertexPtr, class TargetVertexPtr,
          class CandidateSetContainer>
bool UpdateCandidateCallback(
    CandidateSetContainer &candidate_set, QueryVertexPtr *query_vertex_ptr,
    const std::vector<TargetVertexPtr> &possible_candidate) {
  if (possible_candidate.empty()) {
    return true;
  }
  std::vector<TargetVertexPtr> res_candidate;
  auto &query_vertex_candidate = candidate_set[*query_vertex_ptr];
  std::set_intersection(query_vertex_candidate.begin(),
                        query_vertex_candidate.end(),
                        possible_candidate.begin(), possible_candidate.end(),
                        inserter(res_candidate, res_candidate.begin()));
  if (res_candidate.empty()) return false;
  query_vertex_candidate = res_candidate;
  return true;
}

template <class QueryVertexPtr, class TargetVertexPtr>
inline bool MatchCallbackLimit(
    const std::map<QueryVertexPtr, TargetVertexPtr> &, int *max_result) {
  if (max_result && *max_result > 0) {
    (*max_result)--;
    if (*max_result == 0) return false;
  }
  return true;
}

template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool MatchCallbackSaveResult1(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    r->emplace_back(m);
  }

  return MatchCallbackLimit(m, max_result);
}

template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool MatchCallbackSaveResult2(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    using ResultType = typename ResultContainer::value_type;
    ResultType tmp;
    for (const auto &p : m) {
      tmp.emplace_back(p);
    }
    r->emplace_back(std::move(tmp));
  }

  return MatchCallbackLimit(m, max_result);
}

template <class A, class B>
inline void CopyMap(std::map<A, B> &c, const std::map<A, B> &m) {
  c = m;
};

template <class A, class B>
inline void CopyMap(std::list<std::pair<A, B>> &c, const std::map<A, B> &m) {
  for (const auto &p : m) {
    c.emplace_back(p);
  }
};

template <class A, class B>
inline void CopyMap(std::vector<std::pair<A, B>> &c, const std::map<A, B> &m) {
  for (const auto &p : m) {
    c.emplace_back(p);
  }
};

template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool MatchCallbackSaveResult(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, int *max_result,
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
inline void BFS(const QueryGraph &query_graph, InDegreeContainer &in_degree) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using QueryEdgeIDType = typename QueryGraph::EdgeType::IDType;
  std::set<QueryVertexPtr> used_vertex;
  std::set<QueryEdgeIDType> used_edge;
  // bfs get indegree (build dag but not really get new graph)
  for (auto it = query_graph.VertexCBegin(); !it.IsDone(); it++) {
    QueryVertexPtr query_vertex_ptr = it;
    if (used_vertex.count(query_vertex_ptr)) continue;
    std::queue<QueryVertexPtr> bfs_queue;
    in_degree[query_vertex_ptr] = 0;
    used_vertex.insert(query_vertex_ptr);
    bfs_queue.push(query_vertex_ptr);
    while (!bfs_queue.empty()) {
      QueryVertexPtr now_vertex_ptr = bfs_queue.front();
      bfs_queue.pop();
      for (auto edge_it = now_vertex_ptr->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (used_edge.count(edge_it->id())) continue;
        QueryVertexPtr next_vertex_ptr = edge_it->const_dst_ptr();
        used_edge.insert(edge_it->id());
        in_degree[next_vertex_ptr]++;
        if (!used_vertex.count(next_vertex_ptr)) {
          bfs_queue.push(next_vertex_ptr);
          used_vertex.insert(next_vertex_ptr);
        }
      }
      for (auto edge_it = now_vertex_ptr->InEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (used_edge.count(edge_it->id())) continue;
        QueryVertexPtr next_vertex_ptr = edge_it->const_src_ptr();
        used_edge.insert(edge_it->id());
        in_degree[next_vertex_ptr]++;
        if (!used_vertex.count(next_vertex_ptr)) {
          bfs_queue.push(next_vertex_ptr);
          used_vertex.insert(next_vertex_ptr);
        }
      }
    }
  }
}
template <class QueryGraph, class InDegreeContainer, class TopuSeqContainer>
inline void TopuSort(const QueryGraph &query_graph,
                     InDegreeContainer &in_degree, TopuSeqContainer &topu_seq) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using QueryEdgeIDType = typename QueryGraph::EdgeType::IDType;
  std::queue<QueryVertexPtr> topu_sort_queue;
  std::set<QueryEdgeIDType> used_edge;
  // topu sort
  for (auto it = query_graph.VertexCBegin(); !it.IsDone(); it++) {
    QueryVertexPtr query_vertex_ptr = it;
    if (in_degree[query_vertex_ptr] == 0) {
      topu_sort_queue.push(query_vertex_ptr);
    }
  }
  while (!topu_sort_queue.empty()) {
    QueryVertexPtr query_vertex_ptr = topu_sort_queue.front();
    topu_sort_queue.pop();
    topu_seq.emplace_back(query_vertex_ptr);
    for (auto edge_it = query_vertex_ptr->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      if (used_edge.count(edge_it->id())) continue;
      QueryVertexPtr next_vertex_ptr = edge_it->const_dst_ptr();
      in_degree[next_vertex_ptr]--;
      used_edge.insert(edge_it->id());
      if (in_degree[next_vertex_ptr] == 0) {
        topu_sort_queue.push(next_vertex_ptr);
      }
    }
    for (auto edge_it = query_vertex_ptr->InEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      if (used_edge.count(edge_it->id())) continue;
      QueryVertexPtr next_vertex_ptr = edge_it->const_src_ptr();
      in_degree[next_vertex_ptr]--;
      used_edge.insert(edge_it->id());
      if (in_degree[next_vertex_ptr] == 0) {
        topu_sort_queue.push(next_vertex_ptr);
      }
    }
  }
}
template <class QueryGraph, class TopuSeqContainer>
inline void GetTopuSeq(const QueryGraph &query_graph,
                       TopuSeqContainer &topu_seq) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using QueryEdgeIDType = typename QueryGraph::EdgeType::IDType;
  std::map<QueryVertexPtr, int> in_degree;
  BFS(query_graph, in_degree);
  assert(in_degree.size() == query_graph.CountVertex());
  TopuSort(query_graph, in_degree, topu_seq);
}
template <class QueryVertexPtr, class TargetVertexPtr, class EdgeCountContainer>
inline bool EdgeCheck(const QueryVertexPtr &query_vertex_ptr,
                      const QueryVertexPtr &temp_query_vertex_ptr,
                      const TargetVertexPtr &target_vertex_ptr,
                      const TargetVertexPtr &temp_target_vertex_ptr,
                      const EdgeCountContainer &out_edge_count,
                      const EdgeCountContainer &in_edge_count) {
  auto it = out_edge_count.find(temp_query_vertex_ptr);
  if (it != out_edge_count.end()) {
    for (const auto &edge_count : it->second) {
      if (target_vertex_ptr->CountOutEdge(
              edge_count.first, temp_target_vertex_ptr) < edge_count.second)
        return false;
    }
  }
  it = in_edge_count.find(temp_query_vertex_ptr);
  if (it != in_edge_count.end()) {
    for (const auto &edge_count : it->second) {
      if (target_vertex_ptr->CountInEdge(
              edge_count.first, temp_target_vertex_ptr) < edge_count.second)
        return false;
    }
  }
  return true;
}
template <bool is_out_direction, class QueryVertexPtr, class TargetVertexPtr,
          class CandidateSetContainer, class EdgeCountContainer>
inline bool CheckIsLegal(const QueryVertexPtr query_vertex_ptr,
                         const TargetVertexPtr target_vertex_ptr,
                         const std::set<QueryVertexPtr> &used_vertex,
                         const EdgeCountContainer &out_edge_count,
                         const EdgeCountContainer &in_edge_count,
                         CandidateSetContainer &candidate_set) {
  for (auto edge_it = is_out_direction ? query_vertex_ptr->OutEdgeCBegin()
                                       : query_vertex_ptr->InEdgeCBegin();
       !edge_it.IsDone(); edge_it++) {
    QueryVertexPtr next_vertex_ptr =
        is_out_direction ? edge_it->const_dst_ptr() : edge_it->const_src_ptr();
    if (!used_vertex.count(next_vertex_ptr)) continue;
    auto &l1 = candidate_set.find(next_vertex_ptr)->second;
    bool find_flag = false;
    for (auto target_edge_it =
             is_out_direction
                 ? target_vertex_ptr->OutVertexCBegin(edge_it->label())
                 : target_vertex_ptr->InVertexCBegin(edge_it->label());
         !target_edge_it.IsDone(); target_edge_it++) {
      TargetVertexPtr temp_target_vertex_ptr = target_edge_it;
      if (!EdgeCheck(query_vertex_ptr, next_vertex_ptr, target_vertex_ptr,
                     temp_target_vertex_ptr, out_edge_count, in_edge_count))
        continue;
      auto it1 = std::lower_bound(l1.begin(), l1.end(), temp_target_vertex_ptr);
      if (it1 != l1.end() && (*it1) == temp_target_vertex_ptr) {
        find_flag = true;
        break;
      }
    }
    if (!find_flag) return false;
  }
  return true;
}

}  // namespace _DAGDP
template <class QueryGraph, class TargetGraph, class CandidateSetContainer>
inline bool DAGDP(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    const std::vector<typename QueryGraph::VertexConstPtr> &topu_seq,
    CandidateSetContainer &candidate_set) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  using QueryEdgeLabelType = typename QueryGraph::EdgeType::LabelType;
  std::set<QueryVertexPtr> used_vertex;
  CandidateSetContainer temp_candidate_set;
  for (const auto &vertex_ptr : topu_seq) {
    used_vertex.insert(vertex_ptr);
    auto &l = candidate_set.find(vertex_ptr)->second;
    auto &l1 = temp_candidate_set[vertex_ptr];
    std::map<QueryVertexPtr, std::map<QueryEdgeLabelType, int>> in_edge_count,
        out_edge_count;
    for (auto edge_it = vertex_ptr->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      QueryVertexPtr dst_ptr = edge_it->const_dst_ptr();
      if (!used_vertex.count(dst_ptr)) {
        continue;
      }
      out_edge_count[dst_ptr][edge_it->label()]++;
    }
    for (auto edge_it = vertex_ptr->InEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      QueryVertexPtr dst_ptr = edge_it->const_src_ptr();
      if (!used_vertex.count(dst_ptr)) {
        continue;
      }
      in_edge_count[dst_ptr][edge_it->label()]++;
    }
    for (const auto &candidate_it : l) {
      bool candidate_flag = true;
      candidate_flag = _DAGDP::CheckIsLegal<true>(vertex_ptr, candidate_it,
                                                  used_vertex, out_edge_count,
                                                  in_edge_count, candidate_set);
      if (!candidate_flag) {
        continue;
      }
      candidate_flag = _DAGDP::CheckIsLegal<false>(
          vertex_ptr, candidate_it, used_vertex, out_edge_count, in_edge_count,
          candidate_set);
      if (candidate_flag) {
        l1.emplace_back(candidate_it);
      }
    }
    l = l1;
    if (l.empty()) return false;
  }
  return true;
}
template <class QueryGraph, class TargetGraph, class CandidateSetContainer>
inline bool RefineCandidateSet(const QueryGraph &query_graph,
                               const TargetGraph &target_graph,
                               CandidateSetContainer &candidate_set) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  std::vector<QueryVertexPtr> topu_seq;
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

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class MatchCallback, class PruneCallback,
          class UpdateCandidateCallback>
inline int DPISO_Recursive(const QueryGraph &query_graph,
                          const TargetGraph &target_graph,
                          MatchCallback user_callback,
                          PruneCallback prune_callback,
                          UpdateCandidateCallback update_candidate_callback,
                          double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;

  std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> candidate_set;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph,
                                         candidate_set)) {
    return 0;
  }
  if (!RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  if (!update_candidate_callback(candidate_set)) {
    return 0;
  }
  std::map<QueryVertexPtr, TargetVertexPtr> match_state;
  std::set<TargetVertexPtr> target_matched;
  size_t result_count = 0;
  if (query_graph.CountEdge() < large_query_edge) {
    _DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, result_count, user_callback,
        prune_callback, clock(), query_limit_time);
  } else {
    std::map<QueryVertexPtr, std::vector<QueryVertexPtr>> parent;
    std::vector<QueryVertexPtr> fail_set;
    _DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, parent, fail_set,
        result_count, user_callback, prune_callback, clock(), query_limit_time);
  }
  return static_cast<int>(result_count);
}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class MatchCallback, class PruneCallback>
inline int DPISO_Recursive(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    std::map<typename QueryGraph::VertexConstPtr,
             std::vector<typename TargetGraph::VertexConstPtr>> &candidate_set,
    std::map<typename QueryGraph::VertexConstPtr,
             typename TargetGraph::VertexConstPtr> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  std::set<TargetVertexPtr> target_matched;
  for (const auto &match : match_state) {
    target_matched.insert(match.second);
  }
  for (const auto &match : match_state) {
    UpdateCandidateSet<QueryGraph, TargetGraph>(
        match.first, match.second, candidate_set, match_state, target_matched);
  }
  size_t result_count = 0;
  if (query_graph.CountEdge() >= large_query_edge) {
    std::map<QueryVertexPtr, std::vector<QueryVertexPtr>> parent;
    for (const auto &match : match_state) {
      _dp_iso::UpdateParent(match_state, match.first, parent);
    }
    std::vector<QueryVertexPtr> fail_set;
    _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, parent, fail_set,
        result_count, user_callback, prune_callback, clock(), query_limit_time);
  } else {
    _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, result_count, user_callback,
        prune_callback, clock(), query_limit_time);
  }
  return static_cast<int>(result_count);
}
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class MatchCallback, class PruneCallback>
inline int DPISO_Recursive(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    std::map<typename QueryGraph::VertexConstPtr,
             std::vector<typename TargetGraph::VertexConstPtr>> &candidate_set,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  std::map<typename QueryGraph::VertexConstPtr,
           typename TargetGraph::VertexConstPtr>
      match_state;
  return DPISO_Recursive<match_semantics, QueryGraph, TargetGraph>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class MatchCallback, class PruneCallback,
          class UpdateCandidateCallback>
inline int DPISO_Recursive(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    std::map<typename QueryGraph::VertexConstPtr,
             typename TargetGraph::VertexConstPtr> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    UpdateCandidateCallback update_candidate_callback,
    double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  std::map<typename QueryGraph::VertexConstPtr,
           std::vector<typename TargetGraph::VertexConstPtr>>
      candidate_set;
  // std::cout << "begin" << std::endl;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph,
                                         candidate_set)) {
    return 0;
  }
  // std::cout << "init" << std::endl;
  for (const auto &match : match_state) {
    candidate_set[match.first].clear();
    candidate_set[match.first].push_back(match.second);
  }
  if (!RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }
  if (!update_candidate_callback(candidate_set)) {
    return 0;
  }
  return DPISO_Recursive<match_semantics>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class MatchCallback>
inline int DPISO_Recursive(const QueryGraph &query_graph,
                          const TargetGraph &target_graph,
                          MatchCallback user_callback,
                          double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  return DPISO_Recursive<match_semantics>(
      query_graph, target_graph, user_callback,
      std::bind(PruneCallbackEmpty<QueryVertexPtr, TargetVertexPtr>,
                std::placeholders::_1),
      std::bind(UpdateCandidateCallbackEmpty<QueryVertexPtr, TargetVertexPtr>,
                std::placeholders::_1),
      query_limit_time);
}
}  // namespace _dp_iso

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchCallback>
inline int DPISO(const QueryGraph &query_graph, const TargetGraph &target_graph,
                 MatchCallback user_callback, double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, user_callback, query_limit_time);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph>
inline int DPISO(const QueryGraph &query_graph, const TargetGraph &target_graph,
                 int max_result, double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;

  return DPISO<match_semantics>(
      query_graph, target_graph,
      std::bind(_dp_iso::MatchCallbackLimit<QueryVertexPtr, TargetVertexPtr>,
                std::placeholders::_1, &max_result),
      query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class ResultContainer>
inline int DPISO(const QueryGraph &query_graph, const TargetGraph &target_graph,
                 int max_result, ResultContainer &match_result,
                 double query_limit_time = 1200) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  match_result.clear();
  return DPISO<match_semantics>(
      query_graph, target_graph,
      std::bind(
          _dp_iso::MatchCallbackSaveResult<QueryVertexPtr, TargetVertexPtr,
                                           ResultContainer>,
          std::placeholders::_1, &max_result, &match_result),
      query_limit_time);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph>
inline int DPISO(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    typename QueryGraph::VertexConstPtr cal_supp_vertex_ptr,
    const std::vector<typename TargetGraph::VertexConstPtr> &possible_supp,
    std::vector<typename TargetGraph::VertexConstPtr> &supp,
    double single_query_limit_time = 100) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> candidate_set;
  if (!_dp_iso::InitCandidateSet<match_semantics>(query_graph, target_graph,
                                                  candidate_set)) {
    return 0;
  }
  if (!_dp_iso::UpdateCandidateCallback<QueryVertexPtr, TargetVertexPtr>(
          candidate_set, &cal_supp_vertex_ptr, possible_supp)) {
    return 0;
  }
  if (!_dp_iso::RefineCandidateSet(query_graph, target_graph, candidate_set)) {
    return 0;
  }

  for (const auto &target_ptr :
       candidate_set.find(cal_supp_vertex_ptr)->second) {
    /*
    if (target_ptr->id() != 715)
      continue;
    else {
      candidate_set[cal_supp_vertex_ptr].clear();
      candidate_set[cal_supp_vertex_ptr].push_back(target_ptr);
      _dp_iso::RefineCandidateSet(query_graph, target_graph, candidate_set);
      for (const auto &it : candidate_set) {
        std::cout << it.first->id() << " " << it.second.size() << std::endl;
      }
    }
    */
    time_t begin = clock();
    int max_result = 1;
    size_t result_count = 0;
    std::map<QueryVertexPtr, TargetVertexPtr> match_state;
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> temp_candidate_set =
        candidate_set;
    auto prune_callback =
        std::bind(_dp_iso::PruneCallbackEmpty<QueryVertexPtr, TargetVertexPtr>,
                  std::placeholders::_1);
    auto user_callback =
        std::bind(_dp_iso::MatchCallbackLimit<QueryVertexPtr, TargetVertexPtr>,
                  std::placeholders::_1, &max_result);
    /*
    match_state.emplace(cal_supp_vertex_ptr, target_ptr);
    _dp_iso::DPISO_Recursive<match_semantics, QueryGraph, TargetGraph>(
        query_graph, target_graph, temp_candidate_set, match_state,
        user_callback, prune_callback, single_query_limit_time);
    */
    std::set<TargetVertexPtr> target_matched;
    _dp_iso::UpdateState(cal_supp_vertex_ptr, target_ptr, match_state,
                         target_matched);
    auto t_begin = clock();
    _dp_iso::UpdateCandidateSet<QueryGraph, TargetGraph>(
        cal_supp_vertex_ptr, target_ptr, temp_candidate_set, match_state,
        target_matched);
    auto t_end = clock();
    // std::cout << "update time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
    //          << std::endl;
    if (query_graph.CountEdge() >= _dp_iso::large_query_edge) {
      std::map<QueryVertexPtr, std::vector<QueryVertexPtr>> parent;
      _dp_iso::UpdateParent(match_state, cal_supp_vertex_ptr, parent);
      std::vector<QueryVertexPtr> fail_set;

      _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
          temp_candidate_set, match_state, target_matched, parent, fail_set,
          result_count, user_callback, prune_callback, clock(),
          single_query_limit_time);
    } else {
      _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
          temp_candidate_set, match_state, target_matched, result_count,
          user_callback, prune_callback, clock(), single_query_limit_time);
    }

    if (max_result == 0) {
      supp.emplace_back(target_ptr);
    }
    time_t end = clock();
    // std::cout << "call=" << _dp_iso::call << std::endl;
    // std::cout << "id = " << target_ptr->id() << " "
    //          << "time = " << (1.0 * end - begin) / CLOCKS_PER_SEC <<
    //          std::endl;
  }
  return static_cast<int>(supp.size());
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchCallback,
          class PruneCallBack, class UpdateInitCandidateCallback>
inline int DPISO(const QueryGraph &query_graph, const TargetGraph &target_graph,
                 MatchCallback match_callback, PruneCallBack prune_callback,
                 UpdateInitCandidateCallback update_initcandidate_callback,
                 double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, match_callback, prune_callback,
      update_initcandidate_callback, query_limit_time);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchCallback,
          class PruneCallback>
inline int DPISO(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    std::map<typename QueryGraph::VertexConstPtr,
             std::vector<typename TargetGraph::VertexConstPtr>> &candidate_set,
    std::map<typename QueryGraph::VertexConstPtr,
             typename TargetGraph::VertexConstPtr> &match_state,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_graph, target_graph, candidate_set, match_state, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchCallback,
          class PruneCallback>
inline int DPISO(
    const QueryGraph &query_graph, const TargetGraph &target_graph,
    std::map<typename QueryGraph::VertexConstPtr,
             std::vector<typename TargetGraph::VertexConstPtr>> &candidate_set,
    MatchCallback user_callback, PruneCallback prune_callback,
    double query_limit_time = 1200) {
  return _dp_iso::DPISO_Recursive<match_semantics>(
      query_limit_time, target_graph, candidate_set, user_callback,
      prune_callback, query_limit_time);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchCallback,
          class PruneCallback, class UpdateCandidateCallback>
inline int DPISO(const QueryGraph &query_graph, const TargetGraph &target_graph,
                 std::map<typename QueryGraph::VertexConstPtr,
                          typename TargetGraph::VertexConstPtr> &match_state,
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
          class QueryGraph, class TargetGraph, class MatchCallback,
          class SuppContainer>
inline int DPISO(const QueryGraph &query_graph, const TargetGraph &target_graph,
                 SuppContainer &supp_list, MatchCallback user_callback,
                 double single_query_time = 100) {
  using QueryVertexPtr = typename QueryGraph::VertexConstPtr;
  using TargetVertexPtr = typename TargetGraph::VertexConstPtr;
  using MatchMap = std::map<QueryVertexPtr, TargetVertexPtr>;
  using MatchResult = std::vector<MatchMap>;
  using CandidateSetContainerType =
      std::map<QueryVertexPtr, std::vector<TargetVertexPtr>>;
  auto update_callback =
      std::bind(SuppUpdateCallBack<CandidateSetContainerType, SuppContainer>,
                std::placeholders::_1, std::ref(supp_list));

  MatchResult supp_match;
  int max_result = -1;
  auto match_callback =
      std::bind(_dp_iso::MatchCallbackSaveResult<QueryVertexPtr,
                                                 TargetVertexPtr, MatchResult>,
                std::placeholders::_1, &max_result, &supp_match);
  auto prune_callback =
      std::bind(_dp_iso::PruneCallbackEmpty<QueryVertexPtr, TargetVertexPtr>,
                std::placeholders::_1);
  DPISO<match_semantics>(query_graph, target_graph, match_callback,
                         prune_callback, update_callback, double(1200));
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
    for (const auto &it : single_match) {
      if (!std::binary_search(std::begin(candidate_set[it.first]),
                              std::end(candidate_set[it.first]), it.second)) {
        flag = 1;
        break;
      }
    }
    if (flag) continue;
    int max_result = 1;
    auto match_callback = std::bind(
        _dp_iso::MatchCallbackSaveResult<QueryVertexPtr, TargetVertexPtr,
                                         MatchResult>,
        std::placeholders::_1, &max_result, &match_result);
    auto temp_candidate_set = candidate_set;
    DPISO<match_semantics>(query_graph, target_graph, temp_candidate_set,
                           single_match, match_callback, prune_callback,
                           single_query_time);
  }
  for (auto &it : match_result) {
    user_callback(it);
  }
  return static_cast<int>(match_result.size());
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchSet>
inline int DPISO_UsingMatchSet(const QueryGraph &query_graph,
                               const TargetGraph &target_graph,
                               MatchSet &match_set) {
  using Match = typename MatchSet::MatchType;
  auto user_callback = [&match_set](const auto &match_state) {
    Match match;
    for (const auto &single_match : match_state) {
      match.AddMap(single_match.first, single_match.second);
    }
    match_set.AddMatch(match);
    return true;
  };
  return DPISO<match_semantics>(query_graph, target_graph, user_callback,
                                (double)-1);
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class Match, class MatchSet>
inline int DPISO_UsingPatricalMatchAndMatchSet(const QueryGraph &query_graph,
                                               const TargetGraph &target_graph,
                                               const Match &partical_match,
                                               MatchSet &match_set) {
  using PatternVertexConstPtr = typename QueryGraph::VertexConstPtr;
  using DataGraphVertexConstPtr = typename TargetGraph::VertexConstPtr;
  using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
  using MatchContainer = std::vector<MatchMap>;
  using CandidateSetContainer =
      std::map<PatternVertexConstPtr, std::vector<DataGraphVertexConstPtr>>;
  using PatternEdgePtr = typename QueryGraph::EdgeConstPtr;
  using DataGraphEdgePtr = typename TargetGraph::EdgeConstPtr;
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
  std::set<DataGraphVertexConstPtr> target_matched;
  for (auto vertex_it = query_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    PatternVertexConstPtr vertex_ptr = vertex_it;
    if (partical_match.HasMap(vertex_ptr)) {
      DataGraphVertexConstPtr match_vertex_ptr =
          partical_match.MapTo(vertex_ptr);
      match_state.insert(std::make_pair(vertex_ptr, match_vertex_ptr));
      target_matched.insert(match_vertex_ptr);
    }
  }
  int max_result = -1;
  size_t result_count = 0;
  auto user_callback = std::bind(
      _dp_iso::MatchCallbackSaveResult<PatternVertexConstPtr,
                                       DataGraphVertexConstPtr, MatchContainer>,
      std::placeholders::_1, &max_result, &match_result);
  if (query_graph.CountEdge() < _dp_iso::large_query_edge) {
    _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, result_count, user_callback,
        [](auto &match_state) { return false; }, clock(), -1);
  } else {
    using FailSetContainer = std::vector<PatternVertexConstPtr>;
    using ParentContainer =
        std::map<PatternVertexConstPtr, std::vector<PatternVertexConstPtr>>;
    ParentContainer parent;
    FailSetContainer fail_set;
    for (const auto &single_match : match_state) {
      _dp_iso::UpdateParent(match_state, single_match.first, parent);
    }
    _dp_iso::_DPISO<match_semantics, QueryGraph, TargetGraph>(
        candidate_set, match_state, target_matched, parent, fail_set,
        result_count, user_callback, [](auto &match_state) { return false; },
        clock(), -1.0);
  }

  for (const auto &single_match : match_result) {
    Match match;
    for (const auto &match_pair : single_match) {
      match.AddMap(match_pair.first, match_pair.second);
    }
    match_set.AddMatch(match);
  }
  return result_count;
}
}  // namespace GUNDAM
#endif
