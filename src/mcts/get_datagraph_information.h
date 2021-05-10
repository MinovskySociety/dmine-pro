#ifndef _GETDATAGRAPHINFORMATION_H
#define _GETDATAGRAPHINFORMATION_H
#include <iostream>
#include <set>

#include "gundam/type_getter/vertex_handle.h"
namespace gmine_new {
// 不会出现在GPAR里的边的label
std::set<int> erase_edge_label;
// 不会出现在GPAR里的点的label
std::set<int> erase_vertex_label;
template <class DataGraph, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet>
inline void GetDataGraphInformation(const DataGraph &data_graph,
                                    VertexLabelSet &vertex_label_set,
                                    EdgeLabelSet &edge_label_set,
                                    EdgeTypeSet &edge_type_set) {
  using VertexLabelType = typename DataGraph::VertexType::LabelType;
  using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
  using VertexConstPtr = typename GUNDAM::VertexHandle<DataGraph>::type;
  using EdgeTypeSetElementType = typename EdgeTypeSet::value_type;
  for (auto node_iter = data_graph.VertexBegin(); !node_iter.IsDone();
       node_iter++) {
    VertexLabelType src_node_label = node_iter->label();
    if (erase_vertex_label.count(src_node_label)) continue;
    vertex_label_set.insert(node_iter->label());
    VertexConstPtr node_ptr = node_iter;
    for (auto edge_iter = node_ptr->OutEdgeBegin(); !edge_iter.IsDone();
         edge_iter++) {
      // filter data(no web):
      if (erase_edge_label.count(edge_iter->label())) continue;
      EdgeLabelType edge_label = edge_iter->label();
      edge_label_set.insert(edge_label);
      edge_type_set.insert(EdgeTypeSetElementType(
          src_node_label, edge_iter->const_dst_handle()->label(), edge_label));
    }
  }
  std::cout << "node label set size = " << vertex_label_set.size() << std::endl;
  std::cout << "edge label set size = " << edge_label_set.size() << std::endl;
  std::cout << "edge type set size = " << edge_type_set.size() << std::endl;
  return;
}
}  // namespace gmine_new

#endif