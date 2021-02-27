#ifndef _ATTRIBUTE_DICT_H
#define _ATTRIBUTE_DICT_H

#include <list>
#include <map>

#include "gpdef.h"

namespace graphpackage {

class AttributeDict {
 public:
  class AttributeInfo {
    friend class AttributeDict;

   public:
    class AttributeValueInfo {
     public:
      AttributeValueInfo(const std::string &vstr, const Label &label_id)
          : value_str(vstr), attr_value_label_id(label_id){};

      const std::string value_str;
      const Label attr_value_label_id;
      ID shared_attr_vertex_id = 0;
    };

   private:
    using Container = std::list<AttributeValueInfo>;
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

    AttributeInfo(const AttributeKey &key, const Label &label_id,
                  const std::string &tname, const DataType &vtype)
        : attr_key(key),
          attr_label_id(label_id),
          type_name(tname),
          value_data_type(vtype) {}

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
                                     const Label &label_id) {
      auto value_iter = value_index_.lower_bound(value_str);
      if (value_iter != value_index_.end() && value_iter->first == value_str)
        return {value_iter->second, false};

      auto label_iter = label_index_.find(label_id);
      if (label_iter != label_index_.end()) return {label_iter->second, false};

      iterator pos = (value_iter == value_index_.end()) ? container_.end()
                                                        : value_iter->second;

      iterator iter = container_.emplace(pos, value_str, label_id);

      value_index_.emplace(value_str, iter);
      label_index_.emplace(label_id, iter);

      return {iter, true};
    }

   public:
    const AttributeKey attr_key;
    const Label attr_label_id;
    const std::string type_name;
    const DataType value_data_type;

   private:
    Container container_;
    ValueIndex value_index_;
    LabelIndex label_index_;
  };

  using Container = std::list<AttributeInfo>;
  using KeyIndex = std::map<AttributeKey, typename Container::iterator>;
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
    key_index_.clear();
    label_index_.clear();
    container_.clear();
  }

  bool RegisterAttribute(const AttributeKey key, const Label &label_id,
                         const std::string &type_name,
                         const DataType &data_type) {
    KeyIndex::iterator key_iter = key_index_.lower_bound(key);
    if (key_iter != key_index_.end() && key_iter->first == key) return false;

    auto label_iter = label_index_.find(label_id);
    if (label_iter != label_index_.end()) return false;

    iterator pos =
        (key_iter == key_index_.end()) ? container_.end() : key_iter->second;

    iterator iter =
        container_.emplace(pos, key, label_id, type_name, data_type);

    key_index_.emplace(key, iter);
    label_index_.emplace(label_id, iter);

    return true;
  }

  bool RegisterAttributeValue(const AttributeKey key,
                              const std::string &value_str,
                              const Label &attr_value_label_id) {
    auto iter = Find(key);
    if (iter == end()) return false;

    if (!label_index_.emplace(attr_value_label_id, iter).second) return false;

    auto ret = iter->Insert(value_str, attr_value_label_id);
    if (!ret.second) return false;

    return true;
  }

  iterator Find(const AttributeKey &key) {
    auto iter = key_index_.find(key);
    if (iter == key_index_.end()) return container_.end();
    return iter->second;
  }

  const_iterator Find(const AttributeKey &key) const {
    auto iter = key_index_.find(key);
    if (iter == key_index_.end()) return container_.end();
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
  KeyIndex key_index_;
  LabelIndex label_index_;
};

inline std::string CheckAttributeName(const std::string &label_name) {
  auto pos = label_name.find("_a");
  if (pos + 2 != label_name.size()) return {};
  return label_name.substr(0, pos);
}

inline std::pair<std::string, std::string> CheckAttributeValueConstantLabelName(
    const std::string &label_name) {
  auto pos = label_name.find("_a_");
  if (pos == std::string::npos) return {{}, {}};
  return {label_name.substr(0, pos), label_name.substr(pos + 3)};
}

}  // namespace gt

#endif