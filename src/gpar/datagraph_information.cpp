#include "gpar.h"
ReturnType gmine_new::DataGraphInformation::BuildInformation() {
  for (auto it = target_graph_.VertexBegin(); !it.IsDone(); ++it) {
    VertexLabelType u_label = (it)->label();
    all_node_label_.insert(u_label);
  }
  for (auto it = target_graph_.VertexBegin(); !it.IsDone(); ++it) {
    VertexIDType u = it->id();
    for (auto it1 = target_graph_.FindVertex(u)->OutEdgeBegin(); !it1.IsDone();
         it1++) {
      VertexIDType from_id = it1->src_ptr()->id();
      VertexIDType to_id = it1->dst_ptr()->id();
      EdgeLabelType edge_label = it1->label();
      VertexLabelType from_id_label =
          target_graph_.FindVertex(from_id)->label();
      VertexLabelType to_id_label = target_graph_.FindVertex(to_id)->label();
      all_edge_weight_.insert(edge_label);
      if (all_possible_edge_.count(std::make_pair(
              std::make_pair(from_id_label, to_id_label), edge_label))) {
        continue;
      }
      // std::cout << from_id_label << " " << to_id_label << " " << edge_label
      //<< std::endl;
      all_possible_edge_.insert(std::make_pair(
          std::make_pair(from_id_label, to_id_label), edge_label));
    }
  }
  return E_OK;
}