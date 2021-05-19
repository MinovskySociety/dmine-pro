#ifndef _GRAPH_PACKAGE_H
#define _GRAPH_PACKAGE_H

#include <graph_package/attribute_dict.h>
#include <graph_package/gpdef.h>
#include <graph_package/label_dict.h>
#include <graph_package/type_dict.h>

#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace graphpackage {

struct GraphPackageInfo {
  void Reset() {
    name.clear();
    dict_dir.clear();
    graph_dir.clear();
    label_file.clear();
    attr_file.clear();
    type_file.clear();
    vertex_files.clear();
    edge_files.clear();
    has_dict = true;
    has_graph = true;
    has_attr = true;
    is_grouping = true;
    has_skeleton = true;
    check_graph = true;
  }

  std::string name;
  std::filesystem::path dict_dir;
  std::filesystem::path graph_dir;
  std::filesystem::path label_file;
  std::filesystem::path attr_file;
  std::filesystem::path type_file;
  std::vector<std::filesystem::path> vertex_files;
  std::vector<std::filesystem::path> edge_files;
  bool has_dict = true;
  bool has_graph = true;
  bool has_attr = true;
  bool is_grouping = true;
  bool has_skeleton = true;
  bool check_graph = true;
};

template <class Graph>
class GraphPackage {
 public:
  GraphPackageInfo &info() { return info_; };

  const GraphPackageInfo &info() const { return info_; };

  LabelDict &label_dict() { return label_dict_; }

  const LabelDict &label_dict() const { return label_dict_; }

  AttributeDict &attr_dict() { return attr_dict_; }

  const AttributeDict &attr_dict() const { return attr_dict_; }

  TypeDict &type_dict() { return type_dict_; }

  const TypeDict &type_dict() const { return type_dict_; }

  Graph &graph() { return graph_; }

  const Graph &graph() const { return graph_; }

  IDGen &vertex_id_gen() { return vertex_id_gen_; }

  IDGen &edge_id_gen() { return edge_id_gen_; }

  void Reset() {
    info_.Reset();
    label_dict_.Clear();
    attr_dict_.Clear();
    type_dict_.Clear();
    graph_.Clear();
    vertex_id_gen_.Reset();
    edge_id_gen_.Reset();
  }

  void ResetDict() {
    label_dict_.Clear();
    attr_dict_.Clear();
    type_dict_.Clear();

    label_dict_.Insert(ReservedLabel::kSource, "source", LabelType::kRelation);
    label_dict_.Insert(ReservedLabel::kTarget, "target", LabelType::kRelation);
    label_dict_.Insert(ReservedLabel::kHas, "has", LabelType::kRelation);
    label_dict_.Insert(ReservedLabel::kIs, "is", LabelType::kRelation);
    label_dict_.Insert(ReservedLabel::kEqual, "equal", LabelType::kRelation);
  }

  std::pair<Label, bool> RegisterEntity(const std::string &label_name) {
    return RegisterLabel(label_name, LabelType::kEntity);
  }

  std::pair<Label, bool> RegisterRelation(const std::string &label_name) {
    return RegisterLabel(label_name, LabelType::kEntity);
  }

  int RegisterType(const std::string &type_name,
                   const DataType &value_data_type,
                   const std::vector<std::string> &all_value_str) {
    int count = 0;

    // Register TypeDict
    // Value : 1
    auto [value_label_id, r] =
        RegisterLabel(type_name + "_v", LabelType::kValue);
    if (!r) return -1;
    ++count;

    r = type_dict_.RegisterType(type_name, value_label_id, value_data_type);
    if (!r) return -1;

    for (const auto &value_str : all_value_str) {
      Label constant_label_id, value_constant_label_id;

      // Constant : n
      std::tie(constant_label_id, r) =
          RegisterLabel(type_name + "_c_" + value_str, LabelType::kConstant);
      if (!r) return -1;
      ++count;

      // ValueConstant : n
      std::tie(value_constant_label_id, r) = RegisterLabel(
          type_name + "_v_" + value_str, LabelType::kValueConstant);
      if (!r) return -1;
      ++count;

      r = type_dict_.RegisterTypeValue(type_name, value_str, constant_label_id,
                                       value_constant_label_id);
      if (!r) return -1;
    }

    return count;
  }

