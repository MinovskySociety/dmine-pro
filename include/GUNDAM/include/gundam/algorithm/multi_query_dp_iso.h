#ifndef _GUNDAM_ALGORITHM_MULTI_QUERY_DPISO_USING_MATCH_H
#define _GUNDAM_ALGORITHM_MULTI_QUERY_DPISO_USING_MATCH_H

#include "gundam/graph_type/graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"

#include "gundam/io/csvgraph.h"

#include "gundam/match/match.h"

#include "gundam/type_getter/vertex_handle.h"

#include "gundam/algorithm/bfs.h"
#include "gundam/algorithm/dfs.h"

#include "gundam/algorithm/dp_iso.h"
#include "gundam/algorithm/dp_iso_using_match.h"

#include "gundam/tool/same_pattern.h"
#include "gundam/tool/sub_graph_of.h"
#include "gundam/tool/maximal_common_subgraph.h"

namespace GUNDAM{

namespace _multi_query_dp_iso{

template <typename VertexHandle>
inline std::pair<int, bool> GetPatternIdx(VertexHandle vertex_handle){
  return vertex_handle->id();
}

template <typename TriVertexPatternType>
class Tls{
 public:
  Tls(const TriVertexPatternType& tri_vertex_pattern)
             :tri_vertex_pattern_(tri_vertex_pattern){
    auto automorphism = DpisoUsingMatch(this->tri_vertex_pattern_,
                                        this->tri_vertex_pattern_);
    // std::cout << "tri_vertex_pattern.CountVertex(): "
    //           <<  tri_vertex_pattern.CountVertex()
    //           << std::endl
    //           << "tri_vertex_pattern.CountEdge(): "
    //           <<  tri_vertex_pattern.CountEdge()
    //           << std::endl;
    // std::cout << "automorphism: "
    //           <<  automorphism
    //           << std::endl;
    assert(tri_vertex_pattern.CountVertex() == 3);
    assert(automorphism == 1
        || automorphism == 2);
    if (automorphism == 2){
      this->symmetrical_ = true;
      return;
    }
    this->symmetrical_ = false;
    return;
  }

  TriVertexPatternType& tri_vertex_pattern(){
    return this->tri_vertex_pattern_;
  }

  const TriVertexPatternType& tri_vertex_pattern() const{
    return this->tri_vertex_pattern_;
  }
  
  inline bool symmetrical() const {
    return this->symmetrical_;
  }

  inline bool operator==(const Tls& tls) const {
    return SamePattern(this->tri_vertex_pattern_, 
                         tls.tri_vertex_pattern_);
  }

  inline bool operator!=(const Tls& tls) const {
    return !((*this) == tls);
  }

 private:
  TriVertexPatternType tri_vertex_pattern_;
  bool symmetrical_;
};

template <typename TriVertexPatternType>
class TlsSet{
 private:
  using TlsType = Tls<TriVertexPatternType>;

  using ContainerTlsContainerType 
      = Container<ContainerType::Vector,
                       SortType::Default,
                        TlsType>;

 public:
  using       iterator = typename ContainerTlsContainerType::      iterator;
  using const_iterator = typename ContainerTlsContainerType::const_iterator;

  TlsSet(){
    return;
  }

  inline std::pair<iterator, bool> AddTls(const TlsType& tls){
    return this->tls_set_.Insert(tls);
  }

  inline size_t Size() const{
    return this->tls_set_.size();
  }

  inline iterator TlsBegin() {
    return this->tls_set_.begin();
  }

  inline iterator TlsEnd() {
    return this->tls_set_.end();
  }

  inline const_iterator TlsBegin() const {
    return this->tls_set_.cbegin();
  }

  inline const_iterator TlsEnd() const {
    return this->tls_set_.cend();
  }

