#ifndef _GUNDAM_ALGORITHM_VF2_H
#define _GUNDAM_ALGORITHM_VF2_H

#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <type_traits>
#include <vector>

#include "gundam/algorithm/dp_iso.h"
#include "gundam/component/timer.h"
#include "gundam/match/match.h"
#include "match_semantics.h"

namespace GUNDAM {

namespace _vf2 {

enum EdgeState { kIn, kOut };

template <typename Ptr1, typename Ptr2>
class LabelEqual {
 public:
  constexpr bool operator()(const Ptr1 &a, const Ptr2 &b) const {
    return a->label() == b->label();
  }
};
/*
template <class GraphType, class Fn, class VertexRef>
inline bool ForEachVertexIf(
    const GraphType &graph, Fn f,
    LabelEqual<VertexRef,
               typename GraphType::VertexHandle> ,
    const VertexRef &vertex_ref) {
  for (auto vertex_iter = graph.VertexBegin(vertex_ref->label());
       !vertex_iter.IsDone(); ++vertex_iter) {
    if (!f(vertex_iter)) return false;
  }
  return true;
}
*/
template <class GraphType, class Fn, class VertexCompare, class VertexRef>
inline bool ForEachVertexIf(GraphType &graph, Fn f, VertexCompare vertex_comp,
                            const VertexRef &vertex_r) {
  for (auto vertex_iter = graph.VertexBegin(); !vertex_iter.IsDone();
       ++vertex_iter) {
    if (vertex_comp(vertex_r, vertex_iter)) {
      if (!f(vertex_iter)) return false;
    }
  }
  return true;
}

// template <enum EdgeState edge_state, class GraphType, class VertexPtr, class
// Fn,
//          class EdgeHandle1>
// inline bool ForEachEdgeIf(
//    const VertexPtr &vertex_handle, Fn f,
//    LabelEqual<EdgeHandle1, typename GraphType::EdgeConstPtr> /* edge_comp */,
//    const EdgeHandle1 &edge_a_handle) {
//  for (auto edge_iter = (edge_state == EdgeState::kIn)
//                            ?
//                            vertex_handle->InEdgeBegin(edge_a_handle->label())
//                            :
//                            vertex_handle->OutEdgeBegin(edge_a_handle->label());
//       !edge_iter.IsDone(); edge_iter++) {
//    typename GraphType::EdgeConstPtr edge_handle = edge_iter;
//    if (!f(edge_handle)) return false;
//  }
//  return true;
//}
//
// template <enum EdgeState edge_state, class GraphType, class VertexPtr, class
// Fn,
//          class EdgeCompare, class EdgeHandle1>
// inline bool ForEachEdgeIf(const VertexPtr &vertex_handle, Fn f,
//                          EdgeCompare edge_comp, const EdgeHandle1
//                          &edge_a_handle) {
//  for (auto edge_iter = (edge_state == EdgeState::kIn)
//                            ? vertex_handle->InEdgeBegin()
//                            : vertex_handle->OutEdgeBegin();
//       !edge_iter.IsDone(); edge_iter++) {
//    typename GraphType::EdgeConstPtr edge_handle = edge_iter;
//    if (edge_comp(edge_a_handle, edge_handle)) {
//      if (!f(edge_handle)) return false;
//    }
//  }
//  return true;
//}

// Init Candidate Set
template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class VertexCompare>
inline bool InitCandidateSet(
    QueryGraph &query_graph, TargetGraph &target_graph,
    VertexCompare vertex_comp,
    std::map<typename VertexHandle<QueryGraph>::type,
             std::vector<typename VertexHandle<TargetGraph>::type>>
        &candidate_set) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  // std::cout << query_graph.FindVertex(1) << std::endl;
  for (auto query_vertex_iter = query_graph.VertexBegin();
       !query_vertex_iter.IsDone(); ++query_vertex_iter) {
    QueryVertexHandle query_vertex_handle{query_vertex_iter};
    // std::cout << query_vertex_handle->id() << " " << query_vertex_handle <<
    // std::endl;
    auto query_in_count = query_vertex_handle->CountInEdge();
    auto query_out_count = query_vertex_handle->CountOutEdge();

    auto &l = candidate_set[query_vertex_handle];

    ForEachVertexIf(
        target_graph,
        [&query_in_count, &query_out_count,
         &l](const TargetVertexHandle &target_vertex_handle) -> bool {
          auto target_in_count = target_vertex_handle->CountInEdge();
          if (target_in_count >= query_in_count) {
            auto target_out_count = target_vertex_handle->CountOutEdge();
            if (target_out_count >= query_out_count) {
              l.push_back(target_vertex_handle);
            }
          }
          return true;
        },
        vertex_comp, query_vertex_handle);

    if (l.empty()) return false;
  }
  return true;
}

// Check target_id is in C(query_vertex_handle)
template <class QueryVertexHandle, class TargetVertexHandle,
          class TargetVertexID>
inline bool CheckIsInCandidateSet(
    QueryVertexHandle query_vertex_handle,
    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set,
    const TargetVertexID &target_id) {
  for (const auto &v : candidate_set[query_vertex_handle]) {
    if (v->id() == target_id) {
      return true;
    }
  }
  return false;
}

//
template <class QueryVertexHandle, class TargetVertexHandle>
inline QueryVertexHandle DetermineMatchOrder(
    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set,
    const std::map<QueryVertexHandle, TargetVertexHandle> &match_state) {
  std::set<QueryVertexHandle> next_query_set;
  for (const auto &match_pair : match_state) {
    QueryVertexHandle query_vertex_handle = match_pair.first;

    for (auto edge_iter = query_vertex_handle->OutEdgeBegin();
         !edge_iter.IsDone(); ++edge_iter) {
      auto query_opp_vertex_handle = edge_iter->dst_handle();
      if (match_state.count(query_opp_vertex_handle) == 0) {
        next_query_set.insert(query_opp_vertex_handle);
      }
    }

    for (auto edge_iter = query_vertex_handle->InEdgeBegin();
         !edge_iter.IsDone(); ++edge_iter) {
      auto query_opp_vertex_handle = edge_iter->src_handle();
      if (match_state.count(query_opp_vertex_handle) == 0) {
        next_query_set.insert(query_opp_vertex_handle);
      }
    }
  }

  if (next_query_set.empty()) {
    for (const auto &candidate_pair : candidate_set) {
      const auto &query_vertex_handle = candidate_pair.first;
      if (match_state.count(query_vertex_handle) == 0) {
        next_query_set.insert(query_vertex_handle);
      }
    }
  }
  assert(next_query_set.size() != 0);
  QueryVertexHandle res = *(next_query_set.begin());
  return res;
  /*
  QueryVertexHandle res;
  size_t min = SIZE_MAX;
  for (const auto &query_vertex_handle : next_query_set) {
    size_t candidate_count =
        candidate_set.find(query_vertex_handle)->second.size();
    if (candidate_count < min) {
      res = query_vertex_handle;
      min = candidate_count;
    }
  }*/
  // assert(min != SIZE_MAX);
  // return res;
}

// template <enum EdgeState edge_state, typename QueryVertexHandle,
//          typename TargetVertexHandle>
// inline bool JoinableCheck(
//    QueryVertexHandle query_vertex_handle, TargetVertexHandle
//    target_vertex_handle, const std::map<QueryVertexHandle,
//    TargetVertexHandle> &match_state) {
//
//  using QueryGraph = typename QueryVertexHandle::GraphType;
//  using TargetGraph = typename TargetVertexHandle::GraphType;
//  using  QueryEdgeHandle = typename EdgeHandle< QueryGraph>::type;
//  using TargetEdgeHandle = typename EdgeHandle<TargetGraph>::type;
//
//  for (auto query_edge_iter =
//           ((edge_state == EdgeState::kIn) ?
//           query_vertex_handle->InEdgeBegin()
//                                           :
//                                           query_vertex_handle->OutEdgeBegin());
//       !query_edge_iter.IsDone(); query_edge_iter++) {
//    QueryVertexHandle query_opp_vertex_handle =
//        (edge_state == EdgeState::kIn) ? query_edge_iter->src_handle()
//                                       : query_edge_iter->dst_handle();
//
//    // if (match_state.count(query_opp_vertex_handle) > 0) continue;
//    // TargetVertexHandle query_opp_match_vertex_handle =
//    //    match_state[query_opp_vertex_handle];
//
//    auto match_iter = match_state.find(query_opp_vertex_handle);
//    if (match_iter == match_state.end()) continue;
//    TargetVertexHandle query_opp_match_vertex_handle = match_iter->second;
//
//    bool find_target_flag = false;
//    ForEachEdgeIf<edge_state>(
//        target_vertex_handle,
//        [&query_opp_match_vertex_handle,
//         &find_target_flag](const TargetEdgeHandle &edge_handle) -> bool {
//          auto target_opp_vertex_handle = (edge_state == EdgeState::kIn)
//                                           ? edge_handle->src_handle()
//                                           : edge_handle->dst_handle();
//          if (target_opp_vertex_handle->id() ==
//          query_opp_match_vertex_handle->id())
//          {
//            find_target_flag = true;
//            return false;
//          }
//          return true;
//        },
//        LabelEqual<QueryEdgeHandle, TargetEdgeHandle>(), query_edge_iter);
//
//    //for (auto target_edge_iter =
//    //         ((edge_state == EdgeState::kIn)
//    //              ?
//    target_vertex_handle->InEdgeBegin(query_edge_iter->label())
//    //              :
//    target_vertex_handle->OutEdgeBegin(query_edge_iter->label()));
//    //     !target_edge_iter.IsDone(); target_edge_iter++) {
//    //  auto target_opp_vertex_handle = (edge_state == EdgeState::kIn)
//    //                                   ? target_edge_iter->src_handle()
//    //                                   : target_edge_iter->dst_handle();
//
//    //  if (target_opp_vertex_handle->id() ==
//    query_opp_match_vertex_handle->id()) {
//    //    find_target_flag = true;
//    //    break;
//    //  }
//    //}
//    if (!find_target_flag) return false;
//  }
//  return true;
//}

template <enum EdgeState edge_state, class QueryGraph, class TargetGraph,
          class MatchStateMap, class EdgeCompare>
inline bool JoinableCheck(
    typename VertexHandle<QueryGraph>::type &query_vertex_handle,
    typename VertexHandle<TargetGraph>::type &target_vertex_handle,
    MatchStateMap &match_state, EdgeCompare edge_comp) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using QueryEdgeHandle = typename EdgeHandle<QueryGraph>::type;
  using TargetEdgeHandle = typename EdgeHandle<TargetGraph>::type;

