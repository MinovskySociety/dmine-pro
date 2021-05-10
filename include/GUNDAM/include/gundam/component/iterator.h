#ifndef _GUNDAM_COMPONENT_ITERATOR_H
#define _GUNDAM_COMPONENT_ITERATOR_H

#include "gundam/component/container.h"

namespace GUNDAM {

using IteratorDepthType = int8_t;

template <typename  ContainerType_, 
          bool           is_const_, 
          IteratorDepthType depth_>
class InnerIterator_;

template <typename      ContainerType_, 
          bool               is_const_, 
          IteratorDepthType     depth_,
          IteratorDepthType now_depth_>
class _InnerIterator_;

/// the iterator of the container that only has one layer
template <enum ContainerType container_type_, 
          enum      SortType      sort_type_,
          typename KeyType_, 
          bool    is_const_>
class InnerIterator_<Container<container_type_, 
                                    sort_type_, 
                                      KeyType_>,
                     is_const_, 1> {
 private:
  using ContainerType = Container<container_type_, 
                                       sort_type_, 
                                         KeyType_>;
  using ElementType = typename ContainerType::ElementType;

 protected:
  using IteratorType =
      typename std::conditional<is_const_,
        typename ContainerType::const_iterator,
        typename ContainerType::iterator>::type;

 private:
  IteratorType iterator_;
  IteratorType iterator_end_;

 protected:
  inline const IteratorType& ConstInnerIterator() const {
    return this->iterator_;
  }

 public:
  InnerIterator_() : iterator_(), 
                     iterator_end_(iterator_) {
    /// default iterator, return an empty iterator
    assert(this->IsDone());
    return;
  }

  InnerIterator_(const IteratorType& iterator_begin,
                 const IteratorType& iterator_end)
                     : iterator_    (iterator_begin), 
                       iterator_end_(iterator_end) {
    return;
  }

 protected:
  inline bool IsDone() const { 
    return this->iterator_ == this->iterator_end_; 
  }

  inline void ToNext() {
    assert(!this->IsDone());
    this->iterator_++;
    return;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline auto& get_iterator() {
    return this->iterator_;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline const auto& get_iterator() const {
    return this->template get_const_iterator<depth>();
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline const auto& get_const_iterator() const {
    return this->iterator_;
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline auto& get() {
    return (this->iterator_.template get<return_idx>());
  }
  
  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline const auto& get() const {
    return this->template get_const<return_idx,
                                        depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline const auto& get_const() const {
    return (this->iterator_.template get_const<return_idx>());
  }
  
  inline IteratorType& iterator_end(){
    return this->iterator_end_;
  }
};

/// the iterator of the container that only has one layer and more valuetype
template <enum ContainerType container_type_, 
          enum      SortType      sort_type_,
          typename                  KeyType_, 
          typename               ValueType1_, 
          typename...            ValueType2_,
          bool                     is_const_>
class InnerIterator_<Container<container_type_, 
                                    sort_type_, 
                                      KeyType_,
                                   ValueType1_, 
                                   ValueType2_...>,
                     is_const_, 1> {
 private:
  using ContainerType = Container<container_type_, 
                                       sort_type_, 
                                         KeyType_,
                                       ValueType1_, 
                                       ValueType2_...>;

  using ElementType = typename ContainerType::ElementType;

 protected:
  using IteratorType =
      typename std::conditional<is_const_,
        typename ContainerType::const_iterator,
        typename ContainerType::iterator>::type;

 private:
  IteratorType iterator_;
  IteratorType iterator_end_;

 protected:
  inline const IteratorType& ConstInnerIterator() const {
    return this->iterator_;
  }

 public:
  InnerIterator_() : iterator_(), 
                     iterator_end_(iterator_) {
    /// default iterator, return an empty iterator
    assert(this->IsDone());
    return;
  }
  InnerIterator_(const IteratorType& iterator_begin,
                 const IteratorType& iterator_end)
                     : iterator_    (iterator_begin), 
                       iterator_end_(iterator_end) {
    return;
  }

  // protected:
  inline bool IsDone() const { 
    return this->iterator_ == this->iterator_end_; 
  }

  inline void ToNext() {
    assert(!this->IsDone());
    this->iterator_++;
    return;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline auto& get_iterator() {
    return this->iterator_;
  }
  
  template <IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline const auto& get_iterator() const {
    return this->template get_const_iterator<depth>();
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == 0, 
                             bool> = false>
  inline const auto& get_const_iterator() const {
    return this->iterator_;
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            typename std::enable_if<depth == 0, 
                                    bool>::type = false>
  inline auto& get() {
    return (this->iterator_.template get<return_idx>());
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            typename std::enable_if<depth == 0, 
                                    bool>::type = false>
  inline const auto& get() const {
    return this->template get_const<return_idx,
                                         depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            typename std::enable_if<depth == 0, 
                                    bool>::type = false>
  inline const auto& get_const() const {
    return (this->iterator_.template get_const<return_idx>());
  }

  inline IteratorType& iterator_end(){
    return this->iterator_end_;
  }
};

/// iterator of the container that has more than one layer
template <enum ContainerType container_type_, 
          enum      SortType      sort_type_,
          typename                  KeyType_, 
          typename       InnerContainerType_,
          typename...             ValueTypes_, 
          bool is_const_, 
          IteratorDepthType depth_>
class InnerIterator_<Container<container_type_, 
                                    sort_type_, 
                                      KeyType_,
                           InnerContainerType_, 
                                    ValueTypes_...>,
                     is_const_, depth_>
    : protected _InnerIterator_<InnerContainerType_, 
                                          is_const_, 
                                             depth_, 2> {
  static_assert(depth_ >= 2, "Illegal depth");

 private:
  using ContainerType = Container<container_type_, 
                                       sort_type_, 
                                         KeyType_,
                              InnerContainerType_, 
                                       ValueTypes_...>;
  using ElementType = typename ContainerType::ElementType;

 protected:
  using IteratorType =
      typename std::conditional<is_const_,
        typename ContainerType::const_iterator,
        typename ContainerType::iterator>::type;

 private:
  static constexpr TupleIdxType kKeyIdx_ = 0;
  static constexpr TupleIdxType kInnerContainerIdx_ = 1;

  IteratorType iterator_;
  IteratorType iterator_end_;

 protected:
  inline const IteratorType& ConstInnerIterator() const {
    return this->iterator_;
  }

  using InnerContainerType = InnerContainerType_;
  using InnerIteratorType =
       _InnerIterator_<InnerContainerType_, is_const_, depth_, 2>;

  inline static auto inner_iterator_begin(
      const IteratorType& container_iterator) {
    if constexpr (is_const_)
      return (container_iterator.template get_const<kInnerContainerIdx_>()).cbegin();  
    if constexpr (!is_const_)
      return (container_iterator.template get<kInnerContainerIdx_>()).begin();
  }
  
  inline static auto inner_iterator_end(
      const IteratorType& container_iterator) {
    if constexpr (is_const_)
      return (container_iterator.template get_const<kInnerContainerIdx_>()).cend();
    if constexpr (!is_const_)
      return (container_iterator.template get<kInnerContainerIdx_>()).end();
  }

  inline bool InnerIsDone() const {
    return InnerIteratorType::const_iterator() ==
           inner_iterator_end(this->iterator_);
  }

 public:
  InnerIterator_() : iterator_(), 
                     iterator_end_(iterator_) {
    /// default iterator, return an empty iterator
    assert(this->IsDone());
    return;
  }
  
  InnerIterator_(const IteratorType& iterator_begin,
                 const IteratorType& iterator_end)
      : iterator_    (iterator_begin), 
        iterator_end_(iterator_end ) {
    if (iterator_begin != iterator_end) {
      /// the next layer iterator is not empty
      InnerIteratorType::set_iterator(
         InnerIterator_::inner_iterator_begin(iterator_begin));
      InnerIteratorType::set_lower_iterator();
    }
    return;
  }

 public:
  inline bool IsDone() const { 
    return this->iterator_ == this->iterator_end_; 
  }

  inline void ToNext() {
    assert(!this->IsDone());
    assert(!this->InnerIsDone());
    if (!InnerIteratorType::_ToNext()) {
      /// this inner iterator has not moved
      return;
    }
    /// the inner iterator has moved
    if (!this->InnerIsDone()) {
      InnerIteratorType::set_lower_iterator();
      return;
    }
    /// the inner iterator is done
    this->iterator_++;
    if (this->IsDone()) return;
    InnerIteratorType::set_iterator(
       InnerIterator_::inner_iterator_begin(this->iterator_));
    InnerIteratorType::set_lower_iterator();
    return;
  }

  template <IteratorDepthType depth>
  inline auto& get_iterator() {
    if constexpr (depth == 0)
      return this->iterator_;
    if constexpr (depth != 0)
      return InnerIteratorType::template get_iterator<depth>();
  }
  
  template <IteratorDepthType depth>
  inline const auto& get_iterator() const {
    return this->template get_const_iterator<depth>();
  }

  template <IteratorDepthType depth>
  inline const auto& get_const_iterator() const {
    if constexpr (depth == 0)
      return this->iterator_;
    if constexpr (depth != 0)
      return InnerIteratorType::template get_const_iterator<depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth>
  inline auto& get() {
    if constexpr (depth == 0)
      return (this->iterator_.template get<return_idx>());
    if constexpr (depth != 0)
      return InnerIteratorType::template get<return_idx, 
                                                  depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth>
  inline const auto& get() const {
    return this->template get_const<return_idx, depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth>
  inline const auto& get_const() const {
    if constexpr (depth == 0)
      return (this->iterator_.template get_const<return_idx>());
    if constexpr (depth != 0)
      return InnerIteratorType::template get_const<return_idx,
                                                  depth>();
  }
  
  inline IteratorType& iterator_end(){
    return this->iterator_end_;
  }
};

template <enum ContainerType container_type_, 
          enum SortType           sort_type_,
          typename                  KeyType_, 
          typename       InnerContainerType_,
          typename...             ValueTypes_, 
          bool               is_const_, 
          IteratorDepthType     depth_,
          IteratorDepthType now_depth_>
class _InnerIterator_<Container<container_type_, 
                                     sort_type_, 
                                       KeyType_,
                            InnerContainerType_, 
                                     ValueTypes_...>,
                      is_const_, 
                         depth_, 
                     now_depth_>
    : protected _InnerIterator_<InnerContainerType_, 
                                          is_const_, 
                                             depth_,
                                         now_depth_ + 1> {
  static_assert((now_depth_ > 1) 
             && (now_depth_ < depth_), "Illegal depth");

 private:
  static constexpr TupleIdxType kKeyIdx_ = 0;
  static constexpr TupleIdxType kInnerContainerIdx_ = 1;

  using ContainerType = Container<container_type_, 
                                       sort_type_, 
                                         KeyType_,
                              InnerContainerType_,
                                       ValueTypes_...>;
  using ElementType = typename ContainerType::ElementType;
  using IteratorType =
      typename std::conditional<is_const_,
        typename ContainerType::const_iterator,
        typename ContainerType::iterator>::type;

  using InnerContainerType = InnerContainerType_;
  using InnerIteratorType =
      _InnerIterator_<InnerContainerType_, 
                                is_const_, 
                                   depth_, 
                               now_depth_ + 1>;
  IteratorType iterator_;

  inline static auto inner_iterator_begin(const IteratorType& container_iterator) {
    if constexpr (is_const_)
      return (container_iterator.template get_const<kInnerContainerIdx_>()).cbegin();
    if constexpr (!is_const_)
      return (container_iterator.template get<kInnerContainerIdx_>()).begin();
  }

  inline static auto inner_iterator_end(const IteratorType& container_iterator) {
    if constexpr (is_const_)
      return (container_iterator.template get_const<kInnerContainerIdx_>()).cend();
    if constexpr (!is_const_)
      return (container_iterator.template get<kInnerContainerIdx_>()).end();
  }

  inline bool InnerIsDone() const {
    return InnerIteratorType::const_iterator() 
        == inner_iterator_end(this->iterator_);
  }

 protected:
  _InnerIterator_() = default;

  inline const IteratorType& const_iterator() const { 
    return this->iterator_; 
  }

  inline void set_iterator(const IteratorType& new_iterator) {
    this->iterator_ = new_iterator;
    return;
  }

  inline void set_lower_iterator() {
    InnerIteratorType::set_iterator(inner_iterator_begin(this->iterator_));
    InnerIteratorType::set_lower_iterator();
    return;
  }

  /// return whether iterator_ has moved
  inline bool _ToNext() {
    assert(!this->InnerIsDone());
    if (!InnerIteratorType::_ToNext()) {
      /// this inner iterator has not moved
      return false;  /// iterator_ has not moved
    }
    /// the inner iterator has moved
    if (!this->InnerIsDone()) {
      InnerIteratorType::set_lower_iterator();
      return false;  /// iterator_ has not moved
    }
    /// the inner iterator is done
    this->iterator_++;
    return true;
  }

  template <IteratorDepthType depth>
  inline auto& get_iterator() {
    if constexpr (depth == now_depth_ - 1)
      return this->iterator_;
    if constexpr (depth != now_depth_ - 1)
      return InnerIteratorType
           ::template get_iterator<depth>();
  }

  template <IteratorDepthType depth>
  inline const auto& get_iterator() const {
    return this->template get_const_iterator<depth>();
  }

  template <IteratorDepthType depth>
  inline const auto& get_const_iterator() const {
    if constexpr (depth == now_depth_ - 1)
      return this->iterator_;
    if constexpr (depth != now_depth_ - 1)
      return InnerIteratorType
           ::template get_const_iterator<depth>();
  }

  template <TupleIdxType return_idx, 
            IteratorDepthType depth>
  inline auto& get() {
    if constexpr (depth == now_depth_ - 1)
      return (this->iterator_.template get<return_idx>());
    if constexpr (depth != now_depth_ - 1)
      return InnerIteratorType::template get<return_idx, depth>();
  }
  
  template <TupleIdxType return_idx, 
            IteratorDepthType depth>
  inline const auto& get() const {
    return this->template get_const<return_idx, depth>();
  }

  template <TupleIdxType return_idx, 
            IteratorDepthType depth>
  inline const auto& get_const() const {
    if constexpr (depth == now_depth_ - 1)
      return (this->iterator_.template get_const<return_idx>());
    if constexpr (depth != now_depth_ - 1)
      return InnerIteratorType::template get_const<return_idx,
                                                        depth>();
  }
};

template <enum ContainerType container_type_, 
          enum      SortType      sort_type_,
          typename KeyType_, 
          bool    is_const_, 
          IteratorDepthType depth_>
class _InnerIterator_<Container<container_type_, 
                                     sort_type_, 
                                       KeyType_>,
                      is_const_, depth_, depth_> {
  static_assert(depth_ > 1, "Illegal depth");

 private:
  using ContainerType = Container<container_type_, 
                                       sort_type_, 
                                         KeyType_>;
  using ElementType = typename ContainerType::ElementType;
  using IteratorType =
      typename std::conditional<is_const_,
        typename ContainerType::const_iterator,
        typename ContainerType::iterator>::type;

  IteratorType iterator_;

 protected:
  _InnerIterator_() = default;

  inline const IteratorType& const_iterator() const { 
    return this->iterator_; 
  }

  inline void set_iterator(const IteratorType& new_iterator) {
    this->iterator_ = new_iterator;
    return;
  }

  inline void set_lower_iterator() const { return; }

  /// return whether iterator_ has moved
  inline bool _ToNext() {
    this->iterator_++;
    return true;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline auto& get_iterator() {
    return this->iterator_;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get_iterator() const {
    return this->template get_const_iterator<depth>();
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get_const_iterator() const {
    return this->iterator_;
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline auto& get() {
    return (this->iterator_.template get<return_idx>());
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get() const {
    return this->template get_const<return_idx, depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get_const() const {
    return (this->iterator_.template get_const<return_idx>());
  }
};

template <enum ContainerType container_type_, 
          enum      SortType      sort_type_,
          typename      KeyType_, 
          typename    ValueType_, 
          typename... ValueTypes_,
          bool is_const_, 
          IteratorDepthType depth_>
class _InnerIterator_<Container<container_type_, 
                                     sort_type_, 
                                       KeyType_,
                                     ValueType_, 
                                     ValueTypes_...>,
                      is_const_, depth_, depth_> {
  static_assert(depth_ > 1, "Illegal depth");

 private:
  using ContainerType = Container<container_type_, 
                                       sort_type_, 
                                         KeyType_,
                                       ValueType_, 
                                       ValueTypes_...>;
  using ElementType = typename ContainerType::ElementType;
  using IteratorType =
      typename std::conditional<is_const_,
        typename ContainerType::const_iterator,
        typename ContainerType::iterator>::type;

  IteratorType iterator_;

 protected:
  _InnerIterator_() = default;

  inline const IteratorType& const_iterator() const { 
    return this->iterator_;
  }

  inline void set_iterator(const IteratorType& new_iterator) {
    this->iterator_ = new_iterator;
    return;
  }

  inline void set_lower_iterator() const { 
    return; 
  }

  /// return whether iterator_ has moved
  inline bool _ToNext() {
    this->iterator_++;
    return true;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline auto& get_iterator() {
    return this->iterator_;
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get_iterator() const {
    return this->template get_const_iterator<depth>();
  }

  template <IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get_const_iterator() const {
    return this->iterator_;
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline auto& get() {
    return (this->iterator_.template get<return_idx>());
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get() const {
    return this->template get_const<return_idx,
                                         depth>();
  }

  template <TupleIdxType return_idx,
            IteratorDepthType depth,
            std::enable_if_t<depth == depth_ - 1, 
                             bool> = false>
  inline const auto& get_const() const {
    return (this->iterator_.template get_const<return_idx>());
  }

  // template <TupleIdxType return_idx,
  //           IteratorDepthType depth,
  //           std::enable_if_t<depth != depth_ - 1, 
  //                            bool> = false>
  // inline const auto& get_const() const {
  //   // assert false
  //   static_assert(depth  != 1, "depth  equal to 1");
  //   static_assert(depth_ != 3, "depth_ equal to 3");
  //   // static_assert(depth == depth_ - 1, "illegal depth: depth = ");
  //   return (this->iterator_.template get_const<return_idx>());
  // }
};

// Iterator_ "outter"
template <typename ContentIterator_>
class Iterator_ : protected ContentIterator_ {
 private:
  using ContentPtr = typename ContentIterator_ ::ContentPtr;

 public:
  using ContentIterator_::ContentIterator_;

  template <bool judge = ContentIterator_::kIsConst_,
            typename std::enable_if<!judge, bool>::type = false>
  inline ContentPtr operator->() {
    assert(!this->IsDone());
    return ContentIterator_::content_ptr();
  }

  template <bool judge = ContentIterator_::kIsConst_,
            typename std::enable_if<judge, bool>::type = false>
  inline ContentPtr operator->() const {
    assert(!this->IsDone());
    return ContentIterator_::content_ptr();
  }

  inline Iterator_ operator++() {
    /// prefix
    assert(!this->IsDone());
    ContentIterator_::ToNext();
    return *this;
  }
  
  inline Iterator_ operator++(int) {
    /// postfix
    assert(!this->IsDone());
    Iterator_ temp(*this);
    //++(*this);
    ContentIterator_::ToNext();
    return temp;
  }

  inline bool IsDone() const { 
    return ContentIterator_::IsDone(); 
  }
};

template <typename ContentIterator_>
class IteratorWithInstance_ : public Iterator_<ContentIterator_> {
 private:
  using IteratorType = Iterator_<ContentIterator_>;

  using ContentDataType = typename ContentIterator_
                                 ::ContentDataType;

 public:
  using IteratorType::IteratorType;

  template <bool judge = ContentIterator_::kIsConst_,
            typename std::enable_if<!judge, bool>::type = false>
  inline ContentDataType& operator*() {
    assert(!this->IsDone());
    return ContentIterator_::instance();
  }

  template <bool judge = ContentIterator_::kIsConst_,
            typename std::enable_if<judge, bool>::type = false>
  inline const ContentDataType& operator*() const {
    assert(!this->IsDone());
    return ContentIterator_::instance();
  }
};

}  // namespace GUNDAM

#endif  // _ITERATOR_H
