#ifndef _GUNDAM_COMPONENT_UTIL_H
#define _GUNDAM_COMPONENT_UTIL_H

#include <gundam/data_type/datatype.h>

#include <iostream>
namespace GUNDAM {

template <class GraphType>
void PrintGraph(const GraphType& graph) {
  using VertexType = typename GraphType::VertexType;
  using EdgeType = typename GraphType::EdgeType;
  using VertexIDType = typename VertexType::IDType;
  using VertexLabelType = typename VertexType::LabelType;
  using EdgeLabelType = typename EdgeType::LabelType;
  using VertexPtr = typename GraphType::VertexPtr;
  using VertexConstPtr = typename GraphType::VertexConstPtr;
  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
  using AttributeKeyType = typename VertexType::AttributeKeyType;
  using EdgeAttributeKeyType = typename EdgeType::AttributeKeyType;

  std::cout << "vertex:\n";
  for (auto it = graph.VertexCBegin(); !it.IsDone(); it++) {
    std::cout << it->id() << " " << it->label() << " ";
    VertexConstPtr node_ptr = it;
    if (graph.vertex_has_attribute) {
      for (auto attr_it = node_ptr->AttributeBegin(); !attr_it.IsDone();
           attr_it++) {
        enum BasicDataType value_type =
            it->attribute_value_type(attr_it->key());
        std::cout << attr_it->key() << ":";
        switch (value_type) {
          case BasicDataType::kTypeInt:
            std::cout << attr_it->template const_value<int>();
            break;
          case BasicDataType::kTypeDouble:
            std::cout << attr_it->template const_value<double>();
            break;
          case BasicDataType::kTypeString:
            std::cout << attr_it->template const_value<std::string>();
            break;
          case BasicDataType::kTypeDateTime:
            std::cout << attr_it->template const_value<DateTime>();
            break;
          case BasicDataType::kTypeUnknown:
          default:
            std::cout << "unknown";
            break;
        }
        std::cout << "; ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << "edge:\n";
  for (auto it = graph.VertexCBegin(); !it.IsDone(); it++) {
    for (auto edge_it = graph.FindConstVertex(it->id())->OutEdgeCBegin();
         !edge_it.IsDone(); edge_it++) {
      std::cout << edge_it->const_src_handle()->id() << " "
                << edge_it->const_dst_handle()->id() << " " << edge_it->label()
                << " ";
      if (graph.edge_has_attribute) {
        EdgeConstPtr edge_ptr = edge_it;
        for (auto attr_it = edge_ptr->AttributeBegin(); !attr_it.IsDone();
             attr_it++) {
          enum BasicDataType value_type =
              edge_it->attribute_value_type(attr_it->key());
          std::cout << attr_it->key() << ":";
          switch (value_type) {
            case BasicDataType::kTypeInt:
              std::cout << attr_it->template const_value<int>();
              break;
            case BasicDataType::kTypeDouble:
              std::cout << attr_it->template const_value<double>();
              break;
            case BasicDataType::kTypeString:
              std::cout << attr_it->template const_value<std::string>();
              break;
            case BasicDataType::kTypeDateTime:
              std::cout << attr_it->template const_value<DateTime>();
              break;
            case BasicDataType::kTypeUnknown:
            default:
              std::cout << "unknown ";
              break;
          }
          std::cout << "; ";
        }
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}
}  // namespace GUNDAM

#endif