  std::set<typename TargetGraph::EdgeType::IDType> used_edge;

  for (auto query_edge_iter = (edge_state == EdgeState::kIn)
                                  ? query_vertex_handle->InEdgeBegin()
                                  : query_vertex_handle->OutEdgeBegin();
       !query_edge_iter.IsDone(); ++query_edge_iter) {
    QueryVertexHandle query_opp_vertex_handle;

    if constexpr (edge_state == EdgeState::kIn) {
      query_opp_vertex_handle = query_edge_iter->src_handle();
    } else {
      query_opp_vertex_handle = query_edge_iter->dst_handle();
    }

    auto match_iter = match_state.find(query_opp_vertex_handle);
    if (match_iter == match_state.end()) continue;

    TargetVertexHandle query_opp_match_vertex_handle = match_iter->second;

    bool find_target_flag = false;

    if constexpr (TargetGraph::vertex_has_edge_label_index &&
                  std::is_same_v<EdgeCompare, LabelEqual<QueryEdgeHandle,
                                                         TargetEdgeHandle>>) {
      for (auto target_edge_iter =
               (edge_state == EdgeState::kIn)
                   ? target_vertex_handle->InEdgeBegin(query_edge_iter->label())
                   : target_vertex_handle->OutEdgeBegin(
                         query_edge_iter->label());
           !target_edge_iter.IsDone(); ++target_edge_iter) {
        if (used_edge.count(target_edge_iter->id()) > 0) continue;
        TargetVertexHandle target_opp_vertex_handle =
            (edge_state == EdgeState::kIn) ? target_edge_iter->src_handle()
                                           : target_edge_iter->dst_handle();
        if (target_opp_vertex_handle == query_opp_match_vertex_handle) {
          find_target_flag = true;
          used_edge.insert(target_edge_iter->id());
          break;
        }
      }
    } else {
      for (auto target_edge_iter = (edge_state == EdgeState::kIn)
                                       ? target_vertex_handle->InEdgeBegin()
                                       : target_vertex_handle->OutEdgeBegin();
           !target_edge_iter.IsDone(); ++target_edge_iter) {
        if (used_edge.count(target_edge_iter->id()) > 0) continue;
        TargetVertexHandle target_opp_vertex_handle =
            (edge_state == EdgeState::kIn) ? target_edge_iter->src_handle()
                                           : target_edge_iter->dst_handle();
        if (target_opp_vertex_handle != query_opp_match_vertex_handle) continue;
        if (edge_comp(query_edge_iter, target_edge_iter)) {
          find_target_flag = true;
          used_edge.insert(target_edge_iter->id());
          break;
        }
      }
    }

    if (!find_target_flag) return false;
  }

