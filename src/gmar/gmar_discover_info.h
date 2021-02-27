#ifndef _GMARDISCOVERINFO_H
#define _GMARDISCOVERINFO_H
#include <cstdint>
#include <string>
namespace gmine_new {
template <class Pattern, class DataGraph>
struct GPARDiscoverInfo {
  using DataVertexLabelType = typename DataGraph::VertexType::LabelType;
  using DataEdgeLabelType = typename DataGraph::EdgeType::LabelType;
  std::string v_file, e_file, output_dir;
  DataVertexLabelType x_node_label, y_node_label;
  DataEdgeLabelType q_edge_label;
  int32_t supp_limit, max_edge, top_k;
};
template <class Pattern, class DataGraph>
struct GMARDiscoverInfo : public GPARDiscoverInfo<Pattern, DataGraph> {
  int32_t max_iter_num;
};
}  // namespace gmine_new

#endif