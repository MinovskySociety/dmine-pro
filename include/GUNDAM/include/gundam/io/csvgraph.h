#ifndef _GUNDAM_IO_CSVGRAPH_H
#define _GUNDAM_IO_CSVGRAPH_H

#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "gundam/component/generator.h"
#include "gundam/data_type/datatype.h"
#include "gundam/io/rapidcsv.h"
#include "gundam/type_getter/edge_handle.h"
#include "gundam/type_getter/vertex_handle.h"

namespace GUNDAM {

//// universial empty callback function template
// template <class... T>
// inline void EmptyVoidCallback(const T&...){return };
//
// template <class... T>
// inline bool EmptyTrueCallback(const T&...) {
//  return true;
//}
//
// template <class... T>
// inline bool EmptyFalseCallback(const T&...) {
//  return false;
//}

//// Parse col
//// before: vertex_id:int
//// after:  col_name - vertex_id;  value_type - int
// inline void ParseCol(std::vector<std::string>& before_parse_col_name,
//                     std::vector<std::string>& after_parse_col_name,
//                     std::vector<std::string>& after_parse_value_type) {
//  for (const auto& str : before_parse_col_name) {
//    std::string col_name, value_type;
//    bool nameflag = false;
//    for (const auto& c : str) {
//      if (c == ':') {
//        nameflag = true;
//        continue;
//      }
//      if (!nameflag) {
//        col_name.push_back(c);
//      } else {
//        value_type.push_back(c);
//      }
//    }
//    after_parse_col_name.emplace_back(std::move(col_name));
//    after_parse_value_type.emplace_back(std::move(value_type));
//  }
//}
//
//// check Attribute Key type is correct
//// when no attribute after_parse_col_name.size()==begin_pos
// template <typename AttributeKeyType>
// bool CheckAttributeKeyIsCorrect(std::vector<std::string>&
// after_parse_col_name,
//                                size_t begin_pos) {
//  // check col name can change to attrkeytype
//  for (size_t j = begin_pos; j < after_parse_col_name.size(); j++) {
//    // std::cout << after_parse_col_name[j] << std::endl;
//    std::string str = after_parse_col_name[j];
//    std::stringstream ss(str);
//    AttributeKeyType check_flag;
//    ss >> check_flag;
//    std::stringstream s1;
//    std::string str1;
//    s1 << check_flag;
//    s1 >> str1;
//    if (str1 != str) {
//      return false;
//    }
//  }
//  return true;
//}
template <typename AttributeKeyType>
bool GetAttributeInfo(
    const std::vector<std::string>& col_name,
    std::vector<std::pair<AttributeKeyType, enum BasicDataType>>& attr_info) {
  attr_info.clear();

  for (const auto& str : col_name) {
    std::string key_str, type_str;

    bool flag = false;
    for (const auto& c : str) {
      if (c == ':') {
        flag = true;
      } else if (!flag) {
        key_str.push_back(c);
      } else {
        type_str.push_back(c);
      }
    }

    std::stringstream s0(key_str);
    AttributeKeyType attr_key;
    s0 >> attr_key;
    std::stringstream s1;
    std::string str1;
    s1 << attr_key;
    s1 >> str1;
    if (str1 != key_str) {
      return false;
    }

    enum BasicDataType value_type = StringToEnum(type_str.c_str());
    if (value_type == BasicDataType::kTypeUnknown) {
      return false;
    }
    attr_info.emplace_back(std::move(attr_key), std::move(value_type));
  }

  return true;
}

template <bool has_attribute, class GraphType, class VertexEdgePtr,
          class AttributeKeyType,
          typename std::enable_if<!has_attribute, bool>::type = false>
inline bool ReadAttribues(
    GraphType& graph, VertexEdgePtr& vertex_edge_ptr,
    rapidcsv::Document& vertex_edge_file,
    const std::vector<std::pair<AttributeKeyType, enum BasicDataType>>&
        attr_info,
    size_t col_begin, size_t row) {
  return true;
}

template <bool has_attribute, class GraphType, class VertexEdgePtr,
          class AttributeKeyType,
          typename std::enable_if<has_attribute, bool>::type = false>
inline bool ReadAttribues(
    GraphType& graph, VertexEdgePtr& vertex_edge_ptr,
    rapidcsv::Document& vertex_edge_file,
    const std::vector<std::pair<AttributeKeyType, enum BasicDataType>>&
        attr_info,
    size_t col_begin, size_t row) {
  for (size_t col = col_begin; col < attr_info.size(); col++) {
    const AttributeKeyType& attr_key = attr_info[col].first;
    const enum BasicDataType& value_type = attr_info[col].second;

    switch (value_type) {
      case BasicDataType::kTypeString: {
        auto cell = vertex_edge_file.GetCellNew<std::string>(col, row);
        if (cell.second || cell.first.empty()) continue;
        vertex_edge_ptr->AddAttribute(attr_key, cell.first);
        break;
      }
      case BasicDataType::kTypeInt: {
        auto cell = vertex_edge_file.GetCellNew<int>(col, row);
        if (cell.second) continue;
        vertex_edge_ptr->AddAttribute(attr_key, cell.first);
        break;
      }
      case BasicDataType::kTypeInt64: {
        auto cell = vertex_edge_file.GetCellNew<int64_t>(col, row);
        if (cell.second) continue;
        vertex_edge_ptr->AddAttribute(attr_key, cell.first);
        break;
      }
      case BasicDataType::kTypeFloat: {
        auto cell = vertex_edge_file.GetCellNew<float>(col, row);
        if (cell.second) continue;
        vertex_edge_ptr->AddAttribute(attr_key, cell.first);
        break;
      }
      case BasicDataType::kTypeDouble: {
        auto cell = vertex_edge_file.GetCellNew<double>(col, row);
        if (cell.second) continue;
        vertex_edge_ptr->AddAttribute(attr_key, cell.first);
        break;
      }
      case BasicDataType::kTypeDateTime: {
        auto cell = vertex_edge_file.GetCellNew<std::string>(col, row);
        if (cell.second || cell.first.empty()) continue;
        DateTime date_time(cell.first.c_str());
        vertex_edge_ptr->AddAttribute(attr_key, date_time);
        break;
      }
      case BasicDataType::kTypeUnknown:
      default:
        return false;
    }
  }
  return true;
}

// template <class GraphType,
//          typename std::enable_if<!GraphType::vertex_has_attribute,
//                                  bool>::type = false>
// bool LoadVertexAttribue(GraphType& graph,
//                        typename GUNDAM::VertexHandle<GraphType>::type&
//                        vertex_handle, rapidcsv::Document& vertex_file,
//                        std::vector<std::string>& after_parse_col_name,
//                        std::vector<std::string>& after_parse_value_type,
//                        int row_pos) {
//  return false;
//}
//// load Vertex Attribute
// template <class GraphType,
//          typename std::enable_if<GraphType::vertex_has_attribute, bool>::type
//          =
//              false>
// bool LoadVertexAttribue(GraphType& graph,
//                        typename GUNDAM::VertexHandle<GraphType>::type&
//                        vertex_handle, rapidcsv::Document& vertex_file,
//                        std::vector<std::string>& after_parse_col_name,
//                        std::vector<std::string>& after_parse_value_type,
//                        int row_pos) {
//  using VertexType = typename GraphType::VertexType;
//  using VertexAttributeKeyType = typename VertexType::AttributeKeyType;
//
//  int col_num = static_cast<int>(after_parse_col_name.size());
//  for (int i = 2; i < col_num; i++) {
//    // key is i
//    // std::cout << "attribute" << std::endl;
//    std::stringstream ss(after_parse_col_name[i]);
//    VertexAttributeKeyType attr_key;
//    ss >> attr_key;
//    switch (StringToEnum(after_parse_value_type[i].c_str())) {
//      case BasicDataType::kTypeString: {
//        std::pair<std::string, bool> cell =
//            vertex_file.GetCellNew<std::string>(i, row_pos);
//        if (cell.second || cell.first.empty()) continue;
//        vertex_handle->AddAttribute(attr_key, cell.first);
//        break;
//      }
//      case BasicDataType::kTypeInt: {
//        std::pair<int, bool> cell =
//            vertex_file.GetCellNew<int>(i, row_pos);
//        if (cell.second) continue;
//        vertex_handle->AddAttribute(attr_key, cell.first);
//        break;
//      }
//      case BasicDataType::kTypeDouble: {
//        std::pair<double, bool> cell =
//            vertex_file.GetCellNew<double>(i, row_pos);
//        if (cell.second) continue;
//        vertex_handle->AddAttribute(attr_key, cell.first);
//        break;
//      }
//      case BasicDataType::kTypeDateTime: {
//        std::pair<std::string, bool> cell =
//            vertex_file.GetCellNew<std::string>(i, row_pos);
//        if (cell.second || cell.first.empty()) continue;
//        DateTime date_time(cell.first.c_str());
//        vertex_handle->AddAttribute(attr_key, date_time);
//        break;
//      }
//      case BasicDataType::kTypeUnknown:
//      default:
//        return false;
//    }
//  }
//  return true;
//}

// inline bool IsFileExisted(const std::string file) {
//  std::ifstream check(file);
//  if (!check) return false;
//  check.close();
//  return true;
//}

// read vertex set, initialize graph_set and graph_id_to_graph_set_idx
template <bool read_attr = true, typename GraphIDType, typename GraphType,
          typename ReadVertexCallback>
int ReadCSVVertexSetFileWithCallback(
    const std::string& v_set_file, std::vector<GraphType>& graph_set,
    std::map<GraphIDType, size_t>& graph_id_to_graph_set_idx,
    ReadVertexCallback callback) {
  std::cout << "read vertex set" << std::endl;
  graph_id_to_graph_set_idx.clear();
  graph_set.clear();
  // read vertex set file(csv)
  // file format: (vertex_id,label_id,graph_id,...)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;
  using VertexHandleType = typename GUNDAM::VertexHandle<GraphType>::type;

  try {
    std::cout << v_set_file << std::endl;

    rapidcsv::Document vertex_set_file(v_set_file,
                                       rapidcsv::LabelParams(0, -1));

    // phase column names
    std::vector<std::string> col_name = vertex_set_file.GetColumnNames();
    std::vector<std::pair<VertexAttributeKeyType, enum BasicDataType>>
        attr_info;

    // erase graph_id column,because graph_id is not attribute
    for (auto it = col_name.begin(); it != col_name.end();) {
      if ((*it).find("graph_id") != (*it).npos) {
        it = col_name.erase(it);
        continue;
      }
      it++;
    }
    if (!GetAttributeInfo(col_name, attr_info)) {
      std::cout << "Attribute key type is not correct!" << std::endl;
      return -1;
    }

    size_t col_num = attr_info.size();
    // check col num >= 2
    if (col_num < 2 || attr_info[0].first != "vertex_id" ||
        attr_info[1].first != "label_id") {
      std::cout << "Vertex file does not have vertex_id or label_id!"
                << std::endl;
      return -1;
    }
    // check attributes
    if constexpr (read_attr && !GraphType::vertex_has_attribute) {
      if (col_num >= 3) {
        std::cout << "vertex file has attribute but graph does not support!"
                  << std::endl;
        return -1;
      }
    }

    const std::vector<VertexIDType> vertex_id =
        vertex_set_file.GetColumn<VertexIDType>(0);
    const std::vector<VertexLabelType> label_id =
        vertex_set_file.GetColumn<VertexLabelType>(1);
    const std::vector<GraphIDType> graph_id =
        vertex_set_file.GetColumn<GraphIDType>(2);

    int count_success = 0;
    int count_fail = 0;
    size_t sz = vertex_id.size();
    for (size_t row = 0; row < sz; row++) {
      GraphIDType id = graph_id[row];
      graph_id_to_graph_set_idx.emplace(id, graph_id_to_graph_set_idx.size());
    }
    graph_set.resize(graph_id_to_graph_set_idx.size());
    for (size_t row = 0; row < sz; row++) {
      assert(row < graph_id.size());
      GraphIDType id = graph_id[row];
      auto graph_id_to_graph_set_idx_it = graph_id_to_graph_set_idx.find(id);
      assert(graph_id_to_graph_set_idx_it != graph_id_to_graph_set_idx.end());
      size_t graph_idx = graph_id_to_graph_set_idx_it->second;
      assert(graph_idx >= 0 && graph_idx < graph_set.size());
      auto& graph_ref = graph_set[graph_idx];
      auto [vertex_handle, add_ret] =
          graph_ref.AddVertex(vertex_id[row], label_id[row]);

      if (add_ret) {
        // vertex added successfully
        if constexpr (read_attr) {
          add_ret = ReadAttribues<GraphType::vertex_has_attribute>(
              graph_ref, vertex_handle, vertex_set_file, attr_info, 2, row);
        }
      }

      if (!add_ret) {
        // add vertex failed, what ever in AddVertex or ReadAttribues
        count_fail++;
        continue;
      }

      if constexpr (!std::is_null_pointer_v<ReadVertexCallback>) {
        if (!callback(vertex_handle)) return -2;
      }
      count_success++;
    }
    if (count_fail > 0) {
      std::cout << "Failed: " << count_fail << std::endl;
    }
    return count_success;
  } catch (...) {
    return -1;
  }
}

template <bool read_attr = true, class GraphType, class ReadVertexCallback>
int ReadCSVVertexFileWithCallback(const std::string& v_file, GraphType& graph,
                                  ReadVertexCallback callback) {
  // read vertex file(csv)
  // file format: (vertex_id,label_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;
  using VertexHandleType = typename GUNDAM::VertexHandle<GraphType>::type;

  try {
    std::cout << v_file << std::endl;

    rapidcsv::Document vertex_file(v_file, rapidcsv::LabelParams(0, -1));

    // phase column names
    std::vector<std::string> col_name = vertex_file.GetColumnNames();
    std::vector<std::pair<VertexAttributeKeyType, enum BasicDataType>>
        attr_info;
    if (!GetAttributeInfo(col_name, attr_info)) {
      std::cout << "Attribute key type is not correct!" << std::endl;
      return -1;
    }

    size_t col_num = attr_info.size();
    // check col num >= 2
    if (col_num < 2 || attr_info[0].first != "vertex_id" ||
        attr_info[1].first != "label_id") {
      std::cout << "Vertex file does not have vertex_id or label_id!"
                << std::endl;
      return -1;
    }
    // check attributes
    if constexpr (read_attr && !GraphType::vertex_has_attribute) {
      if (col_num >= 3) {
        std::cout << "vertex file has attribute but graph does not support!"
                  << std::endl;
        return -1;
      }
    }

    // if (attr_info[0].second != TypeToEnum<VertexIDType>() ||
    //    attr_info[1].second != TypeToEnum<VertexLabelType>()) {
    //  std::cout << "vertex id or label type is wrong!" << std::endl;
    //  return -1;
    //}

    const std::vector<VertexIDType> vertex_id =
        vertex_file.GetColumn<VertexIDType>(0);
    const std::vector<VertexLabelType> label_id =
        vertex_file.GetColumn<VertexLabelType>(1);

    int count_success = 0;
    int count_fail = 0;
    size_t sz = vertex_id.size();
    for (size_t row = 0; row < sz; row++) {
      auto [vertex_handle, r] = graph.AddVertex(vertex_id[row], label_id[row]);
      if (r) {
        if constexpr (read_attr) {
          r = ReadAttribues<GraphType::vertex_has_attribute>(
              graph, vertex_handle, vertex_file, attr_info, 2, row);
        }
      }

      if (r) {
        if constexpr (!std::is_null_pointer_v<ReadVertexCallback>) {
          if (!callback(vertex_handle)) return -2;
        }
        ++count_success;
      } else {
        ++count_fail;
      }
    }
    if (count_fail > 0) {
      std::cout << "Failed: " << count_fail << std::endl;
    }
    return count_success;
  } catch (...) {
    return -1;
  }
}

// template <
//    class GraphType,
//    typename std::enable_if<!GraphType::edge_has_attribute, bool>::type =
//    false>
// bool LoadEdgeAttribue(GraphType& graph, typename GraphType::EdgePtr&
// edge_vertex_ptr,
//                      rapidcsv::Document& edge_vertex_file,
//                      std::vector<std::string>& after_parse_col_name,
//                      std::vector<std::string>& after_parse_value_type,
//                      size_t row_pos) {
//  return false;
//}
//// load edge attribute
// template <
//    class GraphType,
//    typename std::enable_if<GraphType::edge_has_attribute, bool>::type =
//    false>
// bool LoadEdgeAttribue(GraphType& graph, typename GraphType::EdgePtr&
// edge_vertex_ptr,
//                      rapidcsv::Document& edge_vertex_file,
//                      std::vector<std::string>& after_parse_col_name,
//                      std::vector<std::string>& after_parse_value_type,
//                      size_t row_pos) {
//  using EdgeType = typename GraphType::EdgeType;
//  using EdgeAttributeKeyType = typename EdgeType::AttributeKeyType;
//
//  int col_num = static_cast<int>(after_parse_col_name.size());
//  for (int i = 4; i < col_num; i++) {
//    std::stringstream ss(after_parse_col_name[i]);
//    EdgeAttributeKeyType attr_key;
//    ss >> attr_key;
//    switch (StringToEnum(after_parse_value_type[i].c_str())) {
//      case BasicDataType::kTypeString: {
//        auto cell = edge_vertex_file.GetCellNew<std::string>(i, row_pos);
//        if (cell.second || cell.first.empty()) continue;
//        edge_vertex_ptr->AddAttribute(attr_key, cell.first);
//        break;
//      }
//      case BasicDataType::kTypeInt: {
//        auto cell = edge_vertex_file.GetCellNew<int>(i, row_pos);
//        if (cell.second) continue;
//        edge_vertex_ptr->AddAttribute(attr_key, cell.first);
//        break;
//      }
//      case BasicDataType::kTypeDouble: {
//        auto cell = edge_vertex_file.GetCellNew<double>(i, row_pos);
//        if (cell.second) continue;
//        edge_vertex_ptr->AddAttribute(attr_key, cell.first);
//        break;
//      }
//      case BasicDataType::kTypeDateTime: {
//        auto cell = edge_vertex_file.GetCellNew<std::string>(i, row_pos);
//        if (cell.second || cell.first.empty()) continue;
//        DateTime date_time(cell.first.c_str());
//        edge_vertex_ptr->AddAttribute(attr_key, date_time);
//        break;
//      }
//      case BasicDataType::kTypeUnknown:
//      default:
//        return false;
//    }
//  }
//  return true;
//}

template <bool read_attr = true, typename GraphIDType, typename GraphType,
          typename ReadEdgeCallback>
int ReadCSVEdgeSetFileWithCallback(
    const std::string& e_set_file, std::vector<GraphType>& graph_set,
    const std::map<GraphIDType, size_t>& graph_id_to_graph_set_idx,
    ReadEdgeCallback callback) {
  std::cout << "read edge set" << std::endl;
  assert(graph_set.size() == graph_id_to_graph_set_idx.size());
  // read edge set file(csv)
  // file format: (edge_id,source_id,target_id,label_id,graph_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;
  using EdgeHandleType = typename GUNDAM::EdgeHandle<GraphType>::type;

  try {
    std::cout << e_set_file << std::endl;

    rapidcsv::Document edge_set_file(e_set_file, rapidcsv::LabelParams(0, -1));

    // phase column names
    std::vector<std::string> col_name = edge_set_file.GetColumnNames();

    std::vector<std::pair<EdgeAttributeKeyType, enum BasicDataType>> attr_info;

    // erase graph_id column,because graph_id is not attribute
    for (auto it = col_name.begin(); it != col_name.end();) {
      if ((*it).find("graph_id") != (*it).npos) {
        it = col_name.erase(it);
      } else {
        it++;
      }
    }

    if (!GetAttributeInfo(col_name, attr_info)) {
      std::cout << "Attribute key type is not correct!" << std::endl;
      return -1;
    }

    size_t col_num = attr_info.size();
    // check col_num >= 4
    if (col_num < 4 || attr_info[0].first != "edge_id" ||
        attr_info[1].first != "source_id" ||
        attr_info[2].first != "target_id" || attr_info[3].first != "label_id") {
      std::cout << "edge file is not correct!(col num must >=4)" << std::endl;
      return -1;
    }
    if constexpr (read_attr && !GraphType::edge_has_attribute) {
      if (col_num >= 5) {
        std::cout << "Edge file has attribute but graph does not support!"
                  << std::endl;
        return -1;
      }
    }

    const std::vector<EdgeIDType> edge_id =
        edge_set_file.GetColumn<EdgeIDType>(0);
    const std::vector<VertexIDType> source_id =
        edge_set_file.GetColumn<VertexIDType>(1);
    const std::vector<VertexIDType> target_id =
        edge_set_file.GetColumn<VertexIDType>(2);
    const std::vector<EdgeLabelType> label_id =
        edge_set_file.GetColumn<EdgeLabelType>(3);
    const std::vector<GraphIDType> graph_id =
        edge_set_file.GetColumn<GraphIDType>(4);

    int count_success = 0;
    int count_fail = 0;
    size_t sz = edge_id.size();
    for (size_t row = 0; row < sz; row++) {
      GraphIDType id = graph_id[row];
      auto graph_idx_it = graph_id_to_graph_set_idx.find(id);
      if (graph_idx_it == graph_id_to_graph_set_idx.end()) {
        // this graph_id is not contained in graph_id_to_graph_set_idx
        // i.e. does not contained in the v_set_file
        count_fail++;
        continue;
      }

      size_t graph_idx = graph_idx_it->second;
      assert(graph_idx >= 0 && graph_idx < graph_set.size());
      auto& graph_ref = graph_set[graph_idx];
      auto [edge_handle, add_ret] = graph_ref.AddEdge(
          source_id[row], target_id[row], label_id[row], edge_id[row]);
      if (add_ret) {
        if constexpr (read_attr) {
          add_ret = ReadAttribues<GraphType::edge_has_attribute>(
              graph_ref, edge_handle, edge_set_file, attr_info, 4, row);
        }
      }

      if (!add_ret) {
        count_fail++;
        continue;
      }

      if constexpr (!std::is_null_pointer_v<ReadEdgeCallback>) {
        if (!callback(edge_handle)) {
          return -2;
        }
      }
      ++count_success;
    }
    if (count_fail > 0) {
      std::cout << "Failed: " << count_fail << std::endl;
    }
    return count_success;
  } catch (...) {
    return -1;
  }
}

template <bool read_attr = true, class GraphType, class ReadEdgeCallback>
int ReadCSVEdgeFileWithCallback(const std::string& e_file, GraphType& graph,
                                ReadEdgeCallback callback) {
  // read edge file(csv)
  // file format: (edge_id,source_id,target_id,label_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;
  using EdgeHandleType = typename GUNDAM::EdgeHandle<GraphType>::type;

  try {
    std::cout << e_file << std::endl;

    rapidcsv::Document edge_file(e_file, rapidcsv::LabelParams(0, -1));

    // phase column names
    std::vector<std::string> col_name = edge_file.GetColumnNames();
    std::vector<std::pair<EdgeAttributeKeyType, enum BasicDataType>> attr_info;
    if (!GetAttributeInfo(col_name, attr_info)) {
      std::cout << "Attribute key type is not correct!" << std::endl;
      return -1;
    }

    size_t col_num = attr_info.size();
    // check col_num >= 4
    if (col_num < 4 || attr_info[0].first != "edge_id" ||
        attr_info[1].first != "source_id" ||
        attr_info[2].first != "target_id" || attr_info[3].first != "label_id") {
      std::cout << "edge file is not correct!(col num must >=4)" << std::endl;
      return -1;
    }
    if constexpr (read_attr && !GraphType::edge_has_attribute) {
      if (col_num >= 5) {
        std::cout << "Edge file has attribute but graph does not support!"
                  << std::endl;
        return -1;
      }
    }

    const std::vector<EdgeIDType> edge_id = edge_file.GetColumn<EdgeIDType>(0);
    const std::vector<VertexIDType> source_id =
        edge_file.GetColumn<VertexIDType>(1);
    const std::vector<VertexIDType> target_id =
        edge_file.GetColumn<VertexIDType>(2);
    const std::vector<EdgeLabelType> label_id =
        edge_file.GetColumn<EdgeLabelType>(3);

    int count_success = 0;
    int count_fail = 0;
    size_t sz = edge_id.size();
    for (size_t row = 0; row < sz; row++) {
      bool r;
      EdgeHandleType edge_handle;
      std::tie(edge_handle, r) = graph.AddEdge(source_id[row], target_id[row],
                                               label_id[row], edge_id[row]);
      if (r) {
        if constexpr (read_attr) {
          r = ReadAttribues<GraphType::edge_has_attribute>(
              graph, edge_handle, edge_file, attr_info, 4, row);
        }
      }

      if (r) {
        if constexpr (!std::is_null_pointer_v<ReadEdgeCallback>) {
          if (!callback(edge_handle)) return -2;
        }
        ++count_success;
      } else {
        ++count_fail;
      }
    }
    if (count_fail > 0) {
      std::cout << "Failed: " << count_fail << std::endl;
    }
    return count_success;
  } catch (...) {
    return -1;
  }
}

template <class GraphType, class ReadVertexCallback, class ReadEdgeCallback>
int ReadCSVGraphSetWithCallback(std::vector<GraphType>& graph_set,
                                std::vector<std::string>& graph_name_set,
                                const std::vector<std::string>& v_list,
                                const std::vector<std::string>& e_list,
                                ReadVertexCallback rv_callback,
                                ReadEdgeCallback re_callback) {
  graph_set.clear();

  std::map<std::string, size_t> graph_id_to_graph_set_idx;
  int count_v = 0;
  for (const auto& v_file : v_list) {
    int res = ReadCSVVertexSetFileWithCallback(
        v_file, graph_set, graph_id_to_graph_set_idx, rv_callback);
    if (res < 0) return res;
    count_v += res;
  }

  int count_e = 0;
  for (const auto& e_file : e_list) {
    int res = ReadCSVEdgeSetFileWithCallback(
        e_file, graph_set, graph_id_to_graph_set_idx, re_callback);
    if (res < 0) return res;
    count_e += res;
  }

  std::cout << " Vertex: " << count_v << std::endl;
  std::cout << "   Edge: " << count_e << std::endl;

  assert(graph_set.size() == graph_id_to_graph_set_idx.size());
  graph_name_set.resize(graph_set.size());

  for (const auto& graph_id_idx_pair : graph_id_to_graph_set_idx) {
    assert(graph_id_idx_pair.second >= 0 &&
           graph_id_idx_pair.second < graph_name_set.size());
    graph_name_set[graph_id_idx_pair.second] = graph_id_idx_pair.first;
  }

  return count_v + count_e;
}

template <class GraphType, class ReadVertexCallback, class ReadEdgeCallback>
int ReadCSVGraphSetWithCallback(std::vector<GraphType>& graph_set,
                                const std::vector<std::string>& v_list,
                                const std::vector<std::string>& e_list,
                                ReadVertexCallback rv_callback,
                                ReadEdgeCallback re_callback) {
  std::vector<std::string> graph_name_set;
  return ReadCSVGraphSetWithCallback(graph_set, graph_name_set, v_list, e_list,
                                     rv_callback, re_callback);
}

template <class GraphType, class ReadVertexCallback, class ReadEdgeCallback>
int ReadCSVGraphWithCallback(GraphType& graph,
                             const std::vector<std::string>& v_list,
                             const std::vector<std::string>& e_list,
                             ReadVertexCallback rv_callback,
                             ReadEdgeCallback re_callback) {
  graph.Clear();

  int count_v = 0;
  for (const auto& v_file : v_list) {
    int res = ReadCSVVertexFileWithCallback(v_file, graph, rv_callback);
    if (res < 0) return res;
    count_v += res;
  }

  int count_e = 0;
  for (const auto& e_file : e_list) {
    int res = ReadCSVEdgeFileWithCallback(e_file, graph, re_callback);
    if (res < 0) return res;
    count_e += res;
  }

  std::cout << " Vertex: " << count_v << std::endl;
  std::cout << "   Edge: " << count_e << std::endl;

  return count_v + count_e;
}

template <class GraphType>
inline int ReadCSVGraphSet(std::vector<GraphType>& graph_set,
                           const std::vector<std::string>& v_list,
                           const std::vector<std::string>& e_list) {
  return ReadCSVGraphSetWithCallback(graph_set, v_list, e_list, nullptr,
                                     nullptr);
}

template <class GraphType>
inline int ReadCSVGraphSet(std::vector<GraphType>& graph_set,
                           std::vector<std::string>& graph_name_set,
                           const std::vector<std::string>& v_list,
                           const std::vector<std::string>& e_list) {
  return ReadCSVGraphSetWithCallback(graph_set, graph_name_set, v_list, e_list,
                                     nullptr, nullptr);
}

template <class GraphType>
inline int ReadCSVGraph(GraphType& graph,
                        const std::vector<std::string>& v_list,
                        const std::vector<std::string>& e_list) {
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;

  return ReadCSVGraphWithCallback(graph, v_list, e_list, nullptr, nullptr);
}

template <class GraphType>
inline int ReadCSVGraphSet(std::vector<GraphType>& graph_set,
                           const std::string& v_file,
                           const std::string& e_file) {
  std::vector<std::string> v_list, e_list;
  v_list.push_back(v_file);
  e_list.push_back(e_file);
  return ReadCSVGraphSet(graph_set, v_list, e_list);
}

template <class GraphType>
inline int ReadCSVGraphSet(std::vector<GraphType>& graph_set,
                           std::vector<std::string>& graph_name_set,
                           const std::string& v_file,
                           const std::string& e_file) {
  std::vector<std::string> v_list, e_list;
  v_list.push_back(v_file);
  e_list.push_back(e_file);
  return ReadCSVGraphSet(graph_set, graph_name_set, v_list, e_list);
}

template <class GraphType>
inline int ReadCSVGraph(GraphType& graph, const std::string& v_file,
                        const std::string& e_file) {
  std::vector<std::string> v_list, e_list;
  v_list.push_back(v_file);
  e_list.push_back(e_file);
  return ReadCSVGraph(graph, v_list, e_list);
}

// template <class VertexIDType, class VertexLabelType, class VertexIDGenerator>
// inline void ReadVertexFileCallbackUseID(const size_t& row,
//                                        const VertexIDType& vertex_id,
//                                        const VertexLabelType& label_id,
//                                        bool res, VertexIDGenerator& vidgen) {
//  vidgen.UseID(vertex_id);
//}
//
// template <class VertexIDType, class EdgeIDType, class EdgeLabelType,
//          class EdgeIDGenerator>
// inline void ReadEdgeFileCallbackUseID(const size_t& row,
//                                      const EdgeIDType& vertex_id,
//                                      const VertexIDType& source_id,
//                                      const VertexIDType& target_id,
//                                      const EdgeLabelType& label_id, bool res,
//                                      EdgeIDGenerator& eidgen) {
//  eidgen.UseID(vertex_id);
//}

template <class GraphType, class VertexIDGenerator, class EdgeIDGenerator>
inline int ReadCSVGraph(GraphType& graph,
                        const std::vector<std::string>& v_list,
                        const std::vector<std::string>& e_list,
                        VertexIDGenerator& vidgen, EdgeIDGenerator& eidgen) {
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;

  return ReadCSVGraphWithCallback(
      graph, v_list, e_list,
      [&vidgen](auto& vertex) -> bool {
        vidgen.UseID(vertex->id());
        return true;
      },
      [&eidgen](auto& edge) -> bool {
        eidgen.UseID(edge->id());
        return true;
      });
}

template <class GraphType, class VertexIDGenerator, class EdgeIDGenerator>
inline int ReadCSVGraph(GraphType& graph, const std::string& v_file,
                        const std::string& e_file, VertexIDGenerator& vidgen,
                        EdgeIDGenerator& eidgen) {
  std::vector<std::string> v_list, e_list;
  v_list.push_back(v_file);
  e_list.push_back(e_file);
  return ReadCSVGraph(graph, v_list, e_list, vidgen, eidgen);
}

// Write CSV
constexpr char CsvSeparator = ',';
template <typename T>
std::string ProtectedSeparatorVal(T&& val) {
  //':' is used in CSV head,so cannot used in Separator
  assert(CsvSeparator != ':');
  std::stringstream s_stream;
  s_stream << val;
  std::string stream_str = s_stream.str();
  if (stream_str.find(CsvSeparator) != stream_str.npos &&
      !(stream_str[0] == '"' && stream_str.back() == '"')) {
    return (std::string) "\"" + s_stream.str() + (std::string) "\"";
  }
  return s_stream.str();
}
// Write CSV columns
template <typename StreamType>
inline void WriteCSVColumns(StreamType& s, std::vector<std::string>& key_str,
                            std::vector<std::string>& type_str) {
  assert(CsvSeparator != ':');
  for (size_t i = 0; i < key_str.size(); i++) {
    if (i > 0) s << CsvSeparator;
    s << ProtectedSeparatorVal(key_str[i]) << ":"
      << ProtectedSeparatorVal(type_str[i]);
  }
  s << std::endl;
}

// Write CSV line
template <typename StreamType>
inline void WriteCSVLine(StreamType& s, std::vector<std::string>& cols) {
  assert(CsvSeparator != ':');
  for (size_t i = 0; i < cols.size(); i++) {
    if (i > 0) s << CsvSeparator;
    s << ProtectedSeparatorVal(cols[i]);
  }
  s << std::endl;
}

// template <bool write_attr, class GraphType, class WriteVertexCallback,
//          typename std::enable_if<!write_attr, bool>::type = false>
// void GetVertexAttributeInfo(
//    const GraphType& graph, std::vector<std::string>& key_str,
//    std::vector<std::string>& type_str,
//    std::vector<std::pair<typename GraphType::VertexType::AttributeKeyType,
//                          enum BasicDataType>>& attr_info,
//    WriteVertexCallback wv_callback) {
//}
//
// template <bool write_attr, class GraphType, class WriteVertexCallback,
//          typename std::enable_if<write_attr, bool>::type = false>
// void GetVertexAttributeInfo(
//    const GraphType& graph, std::vector<std::string>& key_str,
//    std::vector<std::string>& type_str,
//    std::vector<std::pair<typename GraphType::VertexType::AttributeKeyType,
//                          enum BasicDataType>>& attr_info,
//    WriteVertexCallback wv_callback) {
//  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
//       ++vertex_it) {
//    if (wv_callback(vertex_it->id(), vertex_it->label())) {
//      for (auto attr_it = vertex_it->AttributeBegin(); !attr_it.IsDone();
//           attr_it++) {
//        auto p = std::make_pair(attr_it->key(), attr_it->value_type());
//        if (std::find(attr_info.begin(), attr_info.end(), attr_key) !=
//            attr_info.end()) {
//          std::stringstream ss;
//          ss << attr_key;
//          std::string attr_key_str;
//          ss >> attr_key_str;
//
//          key_str.emplace_back(std::move(attr_key_str));
//          type_str.emplace_back(EnumToString(attr_it->value_type()));
//        }
//      }
//    }
//  }
//}

// template <
//    class GraphType,
//    typename std::enable_if<!GraphType::edge_has_attribute, bool>::type =
//    false>
// void GetEdgeAttributeValueType(
//    const GraphType& graph, std::vector<std::string>& edge_col_name,
//    std::vector<std::string>& edge_col_value_type,
//    std::vector<typename GraphType::EdgeType::AttributeKeyType>&
//        edge_attr_key) {
//  return;
//}
//
// template <
//    class GraphType,
//    typename std::enable_if<GraphType::edge_has_attribute, bool>::type =
//    false>
// void GetEdgeAttributeValueType(
//    const GraphType& graph, std::vector<std::string>& edge_col_name,
//    std::vector<std::string>& edge_col_value_type,
//    std::vector<typename GraphType::EdgeType::AttributeKeyType>&
//        edge_attr_key) {
//  using VertexType = typename GraphType::VertexType;
//  using EdgeType = typename GraphType::EdgeType;
//  using VertexIDType = typename VertexType::IDType;
//  using VertexLabelType = typename VertexType::LabelType;
//  using EdgeLabelType = typename EdgeType::LabelType;
//  using VertexHandleType = typename GUNDAM::VertexHandle<GraphType>::type;
//  using VertexConstPtr = typename GraphType::VertexConstPtr;
//  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
//  using AttributeKeyType = typename VertexType::AttributeKeyType;
//  using EdgeAttributeKeyType = typename EdgeType::AttributeKeyType;
//  std::set<EdgeLabelType> edge_used_label;
//  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
//  vertex_it++) {
//    VertexConstPtr vertex_handle = vertex_it;
//    for (auto edge_it = vertex_handle->OutEdgeCBegin(); !edge_it.IsDone();
//         edge_it++) {
//      if (edge_used_label.count(edge_it->label())) continue;
//      edge_used_label.insert(edge_it->label());
//      EdgeConstPtr edge_vertex_ptr = edge_it;
//      for (auto edge_attr_it = edge_vertex_ptr->AttributeBegin();
//           !edge_attr_it.IsDone(); edge_attr_it++) {
//        EdgeAttributeKeyType attr_key = edge_attr_it->key();
//        if (std::find(edge_attr_key.begin(), edge_attr_key.end(), attr_key) !=
//            edge_attr_key.end())
//          continue;
//        std::string attr_value_type =
//            edge_vertex_ptr->attribute_value_type_name(attr_key);
//        std::stringstream ss;
//        ss << attr_key;
//        std::string col_name;
//        ss >> col_name;
//        edge_col_name.push_back(col_name);
//        edge_attr_key.push_back(attr_key);
//        edge_col_value_type.push_back(attr_value_type);
//      }
//    }
//  }
//  return;
//}

template <bool write_attr, class VertexEdgePtr, class AttributeKeyType,
          typename std::enable_if<!write_attr, bool>::type = false>
void WriteAttributes(VertexEdgePtr& vertex_edge_ptr,
                     const std::map<AttributeKeyType, size_t>& attr_pos,
                     std::vector<std::string>& line) {}

template <bool write_attr, class VertexEdgePtr, class AttributeKeyType,
          typename std::enable_if<write_attr, bool>::type = false>
void WriteAttributes(VertexEdgePtr& vertex_edge_ptr,
                     const std::map<AttributeKeyType, size_t>& attr_pos,
                     std::vector<std::string>& line) {
  for (auto attr_it = vertex_edge_ptr->AttributeBegin(); !attr_it.IsDone();
       ++attr_it) {
    size_t pos = attr_pos.find(attr_it->key())->second;
    line[pos] = attr_it->value_str();
  }
}

// template <
//    class GraphType,
//    typename std::enable_if<!GraphType::edge_has_attribute, bool>::type =
//    false>
// void WriteEdgeAttribute(
//    const GraphType& graph, typename GraphType::EdgeConstPtr& edge_vertex_ptr,
//    std::vector<typename GraphType::EdgeType::AttributeKeyType>&
//    edge_attr_key, std::vector<std::string>& edge_col_value_type,
//    std::vector<std::string>& line_edge_info) {
//  return;
//}
//
// template <
//    class GraphType,
//    typename std::enable_if<GraphType::edge_has_attribute, bool>::type =
//    false>
// void WriteEdgeAttribute(
//    const GraphType& graph, typename GraphType::EdgeConstPtr& edge_vertex_ptr,
//    std::vector<typename GraphType::EdgeType::AttributeKeyType>&
//    edge_attr_key, std::vector<std::string>& edge_col_value_type,
//    std::vector<std::string>& line_edge_info) {
//  using VertexType = typename GraphType::VertexType;
//  using EdgeType = typename GraphType::EdgeType;
//  using VertexIDType = typename VertexType::IDType;
//  using VertexLabelType = typename VertexType::LabelType;
//  using EdgeLabelType = typename EdgeType::LabelType;
//  using VertexHandleType = typename GUNDAM::VertexHandle<GraphType>::type;
//  using VertexConstPtr = typename GraphType::VertexConstPtr;
//  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
//  using AttributeKeyType = typename VertexType::AttributeKeyType;
//  using EdgeAttributeKeyType = typename EdgeType::AttributeKeyType;
//  for (auto attr_it = edge_vertex_ptr->AttributeBegin(); !attr_it.IsDone();
//       attr_it++) {
//    int attr_key_pos = 0;
//    EdgeAttributeKeyType attr_key = attr_it->key();
//    for (int j = 0; j < edge_attr_key.size(); j++) {
//      if (edge_attr_key[j] == attr_key) {
//        attr_key_pos = j + 4;
//        break;
//      }
//    }
//    enum BasicDataType value_type =
//        edge_vertex_ptr->attribute_value_type(attr_key);
//    switch (value_type) {
//      case BasicDataType::kTypeDouble:
//        line_edge_info[attr_key_pos] =
//            std::to_string(attr_it->template const_value<double>());
//        break;
//      case BasicDataType::kTypeInt:
//        line_edge_info[attr_key_pos] =
//            std::to_string(attr_it->template const_value<int>());
//        break;
//      case BasicDataType::kTypeString:
//        line_edge_info[attr_key_pos] =
//            attr_it->template const_value<std::string>();
//        break;
//      case BasicDataType::kTypeDateTime:
//        line_edge_info[attr_key_pos] =
//            (attr_it->template const_value<DateTime>()).to_string();
//        break;
//      case BasicDataType::kTypeUnknown:
//      default:
//        break;
//    }
//  }
//  return;
//}

// template <class GraphType>
// int WriteCSVGraph(const GraphType& graph, const char* v_file,
//                  const char* e_file) {
//  using VertexType = typename GraphType::VertexType;
//  using EdgeType = typename GraphType::EdgeType;
//  using VertexIDType = typename VertexType::IDType;
//  using EdgeIDType = typename EdgeType::IDType;
//  using VertexLabelType = typename VertexType::LabelType;
//  using EdgeLabelType = typename EdgeType::LabelType;
//  using VertexConstPtr = typename GraphType::VertexConstPtr;
//  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
//  using VertexAttributeKeyType = typename VertexType::AttributeKeyType;
//  using EdgeAttributeKeyType = typename EdgeType::AttributeKeyType;
//
//  // write .v file
//  std::ofstream vertex_file(v_file);
//  int count = 0;
//  std::vector<std::string> vertex_col_name, col_value_type;
//  std::vector<VertexAttributeKeyType> vertex_attr_key;
//  vertex_col_name.push_back("vertex_id");
//  vertex_col_name.push_back("vertex_label");
//  // get ID and Label Type
//  int has_vertex_flag = 0;
//  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
//  vertex_it++) {
//    VertexConstPtr vertex_handle = vertex_it;
//    col_value_type.push_back(TypeToString<VertexIDType>());
//    col_value_type.push_back(TypeToString<VertexLabelType>());
//    has_vertex_flag = 1;
//    break;
//  }
//  if (!has_vertex_flag) {
//    return 0;
//  }
//  // Get Attribute value type
//  if (graph.vertex_has_attribute) {
//    GetVertexAttributeValueType(graph, vertex_col_name, col_value_type,
//                                vertex_attr_key);
//  }
//  // cout col
//  WriteCSVCol(vertex_file, vertex_col_name, col_value_type);
//  // cout each vertex
//  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
//  vertex_it++) {
//    count++;
//    std::vector<std::string> line_vertex_info;
//    line_vertex_info.resize(vertex_col_name.size());
//    line_vertex_info[0] = ToString(vertex_it->id());
//    line_vertex_info[1] = ToString(vertex_it->label());
//    VertexConstPtr vertex_handle = vertex_it;
//    if (graph.vertex_has_attribute) {
//      WriteVertexAttribute(graph, vertex_handle, vertex_attr_key,
//      col_value_type,
//                           line_vertex_info);
//    }
//    WriteLine(vertex_file, line_vertex_info);
//  }
//  // write .e file
//  std::ofstream edge_vertex_file(e_file);
//
//  std::vector<std::string> edge_col_name, edge_col_value_type;
//  std::vector<EdgeAttributeKeyType> edge_attr_key;
//  edge_col_name.push_back("edge_id");
//  edge_col_name.push_back("source_vertex_id");
//  edge_col_name.push_back("target_vertex_id");
//  edge_col_name.push_back("edge_label_id");
//  // get ID and Label Type
//  int insert_edge_flag = 0;
//  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
//  vertex_it++) {
//    VertexConstPtr vertex_handle = vertex_it;
//    for (auto edge_it = vertex_handle->OutEdgeCBegin(); !edge_it.IsDone();
//         edge_it++) {
//      EdgeConstPtr edge_vertex_ptr = edge_it;
//      edge_col_value_type.push_back(TypeToString<EdgeIDType>());
//      edge_col_value_type.push_back(TypeToString<VertexIDType>());
//      edge_col_value_type.push_back(TypeToString<VertexIDType>());
//      edge_col_value_type.push_back(TypeToString<EdgeLabelType>());
//      insert_edge_flag = 1;
//      break;
//    }
//    if (insert_edge_flag) break;
//  }
//  if (!insert_edge_flag) {
//    return count;
//  }
//  // std::cout << "size=" << edge_col_value_type.size() << std::endl;
//  // Get Attribute value type
//  if (graph.edge_has_attribute) {
//    GetEdgeAttributeValueType(graph, edge_col_name, edge_col_value_type,
//                              edge_attr_key);
//  }
//  // cout col
//  WriteCSVCol(edge_vertex_file, edge_col_name, edge_col_value_type);
//  // cout each edge
//  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
//  vertex_it++) {
//    VertexConstPtr vertex_handle = vertex_it;
//    for (auto edge_it = vertex_handle->OutEdgeCBegin(); !edge_it.IsDone();
//         edge_it++) {
//      count++;
//      std::vector<std::string> line_edge_info;
//      line_edge_info.resize(edge_col_name.size());
//      line_edge_info[0] = ToString(edge_it->id());
//      line_edge_info[1] = ToString(edge_it->const_src_handle()->id());
//      line_edge_info[2] = ToString(edge_it->const_dst_handle()->id());
//      line_edge_info[3] = ToString(edge_it->label());
//      EdgeConstPtr edge_vertex_ptr = edge_it;
//      if (graph.edge_has_attribute) {
//        WriteEdgeAttribute(graph, edge_vertex_ptr, edge_attr_key,
//        edge_col_value_type,
//                           line_edge_info);
//      }
//      WriteLine(edge_vertex_file, line_edge_info);
//    }
//  }
//  return count;
//}

template <bool write_attr, class VertexEdgePtr, class AttributeKeyType,
          typename std::enable_if<!write_attr, bool>::type = false>
void GetWriteAttributeInfo(VertexEdgePtr ptr, std::vector<std::string>& key_str,
                           std::vector<std::string>& type_str,
                           std::map<AttributeKeyType, size_t>& attr_pos) {}

template <bool write_attr, class VertexEdgePtr, class AttributeKeyType,
          typename std::enable_if<write_attr, bool>::type = false>
void GetWriteAttributeInfo(VertexEdgePtr ptr, std::vector<std::string>& key_str,
                           std::vector<std::string>& type_str,
                           std::map<AttributeKeyType, size_t>& attr_pos) {
  for (auto attr_it = ptr->AttributeBegin(); !attr_it.IsDone(); ++attr_it) {
    auto attr_key = attr_it->key();
    if (attr_pos.emplace(attr_key, key_str.size()).second) {
      key_str.emplace_back(ToString(attr_key));
      type_str.emplace_back(EnumToString(attr_it->value_type()));
    }
  }
}

template <bool write_attr = true, class GraphType, class WriteVertexCallback>
int WriteCSVVertexSetFileWithCallback(
    const std::vector<GraphType>& graph_set,
    const std::vector<std::string>& graph_name_set,
    const std::string& v_set_file, WriteVertexCallback wv_callback) {
  // write vertex set file(csv)
  // file format: (vertex_id,label_id,graph_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;

  assert(graph_set.size() == graph_name_set.size());

  // get columns
  std::vector<std::string> key_str, type_str;
  std::map<VertexAttributeKeyType, size_t> attr_pos;
  const size_t kNumberOfGraph = graph_set.size();
  for (size_t i = 0; i < kNumberOfGraph; i++) {
    auto& graph = graph_set[i];
    for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
         ++vertex_it) {
      if constexpr (!std::is_null_pointer_v<WriteVertexCallback>) {
        if (!wv_callback(vertex_it)) continue;
      }
      if (key_str.empty()) {
        key_str.emplace_back("vertex_id");
        key_str.emplace_back("label_id");
        // add graph_id
        key_str.emplace_back("graph_id");
        type_str.emplace_back(TypeToString<VertexIDType>());
        type_str.emplace_back(TypeToString<VertexLabelType>());
        type_str.emplace_back(TypeToString<std::string>());
      }
      if constexpr (write_attr) {
        GetWriteAttributeInfo<GraphType::vertex_has_attribute>(
            vertex_it, key_str, type_str, attr_pos);
      }
    }
    if (key_str.empty()) {
      return 0;
    }
  }