  return true;
}

// template <class Key1, class Key2, class Value>
// using Map2 = typename std::map<Key1, std::map<Key2, Value>>;
//
// template <enum EdgeState edge_state, class QueryVertexHandle,
//          class TargetVertexHandle, class MatchStateMap, class EdgeCompare>
// inline bool JoinableCheck2(const QueryVertexHandle &query_vertex_handle,
//                          const TargetVertexHandle &target_vertex_handle,
//                          const MatchStateMap &match_state,
//                          EdgeCompare edge_comp) {
//  using QueryGraph = typename QueryVertexHandle::GraphType;
//  using TargetGraph = typename TargetVertexHandle::GraphType;
//  using  QueryEdgeHandle = typename EdgeHandle< QueryGraph>::type;
//  using TargetEdgeHandle = typename EdgeHandle<TargetGraph>::type;
//  using QueryEdgeLabelType = typename QueryGraph::EdgeType::LabelType;
//  using TargetEdgeLabelType = typename QueryGraph::TargetType::LabelType;
//
//  Map2<QueryEdgeLabelType, QueryVertexHandle, size_t> query_edge_count;
//
//  for (auto query_edge_iter =
//           ((edge_state == EdgeState::kIn) ?
//           query_vertex_handle->InEdgeBegin()
//                                           :
//                                           query_vertex_handle->OutEdgeBegin());
//       !query_edge_iter.IsDone(); query_edge_iter++) {
//    QueryEdgeHandle query_edge_handle = query_edge_iter;
//    QueryVertexHandle query_opp_vertex_handle = (edge_state == EdgeState::kIn)
//                                              ?
//                                              query_edge_handle->src_handle()
//                                              :
//                                              query_edge_handle->dst_handle();
//
//
//    auto match_iter = match_state.find(query_opp_vertex_handle);
//    if (match_iter == match_state.end()) continue;
//
//    TargetVertexHandle query_opp_match_vertex_handle = match_iter->second;
//
//    bool find_target_flag = false;
//    ForEachEdgeIf<edge_state>(
//        target_vertex_handle,
//        [&query_opp_match_vertex_handle,
//         &find_target_flag](const TargetEdgeHandle &target_edge_handle) ->
//         bool {
//          auto target_opp_vertex_handle = (edge_state == EdgeState::kIn)
//                                           ? target_edge_handle->src_handle()
//                                           : target_edge_handle->dst_handle();
//          if (target_opp_vertex_handle->id() ==
//          query_opp_match_vertex_handle->id())
//          {
//            find_target_flag = true;
//            return false;
//          }
//          return true;
//        },
//        edge_comp, query_edge_handle);
//
//    // for (auto target_edge_iter = ((edge_state == EdgeState::kIn)
//    //                                  ? target_vertex_handle->InEdgeBegin()
//    //                                  :
//    target_vertex_handle->OutEdgeBegin());
//    //     !target_edge_iter.IsDone(); target_edge_iter++) {
//    //  auto target_opp_vertex_handle = (edge_state == EdgeState::kIn)
//    //                                   ? target_edge_iter->src_handle()
//    //                                   : target_edge_iter->dst_handle();
//    //  if (target_opp_vertex_handle->id() ==
//    query_opp_match_vertex_handle->id()) {
//    //    // auto query_edge_handle =
//    //    //     (edge_state == EdgeState::kOut)
//    //    //         ?
//    query_vertex_handle->FindOutEdge(query_edge_iter->id())
//    //    //         :
//    //    // query_opp_vertex_handle->FindOutEdge(query_edge_iter->id());
//
//    //    // auto target_edge_handle =
//    //    //     (edge_state == EdgeState::kOut)
//    //    //         ?
//    //    target_vertex_handle->FindOutEdge(target_edge_iter->id())
//    //    //         : query_opp_match_vertex_handle->FindOutEdge(
//    //    //               target_edge_iter->id());
//
//    //    if (edge_comp(query_edge_iter, target_edge_iter)) {
//    //      find_target_flag = true;
//    //      break;
//    //    }
//    //  }
//    //}
//    if (!find_target_flag) return false;
//  }
//  return true;
//}

