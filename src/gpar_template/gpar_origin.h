#ifndef _GPARORIGIN_H
#define _GPARORIGIN_H
#include <omp.h>

#include <algorithm>
#include <chrono>

#include "../gpar/test.cpp"
#include "get_datagraph_information.h"
#include "gpar_new.h"
#include "gundam/csvgraph.h"
#include "gundam/dp_iso.h"
#include "gundam/vf2.h"
namespace gmine_new {
omp_lock_t expand_lock;
omp_lock_t filter_lock;
int gen_maxid = 0;
template <typename T>
inline constexpr double CalTimeOrigin(T &begin, T &end) {
  return double(
             std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                 .count()) *
         std::chrono::microseconds::period::num /
         std::chrono::microseconds::period::den;
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeInPatternOrigin(const GPAR &gpar,
                                      const VertexLabelSet &vertex_label_set,
                                      const EdgeLabelSet &edge_label_set,
                                      const EdgeTypeSet &edge_type_set,
                                      GPARList &expand_gpar_list);
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeOutPatternOrigin(const GPAR &gpar,
                                       const VertexLabelSet &vertex_label_set,
                                       const EdgeLabelSet &edge_label_set,
                                       const EdgeTypeSet &edge_type_set,
                                       GPARList &expand_gpar_list);
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void GPARExpandOrigin(const GPAR &gpar,
                             const VertexLabelSet &vertex_label_set,
                             const EdgeLabelSet &edge_label_set,
                             const EdgeTypeSet &edge_type_set,
                             GPARList &expand_gpar_list);
template <class GPARList>
inline void UniqueGPAROrigin(GPARList &gpar_list);
template <class GPAR, class DataGraph>
inline void CalSuppQ(GPAR &gpar, DataGraph &data_graph);
template <class GPAR, class DataGraph>
inline void CalSuppR(GPAR &gpar, DataGraph &data_graph);
template <class GPARList, typename SuppType, class DataGraph>
inline void FilterGPARUsingSuppRLimit(GPARList &gpar_list,
                                      const DataGraph &data_graph,
                                      const SuppType &sigma);
template <class GPAR, class DataGraph>
inline double diff(GPAR &query, GPAR &target, const DataGraph &data_graph);
template <class GPAR, class DataGraph>
inline double conf(const GPAR &gpar, const DataGraph &data_graph);
template <class GPAR, class DataGraph>
inline double F(GPAR &query, GPAR &target, const DataGraph &data_graph,
                size_t k);
template <class GPAR, class TopKContainerType>
inline bool GPARInTopKContainer(const GPAR &gpar,
                                const TopKContainerType &top_k_container);
template <class GPAR>
inline bool CheckIsIsoOrigin(const GPAR &query, const GPAR &target);
template <class TopKContainerType, class GPARList, class GPAR, class DataGraph>
inline void IncDiv(TopKContainerType &top_k_container, size_t k, GPAR &gpar,
                   GPARList &total_gpar, const DataGraph &data_graph);
template <class GPAR, class DataGraph>
inline bool IsExtendable(const GPAR &gpar, const DataGraph &data_graph);
template <class TopKContainerType>
inline void OutputDiscoverResult(TopKContainerType &top_k_container,
                                 const char *output_dir);
template <class Pattern, class DataGraph>
inline void DMineOrigin(
    const char *v_file, const char *e_file,  // data graph
    const typename Pattern::VertexType::LabelType x_node_label,
    const typename Pattern::VertexType::LabelType y_node_label,
    const typename Pattern::EdgeType::LabelType q_edge_label,  // q(x,y)
    const size_t sigma,                                        // supp_r limit
    const size_t k,                                            // top-k
    const size_t d,                                            // radius of GPAR
    const char *output_dir) {
  omp_set_num_threads(2);
  gen_maxid = 0;
  using DataGraphVertexLabelType = typename DataGraph::VertexType::LabelType;
  using DataGraphEdgeLabelType = typename DataGraph::EdgeType::LabelType;
  // data graph
  DataGraph data_graph;
  auto read_data_graph_begin = std::chrono::system_clock::now();
  GUNDAM::ReadCSVGraph(data_graph, v_file, e_file);
  auto read_data_graph_end = std::chrono::system_clock::now();
  double read_data_graph_time =
      CalTimeOrigin(read_data_graph_begin, read_data_graph_end);
  std::cout << "read end!" << std::endl;
  std::cout << "read data graph time = " << read_data_graph_time << std::endl;

  // DataGraph Information
  std::set<DataGraphVertexLabelType> vertex_label_set;
  std::set<DataGraphEdgeLabelType> edge_label_set;
  std::set<std::tuple<DataGraphVertexLabelType, DataGraphVertexLabelType,
                      DataGraphEdgeLabelType>>
      edge_type_set;
  auto build_data_information_begin = std::chrono::system_clock::now();
  GetDataGraphInformation(data_graph, vertex_label_set, edge_label_set,
                          edge_type_set);
  auto build_data_information_end = std::chrono::system_clock::now();
  double build_data_information_time =
      CalTimeOrigin(build_data_information_begin, build_data_information_end);
  std::cout << "get data graph information time is "
            << build_data_information_time << std::endl;
  using SuppContainer = std::set<typename DataGraph::VertexConstPtr>;
  using SuppType = size_t;
  using ConfType = double;
  using GPARList = std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
  using TopKContainerType =
      std::vector<std::pair<DiscoverdGPAR<Pattern, DataGraph>,
                            DiscoverdGPAR<Pattern, DataGraph>>>;
  //∑
  GPARList total_gpar;
  // M
  GPARList last_round_gpar;
  // L_k
  TopKContainerType top_k_container;
  // cal root gpar 's supp_q ,supp_r and conf
  DiscoverdGPAR<Pattern, DataGraph> root_gpar(x_node_label, y_node_label,
                                              q_edge_label);
  CalSuppQ(root_gpar, data_graph);
  CalSuppR(root_gpar, data_graph);
  size_t supp_r_size = root_gpar.supp_R_size();
  if (supp_r_size < sigma) {
    return;
  }
  root_gpar.Setid(gen_maxid++);
  last_round_gpar.push_back(root_gpar);
  auto begin = std::chrono::system_clock::now();
  omp_init_lock(&expand_lock);
  omp_init_lock(&filter_lock);
  for (int32_t r = 1; r <= d; r++) {
    std::cout << "r = " << r << std::endl;
    // delta E
    GPARList this_round_gpar;
    auto begin = std::chrono::system_clock::now();

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(last_round_gpar.size()); ++i) {
      auto &gpar = last_round_gpar[i];
      std::cout << "gpar id = " << gpar.id() << std::endl;
      GPARExpandOrigin(gpar, vertex_label_set, edge_label_set, edge_type_set,
                       this_round_gpar);
    }
    UniqueGPAROrigin(this_round_gpar);
    auto end = std::chrono::system_clock::now();
    std::cout << "geneator gpar time is " << CalTimeOrigin(begin, end)
              << std::endl;
    std::cout << "gen " << this_round_gpar.size() << " gpars." << std::endl;
    begin = std::chrono::system_clock::now();
    FilterGPARUsingSuppRLimit(this_round_gpar, data_graph, sigma);
    end = std::chrono::system_clock::now();
    std::cout << "Filter GPAR time is " << CalTimeOrigin(begin, end)
              << std::endl;
    last_round_gpar.clear();
    for (const auto &gpar : this_round_gpar) {
      total_gpar.push_back(gpar);
    }
    begin = std::chrono::system_clock::now();
    for (auto &gpar : this_round_gpar) {
      IncDiv(top_k_container, k, gpar, total_gpar, data_graph);
      if (IsExtendable(gpar, data_graph)) {
        last_round_gpar.push_back(gpar);
      }
    }
    end = std::chrono::system_clock::now();
    std::cout << "inc div time is " << CalTimeOrigin(begin, end) << std::endl;
  }
  auto end = std::chrono::system_clock::now();
  std::cout << "total iteration time = " << CalTimeOrigin(begin, end)
            << std::endl;
  OutputDiscoverResult(top_k_container, output_dir);
  return;
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void GPARExpandOrigin(const GPAR &gpar,
                             const VertexLabelSet &vertex_label_set,
                             const EdgeLabelSet &edge_label_set,
                             const EdgeTypeSet &edge_type_set,
                             GPARList &expand_gpar_list) {
  AddNewEdgeInPatternOrigin(gpar, vertex_label_set, edge_label_set,
                            edge_type_set, expand_gpar_list);
  AddNewEdgeOutPatternOrigin(gpar, vertex_label_set, edge_label_set,
                             edge_type_set, expand_gpar_list);
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeInPatternOrigin(const GPAR &gpar,
                                      const VertexLabelSet &vertex_label_set,
                                      const EdgeLabelSet &edge_label_set,
                                      const EdgeTypeSet &edge_type_set,
                                      GPARList &expand_gpar_list) {
  using VertexSizeType = typename GPAR::VertexSizeType;
  using VertexIDType = typename GPAR::VertexIDType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  VertexSizeType pattern_size = gpar.pattern.CountVertex();
  VertexIDType x_node_id = gpar.x_node_ptr()->id();
  VertexIDType y_node_id = gpar.y_node_ptr()->id();
  EdgeLabelType q_edge_label = gpar.q_edge_label();
  for (auto src_node_iter = gpar.pattern.VertexCBegin();
       !src_node_iter.IsDone(); src_node_iter++) {
    for (auto dst_node_iter = gpar.pattern.VertexCBegin();
         !dst_node_iter.IsDone(); dst_node_iter++) {
      // not add self loop
      if (src_node_iter->id() == dst_node_iter->id()) continue;
      if (src_node_iter->id() == x_node_id && dst_node_iter->id() == y_node_id)
        continue;
      for (const auto &possible_edge_label : edge_label_set) {
        // not add edge which does not exist in data graph
        if (!edge_type_set.count(typename EdgeTypeSet::value_type(
                src_node_iter->label(), dst_node_iter->label(),
                possible_edge_label))) {
          continue;
        }
        omp_set_lock(&expand_lock);
        GPAR expand_gpar(gpar);
        expand_gpar.pattern.AddEdge(src_node_iter->id(), dst_node_iter->id(),
                                    possible_edge_label,
                                    expand_gpar.pattern.CountEdge() + 1);
        expand_gpar.Setid(gen_maxid++);
        expand_gpar_list.emplace_back(std::move(expand_gpar));
        omp_unset_lock(&expand_lock);
      }
    }
  }
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeOutPatternOrigin(const GPAR &gpar,
                                       const VertexLabelSet &vertex_label_set,
                                       const EdgeLabelSet &edge_label_set,
                                       const EdgeTypeSet &edge_type_set,
                                       GPARList &expand_gpar_list) {
  using VertexSizeType = typename GPAR::VertexSizeType;
  using VertexIDType = typename GPAR::VertexIDType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  VertexSizeType pattern_size = gpar.pattern.CountVertex();
  VertexIDType x_node_id = gpar.x_node_ptr()->id();
  VertexIDType y_node_id = gpar.y_node_ptr()->id();
  EdgeLabelType q_edge_label = gpar.q_edge_label();
  VertexIDType new_vertex_id = static_cast<VertexIDType>(pattern_size + 1);

  for (auto node_iter = gpar.pattern.VertexCBegin(); !node_iter.IsDone();
       node_iter++) {
    if (node_iter->id() == y_node_id) continue;
    for (const auto &possible_vertex_label : vertex_label_set) {
      for (const auto &possible_edge_label : edge_label_set) {
        // not add edge which does not exist in data graph
        if (edge_type_set.count(typename EdgeTypeSet::value_type(
                node_iter->label(), possible_vertex_label,
                possible_edge_label))) {
          omp_set_lock(&expand_lock);
          GPAR expand_gpar(gpar);
          expand_gpar.pattern.AddVertex(new_vertex_id, possible_vertex_label);
          expand_gpar.pattern.AddEdge(node_iter->id(), new_vertex_id,
                                      possible_edge_label,
                                      expand_gpar.pattern.CountEdge() + 1);
          expand_gpar.Setid(gen_maxid++);
          expand_gpar_list.emplace_back(std::move(expand_gpar));
          omp_unset_lock(&expand_lock);
        }
        if (edge_type_set.count(typename EdgeTypeSet::value_type(
                possible_vertex_label, node_iter->label(),
                possible_edge_label))) {
          omp_set_lock(&expand_lock);
          GPAR expand_gpar(gpar);
          expand_gpar.pattern.AddVertex(new_vertex_id, possible_vertex_label);
          expand_gpar.pattern.AddEdge(new_vertex_id, node_iter->id(),
                                      possible_edge_label,
                                      expand_gpar.pattern.CountEdge() + 1);
          expand_gpar.Setid(gen_maxid++);
          expand_gpar_list.emplace_back(std::move(expand_gpar));
          omp_unset_lock(&expand_lock);
        }
      }
    }
  }
}
template <class GPARList>
inline void UniqueGPAROrigin(GPARList &gpar_list) {
  using GPAR = typename GPARList::value_type;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  GPARList unique_gpar_list;
  for (const auto &query : gpar_list) {
    bool find_iso_flag = false;
    for (const auto &target : unique_gpar_list) {
      MatchResultList match_result;
      GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
          query.pattern, target.pattern, query.x_node_ptr()->id(),
          target.x_node_ptr()->id(),
          GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
          GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1,
          match_result);
      if (match_result.size() >= 1) {
        find_iso_flag = true;
        break;
      }
    }
    if (!find_iso_flag) {
      unique_gpar_list.push_back(query);
    }
  }
  std::swap(gpar_list, unique_gpar_list);
}
template <class GPAR, class DataGraph>
inline void CalSuppQ(GPAR &gpar, DataGraph &data_graph) {
  // TransposePattern(gpar.pattern);
  using SuppType = std::vector<typename DataGraph::VertexConstPtr>;
  using SuppSet = std::set<typename DataGraph::VertexConstPtr>;
  using MatchMap = std::map<typename GPAR::VertexConstPtr,
                            typename DataGraph::VertexConstPtr>;
  using CandidateSetType =
      std::map<typename GPAR::VertexConstPtr,
               std::vector<typename DataGraph::VertexConstPtr>>;
  using MatchResult = std::vector<MatchMap>;
  SuppType &supp_Q = gpar.supp_Q();
  SuppType temp_supp_Q;
  GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      gpar.pattern, data_graph, gpar.x_node_ptr(), supp_Q, temp_supp_Q);
  supp_Q = temp_supp_Q;
  return;
}
template <class GPAR, class DataGraph>
inline void CalSuppR(GPAR &gpar, DataGraph &data_graph) {
  using SuppType = std::vector<typename DataGraph::VertexConstPtr>;
  using SuppSet = std::set<typename DataGraph::VertexConstPtr>;
  using MatchMap = std::map<typename GPAR::VertexConstPtr,
                            typename DataGraph::VertexConstPtr>;
  using MatchResult = std::vector<MatchMap>;
  using CandidateSetType =
      std::map<typename GPAR::VertexConstPtr,
               std::vector<typename DataGraph::VertexConstPtr>>;
  SuppType &supp_R = gpar.supp_R();
  SuppType temp_supp_R;
  size_t gpar_edge = gpar.pattern.CountEdge();
  gpar.pattern.AddEdge(gpar.x_node_ptr()->id(), gpar.y_node_ptr()->id(),
                       gpar.q_edge_label(), gpar_edge + 1);
  GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      gpar.pattern, data_graph, gpar.x_node_ptr(), supp_R, temp_supp_R);
  supp_R = temp_supp_R;
  // std::cout << "supp r=" << temp_supp_R.size() << std::endl;
  gpar.pattern.EraseEdge(gpar_edge + 1);
  return;
}
template <class GPARList, typename SuppType, class DataGraph>
inline void FilterGPARUsingSuppRLimit(GPARList &gpar_list,
                                      const DataGraph &data_graph,
                                      const SuppType &sigma) {
  GPARList filter_gpar_list;
  using SuppContainerType = std::set<typename DataGraph::VertexConstPtr>;
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < static_cast<int>(gpar_list.size()); ++i) {
    auto &gpar = gpar_list[i];

    auto t_begin = clock();
    CalSuppQ(gpar, data_graph);
    auto t_end = clock();
    t_begin = clock();
    CalSuppR(gpar, data_graph);
    t_end = clock();
    omp_set_lock(&filter_lock);
    if (gpar.supp_R().size() >= sigma) {
      gpar.CalConf();
      filter_gpar_list.push_back(gpar);
    }
    omp_unset_lock(&filter_lock);
  }
  std::swap(filter_gpar_list, gpar_list);
}
template <class GPAR, class DataGraph>
inline double diff(GPAR &query, GPAR &target, const DataGraph &data_graph) {
  using SuppType = std::vector<typename DataGraph::VertexConstPtr>;
  SuppType &a_supp_r = query.supp_R();
  SuppType &b_supp_r = target.supp_R();
  SuppType intersection_result;
  auto t_begin = clock();
  std::set_intersection(
      std::cbegin(a_supp_r), std::cend(a_supp_r), std::cbegin(b_supp_r),
      std::cend(b_supp_r),
      inserter(intersection_result, intersection_result.begin()));
  auto t_end = clock();
  size_t intersection_size = intersection_result.size();
  size_t union_size = a_supp_r.size() + b_supp_r.size() - intersection_size;
  return 1 - static_cast<double>(intersection_size) /
                 static_cast<double>(union_size);
  /*
  using SuppContainerType = std::set<typename DataGraph::VertexConstPtr>;
  SuppContainerType query_supp_r, target_supp_r;
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query.pattern, data_graph, [&query_supp_r, &query](auto &match_state) {
        const auto &x_match_ptr = match_state.find(query.x_node_ptr())->second;
        const auto &y_match_ptr = match_state.find(query.y_node_ptr())->second;
        if (query_supp_r.count(x_match_ptr)) {
          return true;
        }
        auto it = x_match_ptr->OutEdgeCBegin(query.q_edge_label(), y_match_ptr);
        if (!it.IsDone()) {
          query_supp_r.insert(x_match_ptr);
        }
        return true;
      });
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query.pattern, data_graph, [&target_supp_r, &target](auto &match_state) {
        const auto &x_match_ptr = match_state.find(target.x_node_ptr())->second;
        const auto &y_match_ptr = match_state.find(target.y_node_ptr())->second;
        if (target_supp_r.count(x_match_ptr)) {
          return true;
        }
        auto it =
            x_match_ptr->OutEdgeCBegin(target.q_edge_label(), y_match_ptr);
        if (!it.IsDone()) {
          target_supp_r.insert(x_match_ptr);
        }
        return true;
      });
  std::vector<typename DataGraph::VertexConstPtr> intersection_result;
  std::set_intersection(
      std::begin(query_supp_r), std::end(query_supp_r),
      std::begin(target_supp_r), std::end(target_supp_r),
      inserter(intersection_result, intersection_result.begin()));
  return (1.0 -
          (static_cast<double>(intersection_result.size())) /
              static_cast<double>((query_supp_r.size() + target_supp_r.size() -
                                   intersection_result.size())));
  */
}
template <class GPAR, class DataGraph>
inline double conf(const GPAR &gpar, const DataGraph &data_graph) {
  return gpar.conf();
  /*
  using SuppContainerType = std::set<typename DataGraph::VertexConstPtr>;
  SuppContainerType supp_q, supp_r;
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      gpar.pattern, data_graph, [&supp_q, &gpar](auto &match_state) {
        const auto &x_match_ptr = match_state.find(gpar.x_node_ptr())->second;
        const auto &y_match_ptr = match_state.find(gpar.y_node_ptr())->second;
        if (supp_q.count(x_match_ptr)) {
          return true;
        }
        supp_q.insert(x_match_ptr);
        return true;
      });
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      gpar.pattern, data_graph, [&supp_r, &gpar](auto &match_state) {
        const auto &x_match_ptr = match_state.find(gpar.x_node_ptr())->second;
        const auto &y_match_ptr = match_state.find(gpar.y_node_ptr())->second;
        if (supp_r.count(x_match_ptr)) {
          return true;
        }
        auto it = x_match_ptr->OutEdgeCBegin(gpar.q_edge_label(), y_match_ptr);
        if (!it.IsDone()) {
          supp_r.insert(x_match_ptr);
        }
        return true;
      });
  return (static_cast<double>(supp_r.size())) /
         (static_cast<double>(supp_q.size()));
  */
}
template <class GPAR, class DataGraph>
inline double F(GPAR &query, GPAR &target, const DataGraph &data_graph,
                size_t k) {
  // this parameter is in experiment in paper
  double lamda = 0.5;
  double ret = (1 - lamda) / (k - 1) *
                   (conf(query, data_graph) + conf(target, data_graph)) +
               2 * lamda / (k - 1) * diff(query, target, data_graph);
  return ret;
}
template <class GPAR>
inline bool CheckIsIsoOrigin(const GPAR &query, const GPAR &target) {
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using VertexIDType = typename GPAR::VertexIDType;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  if (query.pattern.CountVertex() != target.pattern.CountVertex()) return false;
  if (query.pattern.CountEdge() != target.pattern.CountEdge()) return false;
  MatchResultList match_result;
  VertexIDType query_x_id = query.x_node_ptr()->id();
  VertexIDType target_x_id = query.x_node_ptr()->id();
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query.pattern, target.pattern, query_x_id, target_x_id,
      GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
      GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1, match_result);
  if (match_result.size() < 1) return false;
  match_result.clear();
  MatchResultList().swap(match_result);
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      target.pattern, query.pattern, query_x_id, target_x_id,
      GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
      GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1, match_result);
  if (match_result.size() < 1) return false;
  return true;
}
template <class GPAR, class TopKContainerType>
inline bool GPARInTopKContainer(const GPAR &gpar,
                                const TopKContainerType &top_k_container) {
  for (const auto &it : top_k_container) {
    if (gpar.id() == it.first.id() || gpar.id() == it.second.id()) return true;
  }
  return false;
}
template <class TopKContainerType, class GPARList, class GPAR, class DataGraph>
inline void IncDiv(TopKContainerType &top_k_container, size_t k, GPAR &gpar,
                   GPARList &total_gpar, const DataGraph &data_graph) {
  size_t up_limit = k % 2 ? k / 2 + 1 : k / 2;
  if (GPARInTopKContainer(gpar, top_k_container)) {
    return;
  }
  double max_F = 0;
  std::pair<GPAR, GPAR> add_item;
  auto t_begin = clock();
  for (auto &another_gpar : total_gpar) {
    if (top_k_container.size() < up_limit &&
        another_gpar.pattern.CountEdge() != gpar.pattern.CountEdge()) {
      continue;
    }
    if (gpar.id() == another_gpar.id()) {
      continue;
    }
    if (GPARInTopKContainer(another_gpar, top_k_container)) {
      continue;
    }
    // TransposePattern(gpar.pattern);
    // TransposePattern(another_gpar.pattern);
    // std::cout << std::endl;
    auto t_begin = clock();
    double f = F(gpar, another_gpar, data_graph, k);
    auto t_end = clock();
    // std::cout << "cal f time is " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
    //          << std::endl;
    if (max_F < f) {
      add_item = std::make_pair(gpar, another_gpar);
      max_F = f;
    }
  }
  auto t_end = clock();
  // std::cout << "cal time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
  //          << std::endl;
  t_begin = clock();
  if (top_k_container.size() < up_limit) {
    top_k_container.emplace_back(std::move(add_item));
  } else {
    double min_F = std::numeric_limits<double>::max();
    size_t min_pos = -1;
    for (size_t i = 0; i < up_limit; i++) {
      double f =
          F(top_k_container[i].first, top_k_container[i].second, data_graph, k);
      if (f < min_F) {
        min_pos = i;
        min_F = f;
      }
    }
    top_k_container[min_pos] = add_item;
  }
  t_end = clock();
  // std::cout << "insert time is " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
  //          << std::endl;
}
template <class GPAR, class DataGraph>
inline bool IsExtendable(const GPAR &gpar, const DataGraph &data_graph) {
  double gpar_conf = conf(gpar, data_graph);
  constexpr double eps = 1e-7;
  if (fabs(gpar_conf - 1) == eps || fabs(gpar_conf) == eps) return false;
  return true;
}
template <class TopKContainerType>
inline void OutputDiscoverResult(TopKContainerType &top_k_container,
                                 const char *output_dir) {
  int pattern_id = 0;
  for (const auto &it : top_k_container) {
    pattern_id++;
    WriteGPAR(it.first, pattern_id, output_dir);
    pattern_id++;
    WriteGPAR(it.second, pattern_id, output_dir);
  }
}
}  // namespace gmine_new

#endif