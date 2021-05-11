#ifndef _GPAREXPAND_H
#define _GPAREXPAND_H
#include "gpar_config.h"
#include "gpar_new.h"
#include "gundam/algorithm/vf2.h"
namespace gmine_new {
int not_match_num = 0;
int total_num = 0;
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class ExpandGPARList, class GPARList,
          class ManualCheck>
inline void GPARExpand(const GPAR &root, const VertexLabelSet &vertex_label_set,
                       const EdgeLabelSet &edge_label_set,
                       const EdgeTypeSet &edge_type_set,
                       const GPARList &no_match_gpar,
                       const GPARList &less_supp_r_gpar,
                       const typename GPAR::EdgeIDType new_edge_id,
                       ExpandGPARList &expand_gpars, ManualCheck manual_check) {
  AddNewEdgeInPattern(root, edge_label_set, edge_type_set, no_match_gpar,
                      less_supp_r_gpar, new_edge_id, expand_gpars,
                      manual_check);
  AddNewEdgeOutPattern(root, vertex_label_set, edge_label_set, edge_type_set,
                       no_match_gpar, less_supp_r_gpar, new_edge_id,
                       expand_gpars, manual_check);
}
template <class GPAR, class GPARList>
inline bool CheckIsLessSuppR(const GPAR &gpar, const GPARList &gpar_list) {
  using VertexPtr = typename GPAR::VertexPtr;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgePtr = typename GPAR::EdgePtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using VertexIDType = typename GPAR::VertexIDType;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  for (const auto &no_match_gpar : gpar_list) {
    MatchResultList match_result;
    GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
        no_match_gpar.const_pattern(), gpar.const_pattern(),
        no_match_gpar.x_node_ptr()->id(), gpar.x_node_ptr()->id(),
        GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
        GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1,
        match_result);
    if (match_result.size() >= 1) {
      return true;
    }
  }
  return false;
}
template <class GPAR, class GPARList>
inline bool CheckHasMatch(const GPAR &gpar, const GPARList &gpar_list) {
  for (const auto &no_match_gpar : gpar_list) {
    auto t_begin = clock();
    if (GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
            no_match_gpar.pattern, gpar.pattern, 1) >= 1) {
      return true;
    }
  }
  return false;
}
static omp_lock_t cal_lock;
template <class GPAR, class GPARList, class ManualCheck>
inline bool SatiSfyRules(const GPAR &gpar, const GPARList &no_match_gpar,
                         const GPARList &less_supp_r_gpar,
                         ManualCheck manual_check) {
  if constexpr (using_manual_check)
    if (!manual_check(gpar)) return false;
  omp_set_lock(&cal_lock);
  total_num++;
  omp_unset_lock(&cal_lock);
  auto t_begin = clock();
  if (using_prune && CheckHasMatch(gpar, no_match_gpar)) {
    omp_set_lock(&cal_lock);
    not_match_num++;
    omp_unset_lock(&cal_lock);
    return false;
  }
  if (using_prune && CheckIsLessSuppR(gpar, less_supp_r_gpar)) {
    omp_set_lock(&cal_lock);
    not_match_num++;
    omp_unset_lock(&cal_lock);
    return false;
  }
  auto t_end = clock();
  // std::cout << "time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
  //           << std::endl;
  return true;
}

template <class GPAR, class EdgeLabelSet, class EdgeTypeSet,
          class ExpandGPARList, class GPARList, class ManualCheck>
