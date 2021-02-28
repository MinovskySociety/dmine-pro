#ifndef _GPARMATCH_H
#define _GPARMATCH_H
#include <omp.h>

#include <limits>
#include <set>
#include <vector>

#include "gpar_config.h"
#include "gpar_new.h"
#include "gundam/algorithm/dp_iso.h"
#include "gundam/algorithm/vf2.h"
#include "gundam/algorithm/vf2_boost.h"
namespace gmine_new {
template <class GPAR, class DataGraph>
inline void GPARMatch(GPAR &gpar, const DataGraph &data_graph) {
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using VertexIDType = typename GPAR::VertexIDType;
  using QueryVertexPtr = typename GPAR::VertexConstPtr;
  using TargetVertexPtr = typename DataGraph::VertexConstPtr;
  using SuppType = typename GPAR::SuppType;
  VertexConstPtr x_node_ptr = gpar.x_node_ptr();
  VertexIDType x_node_id = x_node_ptr->id();
  SuppType possible_supp_q = gpar.supp_Q();
  QueryVertexPtr query_vertex_ptr = gpar.x_node_ptr();
  SuppType result_list;
  GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      gpar.pattern, data_graph, query_vertex_ptr, possible_supp_q, result_list,
      40.0);
  gpar.supp_Q() = result_list;
  std::sort(gpar.supp_Q().begin(), gpar.supp_Q().end());
}
template <class GPAR, class DataGraph>
inline int CalSuppR(GPAR &gpar, const DataGraph &data_graph) {
  if constexpr (!y_label_count_1) {
    using QueryVertexPtr = typename GPAR::VertexConstPtr;
    using VertexLabelType = typename GPAR::VertexLabelType;
    using SuppType = typename GPAR::SuppType;
    SuppType &supp_r_list = gpar.supp_R();
    SuppType &supp_q_list = gpar.supp_Q();
    if (supp_r_list.empty()) {
      supp_r_list = supp_q_list;
    } else {
      SuppType res;
      std::set_intersection(supp_r_list.begin(), supp_r_list.end(),
                            supp_q_list.begin(), supp_q_list.end(),
                            inserter(res, res.begin()));
      supp_r_list = res;
    }
    size_t max_edge = gpar.pattern.CountEdge();
    gpar.pattern.AddEdge(gpar.x_node_ptr()->id(), gpar.y_node_ptr()->id(),
                         gpar.q_edge_label(), ++max_edge);
    SuppType supp_r;
    QueryVertexPtr query_vertex_ptr = gpar.x_node_ptr();
    GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
        gpar.pattern, data_graph, query_vertex_ptr, supp_r_list, supp_r);
    gpar.pattern.EraseEdge(max_edge);
    supp_r_list = supp_r;
    std::sort(gpar.supp_R().begin(), gpar.supp_R().end());
    return supp_r.size();
  } else {
    using VertexLabelType = typename GPAR::VertexLabelType;
    using SuppType = typename GPAR::SuppType;
    SuppType &supp_list = gpar.supp_Q();
    SuppType &supp_r_list = gpar.supp_R();

    if (supp_r_list.empty()) {
      int32_t supp_r = 0;
      VertexLabelType y_node_label = gpar.y_node_ptr()->label();
      for (const auto &single_ptr : supp_list) {
        for (auto edge_it = single_ptr->OutEdgeCBegin(gpar.q_edge_label());
             !edge_it.IsDone(); edge_it++) {
          if (edge_it->const_dst_ptr()->label() == y_node_label) {
            supp_r++;
            supp_r_list.push_back(single_ptr);
            break;
          }
        }
      }
      std::sort(gpar.supp_R().begin(), gpar.supp_R().end());
      return supp_r;
    } else {
      SuppType intersection_result;
      std::set_intersection(
          supp_list.begin(), supp_list.end(), supp_r_list.begin(),
          supp_r_list.end(),
          inserter(intersection_result, intersection_result.begin()));
      supp_r_list = std::move(intersection_result);
      std::sort(gpar.supp_R().begin(), gpar.supp_R().end());
      return supp_r_list.size();
    }
  }
}
}  // namespace gmine_new

#endif