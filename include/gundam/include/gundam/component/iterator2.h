#ifndef _GUNDAM_COMPONENT_ITERATOR2_H
#define _GUNDAM_COMPONENT_ITERATOR2_H

#include <cassert>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
namespace GUNDAM {

template <class IteratorType, class ValueType>
struct DefaultCast {
  constexpr ValueType &operator()(IteratorType &it) const { return *it; }
};

template <class IteratorType, class ValueType>
struct ReinterpretCast {
  constexpr ValueType &operator()(IteratorType &it) const {
    return *reinterpret_cast<ValueType *>(&*it);
  }
};

template <class IteratorType, class ValueType>
struct PointerCast {
  constexpr ValueType &operator()(IteratorType &it) const { return *(*it); }
};

template <class IteratorType, class ValueType>
struct PairFirstCast {
  constexpr ValueType &operator()(IteratorType &it) const { return it->first; }
};

template <class IteratorType, class ValueType>
struct PairSecondCast {
  constexpr ValueType &operator()(IteratorType &it) const { return it->second; }
};

template <class IteratorType, class ValueType>
struct PairFirstPointerCast {
  constexpr ValueType &operator()(IteratorType &it) const {
    return *(it->first);
  }
};

template <class IteratorType, class ValueType>
struct PairSecondPointerCast {
  constexpr ValueType &operator()(IteratorType &it) const {
    return *(it->second);
  }
};

template <class IteratorType, class ValueType,
          template <class _IteratorType, class _ValueType> class Cast =
              DefaultCast>
class GIterator {
 public:
  GIterator() = default;

  template <class Iter, class IterEnd>
  GIterator(Iter &&it, IterEnd &&end)
      : it_{std::forward<Iter>(it)}, end_{std::forward<IterEnd>(end)} {}

  ValueType &operator*() { return Cast<IteratorType, ValueType>()(it_); }

  const ValueType &operator*() const {
    return Cast<IteratorType, ValueType>()(it_);
  }

  ValueType *operator->() { return &Cast<IteratorType, ValueType>()(it_); }

  const ValueType *operator->() const {
    return &Cast<IteratorType, ValueType>(it_);
  }

  operator ValueType *() { return &Cast<IteratorType, ValueType>()(it_); }

  operator const ValueType *() const {
    return &Cast<IteratorType, ValueType>()(it_);
  }

  GIterator &operator++() {
    /// prefix
    assert(!this->IsDone());
    ++it_;
    return *this;
  }

  GIterator operator++(int) {
    /// postfix
    assert(!this->IsDone());
    GIterator tmp(*this);
    //++(*this);
    ++it_;
    return tmp;
  }

  bool IsDone() const { return it_ == end_; }

 private:
  IteratorType it_;
  IteratorType end_;
};

template <bool is_const, class GraphType, class IteratorType, class ValueType,
          class PointerType>
class GIterator2 {
 public:
  GIterator2() : graph_(nullptr),
                 it_(), end_(it_){
    assert(this->IsDone());
    return;
  }

  template <class GraphPtr, class Iter, class IterEnd>
  GIterator2(GraphPtr &&graph, Iter &&it, IterEnd &&end)
      : graph_(std::forward<GraphPtr>(graph)),
        it_(std::forward<Iter>(it)),
        end_(std::forward<IterEnd>(end)) {}

  ValueType operator*() { 
    assert(!this->IsDone());
    return ValueType(graph_, *it_); 
  }

  const ValueType operator*() const {
    assert(!this->IsDone()); 
    return ValueType(graph_, *it_); 
  }

  PointerType operator->() { 
    assert(!this->IsDone());
    return PointerType(ValueType(graph_, *it_)); 
  }

  const PointerType operator->() const {
    assert(!this->IsDone());
    return PointerType(ValueType(graph_, *it_));
  }

  operator PointerType() { 
    assert(!this->IsDone());
    return PointerType(ValueType(graph_, *it_)); 
  }

  operator const PointerType() const {
    assert(!this->IsDone());
    return PointerType(ValueType(graph_, *it_));
  }

  GIterator2 &operator++() {
    /// prefix
    assert(!this->IsDone());
    ++it_;
    return *this;
  }

  GIterator2 operator++(int) {
    /// postfix
    assert(!this->IsDone());
    GIterator2 tmp(*this);
    //++(*this);
    ++it_;
    return tmp;
  }

  bool operator==(const GIterator2 &b) const {
    assert(graph_);
    assert(b.graph_);
    assert(end_ == b.end_);
    return it_ == b.it_;
  }

  bool IsDone() const {
    // graph_ would be false at beginning if it is initilized by
    // GIterator2()
    assert(graph_ || it_ == end_);
    return it_ == end_;
  }

 private:
  typename std::conditional<is_const, const GraphType *, GraphType *>::type
      graph_;
  IteratorType it_;
  IteratorType end_;
};
template <bool is_const, class GraphType, class IteratorType, class ValueType,
          class PointerType>
class GIterator3 {
 public:
  GIterator3() = default;

  template <class GraphPtr, class Iter, class IterEnd>
  GIterator3(GraphPtr &&graph, Iter &&it, IterEnd &&end)
      : graph_(std::forward<GraphPtr>(graph)),
        it_(std::forward<Iter>(it)),
        end_(std::forward<IterEnd>(end)) {}

  ValueType operator*() { return ValueType(graph_, *(*it_)); }

  const ValueType operator*() const { return ValueType(graph_, *(*it_)); }

  PointerType operator->() {
    ;
    return PointerType(ValueType(graph_, *(*it_)));
  }

  const PointerType operator->() const {
    return PointerType(ValueType(graph_, *(*it_)));
  }

  operator PointerType() { return PointerType(ValueType(graph_, *(*it_))); }

  operator const PointerType() const {
    return PointerType(ValueType(graph_, *(*it_)));
  }

  GIterator3 &operator++() {
    /// prefix
    assert(!this->IsDone());
    ++it_;
    return *this;
  }

  GIterator3 operator++(int) {
    /// postfix
    assert(!this->IsDone());
    GIterator3 tmp(*this);
    //++(*this);
    ++it_;
    return tmp;
  }

  bool operator==(const GIterator3 &b) const {
    assert(graph_);
    assert(b.graph_);
    assert(end_ == b.end_);
    return it_ == b.it_;
  }

  bool IsDone() const {
    assert(graph_);
    return it_ == end_;
  }

 private:
  typename std::conditional<is_const, const GraphType *, GraphType *>::type
      graph_;
  IteratorType it_;
  IteratorType end_;
};
}  // namespace GUNDAM

#endif  // _ITERATOR_H