#ifndef _GUNDAM_TOOL_MAXIMAL_COMMON_SUBGRAPH
#define _GUNDAM_TOOL_MAXIMAL_COMMON_SUBGRAPH

#include "gundam/tool/unique_patterns.h"
#include "gundam/tool/sub_graph_of.h"

namespace GUNDAM{

namespace _maximal_common_subgraph{

template <typename GraphPatternType>
inline void AddNewEdgeInPattern(
                          GraphPatternType&   current_pattern,
              std::vector<GraphPatternType>& expanded_patterns,
                 typename GraphPatternType
                                ::EdgeType
                                  ::IDType& new_edge_id,
        std::set<typename GraphPatternType
                                ::EdgeType
                               ::LabelType>& edge_label_set,
        std::set<std::tuple<
                 typename GraphPatternType
                              ::VertexType
                               ::LabelType,
                 typename GraphPatternType
                                ::EdgeType
                               ::LabelType,
                 typename GraphPatternType
                              ::VertexType
                               ::LabelType>>& edge_type_set) {

  for (auto src_vertex_iter = current_pattern.VertexBegin();
           !src_vertex_iter.IsDone();
            src_vertex_iter++) {
    for (auto dst_vertex_iter = current_pattern.VertexBegin();
             !dst_vertex_iter.IsDone(); 
              dst_vertex_iter++) {
      // not add self loop
      if (src_vertex_iter->id() == dst_vertex_iter->id()) 
        continue;
      for (const auto &edge_label : edge_label_set) {
        // not add edge which does not exist in data graph
        if (!edge_type_set.count(std::tuple(
                src_vertex_iter->label(),
                edge_label, 
                dst_vertex_iter->label()))) {
          continue;
        }
        GraphPatternType expand_pattern(current_pattern);
        auto [edge_handle,
              edge_ret] = expand_pattern.AddEdge(src_vertex_iter->id(), 
                                                 dst_vertex_iter->id(),
                                                 edge_label, 
                                                 new_edge_id);
        assert(edge_ret);
        expanded_patterns.emplace_back(std::move(expand_pattern));
      }
    }
  }
  return;
}

template <typename GraphPatternType>
inline void AddNewEdgeOutPattern(
                          GraphPatternType&   current_pattern,
              std::vector<GraphPatternType>& expanded_patterns,
                 typename GraphPatternType
                              ::VertexType
                                  ::IDType& new_vertex_id,
                 typename GraphPatternType
                                ::EdgeType
                                  ::IDType& new_edge_id,
        std::set<typename GraphPatternType
                              ::VertexType
                               ::LabelType>& vertex_label_set,
        std::set<typename GraphPatternType
                                ::EdgeType
                               ::LabelType>& edge_label_set,
        std::set<std::tuple<
                 typename GraphPatternType
                              ::VertexType
                               ::LabelType,
                 typename GraphPatternType
                                ::EdgeType
                               ::LabelType,
                 typename GraphPatternType
                              ::VertexType
                               ::LabelType>>& edge_type_set) {

  for (auto vertex_iter = current_pattern.VertexBegin(); 
           !vertex_iter.IsDone();
            vertex_iter++) {
    for (const auto &vertex_label : vertex_label_set) {
      for (const auto &edge_label :   edge_label_set) {
        // not add edge which does not exist in data graph
        if (edge_type_set.count(std::tuple(
                                vertex_iter->label(),
                                        edge_label,
                                      vertex_label))) {
          GraphPatternType expand_pattern(current_pattern);
          expand_pattern.AddVertex(new_vertex_id,
                                       vertex_label);
          expand_pattern.AddEdge(vertex_iter->id(), 
                                 new_vertex_id,
                                       edge_label, 
                                   new_edge_id);
          expanded_patterns.emplace_back(std::move(expand_pattern));
        }
        if (edge_type_set.count(std::tuple(
                                      vertex_label,
                                        edge_label, 
                                vertex_iter->label()))) {
          GraphPatternType expand_pattern(current_pattern);
          expand_pattern.AddVertex(new_vertex_id, vertex_label);
          expand_pattern.AddEdge(new_vertex_id, 
                                 vertex_iter->id(),
                                       edge_label,
                                   new_edge_id);
          expanded_patterns.emplace_back(std::move(expand_pattern));
        }
      }
    }
  }
  return;
}

template <typename GraphPatternType>
std::vector<GraphPatternType> ExpandPattern(
        std::vector<GraphPatternType>& current_graph_patterns,
        std::set<typename GraphPatternType
                              ::VertexType
                               ::LabelType>& vertex_label_set,
        std::set<typename GraphPatternType
                                ::EdgeType
                               ::LabelType>& edge_label_set,
        std::set<std::tuple<
                  typename GraphPatternType
                               ::VertexType
                                ::LabelType,
                  typename GraphPatternType
                                 ::EdgeType
                                ::LabelType,
                  typename GraphPatternType
                               ::VertexType
                                ::LabelType>>& edge_type_set){
  
  using VertexIDType = typename GraphPatternType
                                    ::VertexType
                                        ::IDType;

  using   EdgeIDType = typename GraphPatternType
                                      ::EdgeType
                                        ::IDType;

  std::vector<GraphPatternType> expanded_graph_patterns;
  
  for (auto& current_graph_pattern : current_graph_patterns){

    VertexIDType new_vertex_id = 0;
      EdgeIDType new_edge_id   = 0;

    for (auto vertex_it = current_graph_pattern.VertexBegin();
             !vertex_it.IsDone();
              vertex_it++) {
      new_vertex_id = new_vertex_id > vertex_it->id()?
                      new_vertex_id : vertex_it->id();
      for (auto edge_it = vertex_it->OutEdgeBegin();
               !edge_it.IsDone();
                edge_it++) {
        new_edge_id = new_edge_id > edge_it->id()?
                      new_edge_id : edge_it->id();
      }
    }
    new_vertex_id++;
    new_edge_id++;

    AddNewEdgeInPattern(current_graph_pattern,
                       expanded_graph_patterns,
                        new_edge_id,
                            edge_label_set,
                            edge_type_set);

    AddNewEdgeOutPattern(current_graph_pattern,
                        expanded_graph_patterns,
                         new_vertex_id,
                         new_edge_id,
                         vertex_label_set,
                           edge_label_set,
                            edge_type_set);
  }
  return std::move(expanded_graph_patterns);
}

};

template <typename GraphPatternType>
std::vector<GraphPatternType> 
  MaximalCommonSubgraph(GraphPatternType& q0,
                        GraphPatternType& q1,
                  Match<GraphPatternType,
                        GraphPatternType>& match_q0_to_q1){
  if (SubGraphOf(q0, q1)){
    std::vector<GraphPatternType> ret;
    ret.emplace_back(q0);
    return std::move(ret);
  }
  if (SubGraphOf(q1, q0)){
    std::vector<GraphPatternType> ret;
    ret.emplace_back(q1);
    return std::move(ret);
  }
  using VertexIDType = typename GraphPatternType
                                    ::VertexType
                                        ::IDType;
  using VertexLabelType = typename GraphPatternType
                                       ::VertexType
                                        ::LabelType;
  using   EdgeLabelType = typename GraphPatternType
                                         ::EdgeType
                                        ::LabelType;
  std::set<VertexLabelType> q0_vertex_label_set;
  std::set<  EdgeLabelType>   q0_edge_label_set;
  std::set<std::tuple<VertexLabelType,
                        EdgeLabelType,
                      VertexLabelType>> q0_edge_type_set;
  for (auto vertex_it = q0.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    q0_vertex_label_set.emplace(vertex_it->label());
    for (auto edge_it = vertex_it->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      q0_edge_type_set.emplace(edge_it->src_handle()->label(),
                               edge_it->label(),
                               edge_it->dst_handle()->label());
      q0_edge_label_set.emplace(edge_it->label());
    }
  }
  std::set<VertexLabelType> q1_vertex_label_set;
  std::set<  EdgeLabelType>   q1_edge_label_set;
  std::set<std::tuple<VertexLabelType,
                        EdgeLabelType,
                      VertexLabelType>> q1_edge_type_set;
  for (auto vertex_it = q1.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    q1_vertex_label_set.emplace(vertex_it->label());
    for (auto edge_it = vertex_it->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      q1_edge_type_set.emplace(edge_it->src_handle()->label(),
                               edge_it->label(),
                               edge_it->dst_handle()->label());
      q1_edge_label_set.emplace(edge_it->label());
    }
  }

  std::set<VertexLabelType> q0_intersect_q1_vertex_label_set;
  std::set_intersection(q0_vertex_label_set.begin(), q0_vertex_label_set.end(),
                        q1_vertex_label_set.begin(), q1_vertex_label_set.end(),
                        std::inserter(q0_intersect_q1_vertex_label_set,
                                      q0_intersect_q1_vertex_label_set.begin()));

  std::set<EdgeLabelType> q0_intersect_q1_edge_label_set;
  std::set_intersection(q0_edge_label_set.begin(), q0_edge_label_set.end(),
                        q1_edge_label_set.begin(), q1_edge_label_set.end(),
                        std::inserter(q0_intersect_q1_edge_label_set,
                                      q0_intersect_q1_edge_label_set.begin()));

  std::set<std::tuple<VertexLabelType,
                        EdgeLabelType,
                      VertexLabelType>> q0_intersect_q1_edge_type_set;
  std::set_intersection(q0_edge_type_set.begin(), q0_edge_type_set.end(),
                        q1_edge_type_set.begin(), q1_edge_type_set.end(),
                        std::inserter(q0_intersect_q1_edge_type_set,
                                      q0_intersect_q1_edge_type_set.begin()));
  
  if (q0_intersect_q1_vertex_label_set.empty()
   || q0_intersect_q1_edge_type_set.empty()){
    assert(match_q0_to_q1.empty());
    return std::vector<GraphPatternType>();
  }

  std::vector<GraphPatternType> current_graph_patterns;

  GraphPatternType common_pattern;
  Match<GraphPatternType,
        GraphPatternType> common_pattern_to_q0,
                          common_pattern_to_q1;
  if (match_q0_to_q1.empty()){
    for (const auto& vertex_label : q0_intersect_q1_vertex_label_set) {
      GraphPatternType pattern;
      pattern.AddVertex(0, vertex_label);
      current_graph_patterns.emplace_back(pattern);
    }
    assert(q0_intersect_q1_vertex_label_set.size()
                  == current_graph_patterns.size());
  }
  else{
    std::set<VertexIDType> remove_vertex_id_set;
    for (auto vertex_it = q0.VertexBegin();
             !vertex_it.IsDone();
              vertex_it++) {
      if (match_q0_to_q1.HasMap(vertex_it)){
        // this vertex is contained in common pattern
        // does not need to be removed
        continue;
      }
      // this vertex needs to be removed
      remove_vertex_id_set.emplace(vertex_it->id());
    }
    common_pattern = q0;
    for (const auto& vertex_id : remove_vertex_id_set) {
      auto erase_size = common_pattern.EraseVertex(vertex_id);
      assert(erase_size > 0);
    }
    assert(common_pattern.CountVertex() == match_q0_to_q1.size());
    common_pattern_to_q0 = Match(common_pattern, q0, "same_id_map");
    common_pattern_to_q1 = match_q0_to_q1(common_pattern_to_q0);
    current_graph_patterns.emplace_back(std::move(common_pattern));
    assert(current_graph_patterns.size() == 1);
    assert(common_pattern_to_q0.size() == common_pattern.CountVertex());
    assert(common_pattern_to_q1.size() == common_pattern.CountVertex());
    #ifndef NDEBUG
    for (auto vertex_it = common_pattern.VertexBegin();
             !vertex_it.IsDone();
              vertex_it++) {
      assert(common_pattern_to_q0.HasMap(vertex_it));
      assert(common_pattern_to_q1.HasMap(vertex_it));
    }
    #endif // NDEBUG
  }

  while (true) {
    std::vector<GraphPatternType> expanded_graph_patterns;
    expanded_graph_patterns = _maximal_common_subgraph::ExpandPattern(
                                                current_graph_patterns,
                                                q0_intersect_q1_vertex_label_set,
                                                q0_intersect_q1_edge_label_set,
                                                q0_intersect_q1_edge_type_set);

    UniquePatterns(expanded_graph_patterns);

    for (auto expanded_graph_pattern_it
            = expanded_graph_patterns.begin();
              expanded_graph_pattern_it 
           != expanded_graph_patterns.end();) {
      if (match_q0_to_q1.empty()){
        auto match_in_q0 = DpisoUsingMatch(*expanded_graph_pattern_it, q0, 1);
        assert(match_in_q0 == 0
            || match_in_q0 == 1);
        if (match_in_q0 == 0) {
          // cannot be matched into q0
          expanded_graph_pattern_it = expanded_graph_patterns
                              .erase(expanded_graph_pattern_it);
          continue;
        }

        auto match_in_q1 = DpisoUsingMatch(*expanded_graph_pattern_it, q1, 1);
        assert(match_in_q1 == 0
            || match_in_q1 == 1);
        if (match_in_q1 == 0) {
          // cannot be matched into q1
          expanded_graph_pattern_it = expanded_graph_patterns
                              .erase(expanded_graph_pattern_it);
          continue;
        }
        // preserved
        expanded_graph_pattern_it++;
        continue;
      }

      Match<GraphPatternType,
            GraphPatternType> partial_match_expanded_graph_pattern_to_common_pattern(
                                           *expanded_graph_pattern_it,common_pattern, "same_id_map");

      Match<GraphPatternType,
            GraphPatternType> partial_match_expanded_graph_pattern_to_q0
       = common_pattern_to_q0(partial_match_expanded_graph_pattern_to_common_pattern);

      Match<GraphPatternType,
            GraphPatternType> partial_match_expanded_graph_pattern_to_q1
       = common_pattern_to_q1(partial_match_expanded_graph_pattern_to_common_pattern);

      auto match_in_q0 = DpisoUsingMatch(*expanded_graph_pattern_it,q0,
                            partial_match_expanded_graph_pattern_to_q0, 1);
      assert(match_in_q0 == 0
          || match_in_q0 == 1);
      if (match_in_q0 == 0) {
        // cannot be matched into q0
        expanded_graph_pattern_it = expanded_graph_patterns
                            .erase(expanded_graph_pattern_it);
        continue;
      }

      auto match_in_q1 = DpisoUsingMatch(*expanded_graph_pattern_it,q1,
                            partial_match_expanded_graph_pattern_to_q1, 1);
      assert(match_in_q1 == 0
          || match_in_q1 == 1);
      if (match_in_q1 == 0) {
        // cannot be matched into q1
        expanded_graph_pattern_it = expanded_graph_patterns
                            .erase(expanded_graph_pattern_it);
        continue;
      }
      // preserved
      expanded_graph_pattern_it++;
      continue;

    }
    if (!expanded_graph_patterns.empty()){
      // the current pattern can be further expanded is not maximal
      current_graph_patterns.swap(expanded_graph_patterns);
      continue;
    }
    // the pattern(s) in current_graph_patterns is maximal
    // select one
    return current_graph_patterns;
  }
  return std::vector<GraphPatternType>();
}


template <typename GraphPatternType>
std::vector<GraphPatternType> 
  MaximalCommonSubgraph(GraphPatternType& q0,
                        GraphPatternType& q1){
  Match<GraphPatternType,
        GraphPatternType> match_q0_to_q1;
  return MaximalCommonSubgraph(q0,   q1,
                         match_q0_to_q1);
}

};

#endif // _GUNDAM_TOOL_MAXIMAL_COMMON_SUBGRAPH