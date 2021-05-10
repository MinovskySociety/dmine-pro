#ifndef _SERIALIZE_SERIALIZE_H
#define _SERIALIZE_SERIALIZE_H

#include <string>

#include "gundam/data_type/datatype.h"
#include "gundam/type_getter/edge_handle.h"
#include "gundam/type_getter/vertex_handle.h"
namespace GUNDAM {

template <typename GraphType>
std::string& operator<<(std::string& out_string, const GraphType& graph) {
  out_string = std::move(out_string) + " <Graph";

  out_string = std::move(out_string) + " vertex";
  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone(); vertex_it++) {
    out_string = std::move(out_string) + " " + ToString(vertex_it->id()) + " " +
                 ToString(vertex_it->label());
    if constexpr (GraphType::vertex_has_attribute) {
      // AttributeBegin method can be called
      auto attr_it = vertex_it->AttributeBegin();
      if (attr_it.IsDone()) {
        continue;
      }
      // has attribtue
      out_string = std::move(out_string) + " <attr";
      for (; !attr_it.IsDone(); attr_it++) {
        out_string = std::move(out_string) + " " + ToString(attr_it->key()) +
                     " " + ToString(attr_it->value_type()) + " " +
                     ToString(attr_it->value_str());
      }
      out_string = std::move(out_string) + " >";
    }
  }
  out_string = std::move(out_string) + " edge";
  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone(); vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      out_string = std::move(out_string) + " " + ToString(edge_it->src_id()) +
                   " " + ToString(edge_it->dst_id()) + " " +
                   ToString(edge_it->label()) + " " + ToString(edge_it->id());
      if constexpr (GraphType::edge_has_attribute) {
        // AttributeBegin method can be called
        auto attr_it = edge_it->AttributeBegin();
        if (attr_it.IsDone()) {
          continue;
        }
        // has attribtue
        out_string = std::move(out_string) + " <attr";
        for (; !attr_it.IsDone(); attr_it++) {
          out_string = std::move(out_string) + " " + ToString(attr_it->key()) +
                       " " + ToString(attr_it->value_type()) + " " +
                       ToString(attr_it->value_str());
        }
        out_string = std::move(out_string) + " >";
      }
    }
  }
  out_string = std::move(out_string) + " >";
  return out_string;
}

template <typename GraphType>
std::string& operator>>(std::string& in_string, GraphType& graph) {
  using VertexPtr = typename VertexHandle<GraphType>::type;
  using EdgePtr = typename EdgeHandle<GraphType>::type;

  using VertexIDType = typename GraphType::VertexType::IDType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;

  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;

  std::stringstream ss;
  ss << in_string;

  std::string str;

  ss >> str;
  assert(str == "<Graph");

  ss >> str;
  assert(str == "vertex");

  graph.Clear();

  VertexPtr last_vertex_ptr;

  while (ss >> str) {
    if (str == "edge") break;
    if constexpr (GraphType::vertex_has_attribute) {
      using VertexAttributeKeyType =
          typename GraphType ::VertexType ::AttributeKeyType;
      if (str == "<attr") {
        // add attribute to last vertex ptr
        assert(last_vertex_ptr);
        while (ss >> str) {
          if (str == ">") {
            break;
          }

          VertexAttributeKeyType vertex_attr_key =
              GUNDAM::StringToDataType<VertexAttributeKeyType>(str);
          ss >> str;

          GUNDAM::BasicDataType vertex_attr_value_type =
              GUNDAM::StringToEnum(str.c_str());
          ss >> str;

          auto ret = last_vertex_ptr->AddAttribute(vertex_attr_key,
                                                   vertex_attr_value_type, str);

          assert(ret.second);
        }
        continue;
      }
    }
    VertexIDType vertex_id = StringToDataType<VertexIDType>(str);
    ss >> str;
    VertexLabelType vertex_label = StringToDataType<VertexLabelType>(str);

    bool succ_added = false;

    std::tie(last_vertex_ptr, succ_added) =
        graph.AddVertex(vertex_id, vertex_label);

    assert(succ_added);
  }

  assert(str == "edge");

  EdgePtr last_edge_ptr;
  // assert(!last_edge_ptr);

  while (ss >> str) {
    if (str == ">") {
      // end symbol
      break;
    }
    if constexpr (GraphType::edge_has_attribute) {
      using EdgeAttributeKeyType =
          typename GraphType ::EdgeType ::AttributeKeyType;
      if (str == "<attr") {
        // add attribute to last edge ptr
        assert(last_edge_ptr);
        while (ss >> str) {
          if (str == ">") {
            break;
          }

          EdgeAttributeKeyType edge_attr_key =
              GUNDAM::StringToDataType<EdgeAttributeKeyType>(str);
          ss >> str;

          GUNDAM::BasicDataType edge_attr_value_type =
              GUNDAM::StringToEnum(str.c_str());
          ss >> str;

          auto ret = last_edge_ptr->AddAttribute(edge_attr_key,
                                                 edge_attr_value_type, str);

          assert(ret.second);
        }
        continue;
      }
    }
    VertexIDType src_id = StringToDataType<VertexIDType>(str);
    ss >> str;
    VertexIDType dst_id = StringToDataType<VertexIDType>(str);
    ss >> str;
    EdgeLabelType edge_label = StringToDataType<EdgeLabelType>(str);
    ss >> str;
    EdgeIDType edge_id = StringToDataType<EdgeIDType>(str);

    bool succ_added = false;

    std::tie(last_edge_ptr, succ_added) =
        graph.AddEdge(src_id, dst_id, edge_label, edge_id);

    assert(succ_added);
  }
  getline(ss, in_string);
  if (ss.fail()) in_string.clear();
  return in_string;
}

};  // namespace GUNDAM

#endif