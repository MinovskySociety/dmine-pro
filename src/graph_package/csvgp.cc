#include "csvgp.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace graphpackage::csvgp {

std::filesystem::path GetLabelFilename(const std::string &graph_name) {
  std::string filename = graph_name + "_label.csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

std::filesystem::path GetAttributeFilename(const std::string &graph_name) {
  std::string filename = graph_name + "_attr.csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

std::filesystem::path GetTypeFilename(const std::string &graph_name) {
  std::string filename = graph_name + "_type.csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

std::filesystem::path GetVertexFilename(const std::string &graph_name) {
  std::string filename = graph_name + "_v.csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

std::filesystem::path GetVertexFilename(const std::string &graph_name,
                                        const std::string &group_name) {
  std::string filename = graph_name + "_v_" + group_name + ".csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

std::filesystem::path GetEdgeFilename(const std::string &graph_name) {
  std::string filename = graph_name + "_e.csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

std::filesystem::path GetEdgeFilename(const std::string &graph_name,
                                      const std::string &group_name) {
  std::string filename = graph_name + "_e_" + group_name + ".csv";
  std::transform(filename.begin(), filename.end(), filename.begin(), tolower);
  return {filename};
}

bool CheckLabelFilename(const std::filesystem::path &filepath,
                        const std::string &graph_name) {
  return filepath.stem().string() == GetLabelFilename(graph_name);
}

bool CheckAttributeFilename(const std::filesystem::path &filepath,
                            const std::string &graph_name) {
  return filepath.stem().string() == GetAttributeFilename(graph_name);
}

bool CheckTypeFilename(const std::filesystem::path &filepath,
                       const std::string &graph_name) {
  return filepath.stem().string() == GetTypeFilename(graph_name);
}

std::pair<std::string, bool> CheckVertexFilename(
    const std::filesystem::path &filepath, const std::string &graph_name) {
  if (filepath.extension() != ".csv") return {{}, false};

  std::string s1 = filepath.stem().string();

  std::string head = graph_name + "_v";
  if (s1.compare(0, head.length(), head) != 0) return {{}, false};

  std::string s2 = s1.substr(head.length());
  if (s2.empty()) return {{}, true};

  if (s2[0] != '_') return {{}, false};

  return {s2.substr(1), true};
}

std::pair<std::string, bool> CheckEdgeFilename(
    const std::filesystem::path &filepath, const std::string &graph_name) {
  if (filepath.extension() != ".csv") return {{}, false};

  std::string s1 = filepath.stem().string();

  std::string head = graph_name + "_e";
  if (s1.compare(0, head.length(), head) != 0) return {{}, false};

  std::string s2 = s1.substr(head.length());
  if (s2.empty()) return {{}, true};

  if (s2[0] != '_') return {{}, false};

  return {s2.substr(1), true};
}

int ReadLabelFile(const std::filesystem::path &label_file,
                  LabelDict &label_dict) {
  // read label file(csv
  // file format: (label_id:int, label_name:string, label_type:int)
  using namespace GUNDAM;

  label_dict.Clear();

  std::cout << label_file.string() << std::endl;

  // check file exist
  if (!std::filesystem::is_regular_file(label_file)) {
    std::cout << "File not exist!" << std::endl;
    return -1;
  }

  rapidcsv::Document label_csv(label_file.string(),
                               rapidcsv::LabelParams(0, -1));

  // phase column names
  std::vector<std::string> col_name = label_csv.GetColumnNames();
  std::vector<std::pair<AttributeKey, DataType>> attr_info;
  bool res = GetAttributeInfo(col_name, attr_info);
  if (!res) {
    std::cout << "Attribute key type is not correct!" << std::endl;
    return -1;
  }

  if (attr_info.size() != 3 || attr_info[0].first != "label_id" ||
      attr_info[0].second != DataType::kTypeInt ||
      attr_info[1].first != "label_name" ||
      attr_info[1].second != DataType::kTypeString ||
      attr_info[2].first != "label_type" ||
      attr_info[2].second != DataType::kTypeInt) {
    std::cout << "label file format error!" << std::endl;
    return -1;
  }

  int count_success = 0;
  int count_fail = 0;
  for (size_t row = 0; row < label_csv.GetRowCount(); ++row) {
    auto label_id = label_csv.GetCell<Label>(0, row);
    auto label_name = label_csv.GetCell<std::string>(1, row);
    auto label_type = static_cast<LabelType>(label_csv.GetCell<int>(2, row));

    if (label_dict
            .Insert(std::move(label_id), std::move(label_name),
                    std::move(label_type))
            .second) {
      ++count_success;
    } else {
      ++count_fail;
    }
  }
  if (count_fail > 0) {
    std::cout << "Failed: " << count_fail << std::endl;
  }
  return count_success;
}

int WriteLabelFile(const LabelDict &label_dict,
                   const std::filesystem::path &label_file) {
  // write label file(csv
  // file format: (label_id:int, label_name:string, label_type:int)
  using namespace GUNDAM;

  std::cout << label_file.string() << std::endl;

  // get column names
  std::vector<std::string> key_str(3), type_str(3);
  key_str[0] = "label_id";
  key_str[1] = "label_name";
  key_str[2] = "label_type";

  type_str[0] = TypeToString<Label>();
  type_str[1] = TypeToString<std::string>();
  type_str[2] = TypeToString<int>();

  std::ofstream label_csv(label_file);
  if (label_csv.bad()) return -1;

  WriteCSVColumns(label_csv, key_str, type_str);

  // write eact attribute
  int count = 0;
  for (const auto &label_info : label_dict) {
    std::vector<std::string> line;
    line.resize(key_str.size());
    line[0] = ToString(label_info.label_id);
    line[1] = ToString(label_info.label_name);
    line[2] = ToString(static_cast<int>(label_info.label_type));
    WriteCSVLine(label_csv, line);
    ++count;
  }

  return count;
}

int ReadAttributeFile(const std::filesystem::path &attr_file,
                      AttributeDict &attr_dict) {
  // read attr file (csv)
  // file format: (attr_key:string, attr_attr_id:int, type_name:string,
  //   value_data_type:int, value_str:string, attr_value_lable_id:int)
  using namespace GUNDAM;

  attr_dict.Clear();

  std::cout << attr_file.string() << std::endl;

  // check file exist
  if (!std::filesystem::is_regular_file(attr_file)) {
    std::cout << "File not exist!" << std::endl;
    return -1;
  }

  rapidcsv::Document attr_csv(attr_file.string(), rapidcsv::LabelParams(0, -1));

  // phase column names
  std::vector<std::string> col_name = attr_csv.GetColumnNames();
  std::vector<std::pair<AttributeKey, DataType>> attr_info;
  bool res = GetAttributeInfo(col_name, attr_info);
  if (!res) {
    std::cout << "Attribute key type is not correct!" << std::endl;
    return -1;
  }

  if (attr_info.size() != 6 || attr_info[0].first != "attr_key" ||
      attr_info[0].second != DataType::kTypeString ||
      attr_info[1].first != "attr_label_id" ||
      attr_info[1].second != DataType::kTypeInt ||
      attr_info[2].first != "type_name" ||
      attr_info[2].second != DataType::kTypeString ||
      attr_info[3].first != "value_data_type" ||
      attr_info[3].second != DataType::kTypeString ||
      attr_info[4].first != "value_str" ||
      attr_info[4].second != DataType::kTypeString ||
      attr_info[5].first != "attr_value_label_id" ||
      attr_info[5].second != DataType::kTypeInt) {
    std::cout << "attribute file format error!" << std::endl;
    return -1;
  }

  int count = 0;
  for (size_t row = 0; row < attr_csv.GetRowCount(); ++row) {
    auto attr_key = attr_csv.GetCell<std::string>(0, row);
    std::transform(attr_key.begin(), attr_key.end(), attr_key.begin(), tolower);
    auto attr_label_id = attr_csv.GetCell<Label>(1, row);
    auto type_name = attr_csv.GetCell<std::string>(2, row);
    std::transform(type_name.begin(), type_name.end(), type_name.begin(),
                   tolower);
    auto value_data_type_str = attr_csv.GetCell<std::string>(3, row);
    std::transform(value_data_type_str.begin(), value_data_type_str.end(),
                   value_data_type_str.begin(), tolower);
    auto value_data_type = StringToEnum(value_data_type_str.c_str());
    auto value_str = attr_csv.GetCell<std::string>(4, row);
    auto attr_value_label_id = attr_csv.GetCell<Label>(5, row);

    if (value_str.empty()) {
      bool r = attr_dict.RegisterAttribute(attr_key, attr_label_id, type_name,
                                           value_data_type);
      if (!r) return -1;
      ++count;
    } else {
      bool r = attr_dict.RegisterAttributeValue(attr_key, value_str,
                                                attr_value_label_id);
      if (!r) return -1;
      ++count;
    }
  }
  return count;
}

int WriteAttributeFile(const AttributeDict &attr_dict,
                       const std::filesystem::path &attr_file) {
  // write attr file (csv)
  // file format: (attr_key:string, attr_label_id:int, type_name:string,
  //   value_data_type:string, value_str:string, attr_value_lable_id:int)
  using namespace GUNDAM;

  std::cout << attr_file.string() << std::endl;

  // get column names
  std::vector<std::string> key_str{"attr_key",  "attr_label_id",
                                   "type_name", "value_data_type",
                                   "value_str", "attr_value_label_id"};

  std::vector<std::string> type_str{
      EnumToString(DataType::kTypeString), EnumToString(DataType::kTypeInt),
      EnumToString(DataType::kTypeString), EnumToString(DataType::kTypeString),
      EnumToString(DataType::kTypeString), EnumToString(DataType::kTypeInt)};

  std::ofstream attr_csv(attr_file);
  if (attr_csv.bad()) return -1;

  WriteCSVColumns(attr_csv, key_str, type_str);
  int count = 0;
  for (const auto &attr_info : attr_dict) {
    std::vector<std::string> line;
    line.resize(key_str.size());
    line[0] = ToString(attr_info.attr_key);
    line[1] = ToString(attr_info.attr_label_id);
    line[2] = ToString(attr_info.type_name);
    line[3] = ToString(attr_info.value_data_type);
    WriteCSVLine(attr_csv, line);
    ++count;
    for (const auto &value_info : attr_info) {
      line[1] = line[2] = line[3] = "";
      line[4] = value_info.value_str;
      line[5] = ToString(value_info.attr_value_label_id);
      WriteCSVLine(attr_csv, line);
      ++count;
    }
  }

  return count;
}

int ReadTypeFile(const std::filesystem::path &type_file, TypeDict &type_dict) {
  // read type file (csv)
  // file format: (type_name:string, value_label_id:int, value_data_type:string,
  //   value_str:string, value_constant_label_id:int, constant_label_id:int)
  using namespace GUNDAM;

  type_dict.Clear();

  std::cout << type_file.string() << std::endl;

  // check file exist
  if (!std::filesystem::is_regular_file(type_file)) {
    std::cout << "File not exist!" << std::endl;
    return -1;
  }

  rapidcsv::Document type_csv(type_file.string(), rapidcsv::LabelParams(0, -1));

  // phase column names
  std::vector<std::string> col_name = type_csv.GetColumnNames();
  std::vector<std::pair<AttributeKey, DataType>> type_info;
  bool res = GetAttributeInfo(col_name, type_info);
  if (!res) {
    std::cout << "Attribute key type is not correct!" << std::endl;
    return -1;
  }

  if (type_info.size() != 6 || type_info[0].first != "type_name" ||
      type_info[0].second != DataType::kTypeString ||
      type_info[1].first != "value_label_id" ||
      type_info[1].second != DataType::kTypeInt ||
      type_info[2].first != "value_data_type" ||
      type_info[2].second != DataType::kTypeString ||
      type_info[3].first != "value_str" ||
      type_info[3].second != DataType::kTypeString ||
      type_info[4].first != "constant_label_id" ||
      type_info[4].second != DataType::kTypeInt ||
      type_info[5].first != "value_constant_label_id" ||
      type_info[5].second != DataType::kTypeInt) {
    std::cout << "type file format error!" << std::endl;
    return -1;
  }

  int count = 0;
  for (size_t row = 0; row < type_csv.GetRowCount(); ++row) {
    auto type_name = type_csv.GetCell<std::string>(0, row);
    std::transform(type_name.begin(), type_name.end(), type_name.begin(),
                   tolower);
    auto value_label_id = type_csv.GetCell<Label>(1, row);
    auto value_data_type_str = type_csv.GetCell<std::string>(2, row);
    std::transform(value_data_type_str.begin(), value_data_type_str.end(),
                   value_data_type_str.begin(), tolower);
    auto value_data_type = StringToEnum(value_data_type_str.c_str());
    auto value_str = type_csv.GetCell<std::string>(3, row);
    auto constant_label_id = type_csv.GetCell<Label>(4, row);
    auto value_constant_label_id = type_csv.GetCell<Label>(5, row);

    if (value_str.empty()) {
      bool r =
          type_dict.RegisterType(type_name, value_label_id, value_data_type);
      if (!r) return -1;
      ++count;
    } else {
      bool r = type_dict.RegisterTypeValue(
          type_name, value_str, constant_label_id, value_constant_label_id);
      if (!r) return -1;
      ++count;
    }
  }
  return count;
}

int WriteTypeFile(const TypeDict &type_dict,
                  const std::filesystem::path &type_file) {
  // write type file (csv)
  // file format: (type_name:string, value_label_id:int, value_data_type:string,
  //   value_str:string, value_constant_label_id:int, constant_label_id:int)
  using namespace GUNDAM;

  std::cout << type_file.string() << std::endl;

  // get column names
  std::vector<std::string> key_str{
      "type_name", "value_label_id",    "value_data_type",
      "value_str", "constant_label_id", "value_constant_label_id"};

  std::vector<std::string> type_str{
      TypeToString<std::string>(), TypeToString<Label>(),
      TypeToString<std::string>(), TypeToString<std::string>(),
      TypeToString<Label>(),       TypeToString<Label>()};

  std::ofstream type_csv(type_file);
  if (type_csv.bad()) return -1;

  WriteCSVColumns(type_csv, key_str, type_str);
  int count = 0;
  for (const auto &type_info : type_dict) {
    std::vector<std::string> line;
    line.resize(key_str.size());
    line[0] = ToString(type_info.type_name);
    line[1] = ToString(type_info.value_label_id);
    line[2] = ToString(type_info.value_data_type);
    WriteCSVLine(type_csv, line);
    ++count;
    line[1].clear();
    line[2].clear();
    for (const auto &type_value_info : type_info) {
      line[3] = ToString(type_value_info.value_str);
      line[4] = ToString(type_value_info.constant_label_id);
      line[5] = ToString(type_value_info.value_constant_label_id);
      WriteCSVLine(type_csv, line);
      ++count;
    }
  }

  return count;
}

bool CheckDict(const LabelDict &label_dict, const AttributeDict &attr_dict,
               const TypeDict &type_dict) {
  std::set<Label> marked_labels;

  for (const auto &attr_info : attr_dict) {
    auto it_label1 = label_dict.Find(attr_info.attr_label_id);
    if (it_label1 == label_dict.end()) {
      return false;
    }
    if (it_label1->label_type != LabelType::kAttribute ||
        it_label1->label_name != attr_info.attr_key)
      return false;
    if (!marked_labels.emplace(it_label1->label_id).second) {
      return false;
    }

    for (const auto &attr_value_info : attr_info) {
      auto it_label2 = label_dict.Find(attr_value_info.attr_value_label_id);
      if (it_label2 == label_dict.end()) {
        return false;
      }
      if (it_label2->label_type != LabelType::kAttributeValueConstant ||
          it_label2->label_name !=
              attr_info.attr_key + "_a_" + attr_value_info.value_str)
        return false;
      if (!marked_labels.emplace(it_label2->label_id).second) {
        return false;
      }
    }
  }

  for (const auto &type_info : type_dict) {
    auto it_label1 = label_dict.Find(type_info.value_label_id);
    if (it_label1 == label_dict.end()) {
      return false;
    }
    if (it_label1->label_type != LabelType::kValue ||
        it_label1->label_name != type_info.type_name + "_v")
      return false;
    if (!marked_labels.emplace(it_label1->label_id).second) {
      return false;
    }

    for (const auto &type_value_info : type_info) {
      auto it_label2 = label_dict.Find(type_value_info.value_constant_label_id);
      if (it_label2 == label_dict.end()) {
        return false;
      }
      if (it_label2->label_type != LabelType::kValueConstant ||
          it_label2->label_name !=
              type_info.type_name + "_v_" + type_value_info.value_str)
        return false;
      if (!marked_labels.emplace(it_label2->label_id).second) {
        return false;
      }

      auto it_label3 = label_dict.Find(type_value_info.constant_label_id);
      if (it_label3 == label_dict.end()) {
        return false;
      }
      if (it_label3->label_type != LabelType::kConstant ||
          it_label3->label_name !=
              type_info.type_name + "_c_" + type_value_info.value_str)
        return false;
      if (!marked_labels.emplace(it_label3->label_id).second) {
        return false;
      }
    }
  }

  for (const auto &label_info : label_dict) {
    if (label_info.label_type == LabelType::kAttribute ||
        label_info.label_type == LabelType::kAttributeValueConstant ||
        label_info.label_type == LabelType::kValue ||
        label_info.label_type == LabelType::kValueConstant ||
        label_info.label_type == LabelType::kConstant) {
      if (marked_labels.find(label_info.label_id) == marked_labels.end())
        return false;
    }
  }

  return true;
}

inline void InsertGroup(const std::string &group_name, const Label &label_id,
                        GroupDict &groups) {
  auto ret = groups.Insert(group_name);
  auto &label_set = ret.first->second;
  label_set.Insert(label_id);
}

int BuildVertexEdgeGroups(const LabelDict &label_dict,
                          const AttributeDict &attr_dict,
                          const TypeDict &type_dict, GroupDict &vertex_groups,
                          GroupDict &edge_groups) {
  int count = 0;

  for (const auto &label_info : label_dict) {
    switch (label_info.label_type) {
      case LabelType::kEntity:
        InsertGroup(label_info.label_name, label_info.label_id, vertex_groups);
        break;
      case LabelType::kRelation:
        InsertGroup(label_info.label_name, label_info.label_id, vertex_groups);
        InsertGroup(label_info.label_name, label_info.label_id, edge_groups);
        break;
      case LabelType::kAttribute:
        InsertGroup(label_info.label_name, label_info.label_id, vertex_groups);
        break;
      case LabelType::kValue:
        InsertGroup(CheckValueLabelName(label_info.label_name),
                    label_info.label_id, vertex_groups);
        break;
      case LabelType::kConstant:
        InsertGroup(CheckConstantLabelName(label_info.label_name).first,
                    label_info.label_id, vertex_groups);
        break;
      case LabelType::kValueConstant:
        InsertGroup(CheckValueConstantLabelName(label_info.label_name).first,
                    label_info.label_id, vertex_groups);
        break;
      case LabelType::kAttributeValueConstant:
        InsertGroup(
            CheckAttributeValueConstantLabelName(label_info.label_name).first,
            label_info.label_id, vertex_groups);
        break;
      default:
        return -1;
    }
    ++count;
  }

  return count;
}

int CompleteReadGraphPackageInfo(GraphPackageInfo &info) {
  namespace fs = std::filesystem;

  int count = 0;

  if (info.has_dict && !info.name.empty() && !info.dict_dir.empty()) {
    if (!fs::is_directory(info.dict_dir)) {
      std::cout << "Can't find directory: " << info.dict_dir.string() << std::endl;
      return -3;
    }
    info.dict_dir = info.dict_dir.lexically_normal();

    if (info.label_file.empty()) {
      fs::path label_file = GetLabelFilename(info.name);
      if (fs::is_regular_file(info.dict_dir / label_file)) {
        info.label_file = label_file.string();
        ++count;
      }
    }

    if (info.attr_file.empty()) {
      fs::path attr_file = GetAttributeFilename(info.name);
      if (fs::is_regular_file(info.dict_dir / attr_file)) {
        info.attr_file = attr_file.string();
        ++count;
      }
    }

    if (info.type_file.empty()) {
      fs::path type_file = GetTypeFilename(info.name);
      if (fs::is_regular_file(info.dict_dir / type_file)) {
        info.type_file = type_file.string();
        ++count;
      }
    }
  }

  if (info.has_graph && !info.name.empty() && !info.graph_dir.empty()) {
    if (!fs::is_directory(info.graph_dir)) {
      std::cout << "Can't find directory: " << info.graph_dir.string() << std::endl;
      return -3;
    }
    info.graph_dir = info.graph_dir.lexically_normal();

    bool search_v = info.vertex_files.empty();
    bool search_e = info.edge_files.empty();
    if (search_v && search_e) {
      for (auto it = fs::directory_iterator(info.graph_dir);
           it != fs::directory_iterator() && (search_v || search_e); ++it) {
        if (!it->is_regular_file()) continue;
        fs::path filename = it->path().filename();
        if (filename.extension() != ".csv") continue;

        bool r = false;
        std::string group_name;
        if (search_v) {
          std::tie(group_name, r) = CheckVertexFilename(filename, info.name);
          if (r) {
            if (info.is_grouping) {
              if (!group_name.empty()) {
                info.vertex_files.emplace_back(filename.string());
                ++count;
              }
            } else {
              if (group_name.empty()) {
                info.vertex_files.emplace_back(filename.string());
                ++count;
                search_v = false;
              }
            }
          }
        }
        if (!r && search_e) {
          std::tie(group_name, r) = CheckEdgeFilename(filename, info.name);
          if (r) {
            if (info.is_grouping) {
              if (!group_name.empty()) {
                info.edge_files.emplace_back(filename.string());
                ++count;
              }
            } else {
              if (group_name.empty()) {
                info.edge_files.emplace_back(filename.string());
                ++count;
                search_e = false;
              }
            }
          }
        }
      }
    }
  }

  if (!info.has_dict) info.check_graph = false;

  return count;
}

int CompleteWriteGraphPackageInfo(GraphPackageInfo &info) {
  namespace fs = std::filesystem;

  int count = 0;

  if (info.has_dict) {
    if (!info.dict_dir.empty()) {
      info.dict_dir = info.dict_dir.lexically_normal();
      if (!fs::is_directory(info.dict_dir) &&
          !fs::create_directories(info.dict_dir)) {
        std::cout << "Can't create directory: " << info.dict_dir.string() << std::endl;
        return -3;
      }
    }

    if (!info.name.empty()) {
      if (info.label_file.empty()) {
        info.label_file = GetLabelFilename(info.name);
        ++count;
      }

      if (info.attr_file.empty()) {
        info.attr_file = GetAttributeFilename(info.name);
        ++count;
      }

      if (info.type_file.empty()) {
        info.type_file = GetTypeFilename(info.name);
        ++count;
      }
    }
  }

  if (info.has_graph) {
    if (!info.graph_dir.empty()) {
      info.graph_dir = info.graph_dir.lexically_normal();
      if (!fs::is_directory(info.graph_dir) &&
          !fs::create_directories(info.graph_dir)) {
        std::cout << "Can't create directory: " << info.graph_dir.string() << std::endl;
        return -3;
      }
    }
    if (!info.is_grouping) info.has_skeleton = false;
  }

  return count;
}

}  // namespace graphpackage::csvgp