inline void AddNewEdgeInPattern(const GPAR &root,
                                const EdgeLabelSet &edge_label_set,
                                const EdgeTypeSet &edge_type_set,
                                const GPARList &no_match_gpar,
                                const GPARList &less_supp_r_gpar,
                                const typename GPAR::EdgeIDType new_edge_id,
                                ExpandGPARList &expand_gpars,
                                ManualCheck manual_check) {
  typename GPAR::VertexSizeType pattern_size = root.pattern.CountVertex();
  typename GPAR::VertexIDType x_node_id = root.x_node_ptr()->id();
  typename GPAR::VertexIDType y_node_id = root.y_node_ptr()->id();
  typename GPAR::EdgeLabelType q_edge_label = root.q_edge_label();
  for (auto src_node_iter = root.pattern.VertexBegin(); !src_node_iter.IsDone();
       src_node_iter++) {
    for (auto dst_node_iter = root.pattern.VertexBegin();
         !dst_node_iter.IsDone(); dst_node_iter++) {
      // not add self loop
      if (src_node_iter->id() == dst_node_iter->id()) continue;
      for (const auto &possible_edge_label : edge_label_set) {
        // not add q(x,y)
        if (src_node_iter->id() == x_node_id &&
            dst_node_iter->id() == y_node_id &&
            possible_edge_label == q_edge_label)
          continue;
        // not add edge which does not exist in data graph
        if (!edge_type_set.count(typename EdgeTypeSet::value_type(
                src_node_iter->label(), dst_node_iter->label(),
                possible_edge_label))) {
          continue;
        }
        GPAR expand_gpar(root);
        expand_gpar.pattern.AddEdge(src_node_iter->id(), dst_node_iter->id(),
                                    possible_edge_label, new_edge_id);
        if (!SatiSfyRules(expand_gpar, no_match_gpar, less_supp_r_gpar,
                          manual_check))
          continue;
        expand_gpars.emplace_back(std::move(expand_gpar));
      }
    }
  }
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class ExpandGPARList, class GPARList,
          class ManualCheck>
inline void AddNewEdgeOutPattern(
    const GPAR &root, const VertexLabelSet &vertex_label_set,
    const EdgeLabelSet &edge_label_set, const EdgeTypeSet &edge_type_set,
    const GPARList &no_match_gpar, const GPARList &less_supp_r_gpar,
    const typename GPAR::EdgeIDType new_edge_id, ExpandGPARList &expand_gpars,
    ManualCheck manual_check) {
  typename GPAR::VertexSizeType pattern_size = root.pattern.CountVertex();
  typename GPAR::VertexIDType x_node_id = root.x_node_ptr()->id();
  typename GPAR::VertexIDType y_node_id = root.y_node_ptr()->id();
  typename GPAR::EdgeLabelType q_edge_label = root.q_edge_label();
  typename GPAR::VertexIDType new_vertex_id =
      static_cast<typename GPAR::VertexIDType>(pattern_size + 1);

  for (auto node_iter = root.pattern.VertexBegin(); !node_iter.IsDone();
       node_iter++) {
    if (node_iter->label() == root.y_node_ptr()->label()) continue;
    for (const auto &possible_vertex_label : vertex_label_set) {
      for (const auto &possible_edge_label : edge_label_set) {
        // not add edge which does not exist in data graph
        if (edge_type_set.count(typename EdgeTypeSet::value_type(
                node_iter->label(), possible_vertex_label,
                possible_edge_label))) {
          GPAR expand_gpar(root);
          expand_gpar.pattern.AddVertex(new_vertex_id, possible_vertex_label);
          expand_gpar.pattern.AddEdge(node_iter->id(), new_vertex_id,
                                      possible_edge_label, new_edge_id);
          if (SatiSfyRules(expand_gpar, no_match_gpar, less_supp_r_gpar,
                           manual_check)) {
            expand_gpars.emplace_back(std::move(expand_gpar));
          }
        }
        if (edge_type_set.count(typename EdgeTypeSet::value_type(
                possible_vertex_label, node_iter->label(),
                possible_edge_label))) {
          GPAR expand_gpar(root);
          expand_gpar.pattern.AddVertex(new_vertex_id, possible_vertex_label);
          expand_gpar.pattern.AddEdge(new_vertex_id, node_iter->id(),
                                      possible_edge_label, new_edge_id);
          if (SatiSfyRules(expand_gpar, no_match_gpar, less_supp_r_gpar,
                           manual_check)) {
            expand_gpars.push_back(expand_gpar);
          }
        }
      }
    }
  }
}
}  // namespace gmine_new

#endif