  int RegisterTypeValue(const std::string &type_name,
                        const std::string &value_str);

  int RegisterBoolType() {
    std::vector<std::string> value_str{"0", "1"};
    return RegisterType("bool", DataType::kTypeInt, value_str);
  }

  int RegisterIntType(int min, int max) {
    if (min > max) return -1;
    std::vector<std::string> value_str;
    for (int i = min; i <= max; ++i) {
      value_str.push_back(std::to_string(i));
    }
    return RegisterType("int", DataType::kTypeInt, value_str);
  }

  int RegisterAttribute(const AttributeKey &attr_key,
                        const std::string &type_name,
                        const std::vector<std::string> &all_attr_value_str) {
    int count = 0;

    auto type_iter = type_dict_.Find(type_name);
    if (type_iter == type_dict_.end()) return -1;
    const auto &type_info = *type_iter;

    // AttrDict

    // Attribute : 1
    auto [attr_label_id, r] = RegisterLabel(attr_key, LabelType::kAttribute);
    if (!r) return -1;
    ++count;

    r = attr_dict_.RegisterAttribute(attr_key, attr_label_id, type_name,
                                     type_info.value_data_type);
    if (!r) return -1;

    for (const std::string &value_str : all_attr_value_str) {
      auto it_tv = type_info.Find(value_str);
      if (it_tv == type_info.end()) return -1;

      // AttributeValueConstant : n
      auto [attr_value_label_id, r] = RegisterLabel(
          attr_key + "_a_" + value_str, LabelType::kAttributeValueConstant);
      if (!r) return -1;
      ++count;

      r = attr_dict_.RegisterAttributeValue(attr_key, value_str,
                                            attr_value_label_id);
      if (!r) return -1;
    }

    return count;
  }

  int RegisterAttributeValue(const AttributeKey &attr_key,
                             const std::string &value_str);

  int RegisterGraphLabels() {
    int count = 0;

    std::set<Label> label_set;

    for (auto it_v = graph_.VertexBegin(); !it_v.IsDone(); ++it_v) {
      const auto &label = it_v->label();
      if (!label_set.emplace(label).second) continue;

      auto [it_label, r] =
          label_dict_.Insert(label, std::to_string(label), LabelType::kEntity);
      if (r) ++count;
    }

    for (auto it_e = graph_.EdgeBegin(); !it_e.IsDone(); ++it_e) {
      const auto &label = it_e->label();
      if (!label_set.emplace(label).second) continue;

      auto [it_label, r] = label_dict_.Insert(label, std::to_string(label),
                                              LabelType::kRelation);
      if (r) ++count;
    }

    return count;
  }

  int RegisterGraphAttribute(const std::set<Label> &label_set,
                             const AttributeKey &attr_key,
                             const std::string &type_name) {
    int res;

    auto type_iter = type_dict_.Find(type_name);
    if (type_iter == type_dict_.end()) return -1;
    const auto &type_info = *type_iter;

    DataType value_data_type;
    std::set<std::string> all_attr_value_str;
    res = GetGraphAttributeValues(label_set, attr_key, value_data_type,
                                  all_attr_value_str);
    if (res <= 0) return res;
    if (value_data_type != type_info.value_data_type) return -1;

    return RegisterAttribute(
        attr_key, type_name,
        {all_attr_value_str.begin(), all_attr_value_str.end()});
  }

