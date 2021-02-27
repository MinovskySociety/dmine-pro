#ifndef _TYPE_DICT_H
#define _TYPE_DICT_H

#include <list>
#include <map>

#include "gpdef.h"

namespace graphpackage {

class TypeDict {
 private:
  class TypeInfo {
   private:
    friend class TypeDict;

    class TypeValueInfo {
     public:
      TypeValueInfo(const std::string &vstr, const Label &c_label,
                    const Label &vc_label)
          : value_str(vstr),
            constant_label_id(c_label),
            value_constant_label_id(vc_label){};

      const std::string value_str;
      const Label constant_label_id;
      const Label value_constant_label_id;
      ID value_vertex_id = 0;
      // ID constant_vertex_id = 0;
    };

    using Container = std::list<TypeValueInfo>;
    using ValueIndex = std::map<std::string, typename Container::iterator>;
    using LabelIndex = std::map<Label, typename Container::iterator>;

   public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using pointer = typename Container::pointer;
    using const_pointer = typename Container::const_pointer;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;

    TypeInfo(const std::string &type_name, const Label &value_label,
             const DataType &value_data_type)
        : type_name(type_name),
          value_label_id(value_label),
          value_data_type(value_data_type) {}

    bool Empty() const { return container_.empty(); }

    size_t Count() const { return container_.size(); }

    iterator Find(const std::string &value_str) {
      auto iter = value_index_.find(value_str);
      if (iter == value_index_.end()) return container_.end();
      return iter->second;
    }

    const_iterator Find(const std::string &value_str) const {
      auto iter = value_index_.find(value_str);
      if (iter == value_index_.end()) return container_.end();
      return iter->second;
    }

    iterator Find(const Label &label_id) {
      auto iter = label_index_.find(label_id);
      if (iter == label_index_.end()) return container_.end();
      return iter->second;
    }

    const_iterator Find(const Label &label_id) const {
      auto iter = label_index_.find(label_id);
      if (iter == label_index_.end()) return container_.end();
      return iter->second;
    }

    iterator begin() noexcept { return container_.begin(); }

    const_iterator begin() const noexcept { return container_.begin(); }

    const_iterator cbegin() const noexcept { return container_.cbegin(); }

    iterator end() noexcept { return container_.end(); }

    const_iterator end() const noexcept { return container_.end(); }

    const_iterator cend() const noexcept { return container_.cend(); }

   private:
    std::pair<iterator, bool> Insert(const std::string &value_str,
                                     const Label &constant_label_id,
                                     const Label &value_constant_label_id) {
      auto value_iter = value_index_.lower_bound(value_str);
      if (value_iter != value_index_.end() && value_iter->first == value_str)
        return {value_iter->second, false};

      auto label_iter1 = label_index_.find(constant_label_id);
      if (label_iter1 != label_index_.end())
        return {label_iter1->second, false};

      auto label_iter2 = label_index_.find(value_constant_label_id);
      if (label_iter2 != label_index_.end())
        return {label_iter2->second, false};

      iterator pos = (value_iter == value_index_.end()) ? container_.end()
                                                        : value_iter->second;

      iterator iter = container_.emplace(pos, value_str, constant_label_id,
                                         value_constant_label_id);

      value_index_.emplace(value_str, iter);
      label_index_.emplace(constant_label_id, iter);
      label_index_.emplace(value_constant_label_id, iter);

      return {iter, true};
    }

   public:
    const std::string type_name;
    const Label value_label_id;
    const DataType value_data_type;

   private:
    Container container_;
    ValueIndex value_index_;
    LabelIndex label_index_;
  };

  using Container = std::list<TypeInfo>;
  using NameIndex = std::map<std::string, typename Container::iterator>;
  using LabelIndex = std::map<Label, typename Container::iterator>;

 public:
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;

  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  void Clear() {
    name_index_.clear();
    label_index_.clear();
    container_.clear();
  }

  bool RegisterType(const std::string &name, const Label &value_label_id,
                    const DataType &value_data_type) {
    auto name_iter = name_index_.lower_bound(name);
    if (name_iter != name_index_.end() && name_iter->first == name)
      return false;

    auto label_iter = label_index_.find(value_label_id);
    if (label_iter != label_index_.end()) return false;

    iterator pos =
        (name_iter == name_index_.end()) ? container_.end() : name_iter->second;

    iterator iter =
        container_.emplace(pos, name, value_label_id, value_data_type);

    name_index_.emplace(name, iter);
    label_index_.emplace(value_label_id, iter);

    return true;
  }

  bool RegisterTypeValue(const std::string &name, const std::string &value_str,
                         const Label &constant_label_id,
                         const Label &value_constant_label_id) {
    auto iter = Find(name);
    if (iter == end()) return false;

    if (!label_index_.emplace(constant_label_id, iter).second) return false;

    if (!label_index_.emplace(value_constant_label_id, iter).second)
      return false;

    auto ret =
        iter->Insert(value_str, constant_label_id, value_constant_label_id);
    if (!ret.second) return false;

    return true;
  }

  iterator Find(const std::string &name) {
    auto iter = name_index_.find(name);
    if (iter == name_index_.end()) return container_.end();
    return iter->second;
  }

  const_iterator Find(const std::string &name) const {
    auto iter = name_index_.find(name);
    if (iter == name_index_.end()) return container_.end();
    return iter->second;
  }

  iterator Find(const Label &label_id) {
    auto iter = label_index_.find(label_id);
    if (iter == label_index_.end()) return container_.end();
    return iter->second;
  }

  const_iterator Find(const Label &label_id) const {
    auto iter = label_index_.find(label_id);
    if (iter == label_index_.end()) return container_.end();
    return iter->second;
  }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.end(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  Container container_;
  NameIndex name_index_;
  LabelIndex label_index_;
};

inline std::string CheckTypeName(const std::string &label_name) {
  auto pos = label_name.find("_a");
  if (pos + 2 != label_name.size()) return {};
  return label_name.substr(0, pos);
}

inline std::pair<std::string, std::string> CheckTypeValueConstantLabelName(
    const std::string &label_name) {
  auto pos = label_name.find("_a_");
  if (pos == std::string::npos) return {{}, {}};
  return {label_name.substr(0, pos), label_name.substr(pos + 3)};
}

inline std::string CheckValueLabelName(const std::string &label_name) {
  auto pos = label_name.find("_v");
  if (pos == std::string::npos || pos + 2 != label_name.size()) return {};
  return label_name.substr(0, pos);
}

inline std::pair<std::string, std::string> CheckValueConstantLabelName(
    const std::string &label_name) {
  auto pos = label_name.find("_v_");
  if (pos == std::string::npos) return {{}, {}};
  return {label_name.substr(0, pos), label_name.substr(pos + 3)};
}

inline std::pair<std::string, std::string> CheckConstantLabelName(
    const std::string &label_name) {
  auto pos = label_name.find("_c_");
  if (pos == std::string::npos) return {{}, {}};
  return {label_name.substr(0, pos), label_name.substr(pos + 3)};
}

}  // namespace gt

#endif