// template <enum MatchSemantics match_semantics, class QueryVertexHandle,
//          class TargetVertexHandle, class MatchStateMap, class
//          TargetVertexSet>
// inline bool IsJoinable(QueryVertexHandle query_vertex_handle,
//                       TargetVertexHandle target_vertex_handle,
//                       const MatchStateMap &match_state,
//                       const TargetVertexSet &target_matched) {
//  if (match_semantics == MatchSemantics::kIsomorphism &&
//      target_matched.count(target_vertex_handle) > 0) {
//    return false;
//  }
//  if (!JoinableCheck<EdgeState::kIn>(query_vertex_handle,
//  target_vertex_handle,
//                                     match_state)) {
//    return false;
//  }
//  if (!JoinableCheck<EdgeState::kOut>(query_vertex_handle,
//  target_vertex_handle,
//                                      match_state)) {
//    return false;
//  }
//  return true;
//}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexHandle, class TargetVertexHandle,
          class MatchStateMap, class TargetVertexSet, class EdgeCompare>
inline bool IsJoinable(QueryVertexHandle query_vertex_handle,
                       TargetVertexHandle target_vertex_handle,
                       const MatchStateMap &match_state,
                       const TargetVertexSet &target_matched,
                       EdgeCompare edge_comp) {
  if constexpr (match_semantics == MatchSemantics::kIsomorphism) {
    if (target_matched.count(target_vertex_handle) > 0) {
      return false;
    }
  }
  if (!JoinableCheck<EdgeState::kIn, QueryGraph, TargetGraph>(
          query_vertex_handle, target_vertex_handle, match_state, edge_comp)) {
    return false;
  }
  if (!JoinableCheck<EdgeState::kOut, QueryGraph, TargetGraph>(
          query_vertex_handle, target_vertex_handle, match_state, edge_comp)) {
    return false;
  }
  return true;
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class MatchStateMap, class TargetVertexSet>
inline void UpdateState(QueryVertexHandle query_vertex_handle,
                        TargetVertexHandle target_vertex_handle,
                        MatchStateMap &match_state,
                        TargetVertexSet &target_matched) {
  match_state.insert(std::make_pair(query_vertex_handle, target_vertex_handle));
  target_matched.insert(target_vertex_handle);
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class MatchStateMap, class TargetVertexSet>
inline void RestoreState(QueryVertexHandle query_vertex_handle,
                         TargetVertexHandle target_vertex_handle,
                         MatchStateMap &match_state,
                         TargetVertexSet &target_matched) {
  match_state.erase(query_vertex_handle);
  target_matched.erase(target_vertex_handle);
}

// template <enum MatchSemantics match_semantics, class QueryVertexHandle,
//          class TargetVertexHandle, class MatchCallback>
// bool _VF2(
//    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
//    &candidate_set, std::map<QueryVertexHandle, TargetVertexHandle>
//    &match_state, std::set<TargetVertexHandle> &target_matched, size_t
//    &result_count, MatchCallback user_callback) {
//  if (match_state.size() == candidate_set.size()) {
//    result_count++;
//    return user_callback(match_state);
//  }
//
//  QueryVertexHandle next_query_vertex_handle =
//      DetermineMatchOrder(candidate_set, match_state);
//
//  for (const TargetVertexHandle &next_target_vertex_handle :
//       candidate_set.find(next_query_vertex_handle)->second) {
//    if (IsJoinable<match_semantics>(next_query_vertex_handle,
//                                    next_target_vertex_handle, match_state,
//                                    target_matched)) {
//      UpdateState(next_query_vertex_handle, next_target_vertex_handle,
//      match_state,
//                  target_matched);
//
//      if (!_VF2<match_semantics>(candidate_set, match_state, target_matched,
//                                 result_count, user_callback)) {
//        return false;
//      }
//
//      RestoreState(next_query_vertex_handle, next_target_vertex_handle,
//      match_state,
//                   target_matched);
//    }
//  }
//
//  return true;
//}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class EdgeCompare, class MatchCallback>
bool _VF2(const std::map<typename VertexHandle<QueryGraph>::type,
                         std::vector<typename VertexHandle<TargetGraph>::type>>
              &candidate_set,
          std::map<typename VertexHandle<QueryGraph>::type,
                   typename VertexHandle<TargetGraph>::type> &match_state,
          std::set<typename VertexHandle<TargetGraph>::type> &target_matched,
          EdgeCompare edge_comp, size_t &result_count,
          MatchCallback user_callback) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  TimerIntervalReset();

  if (match_state.size() == candidate_set.size()) {
    ++result_count;

    bool flag = user_callback(match_state);
    // std::cout << "call back end!" << std::endl;

    TimerAddUpInterval(1);

    return flag;
  }

  QueryVertexHandle next_query_vertex_handle =
      DetermineMatchOrder(candidate_set, match_state);

  TimerAddUpInterval(2);

  const auto &candidate = candidate_set.find(next_query_vertex_handle)->second;

  TimerAddUpInterval(3);

  for (const TargetVertexHandle &next_target_vertex_handle : candidate) {
    TimerAddUpInterval(5);
    auto t_begin = clock();
    bool is_joinable = IsJoinable<match_semantics, QueryGraph, TargetGraph>(
        next_query_vertex_handle, next_target_vertex_handle, match_state,
        target_matched, edge_comp);
    auto t_end = clock();
    // std::cout << "is joinable time is"
    //          << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC << std::endl;
    TimerAddUpInterval(4);

    if (is_joinable) {
      UpdateState(next_query_vertex_handle, next_target_vertex_handle,
                  match_state, target_matched);

      TimerAddUpInterval(5);

      if (!_VF2<match_semantics, QueryGraph, TargetGraph>(
              candidate_set, match_state, target_matched, edge_comp,
              result_count, user_callback)) {
        return false;
      }

      TimerIntervalReset();

      RestoreState(next_query_vertex_handle, next_target_vertex_handle,
                   match_state, target_matched);
    }
  }

  TimerAddUpInterval(5);

  return true;
}

// template <enum MatchSemantics match_semantics, class QueryVertexHandle,
//          class TargetVertexHandle, class MatchStateMap, class
//          TargetVertexSet, class MatchStack>
// inline bool InitMatch(
//    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
//    &candidate_set, const MatchStateMap &match_state, const TargetVertexSet
//    &target_matched, MatchStack &match_stack) {
//  auto query_vertex_handle = DetermineMatchOrder(candidate_set, match_state);
//  auto &target_candidate = candidate_set.find(query_vertex_handle)->second;
//
//  auto target_vertex_iter = target_candidate.begin();
//  while (target_vertex_iter != target_candidate.end()) {
//    if (IsJoinable<match_semantics>(query_vertex_handle, *target_vertex_iter,
//                                    match_state, target_matched)) {
//      match_stack.emplace(std::make_pair(query_vertex_handle,
//      target_vertex_iter)); return true;
//    }
//    target_vertex_iter++;
//  }
//  return false;
//}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexHandle, class TargetVertexHandle,
          class MatchStateMap, class TargetVertexSet, class EdgeCompare,
          class MatchStack>
inline bool InitMatch(
    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set,
    const MatchStateMap &match_state, const TargetVertexSet &target_matched,
    EdgeCompare edge_comp, MatchStack &match_stack) {
  auto query_vertex_handle = DetermineMatchOrder(candidate_set, match_state);
  auto &target_candidate = candidate_set.find(query_vertex_handle)->second;

  auto target_vertex_iter = target_candidate.begin();
  while (target_vertex_iter != target_candidate.end()) {
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            query_vertex_handle, *target_vertex_iter, match_state,
            target_matched, edge_comp)) {
      match_stack.push(std::make_pair(query_vertex_handle, target_vertex_iter));
      return true;
    }
    ++target_vertex_iter;
  }
  return false;
}