 private:
  ContainerTlsContainerType tls_set_;
};

template <typename     GraphPatternType,
          typename TriVertexPatternType>
size_t Li(GraphPatternType&  q,
   TlsSet<TriVertexPatternType>& tls_set){
  size_t li_q_tls = 0;
  // std::cout << "tls_set.size(): " << tls_set.Size() << std::endl;
  for (auto tls_it  = tls_set.TlsBegin();
            tls_it != tls_set.TlsEnd();
            tls_it++) {
    auto automorphism = DpisoUsingMatch(tls_it.template get<0>().tri_vertex_pattern(), q);
    if (tls_it.template get<0>().symmetrical()) {
      assert(automorphism % 2 == 0);
      automorphism /= 2;
    }
    li_q_tls += automorphism;
  }
  return li_q_tls;
}

template <typename TriVertexPatternType>
TlsSet<TriVertexPatternType> Intersect(TlsSet<TriVertexPatternType>& tls_qi_set,
                                       TlsSet<TriVertexPatternType>& tls_qj_set){
  TlsSet<TriVertexPatternType> tls_qi_qj_set;
  
  for (auto tls_qi_it  = tls_qi_set.TlsBegin();
            tls_qi_it != tls_qi_set.TlsEnd();
            tls_qi_it++) {
    // to find whether this tls is contained in tls_qj_it
    for (auto tls_qj_it  = tls_qi_set.TlsBegin();
              tls_qj_it != tls_qi_set.TlsEnd();
              tls_qj_it++) {
      if (SamePattern(tls_qi_it.template get<0>().tri_vertex_pattern(),
                      tls_qj_it.template get<0>().tri_vertex_pattern())){
        // this tri_vertex_pattern is contained in both 
        // tls_qi_set and tls_qj_set
        // add into tls_qi_qj_set
        tls_qi_qj_set.AddTls(tls_qi_it.template get<0>().tri_vertex_pattern());
        break;
      }
    }
  }
  return tls_qi_qj_set;
}

template <typename TriVertexPatternType>
double Gf(TriVertexPatternType& qi, TlsSet<TriVertexPatternType>& tls_qi_set, size_t tls_qi_size,
          TriVertexPatternType& qj, TlsSet<TriVertexPatternType>& tls_qj_set, size_t tls_qj_size){

  TlsSet<TriVertexPatternType> tls_qi_qj_set 
                   = Intersect(tls_qi_set,
                               tls_qj_set);
  
  size_t li_qi_tls_qi_qj = Li(qi, tls_qi_qj_set);
  size_t li_qj_tls_qi_qj = Li(qj, tls_qi_qj_set);

  size_t numerator = li_qi_tls_qi_qj < li_qj_tls_qi_qj?
                     li_qi_tls_qi_qj : li_qj_tls_qi_qj;

  size_t denominator = tls_qi_size < tls_qj_size?
                       tls_qi_size : tls_qj_size;

  return ((double)numerator) / ((double)denominator);
}

template <typename     GraphPatternType,
          typename TriVertexPatternType>
void BuildTls(GraphPatternType&  q,
   TlsSet<TriVertexPatternType>& q_tls_set){
  assert(q_tls_set.Size() == 0);

  using VertexHandleType = typename VertexHandle<GraphPatternType>::type;
  using VertexIDType     = typename GraphPatternType::VertexType::IDType;

  std::vector<VertexIDType> vertex_id_set;
  vertex_id_set.reserve(q.CountVertex());
  for (auto vertex_it = q.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++) {
    vertex_id_set.emplace_back(vertex_it->id());
  }
  assert(vertex_id_set.size() == q.CountVertex());

  for (auto vertex_it = q.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++) {
    // enumerate the src vertex of the tri_vertex_pattern
    std::vector<VertexHandleType> tri_vertex_handle_set;
    tri_vertex_handle_set.reserve(3);

    auto prune_callback = [&tri_vertex_handle_set](VertexHandleType vertex_handle){
      assert(tri_vertex_handle_set.size() <= 3);
      if (tri_vertex_handle_set.size() == 3){
        // has already contain sufficient vertexes
        return true;
      }
      for (auto& tri_vertex_handle 
               : tri_vertex_handle_set) {
        assert(tri_vertex_handle);
        if (tri_vertex_handle == vertex_handle) {
          // this vertex has been contained, needs to be pruned
          return true;
        }
      }
      // this vertex has not been contained, does not need to be pruned
      return false;
    };
    
    // construct the tri_vertex_pattern
    auto dfs_callback = [&tri_vertex_handle_set,
                         &q,
                         #ifndef NDEBUG
                         &vertex_it,
                         #endif // NDEBUG
                         &q_tls_set,
                         &vertex_id_set](
             VertexHandleType vertex_handle, int dfs_idx, 
                                             int dfs_depth){

      // std::cout << "dfs_depth: "
      //           <<  dfs_depth
      //           << std::endl;

      assert(0 <= dfs_depth && dfs_depth <= 2);

      tri_vertex_handle_set.resize(dfs_depth + 1);

      assert(tri_vertex_handle_set.size() >= 1
          && tri_vertex_handle_set.size() <= 3);
      
      tri_vertex_handle_set.at(dfs_depth) = vertex_handle;

      assert(tri_vertex_handle_set.at(0) == vertex_it);
      if (dfs_depth < 2) {
        // does not need further process
        return true;
      }
      // std::cout << "tri vertex id: " << std::endl;
      for (const auto& tri_vertex_handle
                     : tri_vertex_handle_set) {
        // should not be null
        assert(tri_vertex_handle);
        // std::cout << "\t" << tri_vertex_handle->id();
      }
      // std::cout << std::endl;
      // has already get a tri-vertex tuple
      TriVertexPatternType tri_vertex_pattern(q);
      // get a tri-vertex pattern, obtain it from the graph pattern
      for (const auto& vertex_id : vertex_id_set) {
        bool in_tri_vertex_pattern = false;
        for (const auto& tri_vertex_handle
                       : tri_vertex_handle_set) {
          // should not be null
          assert(tri_vertex_handle);
          if (tri_vertex_handle->id() == vertex_id) {
            in_tri_vertex_pattern = true;
            break;
          }
        }
        if (in_tri_vertex_pattern) {
          // this vertex does not need to be removed
          continue;
        }
        // this vertex is not contained in the tri-vertex-pattern
        // need to be removed 
        auto erase_size_ret = tri_vertex_pattern.EraseVertex(vertex_id);
        // should have removed successfully
        assert(erase_size_ret >= 1);
      }
      // std::cout << "tri_vertex_pattern.CountVertex(): "
      //           <<  tri_vertex_pattern.CountVertex()
      //           << std::endl;
      assert(tri_vertex_pattern.CountVertex() == 3);
      q_tls_set.AddTls(tri_vertex_pattern);
      return true;
    };
    VertexHandleType vertex_handle = vertex_it;
    // use bidir edge, does not remove duplicated vertex
    Dfs<true, false>(q, vertex_handle, dfs_callback, 
                                     prune_callback);
  }
  return;
}

template <typename GraphPatternType>
void TLSGroupMatrix(std::vector<GraphPatternType>& query_graph_list,
                    std::vector<std::vector<std::pair<int, bool>>>& clique_set,
                    double threshold){
  assert(threshold >= 0.0 
      && threshold <= 1.0);

  using CliqueVertexLabelType = int;
  using CliqueVertexIdType    = int;

  using CliqueEdgeLabelType = int;
  using CliqueEdgeIdType    = int;

  using CliqueVertexAttrKeyType = std::string;
  using CliqueEdgeAttrKeyType   = std::string;

  using CliqueGraphType = LargeGraph<CliqueVertexIdType, 
                                     CliqueVertexLabelType, 
                                     CliqueVertexAttrKeyType,
                                     CliqueEdgeIdType, 
                                     CliqueEdgeLabelType, 
                                     CliqueEdgeAttrKeyType>;

  using ClusterVertexHandleType = typename VertexHandle<CliqueGraphType>::type;

  CliqueGraphType m;

  constexpr CliqueVertexLabelType kDefaultVertexLabel = 0;
  constexpr   CliqueEdgeLabelType kDefaultEdgeLabel   = 0;
  const   CliqueVertexAttrKeyType kAddedToCliqueKey   = "add_to_key";

  using TriVertexPatternType = GraphPatternType;

  using TlsSetType = TlsSet<TriVertexPatternType>;
  std::vector<std::pair<TlsSetType, size_t>> query_graph_tls_set_list;
  query_graph_tls_set_list.reserve(query_graph_list.size());
  for (auto& q : query_graph_list) {
    TlsSetType q_tls_set;
    BuildTls(q, q_tls_set);
    auto li = Li(q, q_tls_set);
    // std::cout << "q_tls_set.size(): "
    //           <<  q_tls_set.Size() << std::endl;
    query_graph_tls_set_list.emplace_back(std::move(q_tls_set), li);
  }

  for (int i = 0; i < query_graph_list.size(); i++) {
    auto [ vertex_handle, 
           vertex_ret ] = m.AddVertex(i, kDefaultVertexLabel);
    // should add successfully
    assert(vertex_ret);
  }

  CliqueEdgeIdType edge_id_counter = 0;
  for (int i = 0; i < query_graph_list.size(); i++) {
    for (int j = i + 1; j < query_graph_list.size(); j++) {
      double gf_qi_qj = Gf(query_graph_list[i], 
                           query_graph_tls_set_list[i].first,  
                           query_graph_tls_set_list[i].second,
                           query_graph_list[j], 
                           query_graph_tls_set_list[j].first,  
                           query_graph_tls_set_list[j].second);
      // std::cout << "gf_qi_qj: "
      //           <<  gf_qi_qj << std::endl;
      if (gf_qi_qj >= threshold) {
        // only add one direction would be enough
        // std::cout << "m AddedEdge" << std::endl;
        m.AddEdge(i, j, kDefaultEdgeLabel, edge_id_counter);
        edge_id_counter++;
      }
    }
  }

  for (auto vertex_it = m.VertexBegin();
           !vertex_it.IsDone(); 
            vertex_it++) {
    if (vertex_it->FindAttribute(kAddedToCliqueKey)) {
      // this vertex has already been added into the cluster
      continue;
    }
    // find 
    auto& clique = clique_set.emplace_back();
    auto dfs_callback = [&clique,
                         &kAddedToCliqueKey](ClusterVertexHandleType vertex_handle){
      // should not contain this vertex
      assert(!vertex_handle->FindAttribute(kAddedToCliqueKey));
      clique.emplace_back(vertex_handle->id(), true);
      auto [attr_handle,
            attr_ret] = vertex_handle->AddAttribute(kAddedToCliqueKey, 0);
      // should add successfully
      assert(attr_ret);
      return true;
    };
    Dfs<true>(m, vertex_it, dfs_callback);
    assert(!clique.empty());
  }
  assert(!clique_set.empty());
  return;
}

template <typename GraphPatternType,
          typename VertexHandle>
inline Match<GraphPatternType,
             GraphPatternType>&
    GetMatchToParent(VertexHandle vertex_handle) {
  const std::string kMatchToParentAttributeKey = "match_to_parent";
  using MatchType = Match<GraphPatternType,
                          GraphPatternType>;
  auto attr_handle = vertex_handle->FindAttribute(kMatchToParentAttributeKey);
  assert(attr_handle);
  return attr_handle->template value<MatchType>();
}

template <typename GraphPatternType,
          typename VertexHandle>
inline Match<GraphPatternType,
             GraphPatternType>&
    AddMatchToParent(VertexHandle vertex_handle,
                GraphPatternType& parent_graph,
                GraphPatternType&  child_graph) {

  const std::string kMatchToParentAttributeKey = "match_to_parent";

  using MatchType 
      = Match<GraphPatternType,
              GraphPatternType>;

  using MatchSetType 
      = MatchSet<GraphPatternType,
                 GraphPatternType>;

  MatchSetType match_set_parent_to_child;

  auto match_counter = DpisoUsingMatch(parent_graph,
                                        child_graph,
                          match_set_parent_to_child, 1);
  assert(match_counter == 1);
  assert(match_set_parent_to_child.size() == 1);
  
  MatchType match_child_to_parent(match_set_parent_to_child.MatchBegin()->Reverse());

  auto [attr_handle,
        attr_ret] = vertex_handle->AddAttribute(kMatchToParentAttributeKey,
                                                 match_child_to_parent);
  // should added successfully
  assert(attr_ret);
  return attr_handle->template value<MatchType>();
}

template <typename GraphPatternType,
          typename VertexHandle>
inline Match<GraphPatternType,
             GraphPatternType>&
    AddMatchToParent(VertexHandle vertex_handle,
                GraphPatternType& parent_graph,
                GraphPatternType&  child_graph,
          Match<GraphPatternType,
                GraphPatternType>& partial_match_parent_to_child) {

  const std::string kMatchToParentAttributeKey = "match_to_parent";

  using MatchType 
      = Match<GraphPatternType,
              GraphPatternType>;

  using MatchSetType 
      = MatchSet<GraphPatternType,
                 GraphPatternType>;

  MatchSetType match_set_parent_to_child;

  auto match_counter = DpisoUsingMatch(parent_graph,
                                        child_graph,
                      partial_match_parent_to_child,
                          match_set_parent_to_child, 1);
  assert(match_counter == 1);
  assert(match_set_parent_to_child.size() == 1);
  
  MatchType match_child_to_parent(match_set_parent_to_child.MatchBegin()->Reverse());

  auto [attr_handle,
        attr_ret] = vertex_handle->AddAttribute(kMatchToParentAttributeKey,
                                                 match_child_to_parent);
  // should added successfully
  assert(attr_ret);
  return attr_handle->template value<MatchType>();
}

template <typename GraphPatternType>
inline GraphPatternType& GetPatternRef(
            const std::pair<int, bool>& graph_idx,
            std::vector<GraphPatternType>&      query_graph_list,
            std::vector<GraphPatternType>& additional_graph_list){
  if (graph_idx.second){
    assert(graph_idx.first < query_graph_list.size()
        && graph_idx.first >= 0);
    return query_graph_list[graph_idx.first];
  }
  assert(graph_idx.first < additional_graph_list.size()
      && graph_idx.first >= 0);
  return additional_graph_list[graph_idx.first];
}

template <typename  QueryGraphHandleType,
          typename TargetGraphHandleType>
inline std::map< QueryGraphHandleType,
     std::vector<TargetGraphHandleType>>& 
  GetCandidateRef(const std::pair<int, bool>& graph_idx,
     std::vector<std::map< QueryGraphHandleType,
              std::vector<TargetGraphHandleType>>>&            candidate_set_list,
     std::vector<std::map< QueryGraphHandleType,
              std::vector<TargetGraphHandleType>>>& additional_candidate_set_list){
  if (graph_idx.second){
    assert(graph_idx.first < candidate_set_list.size()
        && graph_idx.first >= 0);
    return candidate_set_list[graph_idx.first];
  }
  assert(graph_idx.first < additional_candidate_set_list.size()
      && graph_idx.first >= 0);
  return additional_candidate_set_list[graph_idx.first];
}

template <typename GraphPatternType>
inline Match<GraphPatternType, 
             GraphPatternType> GetMatch(const std::pair<int, bool>& graph_idx,
     std::vector<Match<GraphPatternType, 
                       GraphPatternType>>&            paritial_match_to_common_graph_list,
     std::vector<Match<GraphPatternType, 
                       GraphPatternType>>& additional_paritial_match_to_common_graph_list){
  if (graph_idx.second){
    assert(graph_idx.first < paritial_match_to_common_graph_list.size()
        && graph_idx.first >= 0);
    return paritial_match_to_common_graph_list[graph_idx.first];
  }
  assert(graph_idx.first < additional_paritial_match_to_common_graph_list.size()
      && graph_idx.first >= 0);
  return additional_paritial_match_to_common_graph_list[graph_idx.first];
}

inline bool NeedToCallChildPatternMatchCallback(
            const std::pair<int, bool>& graph_idx,
            const std::vector<bool>&            call_child_pattern_match_callback,
            const std::vector<bool>& additional_call_child_pattern_match_callback){
  if (graph_idx.second){
    assert(graph_idx.first < call_child_pattern_match_callback.size()
        && graph_idx.first >= 0);
    return call_child_pattern_match_callback[graph_idx.first];
  }
  assert(graph_idx.first < additional_call_child_pattern_match_callback.size()
      && graph_idx.first >= 0);
  return additional_call_child_pattern_match_callback[graph_idx.first];
}

template <typename      PcmTreeType,
          typename GraphPatternType>
std::vector<typename VertexHandle<PcmTreeType>::type>
  BuildPcm(PcmTreeType& pcm_tree,
             // to store the query graph
     std::vector<GraphPatternType>&      query_graph_list,
             // to store the additional graph
     std::vector<GraphPatternType>& additional_graph_list,
     std::vector<Match<
                 GraphPatternType,
                 GraphPatternType>>&            paritial_match_to_common_graph_list,
     std::vector<Match<
                 GraphPatternType,
                 GraphPatternType>>& additional_paritial_match_to_common_graph_list) {

  using PcmTreeVertexLabelType = typename PcmTreeType::VertexType::LabelType;
  using   PcmTreeEdgeLabelType = typename PcmTreeType::  EdgeType::LabelType;

  using PcmVertexHandleType = typename VertexHandle<PcmTreeType>::type;

  const PcmTreeVertexLabelType kDefaultVertexLabel = 0;
  const   PcmTreeEdgeLabelType   kDefaultEdgeLabel = 0;

  const std::string kMatchToParentAttributeKey = "match_to_parent";

  assert(    !query_graph_list.empty());
  assert(additional_graph_list.empty());

  std::vector<std::vector<std::pair<int, bool>>> cliques;

  constexpr double threshold = 0.0;

  TLSGroupMatrix(query_graph_list, cliques, threshold);

  std::vector<std::pair<int, bool>> root_idx_set;
  root_idx_set.reserve(cliques.size());
  size_t edge_counter = 0;
  for (const auto& clique : cliques) {
    // the next level group
    std::vector<std::pair<int, bool>> next_level_graph;
    std::vector<std::pair<int, bool>> query_graph_id_set = clique;
    for (const auto& query_graph_id 
                   : query_graph_id_set) {
      auto [ vertex_handle,
             vertex_ret ] = pcm_tree.AddVertex(query_graph_id, 
                                               kDefaultVertexLabel);
      assert(vertex_ret);
    }
    while (true) {
      assert(next_level_graph.empty());
      assert(!query_graph_id_set.empty());
      #ifndef NDEBUG
      for (const auto& query_graph_id 
                     : query_graph_id_set) {
        assert(pcm_tree.FindVertex(query_graph_id));
      }
      #endif // NDEBUG
      while (query_graph_id_set.size() >= 2) {
        // randomly pick a pair of pattern in the remained
        // patterns in this clique
        size_t query_graph_id_set_idx_i  = rand() % query_graph_id_set.size();
        std::pair<int, bool>
        query_graph_id_i = query_graph_id_set[query_graph_id_set_idx_i];
        // removed the selected graph from current clique
        query_graph_id_set.erase(query_graph_id_set.begin() 
                               + query_graph_id_set_idx_i);
                               
        size_t query_graph_id_set_idx_j  = rand() % query_graph_id_set.size();
        std::pair<int, bool>
        query_graph_id_j = query_graph_id_set[query_graph_id_set_idx_j];
        // removed the selected graph from current clique
        query_graph_id_set.erase(query_graph_id_set.begin() 
                               + query_graph_id_set_idx_j);

        auto& qi = GetPatternRef(query_graph_id_i, query_graph_list, 
                                              additional_graph_list);
        auto& qj = GetPatternRef(query_graph_id_j, query_graph_list,
                                              additional_graph_list);
        // to find whether on graph is contained in another
        if (SubGraphOf(qi, qj)) {
          // qi is the subgraph of qj
          // add an edge from qi to qj
          auto [edge_handle, 
                edge_ret] = pcm_tree.AddEdge(query_graph_id_i,
                                             query_graph_id_j,
                                            kDefaultEdgeLabel,
                                             edge_counter++);
          assert(edge_ret);
          // and add qj into next_level_graph 
          // std::cout << "add to next_level_graph:" << std::endl
          //           << " query_graph_id_i.first: "
          //           <<   query_graph_id_i.first  << std::endl
          //           << " query_graph_id_i.second: "
          //           <<   query_graph_id_i.second << std::endl;
          next_level_graph.emplace_back(query_graph_id_i);
          continue;
        }
        if (SubGraphOf(qj, qi)) {
          // qj is the subgraph of qi
          // add an edge from qj to qi
          auto [edge_handle, 
                edge_ret] = pcm_tree.AddEdge(query_graph_id_j,
                                             query_graph_id_i,
                                            kDefaultEdgeLabel,
                                             edge_counter++);
          assert(edge_ret);
          // and add qi into next_level_graph 
          // std::cout << "add to next_level_graph:" << std::endl
          //           << " query_graph_id_j.first: "
          //           <<   query_graph_id_j.first  << std::endl
          //           << " query_graph_id_j.second: "
          //           <<   query_graph_id_j.second << std::endl;
          next_level_graph.emplace_back(query_graph_id_j);
          continue;
        }
        auto& match_query_graph_i_to_common_graph = GetMatch(query_graph_id_i,
                                                             paritial_match_to_common_graph_list,
                                                  additional_paritial_match_to_common_graph_list);
        auto& match_query_graph_j_to_common_graph = GetMatch(query_graph_id_j,
                                                             paritial_match_to_common_graph_list,
                                                  additional_paritial_match_to_common_graph_list);
        Match<GraphPatternType,
              GraphPatternType> partial_match_qi_to_qj 
                = match_query_graph_j_to_common_graph.Reverse()(match_query_graph_i_to_common_graph);
        // neither qi can be contained in qj nor qj can be contained in qi
        auto mcs_qi_qj_set = MaximalCommonSubgraph(qi, qj, partial_match_qi_to_qj);
        assert(!mcs_qi_qj_set.empty());
        bool has_been_contained = false;
        for (const auto& mcs : mcs_qi_qj_set) {
          // to find whether the mcs has already been contained
          // in this clique or next_level_graph
          for (int query_graph_id_set_idx = 0;
                   query_graph_id_set_idx < query_graph_id_set.size();
                   query_graph_id_set_idx++){
            const auto query_graph_id 
                     = query_graph_id_set[query_graph_id_set_idx];
            // find in this clique to find a new
            auto& query_graph = GetPatternRef(query_graph_id, 
                                              query_graph_list, 
                                         additional_graph_list);
            if (SamePattern(query_graph, mcs)) {
              // add an edge from query_graph_id to qi and qj
              auto [edge_handle_i, 
                    edge_ret_i] = pcm_tree.AddEdge(query_graph_id,
                                                   query_graph_id_i,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_i);
              auto [edge_handle_j, 
                    edge_ret_j] = pcm_tree.AddEdge(query_graph_id,
                                                   query_graph_id_j,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_j);
              // remove query_graph_id from 
              query_graph_id_set.erase(query_graph_id_set.begin() 
                                     + query_graph_id_set_idx);
              // and add query_graph_id into next_level_graph 

              // std::cout << "add to next_level_graph:" << std::endl
              //           << " query_graph_id.first: "
              //           <<   query_graph_id.first  << std::endl
              //           << " query_graph_id.second: "
              //           <<   query_graph_id.second << std::endl;

              #ifndef NDEBUG
              auto vertex_handle = pcm_tree.FindVertex(query_graph_id);
              assert(vertex_handle);
              assert(vertex_handle->CountOutVertex() >= 2);
              #endif

              next_level_graph.emplace_back(query_graph_id);
              has_been_contained = true;
              break;
            }
          }
          if (has_been_contained){
            // this vertex has been contained in this clique
            break;
          }
        }
        // the msc has been contained in this clique, has been processed
        if (has_been_contained) {
          // to process the next graph pair
          continue;
        }
        // to find whether there is a msc that has already been
        // contained in next level
        for (const auto& mcs : mcs_qi_qj_set) {
          // to find whether the mcs has already been contained
          // in this clique or next_level_graph
          for (int next_level_group_idx = 0;
                   next_level_group_idx < next_level_graph.size();
                   next_level_group_idx++){
            const auto& next_level_graph_id 
                      = next_level_graph[next_level_group_idx];
            // find in this clique to find a new
            auto& query_graph = GetPatternRef(next_level_graph_id, 
                                              query_graph_list, 
                                              additional_graph_list);
            if (SamePattern(query_graph, mcs)) {
              // the msc is the same as next_level_graph_id
              // add an edge from next_level_graph_id to both qi and qj
              auto [edge_handle_i, 
                    edge_ret_i] = pcm_tree.AddEdge(next_level_graph_id,
                                                   query_graph_id_i,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_i);
              auto [edge_handle_j, 
                    edge_ret_j] = pcm_tree.AddEdge(next_level_graph_id,
                                                   query_graph_id_j,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_j);
              // does not need to remove anything
              // and add query_graph_id into next_level_graph 
              has_been_contained = true;
              break;
            }
          }
          if (has_been_contained){
            // this vertex has been contained in this clique
            break;
          }
        }
        // the msc has been contained in this clique, has been processed
        if (has_been_contained) {
          // to process the next graph pair
          continue;
        }
        // the msc does not contained in the current clique 
        // nor next level group
        // select a graph from mcs_qi_qj_set
        size_t selected_mcs_idx = rand() % mcs_qi_qj_set.size();
        auto& mcs = mcs_qi_qj_set.at(selected_mcs_idx);
        // add this pattern into the additional_graph_list
        additional_graph_list.emplace_back(std::move(mcs));
        // add an idx that is not contained in the next level
        std::pair next_level_graph_id(additional_graph_list.size() - 1, false);
        assert(SamePattern(additional_graph_list.back(),
                           GetPatternRef(next_level_graph_id, 
                                         query_graph_list, 
                                         additional_graph_list)));
        // find another
        // std::cout << "add to next_level_graph:" << std::endl
        //           << " next_level_graph_id.first: "
        //           <<   next_level_graph_id.first  << std::endl
        //           << " next_level_graph_id.second: "
        //           <<   next_level_graph_id.second << std::endl;
        next_level_graph.emplace_back(next_level_graph_id);
        // add a new vertex in the pcm_tree
        auto [vertex_handle,
              vertex_ret] = pcm_tree.AddVertex(next_level_graph_id,
                                              kDefaultVertexLabel);
        assert(vertex_ret);
        // add an edge from next_level_graph_id to both qi and qj
        
        auto [edge_handle_i, 
              edge_ret_i] = pcm_tree.AddEdge(next_level_graph_id,
                                                  query_graph_id_i,
                                            kDefaultEdgeLabel,
                                              edge_counter++);
        assert(edge_ret_i);
        auto [edge_handle_j, 
              edge_ret_j] = pcm_tree.AddEdge(next_level_graph_id,
                                                  query_graph_id_j,
                                            kDefaultEdgeLabel,
                                              edge_counter++);
        assert(edge_ret_j);
      }
      assert(query_graph_id_set.size() == 0
          || query_graph_id_set.size() == 1);
      #ifndef NDEBUG
      for (const auto& next_level_graph_id 
                     : next_level_graph) {

        // std::cout << "next_level_graph_id.first: "
        //           <<  next_level_graph_id.first
        //           << std::endl
        //           << "next_level_graph_id.second: "
        //           <<  next_level_graph_id.second
        //           << std::endl;
        auto vertex_handle = pcm_tree.FindVertex(next_level_graph_id);
        assert(vertex_handle);
        assert(vertex_handle->CountOutVertex() > 0);
      }
      #endif // NDEBUG
      
      if (query_graph_id_set.size() == 1){
        // there still one pattern remained in this clique
        // add it into the next_level_graph
        next_level_graph.emplace_back(*query_graph_id_set.begin());
      }
      if (next_level_graph.size() == 1) {
        // does not need further grouping
        root_idx_set.emplace_back(*next_level_graph.begin());
        break;
      }
      query_graph_id_set.clear();
      next_level_graph.swap(query_graph_id_set);
    }
  }
  assert(root_idx_set.size() == cliques.size());
  std::vector<PcmVertexHandleType> root_handle_set;
  root_handle_set.reserve(root_idx_set.size());
  for (const auto& root_idx : root_idx_set) {
    auto root_handle = pcm_tree.FindVertex(root_idx);
    assert(root_handle);
    auto add_match_to_parent_callback = [
      &query_graph_list,
      &additional_graph_list
      #ifndef NDEBUG
      ,&root_handle
      #endif // NDEBUG
      ](PcmVertexHandleType pcm_vertex_handle){
      auto parent_edge_it = pcm_vertex_handle->InEdgeBegin();
      if (parent_edge_it.IsDone()){
        // only the root vertex does not have parent
        assert(pcm_vertex_handle == root_handle);
        // does not have parent
        return true;
      }
      auto parent_vertex_handle = parent_edge_it->src_handle();
      #ifndef NDEBUG
      parent_edge_it++;
      // std::cout << "pcm_vertex_handle->id(): "
      //           <<  pcm_vertex_handle->id().first
      //           << std::endl;
      // std::cout << "parent_vertex_handle->id(): "
      //           <<  parent_vertex_handle->id().first
      //           << std::endl;
      // if (!parent_edge_it.IsDone()){
      //   std::cout << "parent_edge_it->src_handle()->id(): "
      //             <<  parent_edge_it->src_handle()->id().first
      //             << std::endl;
      // }
      // should have only one parent
      assert(parent_edge_it.IsDone());
      #endif
      auto& parent_pattern = GetPatternRef(parent_vertex_handle->id(),
                                           query_graph_list,
                                      additional_graph_list);
      auto&  child_pattern = GetPatternRef(pcm_vertex_handle->id(),
                                            query_graph_list,
                                       additional_graph_list);
      AddMatchToParent(pcm_vertex_handle, parent_pattern,
                                           child_pattern);
      // continue dfs
      return true;
    };
    Dfs(pcm_tree, root_handle, add_match_to_parent_callback);
    root_handle_set.emplace_back(root_handle);
  }
  return root_handle_set;
}

template <typename      PcmTreeType,
          typename GraphPatternType>
std::vector<typename VertexHandle<PcmTreeType>::type>
  BuildPcm(PcmTreeType& pcm_tree,
             // to store the query graph
     std::vector<GraphPatternType>&      query_graph_list,
             // to store the additional graph
     std::vector<GraphPatternType>& additional_graph_list) {

  using PcmTreeVertexLabelType = typename PcmTreeType::VertexType::LabelType;
  using   PcmTreeEdgeLabelType = typename PcmTreeType::  EdgeType::LabelType;

  using PcmVertexHandleType = typename VertexHandle<PcmTreeType>::type;

  const PcmTreeVertexLabelType kDefaultVertexLabel = 0;
  const   PcmTreeEdgeLabelType   kDefaultEdgeLabel = 0;

  const std::string kMatchToParentAttributeKey = "match_to_parent";

  assert(    !query_graph_list.empty());
  assert(additional_graph_list.empty());

  std::vector<std::vector<std::pair<int, bool>>> cliques;

  constexpr double threshold = 0.0;

  TLSGroupMatrix(query_graph_list, cliques, threshold);

  // std::cout << "cliques.size(): " << cliques.size() << std::endl;

  std::vector<std::pair<int, bool>> root_idx_set;
  root_idx_set.reserve(cliques.size());
  size_t edge_counter = 0;
  for (const auto& clique : cliques) {
    // the next level group
    std::vector<std::pair<int, bool>> next_level_graph;
    std::vector<std::pair<int, bool>> query_graph_id_set = clique;
    for (const auto& query_graph_id 
                   : query_graph_id_set) {
      auto [ vertex_handle,
             vertex_ret ] = pcm_tree.AddVertex(query_graph_id, 
                                               kDefaultVertexLabel);
      assert(vertex_ret);
    }
    while (true) {
      assert(next_level_graph.empty());
      assert(!query_graph_id_set.empty());
      #ifndef NDEBUG
      for (const auto& query_graph_id 
                     : query_graph_id_set) {
        assert(pcm_tree.FindVertex(query_graph_id));
      }
      #endif // NDEBUG
      while (query_graph_id_set.size() >= 2) {
        // randomly pick a pair of pattern in the remained
        // patterns in this clique
        size_t query_graph_id_set_idx_i  = rand() % query_graph_id_set.size();
        std::pair<int, bool>
        query_graph_id_i = query_graph_id_set[query_graph_id_set_idx_i];
        // removed the selected graph from current clique
        query_graph_id_set.erase(query_graph_id_set.begin() 
                               + query_graph_id_set_idx_i);
                               
        size_t query_graph_id_set_idx_j  = rand() % query_graph_id_set.size();
        std::pair<int, bool>
        query_graph_id_j = query_graph_id_set[query_graph_id_set_idx_j];
        // removed the selected graph from current clique
        query_graph_id_set.erase(query_graph_id_set.begin() 
                               + query_graph_id_set_idx_j);

        auto& qi = GetPatternRef(query_graph_id_i, query_graph_list, 
                                              additional_graph_list);
        auto& qj = GetPatternRef(query_graph_id_j, query_graph_list,
                                              additional_graph_list);
        // to find whether on graph is contained in another
        if (SubGraphOf(qi, qj)) {
          // qi is the subgraph of qj
          // add an edge from qi to qj
          auto [edge_handle, 
                edge_ret] = pcm_tree.AddEdge(query_graph_id_i,
                                             query_graph_id_j,
                                            kDefaultEdgeLabel,
                                             edge_counter++);
          assert(edge_ret);
          // and add qj into next_level_graph 
          // std::cout << "add to next_level_graph:" << std::endl
          //           << " query_graph_id_i.first: "
          //           <<   query_graph_id_i.first  << std::endl
          //           << " query_graph_id_i.second: "
          //           <<   query_graph_id_i.second << std::endl;
          next_level_graph.emplace_back(query_graph_id_i);
          continue;
        }
        if (SubGraphOf(qj, qi)) {
          // qj is the subgraph of qi
          // add an edge from qj to qi
          auto [edge_handle, 
                edge_ret] = pcm_tree.AddEdge(query_graph_id_j,
                                             query_graph_id_i,
                                            kDefaultEdgeLabel,
                                             edge_counter++);
          assert(edge_ret);
          // and add qi into next_level_graph 
          // std::cout << "add to next_level_graph:" << std::endl
          //           << " query_graph_id_j.first: "
          //           <<   query_graph_id_j.first  << std::endl
          //           << " query_graph_id_j.second: "
          //           <<   query_graph_id_j.second << std::endl;
          next_level_graph.emplace_back(query_graph_id_j);
          continue;
        }
        // neither qi can be contained in qj nor qj can be contained in qi
        auto mcs_qi_qj_set = MaximalCommonSubgraph(qi, qj);
        assert(!mcs_qi_qj_set.empty());
        bool has_been_contained = false;
        for (const auto& mcs : mcs_qi_qj_set) {
          // to find whether the mcs has already been contained
          // in this clique or next_level_graph
          for (int query_graph_id_set_idx = 0;
                   query_graph_id_set_idx < query_graph_id_set.size();
                   query_graph_id_set_idx++){
            const auto query_graph_id 
                     = query_graph_id_set[query_graph_id_set_idx];
            // find in this clique to find a new
            auto& query_graph = GetPatternRef(query_graph_id, 
                                              query_graph_list, 
                                         additional_graph_list);
            if (SamePattern(query_graph, mcs)) {
              // add an edge from query_graph_id to qi and qj
              auto [edge_handle_i, 
                    edge_ret_i] = pcm_tree.AddEdge(query_graph_id,
                                                   query_graph_id_i,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_i);
              auto [edge_handle_j, 
                    edge_ret_j] = pcm_tree.AddEdge(query_graph_id,
                                                   query_graph_id_j,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_j);
              // remove query_graph_id from 
              query_graph_id_set.erase(query_graph_id_set.begin() 
                                     + query_graph_id_set_idx);
              // and add query_graph_id into next_level_graph 

              // std::cout << "add to next_level_graph:" << std::endl
              //           << " query_graph_id.first: "
              //           <<   query_graph_id.first  << std::endl
              //           << " query_graph_id.second: "
              //           <<   query_graph_id.second << std::endl;

              #ifndef NDEBUG
              auto vertex_handle = pcm_tree.FindVertex(query_graph_id);
              assert(vertex_handle);
              assert(vertex_handle->CountOutVertex() >= 2);
              #endif

              next_level_graph.emplace_back(query_graph_id);
              has_been_contained = true;
              break;
            }
          }
          if (has_been_contained){
            // this vertex has been contained in this clique
            break;
          }
        }
        // the msc has been contained in this clique, has been processed
        if (has_been_contained) {
          // to process the next graph pair
          continue;
        }
        // to find whether there is a msc that has already been
        // contained in next level
        for (const auto& mcs : mcs_qi_qj_set) {
          // to find whether the mcs has already been contained
          // in this clique or next_level_graph
          for (int next_level_group_idx = 0;
                   next_level_group_idx < next_level_graph.size();
                   next_level_group_idx++){
            const auto& next_level_graph_id 
                      = next_level_graph[next_level_group_idx];
            // find in this clique to find a new
            auto& query_graph = GetPatternRef(next_level_graph_id, 
                                              query_graph_list, 
                                              additional_graph_list);
            if (SamePattern(query_graph, mcs)) {
              // the msc is the same as next_level_graph_id
              // add an edge from next_level_graph_id to both qi and qj
              auto [edge_handle_i, 
                    edge_ret_i] = pcm_tree.AddEdge(next_level_graph_id,
                                                   query_graph_id_i,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_i);
              auto [edge_handle_j, 
                    edge_ret_j] = pcm_tree.AddEdge(next_level_graph_id,
                                                   query_graph_id_j,
                                                  kDefaultEdgeLabel,
                                                   edge_counter++);
              assert(edge_ret_j);
              // does not need to remove anything
              // and add query_graph_id into next_level_graph 
              has_been_contained = true;
              break;
            }
          }
          if (has_been_contained){
            // this vertex has been contained in this clique
            break;
          }
        }
        // the msc has been contained in this clique, has been processed
        if (has_been_contained) {
          // to process the next graph pair
          continue;
        }
        // the msc does not contained in the current clique 
        // nor next level group
        // select a graph from mcs_qi_qj_set
        size_t selected_mcs_idx = rand() % mcs_qi_qj_set.size();
        auto& mcs = mcs_qi_qj_set.at(selected_mcs_idx);
        // add this pattern into the additional_graph_list
        additional_graph_list.emplace_back(std::move(mcs));
        // add an idx that is not contained in the next level
        std::pair next_level_graph_id(additional_graph_list.size() - 1, false);
        assert(SamePattern(additional_graph_list.back(),
                           GetPatternRef(next_level_graph_id, 
                                         query_graph_list, 
                                         additional_graph_list)));
        // find another
        // std::cout << "add to next_level_graph:" << std::endl
        //           << " next_level_graph_id.first: "
        //           <<   next_level_graph_id.first  << std::endl
        //           << " next_level_graph_id.second: "
        //           <<   next_level_graph_id.second << std::endl;
        next_level_graph.emplace_back(next_level_graph_id);
        // add a new vertex in the pcm_tree
        auto [vertex_handle,
              vertex_ret] = pcm_tree.AddVertex(next_level_graph_id,
                                              kDefaultVertexLabel);
        assert(vertex_ret);
        // add an edge from next_level_graph_id to both qi and qj
        
        auto [edge_handle_i, 
              edge_ret_i] = pcm_tree.AddEdge(next_level_graph_id,
                                                  query_graph_id_i,
                                            kDefaultEdgeLabel,
                                              edge_counter++);
        assert(edge_ret_i);
        auto [edge_handle_j, 
              edge_ret_j] = pcm_tree.AddEdge(next_level_graph_id,
                                                  query_graph_id_j,
                                            kDefaultEdgeLabel,
                                              edge_counter++);
        assert(edge_ret_j);
      }
      assert(query_graph_id_set.size() == 0
          || query_graph_id_set.size() == 1);
      #ifndef NDEBUG
      for (const auto& next_level_graph_id 
                     : next_level_graph) {

        // std::cout << "next_level_graph_id.first: "
        //           <<  next_level_graph_id.first
        //           << std::endl
        //           << "next_level_graph_id.second: "
        //           <<  next_level_graph_id.second
        //           << std::endl;
        auto vertex_handle = pcm_tree.FindVertex(next_level_graph_id);
        assert(vertex_handle);
        assert(vertex_handle->CountOutVertex() > 0);
      }
      #endif // NDEBUG
      
      if (query_graph_id_set.size() == 1){
        // there still one pattern remained in this clique
        // add it into the next_level_graph
        next_level_graph.emplace_back(*query_graph_id_set.begin());
      }
      if (next_level_graph.size() == 1) {
        // does not need further grouping
        root_idx_set.emplace_back(*next_level_graph.begin());
        break;
      }
      query_graph_id_set.clear();
      next_level_graph.swap(query_graph_id_set);
    }
  }
  assert(root_idx_set.size() == cliques.size());
  std::vector<PcmVertexHandleType> root_handle_set;
  root_handle_set.reserve(root_idx_set.size());
  for (const auto& root_idx : root_idx_set) {
    auto root_handle = pcm_tree.FindVertex(root_idx);
    assert(root_handle);
    auto add_match_to_parent_callback = [
      &query_graph_list,
      &additional_graph_list
      #ifndef NDEBUG
      ,&root_handle
      #endif // NDEBUG
      ](PcmVertexHandleType pcm_vertex_handle){
      auto parent_edge_it = pcm_vertex_handle->InEdgeBegin();
      if (parent_edge_it.IsDone()){
        // only the root vertex does not have parent
        assert(pcm_vertex_handle == root_handle);
        // does not have parent
        return true;
      }
      auto parent_vertex_handle = parent_edge_it->src_handle();
      #ifndef NDEBUG
      parent_edge_it++;
      // std::cout << "pcm_vertex_handle->id(): "
      //           <<  pcm_vertex_handle->id().first
      //           << std::endl;
      // std::cout << "parent_vertex_handle->id(): "
      //           <<  parent_vertex_handle->id().first
      //           << std::endl;
      // if (!parent_edge_it.IsDone()){
      //   std::cout << "parent_edge_it->src_handle()->id(): "
      //             <<  parent_edge_it->src_handle()->id().first
      //             << std::endl;
      // }
      // should have only one parent
      assert(parent_edge_it.IsDone());
      #endif
      auto& parent_pattern = GetPatternRef(parent_vertex_handle->id(),
                                           query_graph_list,
                                      additional_graph_list);
      auto&  child_pattern = GetPatternRef(pcm_vertex_handle->id(),
                                            query_graph_list,
                                       additional_graph_list);
      AddMatchToParent(pcm_vertex_handle, parent_pattern,
                                           child_pattern);
      // continue dfs
      return true;
    };
    Dfs(pcm_tree, root_handle, add_match_to_parent_callback);
    root_handle_set.emplace_back(root_handle);
  }
  return root_handle_set;
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename PatternTree,
          typename  QueryGraph,
          template<typename> typename QueryGraphContainerType,
          typename TargetGraph>
bool MatchFromParentToChild(
            PatternTree& pcm_tree,
   typename VertexHandle<PatternTree>::type current_pattern_handle,
                    Match<QueryGraph, 
                         TargetGraph> match,
     std::vector<
        std::map<typename VertexHandle< QueryGraph>::type,
     std::vector<typename VertexHandle<TargetGraph>::type>>>&            candidate_set_list,
     std::vector<
        std::map<typename VertexHandle< QueryGraph>::type,
     std::vector<typename VertexHandle<TargetGraph>::type>>>& additional_candidate_set_list,
                QueryGraphContainerType<QueryGraph>&      query_graph_list,
                            std::vector<QueryGraph>& additional_graph_list,
                            std::vector<bool> call_match_callback,
   std::vector<bool>            call_child_pattern_match_callback,
   std::vector<bool> additional_call_child_pattern_match_callback,
                                        TargetGraph& target_graph,
  std::function<bool(int,
                     const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(int,
                     const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback){

  using PcmTreeVertexHandle = typename VertexHandle<PatternTree>::type;
  using   QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using  TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using CandidateSetType = std::map<QueryVertexHandle,
       
                       std::vector<TargetVertexHandle>>;

  using MatchMap = std::map<QueryVertexHandle,
                           TargetVertexHandle>;

  using MatchPatternToPatternType   = Match<QueryGraph,  QueryGraph>;
  using MatchPatternToDataGraphType = Match<QueryGraph, TargetGraph>;

  // std::cout << "##############################" << std::endl;
  // std::cout << "current_pattern_handle->id().first: "
  //           <<  current_pattern_handle->id().first
  //           << std::endl
  //           << "current_pattern_handle->id().second: "
  //           <<  current_pattern_handle->id().second
  //           << std::endl;
  // std::cout << "##############################" << std::endl;
  
  // std::cout << "# Match #" << std::endl;
  // for (auto map_it = match.MapBegin();
  //          !map_it.IsDone();
  //           map_it++){
  //   std::cout << "src id: " << map_it->src_handle()->id() << std::endl
  //             << "dst id: " << map_it->dst_handle()->id() << std::endl;
  // }

  assert( candidate_set_list.size()
         == query_graph_list.size());
  assert(call_match_callback.size()
         == query_graph_list.size());

  assert(additional_candidate_set_list.size()
              == additional_graph_list.size());
  assert(additional_call_child_pattern_match_callback.size()
                             == additional_graph_list.size());

  auto current_pattern_idx = GetPatternIdx(current_pattern_handle);

  // the hold the current match from parent pattern to child pattern
  // the match from parent patten to data graph can be viewed as a
  // parital match from the child pattern to data graph
  auto& current_candidate_set = GetCandidateRef(current_pattern_idx, 
                                                 candidate_set_list, 
                                      additional_candidate_set_list);
  auto&   current_query_graph = GetPatternRef(current_pattern_idx, 
                                                 query_graph_list, 
                                            additional_graph_list);

  // for each pattern, 
  // the current match need to be prune if and only if the prune_callback
  // of it and all its children return true
  auto current_pattern_prune_callback 
   = [&current_pattern_handle,
      &call_match_callback,
             &prune_callback,
             &pcm_tree](const MatchMap& match) -> bool {
    // one-direction bfs at this vertex, if meet one prune_callback return false,
    // then end matching
    bool all_prune_return_true = true;
    auto bfs_callback = [&all_prune_return_true,
                         &prune_callback,
                         &call_match_callback,
                         &current_pattern_handle,
                         &match,
                         &pcm_tree](PcmTreeVertexHandle pcm_tree_vertex_handle) {
      auto pattern_idx = GetPatternIdx(pcm_tree_vertex_handle);
      if (!pattern_idx.second){
        // this pattern is an additional pattern
        // does not need to call prune_callback
        // contine searching on pcm_tree
        return true;
      }
      // this pattern is not additional pattern
      // call the callback on it
      if (!prune_callback(pattern_idx.first, match)) {
        // has found a pattern that cannot be pruned at this 
        // partial match, mark all_prune_return_true as false
        all_prune_return_true = false;
        // the bfs process cannot be stopped, to verify 
        // whether the match callback for the child pattern
        // needs to be called
        return true;
      }
      assert(pattern_idx.second);
      // has been pruned, the match_callback for this pattern
      // does not need to be called
      call_match_callback[pattern_idx.first] = false;
      // needs to be prune
      return true;
    };

    Bfs<false>(pcm_tree, current_pattern_handle, bfs_callback);

    // return true if all prune return true
    return all_prune_return_true;
  };

  // for each pattern, hold a flage to store whether the return value
  // has been true
  // and another flage stores whether all its children return true,
  // return false if and only if the match callback of this pattern
  // and all its child pattern return false
  // if the match callback for this pattern has already been marked
  // as false, then do not call the match callback for this pattern
  // and only all the child pattern that has not been marked as false
  auto current_pattern_match_callback 
   = [&current_pattern_idx,
      &current_pattern_handle,
        &candidate_set_list,
        &additional_candidate_set_list,
        &query_graph_list,
        &additional_graph_list,
        &pcm_tree,
        &call_match_callback,
        &call_child_pattern_match_callback,
        &additional_call_child_pattern_match_callback,
        &target_graph,
        &prune_callback,
        &match_callback](const MatchMap& match) -> bool {

    // std::cout << "## match ##" << std::endl;
    // std::cout << "current_pattern_handle->id().first: "
    //           <<  current_pattern_handle->id().first
    //           << std::endl
    //           << "current_pattern_handle->id().second: "
    //           <<  current_pattern_handle->id().second
    //           << std::endl;

    #ifndef NDEBUG
    if (current_pattern_idx.second){
      assert(current_pattern_idx.first >= 0 
          && current_pattern_idx.first < call_child_pattern_match_callback.size());
      assert(call_child_pattern_match_callback[current_pattern_idx.first]);
    }
    if (!current_pattern_idx.second){
      assert(current_pattern_idx.first >= 0 
          && current_pattern_idx.first < additional_call_child_pattern_match_callback.size());
      assert(additional_call_child_pattern_match_callback[current_pattern_idx.first]);
    }
    #endif // NDEBUG
    
    // call match_callback only when the flage for this pattern
    // is not marked as false
    if (current_pattern_idx.second // the order cannot be changed
     && call_match_callback[current_pattern_idx.first]){
        call_match_callback[current_pattern_idx.first]
           = match_callback(current_pattern_idx.first, match);
    }

    bool all_child_pattern_need_to_be_called = false;
    // get a match from parent pattern to child pattern
    for (auto child_edge_it = current_pattern_handle->OutEdgeBegin();
             !child_edge_it.IsDone();
              child_edge_it++) {
            
      auto child_handle = child_edge_it->dst_handle();
      assert(child_handle);

      // std::cout << "## call match ##" << std::endl;
      // std::cout << "child_handle->id().first: "
      //           <<  child_handle->id().first
      //           << std::endl
      //           << "child_handle->id().second: "
      //           <<  child_handle->id().second
      //           << std::endl;

      assert(candidate_set_list.size()
            == query_graph_list.size());

      auto child_pattern_idx = GetPatternIdx(child_handle);

      if (!NeedToCallChildPatternMatchCallback(child_pattern_idx,
                               call_child_pattern_match_callback,
                    additional_call_child_pattern_match_callback)){
        // the match_callback of this child pattern
        // does not need to be called 
        continue;
      }

      MatchPatternToPatternType child_to_parent_match 
             = GetMatchToParent<QueryGraph>(child_handle);

      MatchPatternToDataGraphType parent_to_target_graph_match;
      for (auto& map : match){
        parent_to_target_graph_match.AddMap(map.first, map.second);
      }

      MatchPatternToDataGraphType child_to_target_graph_partial_match
                               = parent_to_target_graph_match(
                                  child_to_parent_match);
      
      bool child_match_ret = MatchFromParentToChild(
                              pcm_tree,
                              child_handle, 
                              child_to_target_graph_partial_match, 
                              candidate_set_list,
                   additional_candidate_set_list,
                                query_graph_list,
                           additional_graph_list,
                              call_match_callback,
                call_child_pattern_match_callback,
     additional_call_child_pattern_match_callback,
                                    target_graph,
                                  prune_callback,
                                  match_callback);

      if (NeedToCallChildPatternMatchCallback(child_pattern_idx,
                               call_child_pattern_match_callback,
                    additional_call_child_pattern_match_callback)){
        // the match_callback of this child pattern or its children
        // still needs to be called
        all_child_pattern_need_to_be_called = true;
      }
    }

    assert(NeedToCallChildPatternMatchCallback(current_pattern_idx,
                                 call_child_pattern_match_callback,
                      additional_call_child_pattern_match_callback));

    if (current_pattern_idx.second){
      // this pattern is not an additional pattern
      assert(current_pattern_idx.first >= 0
          && current_pattern_idx.first < call_child_pattern_match_callback.size());
      call_child_pattern_match_callback[current_pattern_idx.first]
                  = call_match_callback[current_pattern_idx.first]
                     || all_child_pattern_need_to_be_called;
      return call_child_pattern_match_callback[current_pattern_idx.first];
    }
    // this pattern is an additional pattern
    assert(current_pattern_idx.first >= 0
        && current_pattern_idx.first < additional_call_child_pattern_match_callback.size());
    // only consider whether the match callback for the
    // child patten of it needs to be called
    additional_call_child_pattern_match_callback[current_pattern_idx.first]
              = all_child_pattern_need_to_be_called;
    return additional_call_child_pattern_match_callback[current_pattern_idx.first];
  };

  return DpisoUsingMatch(current_query_graph,
                                target_graph,
                                match,
                         current_candidate_set,
                         current_pattern_prune_callback,
                         current_pattern_match_callback);
}

template <enum MatchSemantics match_semantics,
          typename GraphPatternType,
          typename    DataGraphType>
void CandidateSetForPatternList(
       std::vector<GraphPatternType>& pattern_set, 
                      DataGraphType&  data_graph,
       std::vector<std::map<typename VertexHandle<GraphPatternType>::type, 
                std::vector<typename VertexHandle<   DataGraphType>::type>>>& candidate_set_set){
            
  using GraphPatternVertexHandle = typename VertexHandle<GraphPatternType>::type;
  using    DataGraphVertexHandle = typename VertexHandle<   DataGraphType>::type;

  using CandidateSetType = std::map<GraphPatternVertexHandle, 
                           std::vector<DataGraphVertexHandle>>;

  candidate_set_set.reserve(pattern_set.size());
  for (auto& pattern : pattern_set){
    CandidateSetType candidate_set;
    if (!_dp_iso::InitCandidateSet<match_semantics>(pattern, data_graph,
                                                    candidate_set)) {
      // there is a bug in Dpiso if the candidate set is empty
      assert(false);
      candidate_set_set.emplace_back();
      continue;
    }
    if (!_dp_iso::RefineCandidateSet(pattern, data_graph, 
                                     candidate_set)) {
      // there is a bug in Dpiso if the candidate set is empty
      assert(false);
      candidate_set_set.emplace_back();
      continue;
    }
    candidate_set_set.emplace_back(std::move(candidate_set));
  }

  assert(candidate_set_set.size()
            == pattern_set.size());
  return;
}

};

// with partial match 
template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          template<typename> typename QueryGraphContainerType,
          typename TargetGraph>
inline void MultiQueryDpiso(
  QueryGraphContainerType<QueryGraph>&  query_graph_list,
             TargetGraph & target_graph,
                    QueryGraph& common_graph,
  std::vector<Match<QueryGraph,
                    QueryGraph>>& paritial_match_to_common_graph_list,
  std::function<bool(int,
                     const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(int,
                     const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
   double time_limit = -1.0) {

  using VertexIDType = uint32_t;
  using   EdgeIDType = uint32_t;

  using VertexLabelType = uint32_t;
  using   EdgeLabelType = uint32_t;

  using   QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using  TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using CandidateSetType = std::map<QueryVertexHandle,
                       std::vector<TargetVertexHandle>>;

  using MatchMap = std::map<QueryVertexHandle,
                           TargetVertexHandle>;

  using MatchPatternToPatternType   = Match<QueryGraph,  QueryGraph>;
  using MatchPatternToDataGraphType = Match<QueryGraph, TargetGraph>;

  assert(query_graph_list.size() == paritial_match_to_common_graph_list.size());
  #ifndef NDEBUG
  for (int i = 0; i < query_graph_list.size(); i++) {
    auto& query_graph = query_graph_list[i];
    assert(common_graph.CountVertex()
         <= query_graph.CountVertex());
    assert(common_graph.CountEdge()
         <= query_graph.CountEdge());
    assert(paritial_match_to_common_graph_list[i].size()
        == query_graph.CountVertex());
    std::set<QueryVertexHandle> target_handle_set;
    for (auto query_vertex_it = query_graph.VertexBegin();
             !query_vertex_it.IsDone();
              query_vertex_it++) {
      assert(paritial_match_to_common_graph_list[i].HasMap(query_vertex_it));
      auto target_handle = paritial_match_to_common_graph_list[i].MapTo(query_vertex_it);
      assert(target_handle);
      auto [ target_handle_it,
             target_handle_ret ]
           = target_handle_set.emplace(target_handle);
      assert(target_handle_ret);
      auto find_target_handle = common_graph.FindVertex(target_handle->id());
      assert(find_target_handle == target_handle);
    }
    auto match_pattern_i_common_pattern_i = paritial_match_to_common_graph_list[i]
                                 .Reverse()(paritial_match_to_common_graph_list[i]);
    for (auto map_it = paritial_match_to_common_graph_list[i].MapBegin();
             !map_it.IsDone(); map_it++) {
      assert(paritial_match_to_common_graph_list[i].HasMap(map_it->src_handle()));
      assert(      match_pattern_i_common_pattern_i.HasMap(map_it->src_handle()));
      assert(match_pattern_i_common_pattern_i.MapTo(map_it->src_handle())
                                                 == map_it->src_handle());
    }
  }
  #endif // NDEBUG
  
  using PcmTreeType = GUNDAM::LargeGraph<std::pair<VertexIDType, bool>, VertexLabelType, std::string,
                                            EdgeIDType,   EdgeLabelType, std::string>;

  std::vector<QueryGraph> additional_graph_list;

  std::vector<Match<QueryGraph,
                    QueryGraph>> additional_paritial_match_to_common_graph_list;

  PcmTreeType pcm_tree;
  auto root_handle_set = _multi_query_dp_iso::BuildPcm(pcm_tree, query_graph_list,
                                                            additional_graph_list,
                                              paritial_match_to_common_graph_list,
                                   additional_paritial_match_to_common_graph_list);
  assert(!root_handle_set.empty());
  #ifndef NDEBUG
  for (const auto& root_handle : root_handle_set) {
    assert(root_handle);
  }
  #endif

  std::vector<CandidateSetType> candidate_set_list;
  candidate_set_list.reserve(query_graph_list.size());

  std::vector<CandidateSetType> additional_candidate_set_list;
  additional_candidate_set_list.reserve(additional_graph_list.size());

  // pre-process the candidate set from each pattern & additional_pattern
  // to data graph in advance
  _multi_query_dp_iso::CandidateSetForPatternList<match_semantics>(
                             query_graph_list, target_graph, 
                           candidate_set_list);
  _multi_query_dp_iso::CandidateSetForPatternList<match_semantics>(
                        additional_graph_list, target_graph,
                additional_candidate_set_list);

      // to mark whether the match_callback of this pattern need to be called
  std::vector<bool> call_match_callback(query_graph_list.size(), true),
      // to mark whether the match_callback for the child patterns need to be called
      call_child_pattern_match_callback(query_graph_list.size(), true);

  std::vector<bool> additional_call_child_pattern_match_callback(additional_graph_list.size(), true);
  
  Match<QueryGraph, TargetGraph> match;

  for (auto& root_handle : root_handle_set){
    _multi_query_dp_iso::MatchFromParentToChild(
                          pcm_tree,
                          root_handle, match, 
                          candidate_set_list,
               additional_candidate_set_list,
                            query_graph_list,
                       additional_graph_list,
                          call_match_callback,
            call_child_pattern_match_callback,
 additional_call_child_pattern_match_callback,
                                target_graph,
                              prune_callback,
                              match_callback);
  }
  return;
}

template <enum MatchSemantics match_semantics 
             = MatchSemantics::kIsomorphism,
          typename  QueryGraph,
          template<typename> typename QueryGraphContainerType,
          typename TargetGraph>
inline void MultiQueryDpiso(
  QueryGraphContainerType<QueryGraph>&  query_graph_list,
             TargetGraph & target_graph,
  std::function<bool(int,
                     const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> prune_callback,
  std::function<bool(int,
                     const std::map<typename VertexHandle< QueryGraph>::type, 
                                    typename VertexHandle<TargetGraph>::type>&)> match_callback,
   double time_limit = -1.0) {

  using VertexIDType = uint32_t;
  using   EdgeIDType = uint32_t;

  using VertexLabelType = uint32_t;
  using   EdgeLabelType = uint32_t;
  
  using PcmTreeType = GUNDAM::LargeGraph<std::pair<VertexIDType, bool>, VertexLabelType, std::string,
                                            EdgeIDType,   EdgeLabelType, std::string>;

  std::vector<QueryGraph> additional_graph_list;

  PcmTreeType pcm_tree;
  auto root_handle_set = _multi_query_dp_iso::BuildPcm(pcm_tree, query_graph_list,
                                                            additional_graph_list);
  assert(!root_handle_set.empty());
  #ifndef NDEBUG
  for (const auto& root_handle : root_handle_set) {
    assert(root_handle);
  }
  #endif

  using  QueryVertexHandle = typename VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;

  using CandidateSetType = std::map<QueryVertexHandle, 
                       std::vector<TargetVertexHandle>>;

  using MatchMap = std::map<QueryVertexHandle,
                           TargetVertexHandle>;

  std::vector<CandidateSetType> candidate_set_list;
  candidate_set_list.reserve(query_graph_list.size());

  std::vector<CandidateSetType> additional_candidate_set_list;
  additional_candidate_set_list.reserve(additional_graph_list.size());

  // pre-process the candidate set from each pattern & additional_pattern
  // to data graph in advance
  _multi_query_dp_iso::CandidateSetForPatternList<match_semantics>(
                             query_graph_list, target_graph, 
                           candidate_set_list);
  _multi_query_dp_iso::CandidateSetForPatternList<match_semantics>(
                        additional_graph_list, target_graph,
                additional_candidate_set_list);

      // to mark whether the match_callback of this pattern need to be called
  std::vector<bool> call_match_callback(query_graph_list.size(), true),
      // to mark whether the match_callback for the child patterns need to be called
      call_child_pattern_match_callback(query_graph_list.size(), true);

  std::vector<bool> additional_call_child_pattern_match_callback(additional_graph_list.size(), true);
  
  Match<QueryGraph, TargetGraph> match;

  for (auto& root_handle : root_handle_set){
    _multi_query_dp_iso::MatchFromParentToChild(
                          pcm_tree,
                          root_handle, match, 
                          candidate_set_list,
               additional_candidate_set_list,
                            query_graph_list,
                       additional_graph_list,
                          call_match_callback,
            call_child_pattern_match_callback,
 additional_call_child_pattern_match_callback,
                                target_graph,
                              prune_callback,
                              match_callback);
  }
  return;
}

};

#endif // _GUNDAM_ALGORITHM_MULTI_QUERY_DPISO_USING_MATCH_H