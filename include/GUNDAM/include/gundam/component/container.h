#ifndef _CONTAINER_H
#define _CONTAINER_H

#include <assert.h>

#include <cstdint>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

namespace GUNDAM {

using TupleIdxType = uint8_t;

enum class ContainerType:uint8_t{
    List,   /// for graph with few  elements (Edge types or associated Vertexes)
    Vector, /// for static graph
    Array,  /// for static graph that can statically set the maximum size of neighbors
    Map,    /// for graph with lots elements (Edge types or associated Vertexes)
    Set,    ///
/// unimplemented feature
    Adaptive/// use list when this vertex have few  elements (Edge types or associated Vertexes)
            /// use map  when this vertex have lots elements (Edge types or associated Vertexes)
            /// for dynamic graph with non-balanced elements distribution
};

enum class SortType:uint8_t{
    /// how the vertexes connected by the same type of edge
    /// are sorted
    Default, /// not sorted
     Sorted
    //  Ascend, /// sorted by  ascend order
    // Descend  /// sorted by descend order
};

template <enum ContainerType container_type_,
          enum SortType sort_type_,
          typename        KeyType_,
          typename...   ValueTypes>
class Container {
  /// trick the compiler, same to static_assert(false)
  static_assert( std::is_object<KeyType_>::value, "Unsupported configuration");
  static_assert(!std::is_object<KeyType_>::value, "Unsupported configuration");
};

template <enum SortType sort_type_,
          typename        KeyType_,
          typename...   ValueTypes_>
class Container<ContainerType::Vector,
                    sort_type_,
                      KeyType_,
                    ValueTypes_...> {
 public:
  static constexpr uint8_t kKeyIdx = 0;
  using     KeyType = KeyType_;
  using ElementType = std::tuple<KeyType_,
                               ValueTypes_...>;

 private:
  using InnerContainerType = std::vector<ElementType>;
  
  InnerContainerType container_;
  
  static bool Compare(const ElementType& element,
                      const KeyType& key) {
    return std::get<kKeyIdx>(element) < key;
  }

  template<bool is_const_,
           bool is_reverse_>
  class Iterator_{
   private:
    friend class Container<ContainerType::Vector,
                               sort_type_,
                                 KeyType_,
                               ValueTypes_...>;

    using InnerIteratorType = typename std::conditional<is_const_,
                              typename std::conditional<is_reverse_,
                        typename InnerContainerType::const_reverse_iterator,
                        typename InnerContainerType::const_iterator>::type,
                              typename std::conditional<is_reverse_,
                        typename InnerContainerType::      reverse_iterator,
                        typename InnerContainerType::      iterator>::type
                        >::type;

    InnerIteratorType iterator_;

   public:
    Iterator_():iterator_(){
      return;
    }

    Iterator_(InnerIteratorType iterator)
                     :iterator_(iterator){
      return;
    }

    inline bool operator==(const Iterator_& iterator) const {
      return this->iterator_ == iterator.iterator_;
    }
    inline bool operator!=(const Iterator_& iterator) const {
      return this->iterator_ != iterator.iterator_;
    }
    inline bool operator<(const Iterator_& iterator) const {
      return this->iterator_ <  iterator.iterator_;
    }
    inline bool operator>(const Iterator_& iterator) const {
      return this->iterator_ >  iterator.iterator_;
    }
    inline bool operator<=(const Iterator_& iterator) const {
      return this->iterator_ <= iterator.iterator_;
    }
    inline bool operator>=(const Iterator_& iterator) const {
      return this->iterator_ >= iterator.iterator_;
    }

    inline Iterator_ operator++() {
      /// prefix
      this->iterator_++;
      return *this;
    }

    inline Iterator_ operator++(int) {
      /// postfix
      Iterator_ temp(*this);
      this->iterator_++;
      return temp;
    }

    inline Iterator_ operator--() {
      /// prefix
      this->iterator_--;
      return *this;
    }

    inline Iterator_ operator--(int) {
      /// postfix
      Iterator_ temp(*this);
      this->iterator_--;
      return temp;
    }

    template<TupleIdxType tuple_idx,
             bool _is_const_ = is_const_,
             std::enable_if_t<!_is_const_, bool> = false>
    inline auto& get() { 
      static_assert(_is_const_ == is_const_, "illegal usage of this method");
      return std::get<tuple_idx>(*(this->iterator_));
    }

    template<TupleIdxType tuple_idx>
    inline auto& get() const{ 
      return std::get<tuple_idx>(*(this->iterator_));
    }

    template<TupleIdxType tuple_idx>
    inline const auto& get_const() const{
      return std::get<tuple_idx>(*(this->iterator_));
    }
  };

 public:
  using         const_iterator = Iterator_< true, false>;
  using               iterator = Iterator_<false, false>;
  using const_reverse_iterator = Iterator_< true,  true>;
  using       reverse_iterator = Iterator_<false,  true>;
  using      size_type = typename InnerContainerType::size_type;

  Container() = default;

  Container(const Container&) = default;
  Container(Container&&) = default;

  Container& operator=(const Container&) = default;	  
  Container& operator=(Container&&) = default;	

  inline const_iterator cbegin() const {
    return const_iterator(this->container_.cbegin());
  }
  inline const_iterator cend() const {
    return const_iterator(this->container_.cend());
  }
  inline iterator begin() {
    return iterator(this->container_.begin());
  }
  inline iterator end() {
    return iterator(this->container_.end());
  }
  inline const_iterator begin() const {
    return this->cbegin();
  }
  inline const_iterator end() const {
    return this->cbegin();
  }
  inline const_reverse_iterator crbegin() const {
    return const_reverse_iterator(this->container_.crbegin());
  }
  inline const_reverse_iterator crend() const {
    return const_reverse_iterator(this->container_.crend());
  }
  inline reverse_iterator rbegin() {
    return reverse_iterator(this->container_.rbegin());
  }
  inline reverse_iterator rend() {
    return reverse_iterator(this->container_.rend());
  }
  inline const_reverse_iterator rbegin() const {
    return this->crbegin();
  }
  inline const_reverse_iterator rend() const {
    return this->crend();
  }
  inline size_type size() const {
    return this->container_.size();
  }
  inline bool empty() const {
    return this->container_.empty();
  }
  inline void clear() {
    this->container_.clear();
    return;
  }
  inline void shrink_to_fit() const {
    this->container_.shrink_to_fit();
    return;
  }

  inline std::pair<iterator, bool> Insert(
                  const   KeyType_& key,
                       ValueTypes_&&... values) { 
    if constexpr (sort_type_ == SortType::Sorted){
      auto it = std::lower_bound(this->container_.begin(), 
                                 this->container_.end(), key, Compare);
      if (it != this->container_.end() && std::get<kKeyIdx>(*it) == key){
        /// already existed
        return std::make_pair(it, false);
      }
      return std::make_pair(
          this->container_.emplace(it, key, values...),
          true);
    }
    assert(sort_type_ == SortType::Default);
    for (iterator it  = iterator(this->container_.begin());
                  it != iterator(this->container_. end ());++it){
      if ((it.template get<kKeyIdx>()) == key)
        return std::pair<iterator, bool>(it, false);
    }
    return std::make_pair(this->container_.emplace(
                          this->container_.end(),
                          key, values...), true);
  }

  inline bool Erase(const KeyType& key) {
    auto ret = this->Find(key);
    if (!ret.second) {
      /// does not find this value
      return false;
    }
    this->container_.erase(ret.first.iterator_);
    return true;
  }

  inline iterator Erase(const iterator& it) {
    return this->container_.erase(it.iterator_);
  }

  inline std::pair<iterator, bool> Find(const KeyType& key) {
    if constexpr (sort_type_ == SortType::Sorted){
      auto it = std::lower_bound(this->container_.begin(), 
                                this->container_.end(), key, Compare);
      if (it == container_.end() || std::get<kKeyIdx>(*it) != key) {
        /// not found
        return std::make_pair(this->container_.end(), false);
      }
      /// found 
      return std::make_pair(it, true);
    }
    assert(sort_type_ == SortType::Default);
    for (iterator it  = this->container_.begin();
                  it != this->container_. end ();++it){
      if (it.template get<kKeyIdx>() == key)
        return std::pair<iterator, bool>(it, true);
    }
    return std::pair<iterator, bool>(this->container_.end(), false);
  }

  inline std::pair<const_iterator, bool> Find(const KeyType& key) const{
    return this->FindConst(key);
  }
  
  inline std::pair<const_iterator, bool> FindConst(const KeyType& key) const {
    if constexpr (sort_type_ == SortType::Sorted){
      auto it = std::lower_bound(this->container_.cbegin(), 
                                 this->container_.cend(), key, Compare);
      if (it == container_.cend() || std::get<kKeyIdx>(*it) != key) {
        /// not found
        return std::make_pair(this->container_.cend(), false);
      }
      /// found 
      return std::make_pair(it, true);
    }
    assert(sort_type_ == SortType::Default);
    for (const_iterator cit  = this->container_.cbegin();
                        cit != this->container_. cend (); ++cit){
      if ((cit.template get_const<kKeyIdx>()) == key)
        return std::pair<const_iterator, bool>(cit, true);
    }
    return std::pair<const_iterator, bool>(this->container_.cend(), false);
  }
};

template <enum SortType sort_type_,
          typename        KeyType_>
class Container<ContainerType::Set,
                    sort_type_,
                      KeyType_> {
 public:
  static constexpr uint8_t kKeyIdx = 0;
  using     KeyType = KeyType_;
  using ElementType = std::tuple<KeyType_>;

 private:
  struct tuple_compare {
    bool operator() (const ElementType& lhs, const ElementType& rhs) const {
        return (std::get<kKeyIdx>(lhs)) < (std::get<kKeyIdx>(rhs));
    }
  };

  using InnerContainerType = std::set<ElementType,tuple_compare>;
  InnerContainerType container_;

  static_assert(sort_type_ == SortType::Default
             || sort_type_ == SortType::Sorted,
               "other sorting type are not supported yet");

  template<bool is_const_>
  class Iterator_{
   private:
    friend class Container<ContainerType::Set,
                               sort_type_,
                                 KeyType_>;

    using InnerIteratorType = typename std::conditional<is_const_,
                      typename InnerContainerType::const_iterator,
                      typename InnerContainerType::      iterator>::type;

    InnerIteratorType iterator_;

   public:
    Iterator_():iterator_(){
      return;
    }

    Iterator_(InnerIteratorType iterator)
                     :iterator_(iterator){
      return;
    }

    inline bool operator==(const Iterator_& iterator) const {
      return this->iterator_ == iterator.iterator_;
    }
    inline bool operator!=(const Iterator_& iterator) const {
      return this->iterator_ != iterator.iterator_;
    }
    inline bool operator<(const Iterator_& iterator) const {
      return this->iterator_ <  iterator.iterator_;
    }
    inline bool operator>(const Iterator_& iterator) const {
      return this->iterator_ >  iterator.iterator_;
    }
    inline bool operator<=(const Iterator_& iterator) const {
      return this->iterator_ <= iterator.iterator_;
    }
    inline bool operator>=(const Iterator_& iterator) const {
      return this->iterator_ >= iterator.iterator_;
    }

    inline Iterator_ operator++() {
      /// prefix
      this->iterator_++;
      return *this;
    }

    inline Iterator_ operator++(int) {
      /// postfix
      Iterator_ temp(*this);
      this->iterator_++;
      return temp;
    }

    inline Iterator_ operator--() {
      /// prefix
      this->iterator_--;
      return *this;
    }

    inline Iterator_ operator--(int) {
      /// postfix
      Iterator_ temp(*this);
      this->iterator_--;
      return temp;
    }

    template<TupleIdxType tuple_idx,
             bool _is_const_ = is_const_,
             std::enable_if_t<!_is_const_, bool> = false>
    inline auto& get() { 
      static_assert(_is_const_ == is_const_, "illegal usage of this method");
      return std::get<tuple_idx>(*(this->iterator_));
    }

    template<TupleIdxType tuple_idx>
    inline auto& get() const{
      return this->template get_const<tuple_idx>();
    }

    template<TupleIdxType tuple_idx>
    inline const auto& get_const() const{
      return std::get<tuple_idx>(*(this->iterator_));
    }
  };

 public:
  using const_iterator = Iterator_<true>;
  using       iterator = Iterator_<true>; /// is not an error here
  /// const_iterator and iterator are the same thing in std::set
  using      size_type = typename InnerContainerType::size_type;

  Container() = default;

  Container(const Container&) = default;
  Container(Container&&) = default;

  Container& operator=(const Container&) = default;	  
  Container& operator=(Container&&) = default;	

  inline const_iterator cbegin() const {
    return const_iterator(this->container_.cbegin());
  }
  inline const_iterator cend() const {
    return const_iterator(this->container_.cend());
  }
  inline iterator begin() {
    return iterator(this->container_.begin());
  }
  inline iterator end() {
    return iterator(this->container_.end());
  }
  inline const_iterator begin() const {
    return this->cbegin();
  }
  inline const_iterator end() const {
    return this->cend();
  }
  inline size_type size() const {
    return this->container_.size();
  }
  inline bool empty() const {
    return this->container_.empty();
  }
  inline void clear() {
    this->container_.clear();
    return;
  }
  inline void shrink_to_fit() const {
    this->container_.shrink_to_fit();
    return;
  }

  inline std::pair<iterator, bool> Insert(const KeyType_& key) {
    /// <iterator, bool>
    auto ret = this->container_.emplace(key);
    return std::pair<iterator, bool>(iterator(ret.first),
                                              ret.second);
  }

  inline bool Erase(const KeyType& key) {
    const size_type temp_size = this->size();
    this->container_.erase(key);
    return temp_size != this->size();
  }

  inline iterator Erase(const iterator& it) {
    assert(it != this->container_.end());
    return this->container_.erase(it.iterator_);
  }

  inline std::pair<iterator, bool> Find(const KeyType& key) {
    typename InnerContainerType::iterator ret = this->container_.find(key);
    return std::pair<iterator, bool>(
                     iterator(ret),
                     ret != this->container_.end());
  }

  inline std::pair<const_iterator, bool> Find(const KeyType& key) const {
    return this->FindConst(key);
  }

  inline std::pair<const_iterator, bool> FindConst(const KeyType& key) const {
    typename InnerContainerType::const_iterator ret = this->container_.find(key);
    return std::pair<const_iterator, bool>(
                     const_iterator(ret),
                     ret != this->container_.cend());
  }
};

template <enum SortType sort_type_,
          typename        KeyType_,
          typename      ValueType_,
          typename...   ValueTypes_>
class Container<ContainerType::Map,
                    sort_type_,
                      KeyType_,
                    ValueType_,
                    ValueTypes_...> {
 private:
  using ValueType = std::tuple<ValueType_,
                               ValueTypes_...>;

 public:
  static constexpr uint8_t kKeyIdx = 0;
  using     KeyType = KeyType_;
  using ElementType = std::pair<KeyType,ValueType>;

 private:
  using AbstractElementType = std::tuple<KeyType_,
                                       ValueType_,
                                       ValueTypes_...>;

  using InnerContainerType = std::map<KeyType,ValueType>;
  InnerContainerType container_;

  static_assert(sort_type_ == SortType::Default
             || sort_type_ == SortType::Sorted,
               "other sorting type are not supported yet");

  template<bool is_const_,
           bool is_reverse_>
  class Iterator_{
   private:
    friend class Container<ContainerType::Map,
                               sort_type_,
                                 KeyType_,
                               ValueType_,
                               ValueTypes_...>;

    using InnerIteratorType = typename std::conditional<is_const_,
                              typename std::conditional<is_reverse_,
                        typename InnerContainerType::const_reverse_iterator,
                        typename InnerContainerType::const_iterator>::type,
                              typename std::conditional<is_reverse_,
                        typename InnerContainerType::      reverse_iterator,
                        typename InnerContainerType::      iterator>::type
                        >::type;

    InnerIteratorType iterator_;

   public:
    Iterator_():iterator_(){
      return;
    }

    Iterator_(InnerIteratorType iterator)
                     :iterator_(iterator){
      return;
    }

    inline bool operator==(const Iterator_& iterator) const {
      return this->iterator_ == iterator.iterator_;
    }
    inline bool operator!=(const Iterator_& iterator) const {
      return this->iterator_ != iterator.iterator_;
    }
    inline bool operator<(const Iterator_& iterator) const {
      return this->iterator_ <  iterator.iterator_;
    }
    inline bool operator>(const Iterator_& iterator) const {
      return this->iterator_ >  iterator.iterator_;
    }
    inline bool operator<=(const Iterator_& iterator) const {
      return this->iterator_ <= iterator.iterator_;
    }
    inline bool operator>=(const Iterator_& iterator) const {
      return this->iterator_ >= iterator.iterator_;
    }

    inline Iterator_ operator++() {
      /// prefix
      this->iterator_++;
      return *this;
    }

    inline Iterator_ operator++(int) {
      /// postfix
      Iterator_ temp(*this);
      this->iterator_++;
      return temp;
    }

    inline Iterator_ operator--() {
      /// prefix
      this->iterator_--;
      return *this;
    }

    inline Iterator_ operator--(int) {
      /// postfix
      Iterator_ temp(*this);
      this->iterator_--;
      return temp;
    }

    template<TupleIdxType tuple_idx,
             bool _is_const_ = is_const_,
             std::enable_if_t<!_is_const_ && tuple_idx == 0, bool> = false>
    inline auto& get() { 
      static_assert(_is_const_ == is_const_, "illegal usage of this method");
      return this->iterator_->first;
    }

    template<TupleIdxType tuple_idx,
             bool _is_const_ = is_const_,
             std::enable_if_t<!_is_const_ && tuple_idx != 0, bool> = false>
    inline auto& get() { 
      static_assert(_is_const_ == is_const_, "illegal usage of this method");
      return std::get<tuple_idx - 1>(this->iterator_->second);
    }

    template<TupleIdxType tuple_idx,
             std::enable_if_t<tuple_idx == 0, bool> = false>
    inline auto& get() const{
      return this->iterator_->first;
    }

    template<TupleIdxType tuple_idx,
             std::enable_if_t<tuple_idx != 0, bool> = false>
    inline auto& get() const{
      return std::get<tuple_idx - 1>(this->iterator_->second);
    }

    template<TupleIdxType  tuple_idx,
             std::enable_if_t<tuple_idx == 0, bool> = false>
    inline const auto& get_const() const{
      return this->iterator_->first;
    }

    template<TupleIdxType tuple_idx,
             std::enable_if_t<tuple_idx != 0, bool> = false>
    inline const auto& get_const() const{
      return std::get<tuple_idx - 1>(this->iterator_->second);
    }
  };

 public:
  using         const_iterator = Iterator_< true, false>;
  using               iterator = Iterator_<false, false>;
  using const_reverse_iterator = Iterator_< true,  true>;
  using       reverse_iterator = Iterator_<false,  true>;
  using      size_type = typename InnerContainerType::size_type;

  Container() = default;

  Container(const Container&) = default;
  Container(Container&&) = default;

  Container& operator=(const Container&) = default;	  
  Container& operator=(Container&&) = default;	

  inline const_iterator cbegin() const {
    return const_iterator(this->container_.cbegin());
  }
  inline const_iterator cend() const {
    return const_iterator(this->container_.cend());
  }
  inline iterator begin() {
    return iterator(this->container_.begin());
  }
  inline iterator end() {
    return iterator(this->container_.end());
  }
  inline const_iterator begin() const {
    return this->cbegin();
  }
  inline const_iterator end() const {
    return this->cend();
  }
  inline const_reverse_iterator crbegin() const {
    return const_reverse_iterator(this->container_.crbegin());
  }
  inline const_reverse_iterator crend() const {
    return const_reverse_iterator(this->container_.crend());
  }
  inline reverse_iterator rbegin() {
    return reverse_iterator(this->container_.rbegin());
  }
  inline reverse_iterator rend() {
    return reverse_iterator(this->container_.rend());
  }
  inline const_reverse_iterator rbegin() const {
    return this->crbegin();
  }
  inline const_reverse_iterator rend() const {
    return this->crend();
  }
  inline size_type size() const {
    return this->container_.size();
  }
  inline bool empty() const {
    return this->container_.empty();
  }
  inline void clear() {
    this->container_.clear();
    return;
  }
  inline void shrink_to_fit() const {
    this->container_.shrink_to_fit();
    return;
  }

  inline std::pair<iterator, bool> Insert(
                          const   KeyType_& key,
                          const ValueType_ && value,
                          const ValueTypes_&&... values) {
    /// <iterator, bool>
    auto ret = this->container_.emplace(key ,ValueType(value, values...));
    return std::pair<iterator, bool>(iterator(ret.first),
                                              ret.second);
  }

  inline bool Erase(const KeyType& key) {
    const size_type temp_size = this->size();
    this->container_.erase(key);
    return temp_size != this->size();
  }

  inline iterator Erase(const iterator& it) {
    assert(it != this->container_.end());
    return this->container_.erase(it.iterator_);
  }

  inline std::pair<iterator, bool> Find(const KeyType& key) {
    typename InnerContainerType::iterator ret = this->container_.find(key);
    return std::pair<iterator, bool>(
                     iterator(ret),
                     ret != this->container_.end());
  }

  inline std::pair<const_iterator, bool> Find(const KeyType& key) const {
    return this->FindConst(key);
  }

  inline std::pair<const_iterator, bool> FindConst(const KeyType& key) const {
    typename InnerContainerType::const_iterator ret = this->container_.find(key);
    return std::pair<const_iterator, bool>(
                     const_iterator(ret),
                     ret != this->container_.cend());
  }
};

}

#endif  // _CONTAINER_H