// template <enum MatchSemantics match_semantics, class QueryVertexHandle,
//          class TargetVertexHandle, class MatchStateMap, class
//          TargetVertexSet, class MatchStack>
// inline bool NextMatch(
//    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
//    &candidate_set, const MatchStateMap &match_state, const TargetVertexSet
//    &target_matched, MatchStack &match_stack) {
//  auto query_vertex_handle = match_stack.top().first;
//  auto &target_vertex_iter = match_stack.top().second;
//  auto &target_candidate = candidate_set.find(query_vertex_handle)->second;
//
//  target_vertex_iter++;
//  while (target_vertex_iter != target_candidate.end()) {
//    if (IsJoinable<match_semantics>(query_vertex_handle, *target_vertex_iter,
//                                    match_state, target_matched)) {
//      return true;
//    }
//    target_vertex_iter++;
//  }
//  match_stack.pop();
//  return false;
//}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexHandle, class TargetVertexHandle,
          class MatchStateMap, class TargetVertexSet, class EdgeCompare,
          class MatchStack>
inline bool NextMatch(
    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set,
    const MatchStateMap &match_state, const TargetVertexSet &target_matched,
    EdgeCompare edge_comp, MatchStack &match_stack) {
  auto query_vertex_handle = match_stack.top().first;
  auto &target_vertex_iter = match_stack.top().second;
  auto &target_candidate = candidate_set.find(query_vertex_handle)->second;

  ++target_vertex_iter;
  while (target_vertex_iter != target_candidate.end()) {
    if (IsJoinable<match_semantics, QueryGraph, TargetGraph>(
            query_vertex_handle, *target_vertex_iter, match_state,
            target_matched, edge_comp)) {
      return true;
    }
    ++target_vertex_iter;
  }
  match_stack.pop();
  return false;
}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class VertexCompare, class EdgeCompare,
          class MatchCallback>
inline int VF2_Recursive(QueryGraph &query_graph, TargetGraph &target_graph,
                         VertexCompare vertex_comp, EdgeCompare edge_comp,
                         MatchCallback user_callback) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  TimerStart();

  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph, vertex_comp,
                                         candidate_set)) {
    return 0;
  }

  TimerAddUpInterval(0);

  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
  std::set<TargetVertexHandle> target_matched;
  size_t result_count = 0;
  _VF2<match_semantics, QueryGraph, TargetGraph>(candidate_set, match_state,
                                                 target_matched, edge_comp,
                                                 result_count, user_callback);

  TimerFinish();

  return static_cast<int>(result_count);
}

// template <enum MatchSemantics match_semantics, class QueryGraph,
//          class TargetGraph, class MatchCallback>
// inline int VF2_Recursive(QueryGraph &query_graph,
//                         TargetGraph &target_graph,
//                         MatchCallback user_callback) {
//  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
//  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
//  using  QueryEdgeHandle = typename EdgeHandle< QueryGraph>::type;
//  using TargetEdgeHandle = typename EdgeHandle<TargetGraph>::type;
//
//  return VF2_Recursive<match_semantics>(
//      query_graph, target_graph, LabelEqual<QueryVertexHandle,
//      TargetVertexHandle>(), LabelEqual<QueryEdgeHandle, TargetEdgeHandle>(),
//      user_callback);
//}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class QueryVertexHandle, class TargetVertexHandle,
          class EdgeCompare, class MatchCallback>
