#ifndef _CSVGAR_H
#define _CSVGAR_H

#include "gar.h"
#include "gundam/io/csvgraph.h"

namespace gmine_new {

template <bool is_x, class GAR>
int ReadCSVEdgeLiteral(GAR &gar, rapidcsv::Document &literal_csv,
                       size_t row_pos) noexcept {
  using Pattern = typename GAR::PatternType;
  using DataGraph = typename GAR::DataGraphType;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using EdgeLabel = typename DataGraph::EdgeType::LabelType;

  try {
    PatternVertexID src_id = literal_csv.GetCell<PatternVertexID>(1, row_pos);
    PatternVertexID dst_id = literal_csv.GetCell<PatternVertexID>(3, row_pos);
    EdgeLabel edge_label = literal_csv.GetCell<EdgeLabel>(5, row_pos);

    if (is_x) {
      gar.template AddX<EdgeLiteral<Pattern, DataGraph>>(src_id, dst_id,
                                                         edge_label);
    } else {
      gar.template AddY<EdgeLiteral<Pattern, DataGraph>>(src_id, dst_id,
                                                         edge_label);
    }
    return 1;

  } catch (...) {
    return 0;
  }
}

template <bool is_x, class GAR>
int ReadCSVAttributeLiteral(GAR &gar, rapidcsv::Document &literal_csv,
                            size_t row_pos) noexcept {
  using Pattern = typename GAR::PatternType;
  using DataGraph = typename GAR::DataGraphType;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using VertexAttributeKey = typename DataGraph::VertexType::AttributeKeyType;

  try {
    PatternVertexID x_id = literal_csv.GetCell<PatternVertexID>(1, row_pos);
    VertexAttributeKey x_attr_key =
        literal_csv.GetCell<VertexAttributeKey>(2, row_pos);

    if (is_x) {
      gar.template AddX<AttributeLiteral<Pattern, DataGraph>>(x_id, x_attr_key);
    } else {
      gar.template AddY<AttributeLiteral<Pattern, DataGraph>>(x_id, x_attr_key);
    }
    return 1;

  } catch (...) {
    return 0;
  }
}

template <bool is_x, class GAR>
int ReadCSVVariableLiteral(GAR &gar, rapidcsv::Document &literal_csv,
                           size_t row_pos) noexcept {
  using Pattern = typename GAR::PatternType;
  using DataGraph = typename GAR::DataGraphType;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using VertexAttributeKey = typename DataGraph::VertexType::AttributeKeyType;

  try {
    PatternVertexID x_id = literal_csv.GetCell<PatternVertexID>(1, row_pos);
    VertexAttributeKey x_attr_key =
        literal_csv.GetCell<VertexAttributeKey>(2, row_pos);
    PatternVertexID y_id = literal_csv.GetCell<PatternVertexID>(3, row_pos);
    VertexAttributeKey y_attr_key =
        literal_csv.GetCell<VertexAttributeKey>(4, row_pos);

    if (is_x) {
      gar.template AddX<VariableLiteral<Pattern, DataGraph>>(x_id, x_attr_key,
                                                             y_id, y_attr_key);
    } else {
      gar.template AddY<VariableLiteral<Pattern, DataGraph>>(x_id, x_attr_key,
                                                             y_id, y_attr_key);
    }
    return 1;

  } catch (...) {
    return 0;
  }
}

template <bool is_x, class GAR>
int ReadCSVConstantLiteral(GAR &gar, rapidcsv::Document &literal_csv,
                           size_t row_pos) noexcept {
  using Pattern = typename GAR::PatternType;
  using DataGraph = typename GAR::DataGraphType;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using VertexAttributeKey = typename DataGraph::VertexType::AttributeKeyType;

  try {
    PatternVertexID x_id = literal_csv.GetCell<PatternVertexID>(1, row_pos);
    VertexAttributeKey x_attr_key =
        literal_csv.GetCell<VertexAttributeKey>(2, row_pos);
    std::string value_with_type = literal_csv.GetCell<std::string>(6, row_pos);

    std::string value_str, type_str;
    bool partition_flag = false;
    for (const auto &c : value_with_type) {
      if (c == ';') {
        partition_flag = true;
        continue;
      }
      if (partition_flag)
        type_str.push_back(c);
      else
        value_str.push_back(c);
    }

    GUNDAM::BasicDataType value_type = GUNDAM::StringToEnum(type_str.c_str());

    switch (value_type) {
      case GUNDAM::BasicDataType::kTypeString: {
        if (is_x)
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, std::string>>(
              x_id, x_attr_key, value_str);
        else
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, std::string>>(
              x_id, x_attr_key, value_str);
        break;
      }
      case GUNDAM::BasicDataType::kTypeInt: {
        if (is_x)
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, int>>(
              x_id, x_attr_key, std::stoi(value_str));
        else
          gar.template AddY<ConstantLiteral<Pattern, DataGraph, int>>(
              x_id, x_attr_key, std::stoi(value_str));
        break;
      }
      case GUNDAM::BasicDataType::kTypeInt64: {
        if (is_x)
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, int64_t>>(
              x_id, x_attr_key, std::stoi(value_str));
        else
          gar.template AddY<ConstantLiteral<Pattern, DataGraph, int64_t>>(
              x_id, x_attr_key, std::stoi(value_str));
        break;
      }
      case GUNDAM::BasicDataType::kTypeFloat: {
        if (is_x)
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, float>>(
              x_id, x_attr_key, std::stof(value_str));
        else
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, float>>(
              x_id, x_attr_key, std::stof(value_str));
        break;
      }
      case GUNDAM::BasicDataType::kTypeDouble: {
        if (is_x)
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, double>>(
              x_id, x_attr_key, std::stod(value_str));
        else
          gar.template AddX<ConstantLiteral<Pattern, DataGraph, double>>(
              x_id, x_attr_key, std::stod(value_str));
        break;
      }
      case GUNDAM::BasicDataType::kTypeDateTime: {
        if (is_x)
          gar.template AddX<
              ConstantLiteral<Pattern, DataGraph, GUNDAM::DateTime>>(
              x_id, x_attr_key, GUNDAM::DateTime(value_str));
        else
          gar.template AddX<
              ConstantLiteral<Pattern, DataGraph, GUNDAM::DateTime>>(
              x_id, x_attr_key, GUNDAM::DateTime(value_str));
        break;
      }
      case GUNDAM::BasicDataType::kTypeUnknown:
      default:
        return -1;
    }
    return 1;

  } catch (...) {
    return 0;
  }
}

template <bool is_x, class GAR>
int ReadCSVLiteralFile(GAR &gar, const std::string &literal_file) {
  std::cout << literal_file << std::endl;

  rapidcsv::Document literal_csv(literal_file, rapidcsv::LabelParams(0, -1));

  int count = 0;

  size_t row_count = literal_csv.GetRowCount();
  for (size_t row = 0; row < row_count; row++) {
    std::string literal_type = literal_csv.GetCell<std::string>(0, row);

    int res;
    if (literal_type == "Edge") {
      res = ReadCSVEdgeLiteral<is_x>(gar, literal_csv, row);
    } else if (literal_type == "Attribute") {
      res = ReadCSVAttributeLiteral<is_x>(gar, literal_csv, row);
    } else if (literal_type == "Variable") {
      res = ReadCSVVariableLiteral<is_x>(gar, literal_csv, row);
    } else if (literal_type == "Constant") {
      res = ReadCSVConstantLiteral<is_x>(gar, literal_csv, row);
    } else {
      return -1;
    }
    if (res < 0) return res;

    ++count;
  }

  std::cout << "Literal: " << count << std::endl;

  return count;
}

template <class GAR>
int ReadGAR(GAR &gar, const std::string &v_file, const std::string &e_file,
            const std::string &x_file, const std::string &y_file) {
  int res;

  gar.Reset();

  int count = 0;

  res = GUNDAM::ReadCSVGraph(gar.pattern(), v_file, e_file);
  if (res < 0) return res;
  count += res;

  res = ReadCSVLiteralFile<true>(gar, x_file);
  if (res < 0) return res;
  count += res;

  res = ReadCSVLiteralFile<false>(gar, y_file);
  if (res < 0) return res;
  count += res;

  return count;
}

constexpr char kCSVLiteralHead[] = "type,x_id,x_attr,y_id,y_attr,edge_label,c";

template <class LiteralSet>
int WriteCSVLiteralFile(const LiteralSet &literal_set,
                        const std::string &literal_file) {
  std::ofstream f(literal_file);
  if (!f) return -1;

  f << kCSVLiteralHead << std::endl;

  int count = 0;
  for (const auto &l : literal_set) {
    l->Print(f);
    ++count;
  }

  return count;
}

template <class GAR>
int WriteGAR(const GAR &gar, const std::string &v_file,
             const std::string &e_file, const std::string &x_file,
             const std::string &y_file) {
  int res;

  int count = 0;
  res = GUNDAM::WriteCSVGraph<false>(gar.pattern(), v_file, e_file);
  if (res < 0) return res;
  count += res;

  res = WriteCSVLiteralFile(gar.x_literal_set(), x_file);
  if (res < 0) return res;
  count += res;

  res = WriteCSVLiteralFile(gar.y_literal_set(), y_file);
  if (res < 0) return res;
  count += res;

  return count;
}

};  // namespace gmine_new
#endif