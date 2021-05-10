#ifndef _GUNDAM_COMPONENT_CONTAINER2_H
#define _GUNDAM_COMPONENT_CONTAINER2_H

#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

namespace GUNDAM {

template <class ValueType>
class TreeSet {
 private:
  using InnerContainerType = std::set<ValueType>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  std::pair<iterator, bool> Insert(VArgs&&... vargs) {
    return container_.emplace(std::forward<VArgs>(vargs)...);
  }

  iterator Find(const ValueType& value) { return container_.find(value); }

  const_iterator Find(const ValueType& value) const {
    return container_.find(value);
  }
  
  size_t Erase(const ValueType& value) { return container_.erase(value); }
  
  iterator Erase(const_iterator pos) { return container_.erase(pos); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <class ValueType>
class VectorSet {
 private:
  using InnerContainerType = std::vector<ValueType>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  std::pair<iterator, bool> Insert(VArgs&&... vargs) {
    ValueType value(std::forward<VArgs>(vargs)...);
    for (auto it = container_.begin(); it != container_.end(); ++it) {
      if (*it == value) return std::make_pair(it, false);
    }
    container_.emplace_back(std::move(value));
    return make_pair(container_.end() - 1, true);
  }

  iterator Find(const ValueType& value) {
    for (auto it = container_.begin(); it != container_.end(); ++it) {
      if (*it == value) return it;
    }
    return container_.end();
  }

  const_iterator Find(const ValueType& value) const {
    for (auto it = container_.cbegin(); it != container_.cend(); ++it) {
      if (*it == value) return it;
    }
    return container_.end();
  }

  size_t Erase(const ValueType& value) {
    auto it = Find(value);
    if (it == container_.end()) return 0;
    container_.erase(it);
    return 1;    
  }
  
  iterator Erase(const_iterator pos) { return container_.erase(pos); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <class ValueType>
class SortedVectorSet {
 private:
  using InnerContainerType = std::vector<ValueType>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  std::pair<iterator, bool> Insert(VArgs&&... vargs) {
    ValueType value(std::forward<VArgs>(vargs)...);
    auto it = std::lower_bound(container_.begin(), container_.end(), value);
    if (it != container_.end() && *it == value)
      return std::make_pair(it, false);

    return std::make_pair(container_.emplace(it, std::move(value)), true);
  }

  iterator Find(const ValueType& value) {
    auto it = std::lower_bound(container_.begin(), container_.end(), value);
    if (it == container_.end() || *it != value) return container_.end();
    return it;
  }

  const_iterator Find(const ValueType& value) const {
    auto it = std::lower_bound(container_.cbegin(), container_.cend(), value);
    if (it == container_.cend() || *it != value) return container_.cend();
    return it;
  }

  size_t Erase(const ValueType& value) {
    auto it = Find(value);
    if (it == container_.end()) return 0;
    container_.erase(it);
    return 1;
  }

  iterator Erase(iterator pos) { return container_.erase(pos); }

  iterator Erase(const_iterator pos) { return container_.erase(pos); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <typename KeyType, typename ValueType>
class TreeDict {
 private:
  using InnerContainerType = std::map<KeyType, ValueType>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  std::pair<iterator, bool> Insert(const KeyType& key, VArgs&&... vargs) {
    return container_.emplace(key, ValueType(std::forward<VArgs>(vargs)...));
  }

  iterator Find(const KeyType& key) { return container_.find(key); }

  const_iterator Find(const KeyType& key) const { return container_.find(key); }

  size_t Erase(const KeyType& key) { return container_.erase(key); }
  
  iterator Erase(const_iterator it) { return container_.erase(it); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <typename KeyType, typename ValueType>
class VectorDict {
 private:
  using InnerContainerType = std::vector<std::pair<KeyType, ValueType>>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  std::pair<iterator, bool> Insert(const KeyType& key, VArgs&&... vargs) {
    for (auto it = container_.begin(); it != container_.end(); ++it) {
      if (it->first == key) return std::make_pair(it, false);
    }
    container_.emplace_back(key, ValueType(std::forward<VArgs>(vargs)...));
    return make_pair(container_.end() - 1, true);
  }

  iterator Find(const KeyType& key) {
    for (auto it = container_.begin(); it != container_.end(); ++it) {
      if (it->first == key) return it;
    }
    return container_.end();
  }

  const_iterator Find(const KeyType& key) const {
    for (auto it = container_.cbegin(); it != container_.cend(); ++it) {
      if (it->first == key) return it;
    }
    return container_.cend();
  }

  size_t Erase(const KeyType& key) {
    auto it = Find(key);
    if (it == container_.end()) return 0;
    container_.erase(it);
    return 1;
  }

  iterator Erase(iterator pos) { return container_.erase(pos); }

  iterator Erase(const_iterator pos) { return container_.erase(pos); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <class KeyType, class ValueType>
class SortedVectorDict {
 private:
  using InnerContainerType = std::vector<std::pair<KeyType, ValueType>>;

  static bool Compare(const std::pair<KeyType, ValueType>& pair,
                      const KeyType& key) {
    return pair.first < key;
  }

 public:
  using value_type = typename InnerContainerType::value_type;

  using reference = typename InnerContainerType::reference;

  using const_reference = typename InnerContainerType::const_reference;

  using pointer = typename InnerContainerType::pointer;

  using const_pointer = typename InnerContainerType::const_pointer;

  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  std::pair<iterator, bool> Insert(const KeyType& key, VArgs&&... vargs) {
    auto it =
        std::lower_bound(container_.begin(), container_.end(), key, Compare);
    if (it != container_.end() && it->first == key)
      return std::make_pair(it, false);

    return std::make_pair(
        container_.emplace(it, key, ValueType(std::forward<VArgs>(vargs)...)),
        true);
  }

  iterator Find(const KeyType& key) {
    auto it =
        std::lower_bound(container_.begin(), container_.end(), key, Compare);
    if (it == container_.end() || it->first != key) return container_.end();
    return it;
  }

  const_iterator Find(const KeyType& key) const {
    auto it =
        std::lower_bound(container_.cbegin(), container_.cend(), key, Compare);
    if (it == container_.cend() || it->first != key)
      return container_.cend();
    return it;
  }

  size_t Erase(const KeyType& key) {
    auto it = Find(key);
    if (it == container_.end()) return 0;
    container_.erase(it);
    return 1;
  }

  iterator Erase(iterator pos) { return container_.erase(pos); }

  iterator Erase(const_iterator pos) { return container_.erase(pos); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <class ValueType>
class PosKeyVector {
 private:
  using InnerContainerType = std::vector<ValueType>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  PosKeyVector() = default;

  PosKeyVector(size_t size) { container_.reserve(size); }

  size_t Count() const { return container_.size(); }

  template <class... VArgs>
  iterator Insert(VArgs&&... vargs) {
    container_.emplace_back(std::forward<VArgs>(vargs)...);
    return container_.end() - 1;
  }

  iterator Find(size_t pos) {
    assert(pos < container_.size());
    return container_.begin() + pos;
  }

  const_iterator Find(size_t pos) const {
    assert(pos < container_.size());
    return container_.cbegin() + pos;
  }

  bool Empty() const { return container_.empty(); }

  void Clear() { container_.clear(); }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

  size_t GetPos(const const_iterator& it) const {
    return static_cast<size_t>(it - container_.cbegin());
  }

  size_t GetPos(const ValueType* p) const {
    return static_cast<size_t>(p - &*container_.cbegin());
  }

 private:
  InnerContainerType container_;
};

template <class BaseType>
class PointerVector {
 private:
  using InnerContainerType = std::vector<BaseType*>;

 public:
  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

  using value_type = typename InnerContainerType::value_type;

 public:
  ~PointerVector() { Clear(); }

  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }

  template <class DerivedType = BaseType, class... VArgs>
  void Add(VArgs&&... vargs) {
    BaseType* ptr = new DerivedType(std::forward<VArgs>(vargs)...);
    container_.push_back(ptr);
  }

  iterator Erase(iterator pos) {
    delete *pos;
    return container_.erase(pos);
  }

  iterator Erase(const_iterator pos) {
    delete *pos;
    return container_.erase(pos);
  }

  void Clear() {
    for (auto& ptr : container_) {
      delete ptr;
    }
    container_.clear();
  }

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

 private:
  InnerContainerType container_;
};

template <size_t index, class TupleType>
class GetTuple {
 public:
  constexpr const std::tuple_element_t<index, TupleType>& operator()(
      const TupleType& t) const noexcept {
    return std::get<index>(t);
  }
};

template <class ValueType, class KeyType1, class GetKey1, class KeyType2, class GetKey2>
class SortedVectorMultiKey {
 public:
  using InnerContainerType = std::vector<ValueType>;

  static constexpr bool CompareKey1L(const ValueType& value,
                                    const KeyType1& key1) {
    return GetKey1()(value) < key1;
  }

  static constexpr bool CompareKey1R(const KeyType1& key1,
                                    const ValueType& value) {
    return key1 < GetKey1()(value);
  }

  static constexpr bool CompareKey2L(const ValueType& value,
                                    const KeyType2& key2) {
    return GetKey2()(value) < key2;
  }

  static constexpr bool CompareKey2R(const KeyType2& key2,
                                    const ValueType& value) {
    return key2 < GetKey2()(value);
  }

 public:
  using value_type = typename InnerContainerType::value_type;

  using reference = typename InnerContainerType::reference;

  using const_reference = typename InnerContainerType::const_reference;

  using pointer = typename InnerContainerType::pointer;

  using const_pointer = typename InnerContainerType::const_pointer;

  using iterator = typename InnerContainerType::iterator;

  using const_iterator = typename InnerContainerType::const_iterator;

 public:
  bool Empty() const { return container_.empty(); }

  size_t Count() const { return container_.size(); }  

  template <class... VArgs>
  std::pair<iterator, bool> Insert(const KeyType1& key1, const KeyType2& key2,
                                   VArgs&&... vargs) {
    auto it_l = std::lower_bound(container_.begin(), container_.end(), key1,
                                 CompareKey1L);
    if (it_l != container_.end() && GetKey1()(*it_l) == key1) {
      auto it_u = std::upper_bound(it_l, container_.end(), key1, CompareKey1R);
      it_l = std::lower_bound(it_l, it_u, key2, CompareKey2L);
      if (it_l != container_.end() && GetKey2()(*it_l) == key2) {
        return std::make_pair(it_l, false);
      }
    }
    return std::make_pair(
        container_.emplace(it_l, std::forward<VArgs>(vargs)...), true);
  }

  //template <class V>
  //std::pair<iterator, bool> Insert(V&& value) {
  //  return Insert(GetKey1()(value), GetKey2(value), std::forward<V>(value));
  //}

  iterator Find(const KeyType1& key1, const KeyType2& key2) {
    auto p = Range(key1);
    if (p.second == p.first) return container_.end();
    auto it = std::lower_bound(p.first, p.second, key2, CompareKey2L);
    if (it == container_.end() || GetKey2()(*it) != key2) return container_.end();
    return it;
  }

  const_iterator Find(const KeyType1& key1, const KeyType2 &key2) const {
    auto p = CRange(key1);
    if (p.second == p.first) return container_.end();
    auto it = std::lower_bound(p.first, p.second, key2, CompareKey2L);
    if (it == container_.end() || GetKey2()(*it) != key2)
      return container_.end();
    return it;
  }  

  size_t Erase(const KeyType1& key1, const KeyType2& key2) {
    auto it = Find(key1, key2);
    if (it == container_.end()) return 0;
    container_.erase(it);
    return 1;
  }

  size_t Erase(const KeyType1& key1) {
    auto p = CRange(key1);
    size_t count = p.second - p.first;
    if (count > 0) container_.erase(p.first, p.second);
    return count;
  }

  iterator Erase(const_iterator pos) { return container_.erase(pos); }  

  void Clear() { container_.clear(); }  

  iterator begin() noexcept { return container_.begin(); }

  const_iterator begin() const noexcept { return container_.begin(); }

  const_iterator cbegin() const noexcept { return container_.cbegin(); }

  iterator end() noexcept { return container_.end(); }

  const_iterator end() const noexcept { return container_.cend(); }

  const_iterator cend() const noexcept { return container_.cend(); }

  private:
  template <class IterType, class KeyType, class PredL, class PredR>
  static constexpr std::pair<IterType, IterType> EqualRange(IterType first, IterType last,
                                           const KeyType& key, PredL pred_l,
                                           PredR pred_r) {
    auto count = std::distance(first, last);
    while (count > 0) {
      auto count2 = count / 2;
      const auto mid = std::next(first, count2);
      if (CompareKey1L(*mid, key)) {
        first = std::next(mid, 1);
        count -= count2 + 1;
      } else if (CompareKey1R(key, *mid)) {
        count = count2;
      } else {
        auto first2 = std::lower_bound(first, mid, key, pred_l);
        std::advance(first, count);
        auto last2 =
            std::upper_bound(std::next(mid, 1), last, key, pred_r);
        first = std::move(first2);
        last = std::move(last2);
        break;
      }
    }
    return {first, last};
  }

 public:
  std::pair<iterator, iterator> Range(const KeyType1& key1) {
    return EqualRange(container_.begin(), container_.end(), key1, CompareKey1L,
                      CompareKey1R);
  }

  std::pair<const_iterator, const_iterator> Range(const KeyType1& key1) const {
    return EqualRange(container_.begin(), container_.end(), key1, CompareKey1L,
                      CompareKey1R);
  }

  std::pair<const_iterator, const_iterator> CRange(const KeyType1& key1) const {
    return EqualRange(container_.cbegin(), container_.cend(), key1,
                      CompareKey1L, CompareKey1R);
  }

 private:
  InnerContainerType container_;
};

}  // namespace GUNDAM

#endif  // _CONTAINER2_H