int VF2_NonRecursive(
    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set,
    std::map<QueryVertexHandle, TargetVertexHandle> &match_state,
    std::set<TargetVertexHandle> &target_matched,
    std::stack<
        std::pair<QueryVertexHandle,
                  typename std::vector<TargetVertexHandle>::const_iterator>>
        &match_stack,
    EdgeCompare edge_comp, MatchCallback user_callback) {
  const size_t &query_vertex_count = candidate_set.size();

  int result_count = 0;

  bool match_flag = true;

  do {
    if (match_state.size() == query_vertex_count) {
      result_count++;
      if (!user_callback(match_state)) break;
      match_flag = false;
    } else if (match_flag) {
      match_flag = InitMatch<match_semantics, QueryGraph, TargetGraph>(
          candidate_set, match_state, target_matched, edge_comp, match_stack);
    } else {
      match_flag = NextMatch<match_semantics, QueryGraph, TargetGraph>(
          candidate_set, match_state, target_matched, edge_comp, match_stack);
    }

    if (match_flag) {
      assert(!match_stack.empty());
      UpdateState(match_stack.top().first, *match_stack.top().second,
                  match_state, target_matched);
    } else {
      if (!match_stack.empty()) {
        RestoreState(match_stack.top().first, *match_stack.top().second,
                     match_state, target_matched);
      }
    }

  } while (!match_stack.empty());

  return result_count;
}

template <enum MatchSemantics match_semantics, class QueryGraph,
          class TargetGraph, class VertexCompare, class EdgeCompare,
          class MatchCallback>
inline int VF2_NonRecursive(QueryGraph &query_graph, TargetGraph &target_graph,
                            VertexCompare vertex_comp, EdgeCompare edge_comp,
                            MatchCallback user_callback) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph, vertex_comp,
                                         candidate_set)) {
    return 0;
  }

  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
  std::set<TargetVertexHandle> target_matched;
  std::stack<
      std::pair<QueryVertexHandle,
                typename std::vector<TargetVertexHandle>::const_iterator>>
      match_stack;

  return VF2_NonRecursive<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched, match_stack, edge_comp,
      user_callback);
}

template <enum MatchSemantics match_semantics, class QueryVertexHandle,
          class TargetVertexHandle, class MatchSequence,
          class = typename std::enable_if<std::is_convertible<
              std::pair<QueryVertexHandle, TargetVertexHandle>,
              typename MatchSequence::value_type>::value>::type,
          class EdgeCompare, class MatchCallback>
int VF2_NonRecursive(
    const std::map<QueryVertexHandle, std::vector<TargetVertexHandle>>
        &candidate_set,
    const MatchSequence &match_part, size_t anchor_count, EdgeCompare edge_comp,
    MatchCallback user_callback) {
  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
  std::set<TargetVertexHandle> target_matched;
  std::stack<
      std::pair<QueryVertexHandle,
                typename std::vector<TargetVertexHandle>::const_iterator>>
      match_stack;

  for (auto match_iter = match_part.begin(); match_iter != match_part.end();
       ++match_iter) {
    const QueryVertexHandle &query_vertex_handle = match_iter->first;
    const TargetVertexHandle &target_vertex_handle = match_iter->second;

    match_state.insert(
        std::make_pair(query_vertex_handle, target_vertex_handle));
    target_matched.insert(target_vertex_handle);

    if (anchor_count == 0) {
      auto target_vertex_iter =
          candidate_set.find(query_vertex_handle)->second.cbegin();
      while (*target_vertex_iter != target_vertex_handle) {
        ++target_vertex_iter;
      }
      match_stack.insert(query_vertex_handle, target_vertex_iter);
    } else {
      anchor_count--;
    }
  }

  return VF2_NonRecursive<match_semantics>(candidate_set, match_state,
                                           target_matched, match_stack,
                                           edge_comp, user_callback);
}

template <
    enum MatchSemantics match_semantics, class QueryGraph, class TargetGraph,
    class MatchSequence,
    class = typename std::enable_if<
        std::is_convertible<std::pair<typename VertexHandle<QueryGraph>::type,
                                      typename VertexHandle<TargetGraph>::type>,
                            typename MatchSequence::value_type>::value>::type,
    class VertexCompare,
    class =
        typename std::enable_if<std::is_function<VertexCompare>::value>::type,
    class EdgeCompare,
    class = typename std::enable_if<std::is_function<EdgeCompare>::value>::type,
    class MatchCallback,
    class =
        typename std::enable_if<std::is_function<MatchCallback>::value>::type>

inline int VF2_NonRecursive(QueryGraph &query_graph, TargetGraph &target_graph,
                            const MatchSequence &match_part,
                            size_t anchor_count, VertexCompare vertex_comp,
                            EdgeCompare edge_comp,
                            MatchCallback user_callback) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  if (!InitCandidateSet<match_semantics>(query_graph, target_graph, vertex_comp,
                                         candidate_set)) {
    return 0;
  }

  return VF2_NonRecursive<match_semantics>(
      candidate_set, match_part, anchor_count, edge_comp, user_callback);
}

