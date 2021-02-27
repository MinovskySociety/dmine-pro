#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include <sstream>
#include <string>
#include <vector>

#include "gundam/component/container.h"
#include "gundam/data_type/datatype.h"
#include "gundam/component/iterator.h"

namespace GUNDAM {

enum class AttributeType : uint8_t {
  kSeparated       = 0, /// each vertex/edge has its own vertex container 
  kGrouped         = 1, /// a group of vertexes/edges shares the same key
  kGroupedSameType = 2  /// a group of vertexes/edges shares the same key and value type
};

template<enum AttributeType attr_type,
         bool               is_const_,
         typename       GroupKeyType_,
         typename   AttributeKeyType_,
         enum ContainerType container_type_,
         enum      SortType      sort_type_>
class Attribute_;

template <class A, class B>
inline int CopyAllAttributes(A& from, B& to) {
  int count = 0;
  for (auto it_a = from->AttributeCBegin(); !it_a.IsDone(); ++it_a) {
    switch (it_a->value_type()) {
      case GUNDAM::BasicDataType::kTypeInt:
        if (!to->template AddAttribute<int>(it_a->key(),
                                            it_a->template const_value<int>())
                 .second) {
          return -1;
        }
        break;
      case GUNDAM::BasicDataType::kTypeInt64:
        if (!to->template AddAttribute<int64_t>(
                   it_a->key(), it_a->template const_value<int64_t>())
                 .second) {
          return -1;
        }
        break;
      case GUNDAM::BasicDataType::kTypeDouble:
        if (!to->template AddAttribute<double>(
                   it_a->key(), it_a->template const_value<double>())
                 .second) {
          return -1;
        }
        break;
      case GUNDAM::BasicDataType::kTypeString:
        if (!to->template AddAttribute<std::string>(
                   it_a->key(), it_a->template const_value<std::string>())
                 .second) {
          return -1;
        }
        break;
      case GUNDAM::BasicDataType::kTypeDateTime:
        if (!to->template AddAttribute<GUNDAM::DateTime>(
                   it_a->key(), it_a->template const_value<GUNDAM::DateTime>())
                 .second) {
          return -1;
        }
        break;
      case GUNDAM::BasicDataType::kTypeUnknown:
      default:
        return -2;
    }
    ++count;
  }
  return count;
}

/// seperated attribute container
template<bool               is_const_,
         typename       GroupKeyType_,
         typename   AttributeKeyType_,
         enum ContainerType container_type_,
         enum      SortType      sort_type_>
class Attribute_<AttributeType::kSeparated,
                                 is_const_,
                             GroupKeyType_,
                         AttributeKeyType_,
                           container_type_,
                                sort_type_> {
 private:
  // Value
  class AbstractValue {
  public:
    virtual ~AbstractValue() {}

    virtual std::string value_str() const = 0;
  };

  template <typename ConcreteDataType_>
  class ConcreteValue : public AbstractValue {
   public:
    ConcreteValue(const ConcreteDataType_& value) : value_(value) {}

    virtual ~ConcreteValue() override {}

    virtual std::string value_str() const override {
      if constexpr (TypeToEnum<ConcreteDataType_>() 
                  != BasicDataType::kTypeUnknown){
        std::stringstream ss;
        ss << this->value_;
        return ss.str();
      }
      return "unknown value type";
    }

    const ConcreteDataType_& const_value() const { 
      return this->value_; 
    }

    ConcreteDataType_& value() { 
      return this->value_; 
    }

    void set_value(const ConcreteDataType_& value) { 
      this->value_ = value; 
      return;
    }

   private:
    ConcreteDataType_ value_;
  };

  // Attribute
  static constexpr TupleIdxType kAttributeKeyIdx = 0;
  static constexpr TupleIdxType kAttributeValuePtrIdx = 1;
  static constexpr TupleIdxType kAttributeValueTypeIdx = 2;

  template <typename                  _KeyType_, 
            enum ContainerType _container_type_,
            enum SortType           _sort_type_>
  using AttributeContainer = Container<_container_type_, 
                                            _sort_type_, 
                                              _KeyType_,
                                         AbstractValue*, 
                                    enum BasicDataType>;

  // This"Ptr" is iterator!
  template <typename AttributeContainerType, 
            bool _is_const_>
  class AttributeContentPtr_ {
   private:
    using IteratorType = typename std::conditional<_is_const_, 
              typename AttributeContainerType::const_iterator,
              typename AttributeContainerType::      iterator>::type;

   protected:
    bool is_null_;
    IteratorType iterator_;

    inline bool IsNull() const { 
      return this->is_null_; 
    }

   public:
    AttributeContentPtr_() : is_null_(true), iterator_() { 
      return; 
    }

    AttributeContentPtr_(const IteratorType& iterator)
                : is_null_(false), iterator_(iterator) {
      return;
    }

    inline const typename AttributeContainerType::KeyType& key() const {
      assert(!this->is_null_);
      return this->iterator_.template get<kAttributeKeyIdx>();
    }

    template <typename ConcreteDataType>
    inline const ConcreteDataType& const_value() const {
      assert(!this->is_null_);
      return static_cast<const ConcreteValue<ConcreteDataType>*>(
                this->iterator_.template get_const<kAttributeValuePtrIdx>())
          ->const_value();
    }

    template <typename ConcreteDataType>
    inline ConcreteDataType& value() {
      assert(!this->is_null_);
      return static_cast<ConcreteValue<ConcreteDataType>*>(
                this->iterator_.template get<kAttributeValuePtrIdx>())
          ->value();
    }

    inline std::string value_str() const {
      assert(!this->is_null_);
      return this->iterator_.template get_const<kAttributeValuePtrIdx>()->value_str();
    }

    inline BasicDataType value_type() const {
      assert(!this->is_null_);
      return this->iterator_.template get_const<kAttributeValueTypeIdx>();
    }
  };

  template <typename AttributeContainerType,
            bool _is_const_>
  class AttributePtr_
      : protected AttributeContentPtr_<AttributeContainerType, _is_const_> {
   private:
    friend class AttributePtr_<AttributeContainerType, !_is_const_>;

    using AttributeContentPtrType
        = AttributeContentPtr_<AttributeContainerType, _is_const_>;
        
    using AttributeContentPtr = typename std::conditional<_is_const_, 
                                      const AttributeContentPtrType*,
                                            AttributeContentPtrType*>::type;

   public:
    using AttributeContentPtrType::AttributeContentPtrType;

    bool operator==(const AttributePtr_& attribute_ptr) const {
      return this->iterator_ == attribute_ptr.iterator_;
    }
    bool operator!=(const AttributePtr_& attribute_ptr) const {
      return this->iterator_ != attribute_ptr.iterator_;
    }
    bool operator<(const AttributePtr_& attribute_ptr) const {
      return this->iterator_ < attribute_ptr.iterator_;
    }
    bool operator>(const AttributePtr_& attribute_ptr) const {
      return this->iterator_ > attribute_ptr.iterator_;
    }
    bool operator<=(const AttributePtr_& attribute_ptr) const {
      return this->iterator_ <= attribute_ptr.iterator_;
    }
    bool operator>=(const AttributePtr_& attribute_ptr) const {
      return this->iterator_ >= attribute_ptr.iterator_;
    }
    AttributePtr_& operator=(const AttributePtr_& attribute_ptr) {
      this->iterator_ = attribute_ptr.iterator_;
      this->is_null_ = attribute_ptr.is_null_;
      return *this;
    }

    template <const bool judge = _is_const_,
              typename std::enable_if<!judge, bool>::type = false>
    AttributeContentPtr operator->() {
      static_assert(judge == _is_const_, "Illegal usage of this method");
      AttributeContentPtr const temp_this_ptr = this;
      return temp_this_ptr;
    }

    template <const bool judge = _is_const_,
              typename std::enable_if<judge, bool>::type = false>
    AttributeContentPtr operator->() const {
      static_assert(judge == _is_const_, "Illegal usage of this method");
      const AttributeContentPtr temp_this_ptr = this;
      return temp_this_ptr;
    }

    // template <const bool judge = _is_const_,
    //           typename std::enable_if<!judge, bool>::type = false>
    // AttributeContentPtrType* operator->() {
    //   static_assert(judge == _is_const_, "Illegal usage of this method");
    //   void* temp_ptr = static_cast<void*>(this);
    //   return static_cast<AttributeContentPtrType*>(temp_ptr);
    // }

    // template <const bool judge = _is_const_,
    //           typename std::enable_if<judge, bool>::type = false>
    // const AttributeContentPtrType* operator->() {
    //   static_assert(judge == _is_const_, "Illegal usage of this method");
    //   const void* const temp_ptr = static_cast<const void*>(this);
    //   return static_cast<const AttributeContentPtrType*>(temp_ptr);
    // }
    
    inline bool IsNull() const { 
      return AttributeContentPtrType::IsNull(); 
    }
  };

  /// these ContentIterator classes are transparent to programmers
  template <typename             _KeyType_, 
            typename        ContainerType_, 
            bool                _is_const_,
            IteratorDepthType       depth_, 
            IteratorDepthType begin_depth_,
            TupleIdxType          key_idx_, 
            TupleIdxType    value_ptr_idx_,
            TupleIdxType      value_typex_>
  class AttributeContentIterator_
      : protected InnerIterator_<ContainerType_, _is_const_, depth_> {
   private:
    using InnerIteratorType = InnerIterator_<ContainerType_, _is_const_, depth_>;
    
    friend class Attribute_;

    // friend typename VertexAttributeType::AttributeIterator
    // VertexAttributeType::EraseAttribute(
    //    const typename VertexAttributeType::AttributeIterator&
    //        attribute_iterator);
    // friend typename EdgeAttributeType::AttributeIterator
    // EdgeAttributeType::EraseAttribute(
    //    const typename EdgeAttributeType::AttributeIterator&
    //    attribute_iterator);

    const typename InnerIteratorType::IteratorType& ConstInnerIterator() const {
      return InnerIteratorType::ConstInnerIterator();
    }

   protected:
    using InnerIteratorType::IsDone;
    using InnerIteratorType::ToNext;
    using ContentPtr = typename std::conditional<_is_const_, 
                            const AttributeContentIterator_*,
                                  AttributeContentIterator_*>::type;
    static constexpr bool kIsConst_ = _is_const_;

    template <bool judge = _is_const_,
              typename std::enable_if<!judge, bool>::type = false>
    ContentPtr content_ptr() {
      ContentPtr const temp_this_ptr = this;
      return temp_this_ptr;
    }

    template <bool judge = _is_const_,
              typename std::enable_if<judge, bool>::type = false>
    ContentPtr content_ptr() const {
      ContentPtr const temp_this_ptr = this;
      return temp_this_ptr;
    }

   public:
    using InnerIteratorType::InnerIteratorType;

    const _KeyType_& key() const {
      assert(!this->IsDone());
      return InnerIteratorType::template get_const<key_idx_,
                                                 begin_depth_>();
    }

    template <typename ConcreteDataType>
    const ConcreteDataType& const_value() const {
      assert(!this->IsDone());
      return static_cast<ConcreteValue<const ConcreteDataType>*>(
                InnerIteratorType::template get_const<value_ptr_idx_, 
                                                      begin_depth_>())
          ->const_value();
    }

    template <typename ConcreteDataType>
    ConcreteDataType& value() {
      assert(!this->IsDone());
      return static_cast<ConcreteValue<ConcreteDataType>*>(
                InnerIteratorType::template get<value_ptr_idx_,
                                                begin_depth_>())
          ->value();
    }

    std::string value_str() const {
      assert(!this->IsDone());
      return (InnerIteratorType::template get_const<value_ptr_idx_, 
                                                    begin_depth_>())
          ->value_str();
    }

    enum BasicDataType value_type() const {
      assert(!this->IsDone());
      return InnerIteratorType::template get_const<value_typex_, 
                                                   begin_depth_>();
    }
  };

  using AttributeContainerType
      = AttributeContainer<AttributeKeyType_, container_type_, sort_type_>;

  using AttributeContentIterator 
      = AttributeContentIterator_<
                   AttributeKeyType_, 
                   AttributeContainerType, false, 1, 0,
                  kAttributeKeyIdx, 
                  kAttributeValuePtrIdx,
                  kAttributeValueTypeIdx>;
                  
  using AttributeContentConstIterator 
      = AttributeContentIterator_<
                   AttributeKeyType_, 
                   AttributeContainerType,  true, 1, 0,
                  kAttributeKeyIdx, 
                  kAttributeValuePtrIdx,
                  kAttributeValueTypeIdx>;

 public:
  using AttributeKeyType = AttributeKeyType_;

  using AttributePtr      = AttributePtr_<AttributeContainerType, false>;
  using AttributeConstPtr = AttributePtr_<AttributeContainerType,  true>;

  using AttributeIterator 
      = Iterator_<AttributeContentIterator>;

  using AttributeConstIterator 
      = Iterator_<AttributeContentConstIterator>;
  
  Attribute_() = default;
  
  Attribute_(const GroupKeyType_& group_key){
    /// since the each vertex/edge holds a seperated container, 
    /// the group_key would not be used
    return;
  }
  
  ~Attribute_() {
    for (auto it  = this->attributes_.begin();
              it != this->attributes_.end(); it++)
      delete it.template get<kAttributeValuePtrIdx>();
    return;
  }

  BasicDataType attribute_value_type(const AttributeKeyType_& key) const {
    auto ret = this->attributes_.FindConst(key);
    assert(ret.second);
    return ret.first.template get_const<kAttributeValueTypeIdx>();
  }

  const char* attribute_value_type_name(const AttributeKeyType_& key) const {
    return EnumToString(this->attribute_value_type(key));
  }

  AttributeIterator AttributeBegin() {
    return AttributeIterator(this->attributes_.begin(),
                             this->attributes_.end());
  }

  AttributeConstIterator AttributeCBegin() const {
    return AttributeConstIterator(this->attributes_.cbegin(),
                                  this->attributes_.cend());
  }

  AttributePtr FindAttributePtr(const AttributeKeyType_& key) {
    /// <iterator of attribute container, bool>
    auto ret = this->attributes_.Find(key);
    if (!ret.second) return AttributePtr();
    return AttributePtr(ret.first);
  }

  AttributeConstPtr FindConstAttributePtr(const AttributeKeyType_& key) const {
    /// <constant iterator of attribute container, bool>
    auto ret = this->attributes_.FindConst(key);
    if (!ret.second) return AttributeConstPtr();
    return AttributeConstPtr(ret.first);
  }

  template <typename ConcreteDataType>
  ConcreteDataType& attribute(const AttributeKeyType_& key) {
    /// <iterator of attribute container, bool>
    auto ret = this->attributes_.Find(key);
    assert(ret.second);
    return static_cast<ConcreteValue<ConcreteDataType>*>(
               ret.first.template get<kAttributeValuePtrIdx>())
        ->value();
  }

  template <typename ConcreteDataType>
  const ConcreteDataType& const_attribute(const AttributeKeyType_& key) const {
    /// <iterator of attribute container, bool>
    auto ret = this->attributes_.FindConst(key);
    assert(ret.second);
    return static_cast<const ConcreteValue<ConcreteDataType>*>(
               ret.first.template get_const<kAttributeValuePtrIdx>())
        ->const_value();
  }

  template <typename ConcreteDataType>
  std::pair<AttributePtr, bool> AddAttribute(const AttributeKeyType_& key,
                                             const ConcreteDataType& value) {
    auto concrete_value_ptr = new ConcreteValue<ConcreteDataType>(value);
    /// <iterator of attribute container, bool>
    auto ret = this->attributes_.Insert(key, 
                                        std::move(concrete_value_ptr), 
                                        TypeToEnum<ConcreteDataType>());
    if (!ret.second) {
      /// has already existed in the Container
      delete concrete_value_ptr;
      return std::pair<AttributePtr, bool>(AttributePtr(ret.first), false);
    }
    return std::pair<AttributePtr, bool>(AttributePtr(ret.first), true);
  }

  std::pair<AttributePtr, bool> AddAttribute(
      const AttributeKeyType_& key, const enum BasicDataType& data_type,
      const std::string& value_str) {
    switch (data_type) {
      case BasicDataType::kTypeString:
        return AddAttribute<std::string>(key, value_str);
      case BasicDataType::kTypeInt:
        return AddAttribute<int>(key, std::stoi(value_str));
      case BasicDataType::kTypeInt64:
        return AddAttribute<int64_t>(key, std::stoll(value_str));
      case BasicDataType::kTypeFloat:
        return AddAttribute<float>(key, std::stof(value_str));
      case BasicDataType::kTypeDouble:
        return AddAttribute<double>(key, std::stod(value_str));
      case BasicDataType::kTypeDateTime:
        return AddAttribute<DateTime>(key, DateTime(value_str));
      case BasicDataType::kTypeUnknown:
      default:
        break;
    }
    return std::make_pair(AttributePtr(), false);
  }

  template <typename ConcreteDataType>
  std::pair<AttributePtr, bool> SetAttribute(const AttributeKeyType_& key,
                                             const ConcreteDataType& value) {
    /// the constant attribute should not be modified
    /// <iterator of attribute container, bool>
    auto ret = this->attributes_.Find(key);
    if (!ret.second) {
      /// not find that attribute key in the container
      return std::pair<AttributePtr, bool>(AttributePtr(), false);
    }
    const enum BasicDataType value_type = TypeToEnum<ConcreteDataType>();
    if (ret.first.template get<kAttributeValueTypeIdx>() == value_type){
      /// the new added value have the same type
      static_cast<ConcreteValue<ConcreteDataType>*>(
          ret.first.template get<kAttributeValuePtrIdx>())
          ->set_value(value);
      return std::pair<AttributePtr, bool>(AttributePtr(ret.first), true);
    }
    /// the new value does not have the same type
    /// needs to delete the ptr and "new" a new one
    delete ret.first.template get<kAttributeValuePtrIdx>();
    ret.first.template get<kAttributeValuePtrIdx>()
      = new ConcreteValue<ConcreteDataType>(value);
    ret.first.template get<kAttributeValueTypeIdx>() = value_type;
    return std::pair<AttributePtr, bool>(AttributePtr(ret.first), true);
  }

  AttributeIterator EraseAttribute(
      const AttributeIterator& attribute_iterator) {
    const void* const ptr = &attribute_iterator;
    /// iterator of AttributeContainer
    auto it = this->attributes_.Erase(
        static_cast<const AttributeContentIterator*>(ptr)
            ->ConstInnerIterator());
    return AttributeIterator(it, this->attributes_.end());
  }

  size_t EraseAttribute(const AttributeKeyType_& key) {
    return this->attributes_.Erase(key) ? 1 : 0;
  }

 private:
  AttributeContainerType attributes_;
};

/// grouped attribute container
/// the attribute containers in a same graph share the same key & value type
template<bool           attribute_is_const_, 
         typename             GroupKeyType_, 
         typename         AttributeKeyType_,
         enum ContainerType container_type_,
         enum      SortType      sort_type_>
class Attribute_<AttributeType::kGrouped,
                     attribute_is_const_,
                           GroupKeyType_,
                       AttributeKeyType_,
                         container_type_,
                              sort_type_>{
 private:
  /// container of attribute group
  template<enum ContainerType attribute_list_container_type_,
           enum      SortType      attribute_list_sort_type_>
  class AttributeContainerGroup_;

  using AttributeContainerGroupType 
      = AttributeContainerGroup_<container_type_,
                                      sort_type_>;

  static constexpr TupleIdxType kGroupKeyIdx                   = 0;
  static constexpr TupleIdxType kAttributeContainerGroupPtrIdx = 1;
  using GroupContainerType = Container<container_type_,
                                            sort_type_,
                                         GroupKeyType_,
                          AttributeContainerGroupType*>;
  
  using ContainerIDType = typename AttributeContainerGroupType
                                             ::ContainerIDType;

  template<enum ContainerType attribute_list_container_type_,
           enum      SortType attribute_list_sort_type_>
  class AttributeContainerGroup_{
   private:
    /// list of abstract attributes
    class AttributeList_;
    
    using AttributeListType
        = AttributeList_;
           
   public:
    using ContainerIDType = typename AttributeListType::ContainerIDType;

   private:               
    static constexpr TupleIdxType kAttributeKeyIdx     = 0;
    static constexpr TupleIdxType kAttributeListPtrIdx = 1;
    using AttributeListPtrContainerType
                        = Container<attribute_list_container_type_,
                                    attribute_list_sort_type_,
                                    AttributeKeyType_,
                                    AttributeListType*>;

    template <typename              ContainerType_, 
              bool                       is_const_, 
              IteratorDepthType             depth_,
              TupleIdxType      attribute_key_idx_,
              TupleIdxType attribute_list_ptr_idx_>
    class AttributeContentIterator_
         : protected InnerIterator_<ContainerType_,
                                         is_const_,
                                            depth_> {
     private:

      friend class AttributeContainerGroup_;

      using AttributeListPtr      =       AttributeListType*;
      using AttributeListConstPtr = const AttributeListType*;

      using InnerIteratorType = InnerIterator_<ContainerType_,
                                                    is_const_,
                                                       depth_>;

      template<bool judge = is_const_,
               typename std::enable_if<!judge, bool>::type = false>
      inline AttributeListPtr attribute_list_ptr() {
        static_assert(judge == is_const_, "illegal usage of this method");
        return InnerIteratorType::template get<attribute_list_ptr_idx_,
                                               depth_ - 1>();
      }

      inline AttributeListConstPtr attribute_list_const_ptr() const {
        return InnerIteratorType::template get_const<attribute_list_ptr_idx_,
                                                     depth_ - 1>();
      }
      
      inline bool HasValue() const {
        assert(!this->IsDone());
        return this->attribute_list_const_ptr()
                   ->HasValue(this->container_id_);
      }

     protected:
      using InnerIteratorType::IsDone;
      using ContentPtr = typename std::conditional<is_const_, 
                             const AttributeContentIterator_*,
                                   AttributeContentIterator_*>::type;
      static constexpr bool kIsConst_ = is_const_;

      inline void ToNext() {
        assert(!this->IsDone());
        InnerIteratorType::ToNext();
        while(!this->IsDone()){
          if (this->HasValue()) {
            return;
          }
          InnerIteratorType::ToNext();
        }
        return;
      }

      template <bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline ContentPtr content_ptr() {
        assert(!this->IsDone());
        ContentPtr const temp_this_ptr = this;
        return temp_this_ptr;
      }

      template <bool judge = is_const_,
                typename std::enable_if<judge, bool>::type = false>
      inline ContentPtr content_ptr() const {
        assert(!this->IsDone());
        ContentPtr const temp_this_ptr = this;
        return temp_this_ptr;
      }

     public:
      AttributeContentIterator_() : InnerIteratorType(),
                                        container_id_(){
        assert(this->IsDone());
        return;
      }

      template <typename... ParameterTypes>
      AttributeContentIterator_(const ContainerIDType& container_id,
                                const   ParameterTypes&... parameters)
                                  : InnerIteratorType(parameters...),
                                        container_id_(container_id) {
        /// the iterator may point to a nullptr at the beginning
        while(!this->IsDone()){
          if (this->HasValue()) {
            return;
          }
          InnerIteratorType::ToNext();
        }
        return;
      }
      
      inline const AttributeKeyType_& key() const {
        assert(!this->IsDone());
        return InnerIteratorType::template get_const<attribute_key_idx_,
                                                     depth_ - 1>();
      }

      template <typename ConcreteDataType>
      inline const ConcreteDataType& const_value() const {
        assert(!this->IsDone());
        return this->attribute_list_const_ptr()
                   ->template const_value<ConcreteDataType>(this->container_id_);
      }

      template <typename ConcreteDataType,
                bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline ConcreteDataType& value() {
        static_assert(judge == is_const_, "illegal usage of this method");
        assert(!this->IsDone());
        return this->attribute_list_ptr()
                   ->template value<ConcreteDataType>(this->container_id_);
      }

      inline std::string value_str() const {
        assert(!this->IsDone());
        return this->attribute_list_const_ptr()
                   ->value_str(this->container_id_);
      }

      inline BasicDataType value_type() const {
        assert(!this->IsDone());
        return this->attribute_list_const_ptr()
                   ->value_type(this->container_id_);
      }

      const char* value_type_name() const {
        assert(!this->IsDone());
        return this->attribute_list_const_ptr()
                   ->value_type_name(this->container_id_);
      }

     private:
      ContainerIDType container_id_;
    };

    template<bool is_const_,
             TupleIdxType      attribute_key_idx_,
             TupleIdxType attribute_list_ptr_idx_>
    class AttributeContentPtr_{
     private:
      using AttributeListPtr      =       AttributeList_*;
      using AttributeListConstPtr = const AttributeList_*;

     protected:
      using AttributeListPtrContainerIteratorType
                  = typename std::conditional<is_const_,
                    typename AttributeListPtrContainerType::const_iterator,
                    typename AttributeListPtrContainerType::      iterator>::type;

     private:
      template<bool judge = is_const_,
              typename std::enable_if<!judge, bool>::type = false>
      inline AttributeListPtr attribute_list_ptr() {
        return this->attribute_list_ptr_container_iterator_
                    .template get<attribute_list_ptr_idx_>();
      }

      inline AttributeListConstPtr attribute_list_const_ptr() const {
        return this->attribute_list_ptr_container_iterator_
                    .template get_const<attribute_list_ptr_idx_>();
      }
        
      inline bool HasValue() const {
        return this->attribute_list_const_ptr()->HasValue();
      }

     protected:
      inline bool IsNull() const{
        return this->is_null_;
      }

     public:
      AttributeContentPtr_() : container_id_(),
      attribute_list_ptr_container_iterator_(),
                                    is_null_(true){
        return;
      }

      AttributeContentPtr_( const ContainerIDType& container_id,
      const AttributeListPtrContainerIteratorType&     iterator)
                                  : container_id_(container_id),
            attribute_list_ptr_container_iterator_(iterator),
                                          is_null_(false){
        return;
      }
      
      inline const AttributeKeyType_& key() const {
        assert(!this->IsNull());
        return this->attribute_list_ptr_container_iterator_
                    .template get_const<attribute_key_idx_>();
      }

      template <typename ConcreteDataType>
      inline const ConcreteDataType& const_value() const {
        assert(!this->IsNull());
        return this->attribute_list_const_ptr()
                    ->template const_value<ConcreteDataType>(this->container_id_);
      }

      template <typename ConcreteDataType,
                bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline ConcreteDataType& value() {
        static_assert(judge == is_const_, "illegal usage of this method");
        assert(!this->IsNull());
        return this->attribute_list_ptr()
                    ->template value<ConcreteDataType>(this->container_id_);
      }

      inline std::string value_str() const {
        assert(!this->IsNull());
        return this->attribute_list_const_ptr()
                    ->value_str(this->container_id_);
      }

      inline BasicDataType value_type() const {
        assert(!this->IsNull());
        return this->attribute_list_const_ptr()
                    ->value_type(this->container_id_);
      }

      const char* value_type_name() const {
        assert(!this->IsNull());
        return this->attribute_list_const_ptr()
                    ->value_type_name(this->container_id_);
      }

     private:
      AttributeListPtrContainerIteratorType attribute_list_ptr_container_iterator_;
      ContainerIDType container_id_;
      bool is_null_;
    };
    
    template<bool is_const_,
             TupleIdxType      attribute_key_idx_,
             TupleIdxType attribute_list_ptr_idx_>
    class AttributePtr_ : protected AttributeContentPtr_<is_const_,
                                                attribute_key_idx_,
                                           attribute_list_ptr_idx_>{
     private:
      using AttributeContentPtrType
          = AttributeContentPtr_<is_const_,
                        attribute_key_idx_,
                    attribute_list_ptr_idx_>;
                
      using AttributeListPtrContainerIteratorType 
               = typename AttributeContentPtrType
          ::AttributeListPtrContainerIteratorType;

     public:
      // template <typename... ParameterTypes>
      // AttributePtr_(const ParameterTypes&... parameters) 
      //               : AttributeContentPtrType(parameters...){
      //   return;
      // }
      
      AttributePtr_() : AttributeContentPtrType(){
        return;
      }

      AttributePtr_(const ContainerIDType& container_id,
                    const AttributeListPtrContainerIteratorType& iterator)
                  : AttributeContentPtrType(container_id,
                                                iterator){
        return;
      }

      inline bool IsNull() const{
        return AttributeContentPtrType::IsNull();
      }

      template<bool judge = is_const_,
                typename std::enable_if<judge, bool>::type = false>
      inline const AttributeContentPtrType* operator->() const{
        const void* temp_ptr = static_cast<const void*>(this);
        return static_cast<const AttributeContentPtrType*>(temp_ptr);
      }

      template<bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline AttributeContentPtrType* operator->(){
        void* temp_ptr = static_cast<void*>(this);
        return static_cast<AttributeContentPtrType*>(temp_ptr);
      }
    };

    class AttributeList_{
     private:
      class AbstractAttribute_;

      template<typename ConcreteValueType>
      class ConcreteAttribute_;

      class AbstractAttribute_{
       public:
        using AttributePtr      =       AbstractAttribute_*;
        using AttributeConstPtr = const AbstractAttribute_*;

        AbstractAttribute_(){
          return;
        }

        virtual ~AbstractAttribute_(){
          return;
        }

        template<typename ConcreteValueType>
        inline ConcreteValueType& value() {
          auto temp_concrete_ptr = static_cast<ConcreteAttribute_<
                                               ConcreteValueType>*>(this);
          return temp_concrete_ptr->value();
        }

        template<typename ConcreteValueType>
        inline const ConcreteValueType& const_value() const {
          auto temp_concrete_ptr = static_cast<const ConcreteAttribute_<
                                                     ConcreteValueType>*>(this);
          return temp_concrete_ptr->const_value();
        }
        
        virtual enum BasicDataType value_type() const = 0;

        virtual std::string value_str() const = 0;
      };
      
      template<typename ConcreteValueType>
      class ConcreteAttribute_ : public AbstractAttribute_{
       public:
        ConcreteAttribute_(const ConcreteValueType& value)
                                :AbstractAttribute_(),
                                             value_(value){
          return;
        }

        ~ConcreteAttribute_(){
          return;
        }
        
        inline ConcreteValueType& value(){
          return this->value_;
        }
        
        inline const ConcreteValueType& const_value() const {
          return this->value_;
        }
        
        inline enum BasicDataType value_type() const override{
          return TypeToEnum<ConcreteValueType>();
        }

        inline std::string value_str() const override{
          if constexpr (TypeToEnum<ConcreteValueType>() 
                      != BasicDataType::kTypeUnknown){
            std::stringstream ss;
            ss << this->value_;
            return ss.str();
          }
          return "unknown value type";
        }

       private:
        ConcreteValueType value_;
      };
      
      using AbstractAttributePtrContainerType = std::vector<AbstractAttribute_*>;
      
     public:
      using ContainerIDType = typename AbstractAttributePtrContainerType
                                                             ::size_type;

     private:
      inline void ExtendTo(const ContainerIDType& container_id){
        // does not use resize since basically only one container
        // would be inserted each time
        while(this->abstract_attr_ptr_list_.size() <= container_id){
          this->abstract_attr_ptr_list_.emplace_back(nullptr);
        }
        return;
      }

     public:
      AttributeList_(){
        return;
      }

      ~AttributeList_(){
        for (auto& attr_ptr : this->abstract_attr_ptr_list_){
          if (attr_ptr == nullptr){
            /// there is no value for this container
            continue;
          }
          /// release this value
          delete attr_ptr;
        }
        return;
      }

      // return whether there is a value for the given container id
      inline bool HasValue(const ContainerIDType& container_id) const {
        if (this->abstract_attr_ptr_list_.size() <= container_id){
          /// the size of attr_ptr_list is not big enough
          return false;
        }
        /// the list is big enough, the value is empty iff
        /// the pointer is nullptr
        return this->abstract_attr_ptr_list_.at(container_id) != nullptr;
      }
      
      template <typename ConcreteDataType>
      inline ConcreteDataType& value(
        const ContainerIDType& container_id) {
        assert(this->HasValue(container_id));
        return this->abstract_attr_ptr_list_.at(container_id)
                   ->template value<ConcreteDataType>();
      }
      
      template <typename ConcreteDataType>
      inline const ConcreteDataType& const_value(
              const ContainerIDType& container_id) const {
        assert(this->HasValue(container_id));
        return this->abstract_attr_ptr_list_.at(container_id)
                   ->template const_value<ConcreteDataType>();
      }
      
      inline const enum BasicDataType value_type(
           const ContainerIDType& container_id) const {
        assert(this->HasValue(container_id));
        return this->abstract_attr_ptr_list_.at(container_id)->value_type();
      }

      inline std::string value_str(
           const ContainerIDType& container_id) const {
        assert(this->HasValue(container_id));
        return this->abstract_attr_ptr_list_.at(container_id)->value_str();
      }
      
      /// return true if successfully added, false if not
      template<typename ConcreteValueType>
      inline bool AddAttribute(const   ContainerIDType& container_id,
                               const ConcreteValueType& attribute_value){
        if (this->HasValue(container_id)){
          /// already have value for that container_id
          return false;
        }
        /// does not have value corresponds for that container_id
        /// add a new one to it 
        this->ExtendTo(container_id); /// ensure the attribute list is big enough
        assert(this->abstract_attr_ptr_list_.at(container_id) == nullptr);
        this->abstract_attr_ptr_list_.at(container_id)
          = new ConcreteAttribute_<ConcreteValueType>(attribute_value);
        return true;
      }

      template<typename ConcreteValueType>
      inline bool SetAttribute(const ContainerIDType& container_id,
                               const ConcreteValueType& attribute_value){
        if (!this->HasValue(container_id)){
          /// does not have value for that container_id
          return false;
        }
        assert(this->abstract_attr_ptr_list_.at(container_id) != nullptr);
        delete this->abstract_attr_ptr_list_.at(container_id);
        this->abstract_attr_ptr_list_.at(container_id)
          = new ConcreteAttribute_<ConcreteValueType>(attribute_value);
        return true;
      }

      inline bool EraseAttribute(const ContainerIDType& container_id){
        if (!this->HasValue(container_id)){
          /// does not have this value
          return false;
        }
        assert(this->abstract_attr_ptr_list_.size() > container_id);
        delete this->abstract_attr_ptr_list_.at(container_id);
        this->abstract_attr_ptr_list_.at(container_id) = nullptr;
        return true;
      }

      inline bool FindAttributePtr(const ContainerIDType& container_id){
        return this->HasValue(container_id);
      }
      
      inline bool FindAttributeConstPtr(const ContainerIDType& container_id) const{
        return this->HasValue(container_id);
      }

     private:
      AbstractAttributePtrContainerType abstract_attr_ptr_list_;
    };

    using AttributeContentIterator 
        = AttributeContentIterator_<
          AttributeListPtrContainerType, false, 1, 
         kAttributeKeyIdx,
         kAttributeListPtrIdx>;
                    
    using AttributeContentConstIterator 
        = AttributeContentIterator_<
          AttributeListPtrContainerType,  true, 1, 
         kAttributeKeyIdx,
         kAttributeListPtrIdx>;;

   public:
    using AttributeKeyType = AttributeKeyType_;

    using AttributePtr      = AttributePtr_<false, 
                             kAttributeKeyIdx,
                             kAttributeListPtrIdx>;
    using AttributeConstPtr = AttributePtr_< true, 
                             kAttributeKeyIdx,
                             kAttributeListPtrIdx>;

    using AttributeIterator
        = Iterator_<AttributeContentIterator>;

    using AttributeConstIterator
        = Iterator_<AttributeContentConstIterator>;

    AttributeContainerGroup_()
           : container_id_counter_(0),
             top_container_id_(0){
      return;
    }

    ~AttributeContainerGroup_(){
      for (auto it  = this->attribute_list_ptr_container_.begin();
                it != this->attribute_list_ptr_container_. end (); it++){
        assert(it.template get<kAttributeListPtrIdx>() != nullptr);
        delete it.template get<kAttributeListPtrIdx>();
      }
      return;
    }

    template<typename ConcreteValueType>
    inline ConcreteValueType& attribute(const  ContainerIDType&  container_id,
                                        const AttributeKeyType_& key) {
      /// find the attribute list corresponds to that key, 
      /// <iterator, bool>
      auto ret = this->attribute_list_ptr_container_.Find(key);
      /// should have found that attribute list
      assert(ret.second); 
      /// otherwise, the behavior is undefined
      return ret.first.template get<kAttributeListPtrIdx>()
                     ->template value<ConcreteValueType>(container_id);
    }

    template<typename ConcreteValueType>
    inline const ConcreteValueType& const_attribute(
            const  ContainerIDType& container_id,
            const AttributeKeyType_&         key) const {
      /// <const_iterator, bool>
      auto ret = this->attribute_list_ptr_container_.FindConst(key);
      /// should have found that key
      assert(ret.second);
      /// otherwise, the behavior is undefined
      return ret.first.template get_const<kAttributeListPtrIdx>()
                     ->template const_value<ConcreteValueType>(container_id);
    }

    template<typename ConcreteValueType>
    inline std::pair<AttributePtr, bool> AddAttribute(
                const   ContainerIDType&  container_id,
                const  AttributeKeyType_&          key,
                const ConcreteValueType&  attribute_value){
      auto attr_list_ptr = new AttributeList_();
      /// <iterator, bool>
      auto ret = this->attribute_list_ptr_container_.Insert(key, std::move(attr_list_ptr));
      if (!ret.second){
        /// already has attr_list for this key
        delete attr_list_ptr;
      }
      const bool kAddAttributeRet = ret.first
                                       .template get<kAttributeListPtrIdx>()
                                      ->AddAttribute(container_id,
                                                     attribute_value);
      return std::make_pair(AttributePtr(container_id, ret.first),
                        kAddAttributeRet);
    }

    inline const BasicDataType attribute_value_type(
           const ContainerIDType& container_id,
           const AttributeKeyType_& key) const {
      /// <iterator, bool>
      auto ret = this->attribute_list_ptr_container_.FindConst(key);
      assert(ret.second); /// should already has such a key
      return ret.first.template get_const<kAttributeListPtrIdx>()
                     ->value_type(container_id);
    }

    const char* attribute_value_type_name(
          const ContainerIDType& container_id,
          const AttributeKeyType_& key) const {
      return EnumToString(this->attribute_value_type(container_id, key));
    }

    inline AttributeIterator AttributeBegin(
           const ContainerIDType& container_id){
      return AttributeIterator(container_id,
      this->attribute_list_ptr_container_.begin(),
      this->attribute_list_ptr_container_.end());
    }

    inline AttributeConstIterator AttributeCBegin(
                const ContainerIDType& container_id) const{
      return AttributeConstIterator(container_id,
           this->attribute_list_ptr_container_.cbegin(),
           this->attribute_list_ptr_container_.cend());
    }

    inline AttributePtr FindAttributePtr(
                      const ContainerIDType&  container_id,
                              const AttributeKeyType_& key) {
      /// <iterator, bool> 
      auto ret = this->attribute_list_ptr_container_.Find(key);
      if (!ret.second){
        // does not have attribute list for that key
        return AttributePtr();
      }
      /// found that attribute list,
      /// return the corresponds attribute ptr for that container_id
      const bool kAddAttributeRet = ret.first
                                       .template get<kAttributeListPtrIdx>()
                                      ->FindAttributePtr(container_id);
      if (kAddAttributeRet){
        /// has found
        return AttributePtr(container_id, ret.first); /// not null
      }
      /// does nto found
      return AttributePtr(); /// null
    }

    inline AttributeConstPtr FindConstAttributePtr(
                      const ContainerIDType&  container_id,
                              const AttributeKeyType_& key) const {
      /// <iterator, bool> 
      auto ret = this->attribute_list_ptr_container_.FindConst(key);
      if (!ret.second){
        // does not have attribute list for that key
        return AttributeConstPtr();
      }
      /// found that attribute list,
      /// return the corresponds attribute ptr for that container_id
      const bool kAddAttributeRet = ret.first
                                       .template get_const<kAttributeListPtrIdx>()
                                      ->FindAttributeConstPtr(container_id);
      if (kAddAttributeRet){
        /// has found
        return AttributeConstPtr(container_id, ret.first); /// not null
      }
      /// does nto found
      return AttributeConstPtr(); /// null
    }

    template <typename ConcreteDataType>
    std::pair<AttributePtr, bool> SetAttribute(
                      const ContainerIDType& container_id,
                      const AttributeKeyType_& key,
                      const ConcreteDataType& value) {
      /// <iterator, bool>
      auto ret = this->attribute_list_ptr_container_.Find(key);
      if (!ret.second){
        /// does not have attribute list corresponds to that key
        return std::make_pair(AttributePtr(), false);
      }
      const bool kSetAttributeRet 
               = ret.first.template get<kAttributeListPtrIdx>() 
                         ->SetAttribute(container_id, value);
      return std::make_pair(AttributePtr(container_id, ret.first),
                        kSetAttributeRet);
    }

    inline AttributeIterator EraseAttribute(
                      const ContainerIDType& container_id,
                      const AttributeIterator& attribute_iterator) {
      const void* const ptr = &attribute_iterator;
      const AttributeContentIterator* attr_it_ptr 
        = static_cast<const AttributeContentIterator*>(ptr);
      assert(container_id == attr_it_ptr->container_id_);
      /// iterator of AttributeContainer
      auto it = this->attribute_list_ptr_container_.Erase(
                      attr_it_ptr->ConstInnerIterator());

      return AttributeIterator(container_id, it,
                         this->attribute_list_ptr_container_.end());
    }

    /// return 1 if erased successfully, 0 if failed
    inline size_t EraseAttribute(
                      const  ContainerIDType& container_id,
                      const AttributeKeyType_& key) {
      /// <iterator, bool>
      auto ret = this->attribute_list_ptr_container_.Find(key);
      if (!ret.second){
        /// does not have attribute for this key
        return 0;
      }
      assert(ret.second); /// such a attribute list should have existed
      /// allows the attribute to be set as a differet data type
      ret.first.template get<kAttributeListPtrIdx>()
              ->EraseAttribute(container_id);
      return 1;
    }

    inline const ContainerIDType& ContainerSize() const{
      // total container in this group
      return this->container_id_counter_;
    }

    inline ContainerIDType AllocID(){
      this->container_id_counter_++;
      if (this->free_container_id_ranges_.empty()){
        /// does not have free id now,
        /// needs to alloc a new one to the container
        return this->top_container_id_++;
      }
      /// has free id now
      auto first_free_id_range_it 
         = this->free_container_id_ranges_.begin();
      const ContainerIDType alloc_id 
                   = first_free_id_range_it->second;
      if (alloc_id > first_free_id_range_it->first){
        /// the size of this fragment is bigger than 1
        /// this fragement would still contains other free
        /// id after this alloc_id has been allocated
        first_free_id_range_it->second--;
        assert(first_free_id_range_it->second 
            >= first_free_id_range_it->first);
        return alloc_id;
      }
      /// this fragment only has one free container_id,
      /// needs to be removed after this one has been
      /// allocated
      assert(first_free_id_range_it->second 
          == first_free_id_range_it->first);
      this->free_container_id_ranges_.erase(first_free_id_range_it);
      return alloc_id;
    }

    inline bool Recycle(const ContainerIDType& container_id){
      /// recycle the input id
      if (container_id >= this->top_container_id_){
        // container_id is bigger than top_container_id_,
        // should not have been alloc before
        return false;
      }
      if (this->container_id_counter_ == 0){
        /// this->container_id_counter_ shows that 
        /// there are no container yet
        return false;
      }
      // iterator
      auto ge_it = this->free_container_id_ranges_
                        .lower_bound(container_id);
      if (ge_it == this->free_container_id_ranges_.end()){
        if (this->free_container_id_ranges_.empty()){
          /// does not have any range for free id yet,
          /// add a new range
          /// all other allocated id should be occupied now
          assert(this->container_id_counter_ 
              == this->top_container_id_);
          this->free_container_id_ranges_.emplace(container_id,
                                                  container_id);
          this->container_id_counter_--;
          return true;
        }
        /// free_container_id_ranges_ is not empty
        /// for all range [begin_id, end_id] in
        /// free_container_id_ranges_, there are
        /// begin_id < container_id, just test whether
        /// container_id is contained in the range pointed
        /// by lt_it
        auto lt_it = this->free_container_id_ranges_.end();
        --lt_it;
        assert(lt_it->first < container_id);

        if (lt_it->second == container_id){
          /// container_id is not free yet, should not
          /// be contained in this range for free container id
          return false;
        }
        if (lt_it->second == container_id - 1){
          /// the new freed container_id can be merged into
          /// this range
          lt_it->second++;
          this->container_id_counter_--;
          return true;
        }
        /// needs to alloc a new range of free ids that
        /// only contains this one
        this->free_container_id_ranges_.emplace_hint(lt_it,
                                              container_id,
                                              container_id);
        this->container_id_counter_--;
        return true;
      }
      if (ge_it->first == container_id){
        /// container_id is not free yet, should not
        /// be contained in this range for free container id
        return false;
      }
      if (ge_it->first == container_id + 1){
        /// the container_id can be merged into the
        /// range pointed by ge_it
        if (ge_it != this->free_container_id_ranges_.begin()){
          /// there are other free range of container id
          /// ahead of *ge_it
          /// iterator
          auto lt_it = ge_it;
          --lt_it;
          if (lt_it->second == container_id - 1){
            /// the container_id can also be merged into
            /// the range pointed by lt_it
            /// *lt_it, container_id and *gt_it can be merged
            /// into one range
            const ContainerIDType kLtBeginIdx = lt_it->first,
                                    kGeEndIdx = ge_it->second;
            /// iterator 
            auto erase_lt_ret = this->free_container_id_ranges_
                                     .erase(lt_it);
            assert(erase_lt_ret == ge_it);
            auto erase_ge_ret = this->free_container_id_ranges_
                                     .erase(ge_it);
            this->free_container_id_ranges_
                 .emplace_hint(erase_ge_ret,
                                kLtBeginIdx,
                                  kGeEndIdx);
            this->container_id_counter_--;
            return true;
          }
        }
        /// cannot be merged into the range pointed by lt_it
        /// or ge_it is the first free range of container id
        /// only needs to merge it with the range pointed by
        /// ge_it
        const ContainerIDType kGeEndIdx = ge_it->second;
        /// iterator
        auto erase_ge_ret = this->free_container_id_ranges_
                                 .erase(ge_it);
        this->free_container_id_ranges_
             .emplace_hint(erase_ge_ret,
                           container_id,
                               kGeEndIdx);
        this->container_id_counter_--;
        return true;
      }
      /// the container_id cannot be merged into the
      /// range pointed by ge_it
      if (ge_it != this->free_container_id_ranges_.begin()){
        /// there are other free range of container id
        /// ahead of *ge_it
        /// iterator
        auto lt_it = ge_it; /// less than
        --lt_it;
        if (lt_it->second == container_id - 1){
          /// the container_id can be merged into the
          /// range pointed by lt_it
          lt_it->second++;
          this->container_id_counter_--;
          return true;
        }
      }
      /// container_id cannot be merged into any
      /// adjacent ranges, alloc a new range for it
      this->free_container_id_ranges_.emplace_hint(ge_it,
                                            container_id,
                                            container_id);
      this->container_id_counter_--;
      return true;
    }
    
    /// retun 1 if erase successfully, 0 if not 
    inline size_t EraseContainer(
                 const ContainerIDType& container_id){
      // recycle the container_id
      if (!this->Recycle(container_id)){
        /// recycle fail, such a container_id does not exist
        return 0;
      }
      // erase all attribute correspond to that container
      for (auto it  = this->attribute_list_ptr_container_.begin();
                it != this->attribute_list_ptr_container_. end ();it++){
        /// erase the container_id from each attribute list 
        it.template get<kAttributeListPtrIdx>()
         ->EraseAttribute(container_id);
      }
      return 1;
    }

   private:
    AttributeListPtrContainerType attribute_list_ptr_container_;

    /// holds the totoal id currently in this group
    ContainerIDType container_id_counter_;

    /// similar to the heap memory mangement in OS
    std::map<ContainerIDType, ContainerIDType> free_container_id_ranges_;
    
    /// holds the maximum id that has ever allocated
    ContainerIDType top_container_id_;
  };
  
 public:
  using AttributeKeyType = AttributeKeyType_;

  /// iterator types
  using AttributeIterator      = typename AttributeContainerGroupType
                                        ::AttributeIterator;
  using AttributeConstIterator = typename AttributeContainerGroupType
                                        ::AttributeConstIterator;

  /// pointer types
  using AttributePtr      = typename AttributeContainerGroupType
                                   ::AttributePtr;
  using AttributeConstPtr = typename AttributeContainerGroupType
                                   ::AttributeConstPtr;

  Attribute_(const GroupKeyType_& group_key) {
    /// automatically alloc a container id in this group
    auto attr_group_ptr = new AttributeContainerGroupType();
    /// <iterator, bool>
    auto ret = Attribute_::attribute_container_group_ptr_container_
                          .Insert(group_key, std::move(attr_group_ptr));
    if (!ret.second){
      // this group has already existes, delete the allocated pointer
      delete attr_group_ptr;
    }
    this->attribute_container_group_ptr_
        = ret.first.template get<kAttributeContainerGroupPtrIdx>();
    /// alloc a new id for this container
    this->attribute_container_id_ = this->attribute_container_group_ptr_
                                        ->AllocID();
    // std::cout<<"allocated id: "<<this->attribute_container_id_<<std::endl;
    return;
  }

  ~Attribute_(){
    this->attribute_container_group_ptr_
        ->EraseContainer(this->attribute_container_id_);
    if (this->attribute_container_group_ptr_->ContainerSize() == 0){
      /// this group of contianer is empty, should be released 
      /// since the group key of this container is not held, 
      /// the entire container of the group pointer needsd to be traversed
      /// in order to find the corresponding ptr
      for(auto it  = Attribute_::attribute_container_group_ptr_container_.begin();
               it != Attribute_::attribute_container_group_ptr_container_. end ();
               it ++){
        if (it.template get<kAttributeContainerGroupPtrIdx>() 
                    == this->attribute_container_group_ptr_) {
          /// found the corresponds pointer of the container group
          assert(it.template get<kAttributeContainerGroupPtrIdx>()
                  ->ContainerSize() == 0);
          /// release this group, in order to avoid memory leaking
          delete it.template get<kAttributeContainerGroupPtrIdx>();
          /// remove the group from the container
          Attribute_::attribute_container_group_ptr_container_.Erase(it);
          return;
        }
      }
      /// should had found the corresponds container group pointer
      assert(false);
    }
    /// the corresponding group does not required to be removed
    return;
  }

  inline enum BasicDataType attribute_value_type(const AttributeKeyType_& key) const {
    return this->attribute_container_group_ptr_
               ->attribute_value_type(this->attribute_container_id_, key);
  }

  inline const char* attribute_value_type_name(const AttributeKeyType_& key) const {
    return this->attribute_container_group_ptr_
               ->attribute_value_type_name(this->attribute_container_id_, key);
  }

  inline AttributeIterator AttributeBegin() {
    return this->attribute_container_group_ptr_
               ->AttributeBegin(this->attribute_container_id_);
  }

  inline AttributeConstIterator AttributeCBegin() const {
    return this->attribute_container_group_ptr_
               ->AttributeCBegin(this->attribute_container_id_);
  }

  inline AttributePtr FindAttributePtr(const AttributeKeyType_& key) {
    return this->attribute_container_group_ptr_
               ->FindAttributePtr(this->attribute_container_id_, key);
  }

  inline AttributeConstPtr FindConstAttributePtr(const AttributeKeyType_& key) const {
    return this->attribute_container_group_ptr_
               ->FindConstAttributePtr(this->attribute_container_id_, key);
  }

  template <typename ConcreteDataType>
  inline ConcreteDataType& attribute(const AttributeKeyType_& key) {
    return this->attribute_container_group_ptr_
               ->template attribute<ConcreteDataType>(
                    this->attribute_container_id_, key);
  }

  template <typename ConcreteDataType>
  inline const ConcreteDataType& const_attribute(const AttributeKeyType_& key) const {
    return this->attribute_container_group_ptr_
               ->template const_attribute<ConcreteDataType>(
                          this->attribute_container_id_, key);
  }

  template <typename ConcreteDataType>
  inline std::pair<AttributePtr, bool> 
                AddAttribute(const AttributeKeyType_& key,
                             const ConcreteDataType& value) {
    return this->attribute_container_group_ptr_
               ->AddAttribute(this->attribute_container_id_, 
                              key, value);
  }

  inline std::pair<AttributePtr, bool> AddAttribute(
                    const AttributeKeyType_& key, 
                    const enum BasicDataType& data_type,
                    const std::string& value_str) {
    switch (data_type) {
      case BasicDataType::kTypeString:
        return this->AddAttribute<std::string>(key, value_str);
      case BasicDataType::kTypeInt:
        return this->AddAttribute<int>(key, std::stoi(value_str));
      case BasicDataType::kTypeInt64:
        return this->AddAttribute<int64_t>(key, std::stoll(value_str));
      case BasicDataType::kTypeFloat:
        return this->AddAttribute<float>(key, std::stof(value_str));
      case BasicDataType::kTypeDouble:
        return this->AddAttribute<double>(key, std::stod(value_str));
      case BasicDataType::kTypeDateTime:
        return this->AddAttribute<DateTime>(key, DateTime(value_str));
      case BasicDataType::kTypeUnknown:
      default:
        break;
    }
    return std::make_pair(AttributePtr(), false);
  }

  template <typename ConcreteDataType>
  inline std::pair<AttributePtr, bool> SetAttribute(
                         const AttributeKeyType_& key,
                const ConcreteDataType& value) {
    return this->attribute_container_group_ptr_
               ->SetAttribute(this->attribute_container_id_, 
                              key, value);
  }

  inline AttributeIterator EraseAttribute(
   const AttributeIterator& attribute_iterator) {
    /// although the container_id is contained in the attribute_iterator
    /// pass this->attribute_container_id_ into group_ptr_container_ to check whether
    /// it is consist with teh container_id in attribute_iterator
    return this->attribute_container_group_ptr_
               ->EraseAttribute(this->attribute_container_id_,  
                                      attribute_iterator);
  }

  inline size_t EraseAttribute(const AttributeKeyType_& key) {
    return this->attribute_container_group_ptr_
               ->EraseAttribute(this->attribute_container_id_, key);
  }

 private:
  inline static GroupContainerType attribute_container_group_ptr_container_;
  
  /// the id of this container in group
  ContainerIDType attribute_container_id_;

  AttributeContainerGroupType* attribute_container_group_ptr_;
};

};

#endif ///_ATTRIBUTE_H