  std::cout << v_set_file << std::endl;

  std::ofstream vertex_set_file(v_set_file);

  WriteCSVColumns(vertex_set_file, key_str, type_str);

  // write each vertex
  int count = 0;
  for (size_t i = 0; i < kNumberOfGraph; i++) {
    auto& graph = graph_set[i];
    for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
         ++vertex_it) {
      if constexpr (!std::is_null_pointer_v<WriteVertexCallback>) {
        if (!wv_callback(vertex_it)) continue;
      }
      std::vector<std::string> line;
      line.resize(key_str.size());
      line[0] = ToString(vertex_it->id());
      line[1] = ToString(vertex_it->label());
      line[2] = graph_name_set[i];
      if constexpr (write_attr) {
        WriteAttributes<GraphType::vertex_has_attribute>(vertex_it, attr_pos,
                                                         line);
      }
      WriteCSVLine(vertex_set_file, line);
      ++count;
    }
  }

  return count;
}

template <bool write_attr = true, class GraphType, class WriteVertexCallback>
int WriteCSVVertexFileWithCallback(const GraphType& graph,
                                   const std::string& v_file,
                                   WriteVertexCallback wv_callback) {
  // write vertex file(csv)
  // file format: (vertex_id,label_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;

  // get columns
  std::vector<std::string> key_str, type_str;
  std::map<VertexAttributeKeyType, size_t> attr_pos;
  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone(); ++vertex_it) {
    if constexpr (!std::is_null_pointer_v<WriteVertexCallback>) {
      if (!wv_callback(vertex_it)) continue;
    }
    if (key_str.empty()) {
      key_str.emplace_back("vertex_id");
      key_str.emplace_back("label_id");
      type_str.emplace_back(TypeToString<VertexIDType>());
      type_str.emplace_back(TypeToString<VertexLabelType>());
    }
    if constexpr (write_attr) {
      GetWriteAttributeInfo<GraphType::vertex_has_attribute>(
          vertex_it, key_str, type_str, attr_pos);
    }
  }
  if (key_str.empty()) {
    return 0;
  }

  std::cout << v_file << std::endl;

  std::ofstream vertex_file(v_file);

  WriteCSVColumns(vertex_file, key_str, type_str);

  // write each vertex
  int count = 0;
  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone(); ++vertex_it) {
    if constexpr (!std::is_null_pointer_v<WriteVertexCallback>) {
      if (!wv_callback(vertex_it)) continue;
    }
    std::vector<std::string> line;
    line.resize(key_str.size());
    line[0] = ToString(vertex_it->id());
    line[1] = ToString(vertex_it->label());
    if constexpr (write_attr) {
      WriteAttributes<GraphType::vertex_has_attribute>(vertex_it, attr_pos,
                                                       line);
    }
    WriteCSVLine(vertex_file, line);
    ++count;
  }

  return count;
}

