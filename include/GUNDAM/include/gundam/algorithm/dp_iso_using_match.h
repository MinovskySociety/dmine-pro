#ifndef _DPISO_USING_MATCH_H
#define _DPISO_USING_MATCH_H
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

namespace GUNDAM {

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          typename TargetGraph,
          typename  QueryGraphVertexPtr,
          typename TargetGraphVertexPtr>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
  Match<QueryGraph,
       TargetGraph>& partial_match,
  std::map<QueryGraphVertexPtr,
           std::vector<TargetGraphVertexPtr>>& candidate_set,
  std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                    typename TargetGraph::VertexConstPtr>&)> prune_callback,
  std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                    typename TargetGraph::VertexConstPtr>&)> match_callback,
   double time_limit = -1.0) {

  using MatchMap = std::map<QueryGraphVertexPtr, 
                           TargetGraphVertexPtr>;

  using MatchContainer = std::vector<MatchMap>;

  MatchMap match_state;
  std::set<TargetGraphVertexPtr> target_matched;
  for (auto vertex_it = query_graph.VertexBegin(); 
           !vertex_it.IsDone();
            vertex_it++) {
    const QueryGraphVertexPtr src_ptr = vertex_it;
    if (!partial_match.HasMap(src_ptr)) {
      continue;
    }
    const auto dst_ptr = partial_match.MapTo(src_ptr);
    match_state.emplace(src_ptr, dst_ptr);
    target_matched.insert(dst_ptr);
  }

  if (query_graph.CountEdge() < _dp_iso::large_query_edge) {
    size_t result_count = 0;
    _dp_iso::_DPISO<match_semantics, 
                    QueryGraph, 
                   TargetGraph>(
        candidate_set, match_state, target_matched, result_count, 
        match_callback,
        prune_callback, 
        clock(), time_limit);
    return result_count;
  }

  using FailSetContainer = std::vector<QueryGraphVertexPtr>;
  using  ParentContainer = std::map<QueryGraphVertexPtr,
                                    std::vector<QueryGraphVertexPtr>>;
  FailSetContainer fail_set;
   ParentContainer parent;
  for (const auto &map : match_state) {
    _dp_iso::UpdateParent(match_state, map.first, parent);
  }

  size_t result_count = 0;
  _dp_iso::_DPISO<match_semantics, 
                  QueryGraph, 
                 TargetGraph>(
      candidate_set, match_state, target_matched, parent, fail_set,
      result_count,
      match_callback,
      prune_callback,
      clock(), time_limit);
    
  return result_count;
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          typename TargetGraph,
          typename  QueryGraphVertexPtr,
          typename TargetGraphVertexPtr>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
  std::map<QueryGraphVertexPtr, 
            std::vector<TargetGraphVertexPtr>>& candidate_set,
  std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                    typename TargetGraph::VertexConstPtr>&)> prune_callback,
  std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                    typename TargetGraph::VertexConstPtr>&)> match_callback,
   double time_limit = -1.0) {

  Match<QueryGraph, TargetGraph> match_state;

  return DpisoUsingMatch(query_graph,
                        target_graph,
                         match_state,
                       candidate_set,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
  Match<QueryGraph,
       TargetGraph>& partial_match,
  std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                    typename TargetGraph::VertexConstPtr>&)> prune_callback,
  std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                    typename TargetGraph::VertexConstPtr>&)> match_callback,
   double time_limit = -1.0) {

  using  QueryGraphVertexPtr = typename  QueryGraph::VertexConstPtr;
  using TargetGraphVertexPtr = typename TargetGraph::VertexConstPtr;

  using CandidateSetType = std::map<QueryGraphVertexPtr, 
                       std::vector<TargetGraphVertexPtr>>;

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

  return DpisoUsingMatch(query_graph,
                        target_graph,
                       partial_match,
                       candidate_set,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
        QueryGraph&  query_graph,
       TargetGraph& target_graph,
    std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                      typename TargetGraph::VertexConstPtr>&)> prune_callback,
    std::function<bool(const std::map<typename  QueryGraph::VertexConstPtr, 
                                      typename TargetGraph::VertexConstPtr>&)> match_callback,
   double time_limit = -1.0) {

  Match<QueryGraph, TargetGraph> partial_match;

  return DpisoUsingMatch(query_graph,
                        target_graph,
                       partial_match,
                      prune_callback,
                      match_callback,
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          typename TargetGraph>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
   int64_t max_match = -1,
   double time_limit = -1.0) {

  using  QueryGraphVertexPtr = typename  QueryGraph::VertexConstPtr;
  using TargetGraphVertexPtr = typename TargetGraph::VertexConstPtr;

  using MatchMap = std::map<QueryGraphVertexPtr, 
                           TargetGraphVertexPtr>;

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

  return DpisoUsingMatch(query_graph,
                        target_graph,
                         match_state,
                      prune_callback,
                      match_callback, 
                          time_limit);
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename    QueryGraph,
          typename   TargetGraph>
inline size_t DpisoUsingMatch(
   QueryGraph&  query_graph, 
  TargetGraph& target_graph,
  MatchSet<QueryGraph,
          TargetGraph>& match_result,
   int64_t max_match = -1,
   double time_limit = -1.0) {

  using  QueryGraphVertexPtr = typename  QueryGraph::VertexConstPtr;
  using TargetGraphVertexPtr = typename TargetGraph::VertexConstPtr;

  using MatchMap = std::map<QueryGraphVertexPtr, 
                           TargetGraphVertexPtr>;

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

  return DpisoUsingMatch(query_graph,
                        target_graph,
                         match_state,
                      prune_callback,
                      match_callback, 
                          time_limit);
}

}  // namespace GUNDAM

#endif //_DPISO_USING_MATCH_H
