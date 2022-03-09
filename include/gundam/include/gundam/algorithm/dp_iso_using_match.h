#ifndef _GUNDAM_ALGORITHM_DPISO_USING_MATCH_H
#define _GUNDAM_ALGORITHM_DPISO_USING_MATCH_H
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

#include "gundam/algorithm/dp_iso.h"
#include "gundam/match/match.h"

#include "gundam/type_getter/vertex_handle.h"

namespace GUNDAM {

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
  const Match<QueryGraph,
             TargetGraph>& partial_match,
  const std::map<typename VertexHandle<QueryGraph>::type,
           std::vector<typename VertexHandle<TargetGraph>::type>>& candidate_set,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using MatchMap = std::map<QueryVertexHandle, 
                           TargetVertexHandle>;

  std::map<typename VertexHandle<QueryGraph>::type,
          std::vector<typename VertexHandle<TargetGraph>::type>> temp_candidate_set = candidate_set;

  MatchMap match_state;
  if (!partial_match.empty()){
    for (auto vertex_it = query_graph.VertexBegin(); 
             !vertex_it.IsDone();
              vertex_it++) {
      const QueryVertexHandle src_handle = vertex_it;
      if (!partial_match.HasMap(src_handle)) {
        continue;
      }
      const auto dst_handle = partial_match.MapTo(src_handle);
      assert(dst_handle);
      match_state.emplace(src_handle, dst_handle);
    }
    assert(match_state.size() == partial_match.size());
  }
  
  #ifndef NDEBUG
  for (const auto& candidate : candidate_set) {
    assert(candidate.first);
    assert(candidate.first == query_graph.FindVertex(candidate.first->id()));
  }
  #endif // NDEBUG

  return _dp_iso::DPISO_Recursive<match_semantics>(
                     query_graph,
                    target_graph,
                    temp_candidate_set, 
                      match_state, 
                      match_callback, 
                      prune_callback, 
                      time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
  const std::map<typename VertexHandle<QueryGraph>::type, 
            std::vector<typename VertexHandle<TargetGraph>::type>>& candidate_set,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  Match<QueryGraph, TargetGraph> match_state;

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(
                         query_graph,
                        target_graph,
                         match_state,
                       candidate_set,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
  const Match<QueryGraph, 
             TargetGraph>& partial_match,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using CandidateSetType = std::map<QueryVertexHandle, 
                       std::vector<TargetVertexHandle>>;

  CandidateSetType candidate_set;
  if (!_dp_iso::InitCandidateSet<match_semantics>(query_graph,
                                                 target_graph,
                                                  candidate_set)) {
    return 0;
  }
  if (!_dp_iso::RefineCandidateSet(query_graph, 
                                  target_graph, 
                                   candidate_set)) {
    return 0;
  }

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(
                         query_graph,
                        target_graph,
                       partial_match,
                       candidate_set,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
  const Match<QueryGraph, 
             TargetGraph>& partial_match,
   int64_t max_match = -1,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using MatchMap = std::map<QueryVertexHandle, 
                           TargetVertexHandle>;

  auto prune_callback = [](const MatchMap& match){
    // prune nothing, continue matching
    return false;
  };

  int64_t match_counter = 0;

  auto match_callback = [&max_match,
                         &match_counter](const MatchMap& match){
    if (max_match == -1) {
      // does not have support nothing
      // do nothing continue matching
      return true;
    }
    match_counter++;
    if (match_counter >= max_match){
      // reach max match, end matching
      return false;
    }
    // does not reach max match, continue matching
    return true;
  };

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(
                         query_graph,
                        target_graph,
                       partial_match,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  Match<QueryGraph, TargetGraph> partial_match;

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(
                         query_graph,
                        target_graph,
                       partial_match,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
   int64_t max_match = -1,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using MatchMap = std::map<QueryVertexHandle, 
                           TargetVertexHandle>;

  auto prune_callback = [](const MatchMap& match){
    // prune nothing, continue matching
    return false;
  };

  int64_t match_counter = 0;

  auto match_callback = [&max_match,
                         &match_counter](const MatchMap& match){
    if (max_match == -1) {
      // does not have support nothing
      // do nothing continue matching
      return true;
    }
    match_counter++;
    if (match_counter >= max_match){
      // reach max match, end matching
      return false;
    }
    // does not reach max match, continue matching
    return true;
  };

  // the initial partial match is empty
  Match<QueryGraph, TargetGraph> match_state;

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(
                         query_graph,
                        target_graph,
                         match_state,
                      prune_callback,
                      match_callback, 
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
  const Match<QueryGraph, 
             TargetGraph>& partial_match,
     MatchSet<QueryGraph, 
             TargetGraph>& match_result,
   int64_t max_match = -1,
   double time_limit = -1.0){

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using MatchMap = std::map<QueryVertexHandle, 
                           TargetVertexHandle>;

  auto prune_callback = [](const MatchMap& match){
    // prune nothing, continue matching
    return false;
  };

  auto match_callback = [&match_result,
                         &max_match](const MatchMap& match){
    // add this match to match_result, continue matching
    Match<QueryGraph, TargetGraph> new_match;
    for (const auto& map : match){
      new_match.AddMap(map.first, map.second);
    }
    match_result.AddMatch(new_match);
    if ((max_match != -1) && (match_result.size() >= max_match)){
      // reach max match, end matching
      return false;
    }
   return true;
  };

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(query_graph, 
                                         target_graph,
                                        partial_match,
                                          prune_callback,
                                          match_callback);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
  MatchSet<QueryGraph, TargetGraph>& match_result,
   int64_t max_match = -1,
   double time_limit = -1.0) {

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using MatchMap = std::map<QueryVertexHandle, 
                           TargetVertexHandle>;

  auto prune_callback = [](const MatchMap& match){
    // prune nothing, continue matching
    return false;
  };

  auto match_callback = [&match_result,
                         &max_match](const MatchMap& match){
    // add this match to match_result, continue matching
    Match<QueryGraph, TargetGraph> new_match;
    for (const auto& map : match){
      new_match.AddMap(map.first, map.second);
    }
    match_result.AddMatch(new_match);
    if ((max_match != -1) && (match_result.size() >= max_match)){
      // reach max match, end matching
      return false;
    }
   return true;
  };

  // the initial partial match is empty
  Match<QueryGraph, TargetGraph> match_state;

  return DpisoUsingMatch<match_semantics,
                         match_algorithm>(
                         query_graph,
                        target_graph,
                         match_state,
                      prune_callback,
                      match_callback, 
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          enum MatchAlgorithm match_algorithm
             = MatchAlgorithm::kDagDp,
          typename    QueryGraph, 
          typename   TargetGraph>
inline int IncrementalDpisoUsingMatch(
                      QueryGraph &query_graph, 
                     TargetGraph &target_graph,
     std::vector<typename VertexHandle<TargetGraph>::type> &delta_target_graph,
  std::function<bool(const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
                           double query_limit_time = -1){
  return IncreamentDPISO<match_semantics>(
                         query_graph, 
                        target_graph,
                  delta_target_graph,
                         match_callback,
                         query_limit_time);
}

}  // namespace GUNDAM

#endif //_DPISO_USING_MATCH_H