template <bool write_attr = true, class GraphType, class WriteEdgeCallback>
int WriteCSVEdgeSetFileWithCallback(
    const std::vector<GraphType>& graph_set,
    const std::vector<std::string>& graph_name_set, const std::string& e_file,
    WriteEdgeCallback we_callback) {
  // write edge set file(csv)
  // file format: (edge_id,source_id,target_id,label_id,graph_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;

  assert(graph_set.size() == graph_name_set.size());

  // get columns
  std::vector<std::string> key_str, type_str;
  std::map<EdgeAttributeKeyType, size_t> attr_pos;
  // for (auto edge_it = graph.EdgeBegin(); !edge_it.IsDone(); ++edge_it) {
  /// modified by wenzhi, from for(edges){} to for (vertex){ for (edge in
  /// vertex){} }
  const size_t kNumberOfGraph = graph_set.size();
  for (int i = 0; i < kNumberOfGraph; i++) {
    auto& graph = graph_set[i];
    for (auto vertex_cit = graph.VertexBegin(); !vertex_cit.IsDone();
         ++vertex_cit) {
      for (auto edge_it = vertex_cit->OutEdgeBegin(); !edge_it.IsDone();
           ++edge_it) {
        if constexpr (!std::is_null_pointer_v<WriteEdgeCallback>) {
          if (!we_callback(edge_it)) continue;
        }
        if (key_str.empty()) {
          assert(type_str.empty());
          key_str.emplace_back("edge_id");
          key_str.emplace_back("source_id");
          key_str.emplace_back("target_id");
          key_str.emplace_back("label_id");
          key_str.emplace_back("graph_id");
          type_str.emplace_back(TypeToString<EdgeIDType>());
          type_str.emplace_back(TypeToString<VertexIDType>());
          type_str.emplace_back(TypeToString<VertexIDType>());
          type_str.emplace_back(TypeToString<EdgeLabelType>());
          type_str.emplace_back(TypeToString<int>());
        }
        if constexpr (write_attr) {
          GetWriteAttributeInfo<GraphType::edge_has_attribute>(
              edge_it, key_str, type_str, attr_pos);
        }
      }
    }
  }

  if (key_str.empty()) {
    std::cout << "empty" << std::endl;
    return 0;
  }

  std::cout << e_file << std::endl;

  std::ofstream edge_set_file(e_file);

  WriteCSVColumns(edge_set_file, key_str, type_str);

  // write each edge
  int count = 0;
  // for (auto edge_it = graph.EdgeBegin(); !edge_it.IsDone(); ++edge_it) {
  /// modified by wenzhi, from for(edges){} to for (vertex){ for (edge in
  /// vertex){} }
  for (int i = 0; i < kNumberOfGraph; i++) {
    auto& graph = graph_set[i];
    for (auto vertex_cit = graph.VertexBegin(); !vertex_cit.IsDone();
         ++vertex_cit) {
      for (auto edge_it = vertex_cit->OutEdgeBegin(); !edge_it.IsDone();
           ++edge_it) {
        if constexpr (!std::is_null_pointer_v<WriteEdgeCallback>) {
          if (!we_callback(edge_it)) continue;
        }
        std::vector<std::string> line;
        line.resize(key_str.size());
        line[0] = ToString(edge_it->id());
        line[1] = ToString(edge_it->const_src_handle()->id());
        line[2] = ToString(edge_it->const_dst_handle()->id());
        line[3] = ToString(edge_it->label());
        line[4] = graph_name_set[i];
        if constexpr (write_attr) {
          WriteAttributes<GraphType::edge_has_attribute>(edge_it, attr_pos,
                                                         line);
        }
        WriteCSVLine(edge_set_file, line);
        ++count;
      }
    }
  }

  return count;
}