  int RegisterGraphAttributeAndValues(const std::set<Label> &label_set,
                                      const AttributeKey &attr_key) {
    int res;

    DataType value_data_type;
    std::set<std::string> all_attr_value_str;
    res = GetGraphAttributeValues(label_set, attr_key, value_data_type,
                                  all_attr_value_str);
    if (res < 0) return res;

    int count = 0;

    // Register type
    std::vector<std::string> all_value_str{all_attr_value_str.begin(),
                                           all_attr_value_str.end()};
    res = RegisterType(attr_key, value_data_type, all_value_str);
    if (res < 0) return res;
    count += res;

    // Register attribute
    res = RegisterAttribute(attr_key, attr_key, all_value_str);
    if (res < 0) return res;
    count += res;

    return count;
  }

  // int RegisterGraphAttribute(const std::vector<std::string> &label_name_list,
  //                           const AttributeKey &attr_key,
  //                           const std::string &type_name) {
  //  std::set<Label> label_set;
  //  for (const auto &label_name : label_name_list) {
  //    auto it_label = label_dict_.Find(label_name);
  //    if (it_label == label_dict_.end()) return -1;
  //    auto [it, r] = label_set.emplace(it_label->label_id);
  //    if (!r) return -1;
  //  }

  //  return RegisterGraphAttribute(label_set, attr_key, type_name);
  //}

  // int RegisterGraphAttributeAndValues(const std::vector<std::string>
  // &label_set,
  //                                    const AttributeKey &attr_key) {
  //  std::set<Label> label_set;
  //  for (const auto &label_name : label_name_list) {
  //    auto it_label = label_dict_.Find(label_name);
  //    if (it_label == label_dict_.end()) return -1;
  //    auto [it, r] = label_set.emplace(it_label->label_id);
  //    if (!r) return -1;
  //  }

  //  return RegisterGraphAttributeAndValues(label_set, attr_key);
  //}

 private:
  std::pair<Label, bool> RegisterLabel(const std::string &label_name,
                                       const LabelType &label_type) {
    return label_dict_.Register(label_name, label_type);
  }

  int GetGraphAttributeValues(const std::set<Label> &label_set,
                              const AttributeKey &attr_key,
                              DataType &value_data_type,
                              std::set<std::string> &all_attr_value_str) {
    value_data_type = DataType::kTypeUnknown;
    all_attr_value_str.clear();

    int count = 0;

    for (auto it_v = graph_.VertexBegin(); !it_v.IsDone(); ++it_v) {
      if (!label_set.empty() && label_set.count(it_v->label()) == 0) continue;

      auto it_attr = it_v->FindAttribute(attr_key);
      if (it_attr.IsNull()) continue;

      if (all_attr_value_str.emplace(it_attr->value_str()).second) {
        ++count;
      }

      if (value_data_type == DataType::kTypeUnknown)
        value_data_type = it_attr->value_type();

      if (value_data_type != it_attr->value_type()) {
        return -1;
      }
    }

    // for (auto it_e = graph_.EdgeBegin(); !it_e.IsDone(); ++it_e) {
    // modified by wenzhi
    for (auto it_v = graph_.VertexBegin(); !it_v.IsDone(); ++it_v) {
      for (auto it_e = it_v->OutEdgeBegin(); !it_e.IsDone(); ++it_e) {
        if (!label_set.empty() && label_set.count(it_e->label()) == 0) continue;

        auto it_attr = it_e->FindAttribute(attr_key);
        if (it_attr.IsNull()) continue;

        if (all_attr_value_str.emplace(it_attr->value_str()).second) {
          ++count;
        }

        if (value_data_type == DataType::kTypeUnknown)
          value_data_type = it_attr->value_type();

        if (value_data_type != it_attr->value_type()) {
          return -1;
        }
      }
    }

    return count;
  }

 private:
  GraphPackageInfo info_;
  LabelDict label_dict_;
  AttributeDict attr_dict_;
  TypeDict type_dict_;
  Graph graph_;
  IDGen vertex_id_gen_;
  IDGen edge_id_gen_;
};

}  // namespace graphpackage

#endif