//
// template <
//    enum MatchSemantics match_semantics, class QueryGraph, class TargetGraph,
//    class VertexCompare,
//    class =
//        typename std::enable_if<std::is_function<VertexCompare>::value>::type,
//    class EdgeCompare,
//    class = typename
//    std::enable_if<std::is_function<EdgeCompare>::value>::type, class
//    MatchCallback, class =
//        typename std::enable_if<std::is_function<MatchCallback>::value>::type>
// int VF2_NonRecursive(QueryGraph &query_graph,
//                     TargetGraph &target_graph, VertexCompare
//                     vertex_comp, EdgeCompare edge_comp, MatchCallback
//                     user_callback) {
//  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
//  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
//
//  const size_t &query_vertex_count = query_graph.CountVertex();
//
//  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
//  if (!InitCandidateSet<match_semantics>(query_graph, target_graph,
//  vertex_comp,
//                                         candidate_set)) {
//    return 0;
//  }
//
//  assert(candidate_set.size() == query_vertex_count);
//
//  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
//  std::set<TargetVertexHandle> target_matched;
//  std::stack<std::pair<QueryVertexHandle,
//                       typename
//                       std::vector<TargetVertexHandle>::const_iterator>>
//      match_stack;
//
//  int result_count = 0;
//
//  do {
//    bool match_flag;
//
//    if (match_state.size() == query_vertex_count) {
//      result_count++;
//      if (!user_callback(match_state)) break;
//      match_flag = false;
//    } else if (match_stack.size() == match_state.size()) {
//      match_flag = InitMatch<match_semantics>(
//          candidate_set, match_state, target_matched, edge_comp, match_stack);
//    } else {
//      assert(match_stack.size() == match_state.size() + 1);
//      match_flag = NextMatch<match_semantics>(
//          candidate_set, match_state, target_matched, edge_comp, match_stack);
//    }
//
//    if (match_flag) {
//      assert(!match_stack.empty());
//      UpdateState(match_stack.top().first, *match_stack.top().second,
//                  match_state, target_matched);
//    } else {
//      if (!match_stack.empty()) {
//        RestoreState(match_stack.top().first, *match_stack.top().second,
//                     match_state, target_matched);
//      }
//    }
//
//  } while (!match_stack.empty());
//
//  return result_count;
//}

template <class QueryVertexHandle, class TargetVertexHandle>
inline bool MatchCallbackLimit(
    const std::map<QueryVertexHandle, TargetVertexHandle> &, int *max_result) {
  if (max_result && *max_result > 0) {
    (*max_result)--;
    if (*max_result == 0) return false;
  }
  return true;
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class ResultContainer>
inline bool MatchCallbackSaveResult1(
    const std::map<QueryVertexHandle, TargetVertexHandle> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    r->emplace_back(m);
  }

  return MatchCallbackLimit(m, max_result);
}

template <class QueryVertexHandle, class TargetVertexHandle,
          class ResultContainer>
inline bool MatchCallbackSaveResult2(
    const std::map<QueryVertexHandle, TargetVertexHandle> &m, int *max_result,
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

template <class QueryVertexHandle, class TargetVertexHandle,
          class ResultContainer>
inline bool MatchCallbackSaveResult(
    const std::map<QueryVertexHandle, TargetVertexHandle> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    using ResultType = typename ResultContainer::value_type;

    ResultType tmp;
    CopyMap(tmp, m);
    r->emplace_back(std::move(tmp));
  }

  return MatchCallbackLimit(m, max_result);
}

};  // namespace _vf2

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class VertexCompare,
          class EdgeCompare, class MatchCallback>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               VertexCompare vertex_comp, EdgeCompare edge_comp,
               MatchCallback user_callback) {
  return _vf2::VF2_Recursive<match_semantics>(
      query_graph, target_graph, vertex_comp, edge_comp, user_callback);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class MatchCallback>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               MatchCallback user_callback) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using QueryEdgeHandle = typename EdgeHandle<QueryGraph>::type;
  using TargetEdgeHandle = typename EdgeHandle<TargetGraph>::type;

  return VF2<match_semantics>(
      query_graph, target_graph,
      _vf2::LabelEqual<QueryVertexHandle, TargetVertexHandle>(),
      _vf2::LabelEqual<QueryEdgeHandle, TargetEdgeHandle>(), user_callback);
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class VertexCompare,
          class EdgeCompare>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               VertexCompare vertex_comp, EdgeCompare edge_comp,
               int max_result) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  return VF2<match_semantics>(
      query_graph, target_graph, vertex_comp, edge_comp,
      std::bind(_vf2::MatchCallbackLimit<QueryVertexHandle, TargetVertexHandle>,
                std::placeholders::_1, &max_result));
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               int max_result) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  return VF2<match_semantics>(
      query_graph, target_graph,
      std::bind(_vf2::MatchCallbackLimit<QueryVertexHandle, TargetVertexHandle>,
                std::placeholders::_1, &max_result));
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class VertexCompare,
          class EdgeCompare, class ResultContainer>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               VertexCompare vertex_comp, EdgeCompare edge_comp, int max_result,
               ResultContainer &match_result) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  match_result.clear();

  return VF2<match_semantics>(
      query_graph, target_graph, vertex_comp, edge_comp,
      std::bind(
          _vf2::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                        ResultContainer>,
          std::placeholders::_1, &max_result, &match_result));
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class ResultContainer>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               int max_result, ResultContainer &match_result) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  match_result.clear();

  return VF2<match_semantics>(
      query_graph, target_graph,
      std::bind(
          _vf2::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                        ResultContainer>,
          std::placeholders::_1, &max_result, &match_result));
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class ResultContainer,
          class VertexCompare, class EdgeCompare>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               const typename QueryGraph::VertexType::IDType query_id,
               const typename TargetGraph::VertexType::IDType target_id,
               VertexCompare vertex_comp, EdgeCompare edge_comp, int max_result,
               ResultContainer &match_result) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  match_result.clear();
  std::map<QueryVertexHandle, std::vector<TargetVertexHandle>> candidate_set;
  auto t_begin = clock();
  if (!_vf2::InitCandidateSet<match_semantics, QueryGraph, TargetGraph>(
          query_graph, target_graph, vertex_comp, candidate_set))
    return 0;
  auto t_end = clock();
  // std::cout << "build time is " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
  //          << std::endl;
  QueryVertexHandle query_handle = query_graph.FindVertex(query_id);
  int find_target_flag = 0;
  for (const auto &target_handle : candidate_set[query_handle]) {
    if (target_handle->id() == target_id) {
      find_target_flag = 1;
      break;
    }
  }
  if (!find_target_flag) return 0;
  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
  std::set<TargetVertexHandle> target_matched;
  TargetVertexHandle target_handle = target_graph.FindVertex(target_id);
  _vf2::UpdateState(query_handle, target_handle, match_state, target_matched);
  size_t result_count = 0;
  return _vf2::_VF2<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched, edge_comp, result_count,
      std::bind(
          _vf2::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                        ResultContainer>,
          std::placeholders::_1, &max_result, &match_result));
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class ResultContainer,
          class VertexCompare, class EdgeCompare>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               std::map<typename VertexHandle<QueryGraph>::type,
                        std::vector<typename VertexHandle<TargetGraph>::type>>
                   &candidate_set,
               const typename QueryGraph::VertexType::IDType query_id,
               const typename TargetGraph::VertexType::IDType target_id,
               VertexCompare vertex_comp, EdgeCompare edge_comp, int max_result,
               ResultContainer &match_result) {
  using QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  QueryVertexHandle query_handle = query_graph.FindVertex(query_id);
  int find_target_flag = 0;
  auto t_begin = clock();
  for (const auto &target_handle : candidate_set[query_handle]) {
    if (target_handle->id() == target_id) {
      find_target_flag = 1;
      break;
    }
  }
  auto t_end = clock();
  // std::cout << "find time is " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
  //          << std::endl;
  if (!find_target_flag) return 0;
  std::map<QueryVertexHandle, TargetVertexHandle> match_state;
  std::set<TargetVertexHandle> target_matched;
  TargetVertexHandle target_handle = target_graph.FindVertex(target_id);
  _vf2::UpdateState(query_handle, target_handle, match_state, target_matched);
  size_t result_count = 0;
  return _vf2::_VF2<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched, edge_comp, result_count,
      std::bind(
          _vf2::MatchCallbackSaveResult<QueryVertexHandle, TargetVertexHandle,
                                        ResultContainer>,
          std::placeholders::_1, &max_result, &match_result));
}
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph, class QueryVertexPtr,
          class TargetVertexPtr, class VertexCompare, class EdgeCompare,
          class UserCallback>