template <bool write_attr = true, class GraphType, class WriteEdgeCallback>
int WriteCSVEdgeFileWithCallback(const GraphType& graph,
                                 const std::string& e_file,
                                 WriteEdgeCallback we_callback) {
  // write edge file(csv)
  // file format: (edge_id,source_id,target_id,label_id,......)
  using VertexIDType = typename GraphType::VertexType::IDType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;

  // get columns
  std::vector<std::string> key_str, type_str;
  std::map<EdgeAttributeKeyType, size_t> attr_pos;
  // for (auto edge_it = graph.EdgeBegin(); !edge_it.IsDone(); ++edge_it) {
  /// modified by wenzhi, from for(edges){} to for (vertex){ for (edge in
  /// vertex){} }
  for (auto vertex_cit = graph.VertexBegin(); !vertex_cit.IsDone();
       ++vertex_cit) {
    for (auto edge_it = vertex_cit->OutEdgeBegin(); !edge_it.IsDone();
         ++edge_it) {
      if constexpr (!std::is_null_pointer_v<WriteEdgeCallback>) {
        if (!we_callback(edge_it)) continue;
      }
      if (key_str.empty()) {
        assert(type_str.empty());
        key_str.emplace_back("edge_id");
        key_str.emplace_back("source_id");
        key_str.emplace_back("target_id");
        key_str.emplace_back("label_id");
        type_str.emplace_back(TypeToString<EdgeIDType>());
        type_str.emplace_back(TypeToString<VertexIDType>());
        type_str.emplace_back(TypeToString<VertexIDType>());
        type_str.emplace_back(TypeToString<EdgeLabelType>());
      }
      if constexpr (write_attr) {
        GetWriteAttributeInfo<GraphType::edge_has_attribute>(
            edge_it, key_str, type_str, attr_pos);
      }
    }
  }
  if (key_str.empty()) {
    return 0;
  }

  std::cout << e_file << std::endl;

  std::ofstream edge_file(e_file);

  WriteCSVColumns(edge_file, key_str, type_str);

  // write each edge
  int count = 0;
  // for (auto edge_it = graph.EdgeBegin(); !edge_it.IsDone(); ++edge_it) {
  /// modified by wenzhi, from for(edges){} to for (vertex){ for (edge in
  /// vertex){} }
  for (auto vertex_cit = graph.VertexBegin(); !vertex_cit.IsDone();
       ++vertex_cit) {
    for (auto edge_it = vertex_cit->OutEdgeBegin(); !edge_it.IsDone();
         ++edge_it) {
      if constexpr (!std::is_null_pointer_v<WriteEdgeCallback>) {
        if (!we_callback(edge_it)) continue;
      }
      std::vector<std::string> line;
      line.resize(key_str.size());
      line[0] = ToString(edge_it->id());
      line[1] = ToString(edge_it->const_src_handle()->id());
      line[2] = ToString(edge_it->const_dst_handle()->id());
      line[3] = ToString(edge_it->label());
      if constexpr (write_attr) {
        WriteAttributes<GraphType::edge_has_attribute>(edge_it, attr_pos, line);
      }
      WriteCSVLine(edge_file, line);
      ++count;
    }
  }

  return count;
}

