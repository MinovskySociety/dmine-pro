#ifndef _LABEL_DICT_H
#define _LABEL_DICT_H

#include <list>
#include <map>

#include "gpdef.h"

namespace graphpackage {

constexpr Label kReservedStart = 1;
constexpr Label kEntityStart = 1001;
constexpr Label kRelationStart = 2001;
constexpr Label kAttributeStart = 3001;
constexpr Label kValueStart = 4001;
constexpr Label kConstantStart = 1000001;
constexpr Label kValueConstantStart = 2000001;
constexpr Label kAttributeValueConstantStart = 1000000001;

class LabelDict {
 private:
  struct LabelInfo {
    template <class ULabel, class UString>
    LabelInfo(ULabel &&id, UString &&name, const LabelType &type)
        : label_id{std::forward<ULabel>(id)},
          label_name{std::forward<UString>(name)},
          label_type{type} {}

    const Label label_id;
    const std::string label_name;
    const LabelType label_type;
  };

  using InfoContainer = std::list<LabelInfo>;
  using IDIndex = std::map<Label, typename InfoContainer::iterator>;
  using NameIndex = std::map<std::string, typename InfoContainer::iterator>;

 public:
  using value_type = typename InfoContainer::value_type;
  using reference = typename InfoContainer::reference;
  using const_reference = typename InfoContainer::const_reference;
  using pointer = typename InfoContainer::pointer;
  using const_pointer = typename InfoContainer::const_pointer;
  using iterator = typename InfoContainer::iterator;
  using const_iterator = typename InfoContainer::const_iterator;

  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  void Clear() {
    id_index_.clear();
    name_index_.clear();
    container_.clear();
  }

  std::pair<iterator, bool> Insert(const Label &id, const std::string &name,
                                   const LabelType &type) {
    IDIndex::iterator id_iter = id_index_.lower_bound(id);
    if (id_iter != id_index_.end() && id_iter->first == id)
      return {id_iter->second, false};

    auto name_iter = name_index_.find(name);
    if (name_iter != name_index_.end()) return {name_iter->second, false};

    iterator pos =
        (id_iter == id_index_.end()) ? container_.end() : id_iter->second;

    iterator iter =
        container_.emplace(pos, id, name, type);

    id_index_.emplace(iter->label_id, iter);
    name_index_.emplace(iter->label_name, iter);

    return {iter, true};
  }

  std::pair<Label, bool> Register(const std::string &name, const LabelType &type) {
    auto name_iter = name_index_.find(name);
    if (name_iter != name_index_.end())
      return {name_iter->second->label_id, false};

    Label id;
    switch (type) {
      case LabelType::kEntity:
        id = kEntityStart;
        break;
      case LabelType::kRelation:
        id = kRelationStart;
        break;
      case LabelType::kAttribute:
        id = kAttributeStart;
        break;
      case LabelType::kValue:
        id = kValueStart;
        break;
      case LabelType::kConstant:
        id = kConstantStart;
        break;
      case LabelType::kValueConstant:
        id = kValueConstantStart;
        break;
      case LabelType::kAttributeValueConstant:
        id = kAttributeValueConstantStart;
        break;
      default:
        return {0, false};
    }
    
    auto id_iter = id_index_.lower_bound(id);
    while (id_iter != id_index_.end() && id_iter->second->label_type == type) {
      id = id_iter->first + 1;
      ++id_iter;
    }
    iterator pos =
        (id_iter == id_index_.end()) ? container_.end() : id_iter->second;

    auto iter =
        container_.emplace(pos, id, name, type);

    id_index_.emplace(iter->label_id, iter);
    name_index_.emplace(iter->label_name, iter);

    return {iter->label_id, true};
  }

  const_iterator Find(const Label &id) const {
    auto iter = id_index_.find(id);
    if (iter == id_index_.end()) return container_.end();
    return iter->second;
  }

  const_iterator Find(const std::string &name) const {
    auto iter = name_index_.find(name);
    if (iter == name_index_.end()) return container_.end();
    return iter->second;
  }

  Label GetLabelID(const std::string &name) const {
    auto iter = name_index_.find(name);
    if (iter == name_index_.end()) return -1;
    return name_index_.at(name)->label_id;
  }

  std::string GetLabelName(const Label &id) const {
    auto iter = id_index_.find(id);
    if (iter == id_index_.end()) return "";
    return iter->second->label_name;
  }

  LabelType GetLabelType(const Label &id) const {
    auto iter = id_index_.find(id);
    if (iter == id_index_.end()) return LabelType::kUnknown;
    return iter->second->label_type;
  }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.end(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InfoContainer container_;
  IDIndex id_index_;
  NameIndex name_index_;
};

}  // namespace gt

#endif