inline int VF2(
    QueryGraph &query_graph, TargetGraph &target_graph,
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    std::map<QueryVertexPtr, TargetVertexPtr> &match_state,
    EdgeCompare edge_cmp, UserCallback user_callback) {
  std::set<TargetVertexPtr> target_matched;
  for (auto &[query_ptr, target_ptr] : match_state) {
    target_matched.emplace(target_ptr);
  }
  size_t result_count = 0;
  return _vf2::_VF2<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched, edge_cmp, result_count,
      user_callback);
}
// using GUNDAM::MatchSet
template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               MatchSet<QueryGraph, TargetGraph> &match_set) {
  using PatternVertexHandle = typename VertexHandle<QueryGraph>::type;
  using DataGraphVertexHandle = typename VertexHandle<TargetGraph>::type;
  using MatchMap = std::map<PatternVertexHandle, DataGraphVertexHandle>;
  using MatchContainer = std::vector<MatchMap>;
  MatchContainer match_result;
  size_t result_count = VF2<match_semantics, QueryGraph, TargetGraph>(
      query_graph, target_graph, -1, match_result);
  for (const auto &single_match : match_result) {
    Match<QueryGraph, TargetGraph> match;
    for (const auto &match_pair : single_match) {
      match.AddMap(match_pair.first, match_pair.second);
    }
    match_set.AddMatch(match);
  }
  return result_count;
}

template <enum MatchSemantics match_semantics = MatchSemantics::kIsomorphism,
          class QueryGraph, class TargetGraph>
inline int VF2(QueryGraph &query_graph, TargetGraph &target_graph,
               Match<QueryGraph, TargetGraph> &partical_match,
               MatchSet<QueryGraph, TargetGraph> &match_set) {
  using PatternVertexHandle = typename VertexHandle<QueryGraph>::type;
  using DataGraphVertexHandle = typename VertexHandle<TargetGraph>::type;
  using MatchMap = std::map<PatternVertexHandle, DataGraphVertexHandle>;
  using MatchContainer = std::vector<MatchMap>;
  using CandidateSetContainer =
      std::map<PatternVertexHandle, std::vector<DataGraphVertexHandle>>;
  using PatternEdgeHandle = typename EdgeHandle<QueryGraph>::type;
  using DataGraphEdgeHandle = typename EdgeHandle<TargetGraph>::type;
  CandidateSetContainer candidate_set;
  _vf2::InitCandidateSet<match_semantics>(
      query_graph, target_graph,
      _vf2::LabelEqual<PatternVertexHandle, DataGraphVertexHandle>(),
      candidate_set);
  MatchMap match_state;
  MatchContainer match_result;
  std::set<DataGraphVertexHandle> target_matched;
  for (auto vertex_it = query_graph.VertexBegin(); !vertex_it.IsDone();
       vertex_it++) {
    PatternVertexHandle vertex_handle = vertex_it;
    if (partical_match.HasMap(vertex_handle)) {
      DataGraphVertexHandle match_vertex_handle =
          partical_match.MapTo(vertex_handle);
      match_state.insert(std::make_pair(vertex_handle, match_vertex_handle));
      target_matched.insert(match_vertex_handle);
    }
  }
  int max_result = -1;
  size_t result_count = 0;
  auto user_callback = std::bind(
      _vf2::MatchCallbackSaveResult<PatternVertexHandle, DataGraphVertexHandle,
                                    MatchContainer>,
      std::placeholders::_1, &max_result, &match_result);

  _vf2::_VF2<match_semantics, QueryGraph, TargetGraph>(
      candidate_set, match_state, target_matched,
      _vf2::LabelEqual<PatternEdgeHandle, DataGraphEdgeHandle>(), result_count,
      user_callback);
  for (const auto &single_match : match_result) {
    Match<QueryGraph, TargetGraph> match;
    for (const auto &match_pair : single_match) {
      match.AddMap(match_pair.first, match_pair.second);
    }
    match_set.AddMatch(match);
  }
  return result_count;
}

}  // namespace GUNDAM

#endif