template <bool write_attr = true, class GraphType>
int WriteCSVGraphSet(const std::vector<GraphType>& graph_set,
                     const std::string& v_file, const std::string& e_file) {
  std::vector<std::string> graph_name_set;
  graph_name_set.reserve(graph_set.size());
  for (size_t i = 0; i < graph_set.size(); i++) {
    graph_name_set.emplace_back(std::to_string(i));
  }
  return WriteCSVGraphSet(graph_set, graph_name_set, v_file, e_file);
}

template <bool write_attr = true, class GraphType>
int WriteCSVGraphSet(const std::vector<GraphType>& graph_set,
                     const std::vector<std::string>& graph_name_set,
                     const std::string& v_file, const std::string& e_file) {
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;

  int res;
  int count_v, count_e;

  res = WriteCSVVertexSetFileWithCallback<write_attr>(graph_set, graph_name_set,
                                                      v_file, nullptr);
  if (res < 0) return res;

  count_v = res;

  res = WriteCSVEdgeSetFileWithCallback<write_attr>(graph_set, graph_name_set,
                                                    e_file, nullptr);
  if (res < 0) return res;

  count_e = res;

  return count_v + count_e;
}

template <bool write_attr = true, class GraphType>
int WriteCSVGraph(const GraphType& graph, const std::string& v_file,
                  const std::string& e_file) {
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;

  int res;
  int count_v, count_e;

  res = WriteCSVVertexFileWithCallback<write_attr>(graph, v_file, nullptr);
  if (res < 0) return res;

  count_v = res;

  res = WriteCSVEdgeFileWithCallback<write_attr>(graph, e_file, nullptr);
  if (res < 0) return res;

  count_e = res;

  return count_v + count_e;
}

template <bool write_attr = true, class GraphType, class VertexSet,
          class EdgeSet>
int WriteCSVParticalGraph(const GraphType& graph, const std::string& v_file,
                          const std::string& e_file,
                          const VertexSet& vertex_set,
                          const EdgeSet& edge_set) {
  using VertexIDType = typename GraphType::VertexType::IDType;
  using VertexLabelType = typename GraphType::VertexType::LabelType;
  using VertexAttributeKeyType =
      typename GraphType::VertexType::AttributeKeyType;
  using EdgeIDType = typename GraphType::EdgeType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using EdgeAttributeKeyType = typename GraphType::EdgeType::AttributeKeyType;

  int res;
  int count_v, count_e;

  res = WriteCSVVertexFileWithCallback<write_attr>(
      graph, v_file, [&vertex_set](auto& vertex) -> bool {
        return (vertex_set.find(vertex->id()) != vertex_set.end());
      });
  if (res < 0) return res;

  count_v = res;

  res = WriteCSVEdgeFileWithCallback<write_attr>(
      graph, e_file, [&edge_set](auto& edge) -> bool {
        return (edge_set.find(edge->id()) != edge_set.end());
      });
  if (res < 0) return res;

  count_e = res;

  return count_v + count_e;
}
}  // namespace GUNDAM

#endif