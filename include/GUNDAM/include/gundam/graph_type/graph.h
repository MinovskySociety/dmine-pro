#ifndef _GRAPH_H
#define _GRAPH_H

#include "gundam/component/container.h"
#include "gundam/data_type/datatype.h"
#include "gundam/data_type/datetime.h"
#include "gundam/component/generator.h"
#include "gundam/graph_type/graph_configure.h"
#include "gundam/component/iterator.h"
#include "gundam/data_type/label.h"
#include "gundam/component/attribute.h"

#include <iostream>
#include <set>
#include <tuple>

namespace GUNDAM {

enum class EdgeDirection : bool { InputEdge, OutputEdge };

template <typename... configures>
class Graph {
 public:
  using Configures = GraphConfigures<configures...>;
  static constexpr bool vertex_has_attribute =
      Configures::vertex_has_static_attribute |
      Configures::vertex_has_dynamic_attribute;
  static constexpr bool edge_has_attribute =
      Configures::edge_has_static_attribute |
      Configures::edge_has_dynamic_attribute;

  static constexpr bool graph_has_vertex_label_index = true;

  static constexpr bool vertex_has_edge_label_index = true;

 private:
  /// #######################################
  /// ##  only used in Graph class         ##
  /// ##  use Graph<>::VertexType::IDType  ##
  /// ##  outside Graph class instead      ##
  /// #######################################

  /// types related to Vertex
  using VertexIDType              = typename Configures::VertexIDType;
  using VertexLabelType           = typename Configures::VertexLabelType;
  using VertexAttributeKeyType    = typename Configures::VertexAttributeKeyType;
  using VertexStaticAttributeType = typename Configures::VertexStaticAttributeType;
  /// types related to Edge
  using EdgeIDType                = typename Configures::EdgeIDType;
  using EdgeLabelType             = typename Configures::EdgeLabelType;
  using EdgeAttributeKeyType      = typename Configures::EdgeAttributeKeyType;
  using EdgeStaticAttributeType   = typename Configures::EdgeStaticAttributeType;

  static constexpr enum StoreData store_data = Configures::store_data;

  static constexpr bool vertex_label_is_const =
      Configures::vertex_label_is_const;
  static constexpr enum AttributeType vertex_attribute_store_type =
      Configures::vertex_attribute_store_type;
  static constexpr bool vertex_attribute_is_const =
      Configures::vertex_attribute_is_const;
  static constexpr bool vertex_has_static_attribute =
      Configures::vertex_has_static_attribute;
  static constexpr bool vertex_has_dynamic_attribute =
      Configures::vertex_has_dynamic_attribute;
  static constexpr bool edge_label_is_const = Configures::edge_label_is_const;
  static constexpr enum AttributeType edge_attribute_store_type =
      Configures::edge_attribute_store_type;
  static constexpr bool edge_attribute_is_const =
      Configures::edge_attribute_is_const;
  static constexpr bool edge_has_static_attribute =
      Configures::edge_has_static_attribute;
  static constexpr bool edge_has_dynamic_attribute =
      Configures::edge_has_dynamic_attribute;

  static constexpr bool allow_duplicate_edge = Configures::allow_duplicate_edge;

  static constexpr enum ContainerType vertex_attribute_container_type =
      Configures::vertex_attribute_container_type;
  static constexpr enum ContainerType edge_attribute_container_type =
      Configures::edge_attribute_container_type;
  static constexpr enum ContainerType decomposed_edge_container_type =
      Configures::decomposed_edge_container_type;
  static constexpr enum ContainerType vertex_ptr_container_type =
      Configures::vertex_ptr_container_type;
  static constexpr enum ContainerType edge_label_container_type =
      Configures::edge_label_container_type;
  static constexpr enum ContainerType vertex_id_container_type =
      Configures::vertex_id_container_type;
  static constexpr enum ContainerType vertex_label_container_type =
      Configures::vertex_label_container_type;

  static constexpr enum SortType vertex_attribute_container_sort_type =
      Configures::vertex_attribute_container_sort_type;
  static constexpr enum SortType edge_attribute_container_sort_type =
      Configures::edge_attribute_container_sort_type;
  static constexpr enum SortType decomposed_edge_container_sort_type =
      Configures::decomposed_edge_container_sort_type;
  static constexpr enum SortType vertex_ptr_container_sort_type =
      Configures::vertex_ptr_container_sort_type;
  static constexpr enum SortType edge_label_container_sort_type =
      Configures::edge_label_container_sort_type;
  static constexpr enum SortType vertex_id_container_sort_type =
      Configures::vertex_id_container_sort_type;
  static constexpr enum SortType vertex_label_container_sort_type =
      Configures::vertex_label_container_sort_type;

  /// type declaration
  template <typename IDType_>
  class WithID_;
  template <typename LabelType_, bool is_const_>
  class WithLabel_;

  class InnerVertex_;
  
 public:
  /// non-constant pointer
  using VertexPtr = typename InnerVertex_::VertexPtr;
  using   EdgePtr = typename InnerVertex_::  EdgePtr;
  /// constant pointer
  using VertexConstPtr = typename InnerVertex_::VertexConstPtr;
  using   EdgeConstPtr = typename InnerVertex_::  EdgeConstPtr;
  
 private:
  using VertexAttributeType = typename InnerVertex_::VertexAttributeType;
  using   EdgeAttributeType = typename InnerVertex_::  EdgeAttributeType;

  template <typename      ContainerType_, 
            bool               is_const_, 
            IteratorDepthType     depth_,
            TupleIdxType vertex_ptr_idx_>
  using VertexContentIterator_
      = typename InnerVertex_
      ::template VertexContentIterator_<
                         ContainerType_, 
                              is_const_,
                                 depth_,
                        vertex_ptr_idx_>;

 private:
  static constexpr TupleIdxType kVertexIDIdx = 0;
  static constexpr TupleIdxType kVertexPtrIdx = 1;
  using VertexIDContainerType
              = Container<vertex_id_container_type,
                          vertex_id_container_sort_type,
                          VertexIDType,
                          VertexPtr>;

  static constexpr TupleIdxType kVertexLabelIdx = 0;
  static constexpr TupleIdxType kVertexIDContainerIdx = 1;
  using VertexLabelContainerType
                 = Container<vertex_label_container_type,
                             vertex_label_container_sort_type,
                             VertexLabelType,
                             VertexIDContainerType>;

  using VertexContentIterator
      = VertexContentIterator_<VertexLabelContainerType, false, 2,
                              kVertexPtrIdx>;
  using VertexContentConstIterator
      = VertexContentIterator_<VertexLabelContainerType,  true, 2,
                              kVertexPtrIdx>;
  using VertexContentIteratorSpecifiedLabel
      = VertexContentIterator_<VertexIDContainerType, false, 1, 
                              kVertexPtrIdx>;
  using VertexContentConstIteratorSpecifiedLabel
      = VertexContentIterator_<VertexIDContainerType,  true, 1, 
                              kVertexPtrIdx>;

 public:
  using   EdgeSizeType = typename InnerVertex_::EdgeSizeType;
  using VertexSizeType = typename VertexIDContainerType::size_type;
  /// non-constant iterator
  using VertexIterator 
            = Iterator_<VertexContentIterator>;
  using VertexIteratorSpecifiedLabel
            = Iterator_<VertexContentIteratorSpecifiedLabel>;

  /// constant iterator
  using VertexConstIterator 
                 = Iterator_<VertexContentConstIterator>;
  using VertexConstIteratorSpecifiedLabel
                 = Iterator_<VertexContentConstIteratorSpecifiedLabel>;

  using VertexAttributePtr           = typename VertexAttributeType::AttributePtr;
  using VertexAttributeIterator      = typename VertexAttributeType::AttributeIterator;
  using VertexAttributeConstPtr      = typename VertexAttributeType::AttributeConstPtr;
  using VertexAttributeConstIterator = typename VertexAttributeType::AttributeConstIterator;
  using   EdgeAttributePtr           = typename   EdgeAttributeType::AttributePtr;
  using   EdgeAttributeIterator      = typename   EdgeAttributeType::AttributeIterator;
  using   EdgeAttributeConstPtr      = typename   EdgeAttributeType::AttributeConstPtr;
  using   EdgeAttributeConstIterator = typename   EdgeAttributeType::AttributeConstIterator;
  
  template <typename IDType_, 
            typename LabelType_, 
            typename AttributeType_>
  class Type_;

  using VertexType = Type_<VertexIDType, 
                           VertexLabelType, 
                           VertexAttributeType>;

  using   EdgeType = Type_<EdgeIDType, 
                           EdgeLabelType, 
                           EdgeAttributeType>;

 private:
  template <typename IDType_>
  class WithID_ {
   private:
    const IDType_ id_;

   public:
    WithID_(const IDType_& id) : id_(id) { return; }
    inline const IDType_& id() const { return this->id_; }
  };

  template <typename LabelType_, bool is_const_>
  class WithLabel_ {
   private:
    typename std::conditional<is_const_, const LabelType_, LabelType_>::type
        label_;

   public:
    WithLabel_(const LabelType_& label) : label_(label) { return; }
    inline const LabelType_& label() const { return this->label_; }
    template <bool judge = is_const_,
              typename std::enable_if<!judge, bool>::type = false>
    inline void set_label(const LabelType_& label) {
      static_assert(judge == is_const_, "Illegal usage of this method");
      this->label_ = label;
      return;
    }
  };

 private:
  template <typename ContainerType_, 
            bool is_const_, 
            IteratorDepthType depth_,
            TupleIdxType edge_label_idx_>
  class EdgeLabelContentIterator_
       : protected InnerIterator_<ContainerType_, 
                                       is_const_, 
                                          depth_> {
   private:
    using InnerIteratorType = InnerIterator_<ContainerType_, 
                                                  is_const_, 
                                                     depth_>;

   protected:
    using InnerIteratorType::InnerIteratorType;
    using InnerIteratorType::IsDone;
    using InnerIteratorType::ToNext;
    using ContentPtr = typename std::conditional<is_const_, 
                           const EdgeLabelContentIterator_*,
                                 EdgeLabelContentIterator_*>::type;
    static constexpr bool kIsConst_ = is_const_;

    template <bool judge = is_const_,
              typename std::enable_if<!judge, bool>::type = false>
    inline ContentPtr content_ptr() {
      static_assert(judge == is_const_, "illegal usage of this method");
      assert(!this->IsDone());
      ContentPtr const temp_this_ptr = this;
      return temp_this_ptr;
    }

    template <bool judge = is_const_,
              typename std::enable_if<judge, bool>::type = false>
    inline ContentPtr content_ptr() const {
      static_assert(judge == is_const_, "illegal usage of this method");
      assert(!this->IsDone());
      ContentPtr const temp_this_ptr = this;
      return temp_this_ptr;
    }

   public:
    const EdgeLabelType& operator*() const {
      assert(!this->IsDone());
      return InnerIteratorType::template get_const<
           edge_label_idx_, depth_ - 1>();
    }
  };

 public:
  /// the class that provides the basic information about both vertex and edge
  template <typename IDType_, 
            typename LabelType_,
            typename AttributeType_>
  class Type_ {
   public:
    using IDType = IDType_;
    using LabelType = LabelType_;
    using AttributeKeyType  = typename AttributeType_::AttributeKeyType;
    using AttributePtr      = typename AttributeType_::AttributePtr;
    using AttributeConstPtr = typename AttributeType_::AttributeConstPtr;
  };

 private:
  /// this class is transparent to user
  class InnerVertex_ 
    : public WithID_       <VertexIDType>,
      public WithLabel_    <VertexLabelType, 
                            vertex_label_is_const>,
      // public WithAttribute_<VertexStaticAttributeType, 
      //                       vertex_attribute_is_const,
      //                       vertex_has_dynamic_attribute, 
      //                       VertexAttributeKeyType,
      //                       vertex_attribute_container_type,
      //                       vertex_attribute_container_sort_type> 
      public Attribute_    <vertex_attribute_store_type,
                            vertex_attribute_is_const,
                            std::pair<VertexLabelType, bool>,
                            VertexAttributeKeyType,
                            vertex_attribute_container_type,
                            vertex_attribute_container_sort_type>{
   public:   
    template <bool is_const_>
    class VertexPtr_;

    using VertexPtr      = VertexPtr_<false>;
    using VertexConstPtr = VertexPtr_< true>;

   private:
    using VertexWithIDType   
              = WithID_<VertexIDType>;
    using VertexWithLabelType 
              = WithLabel_<VertexLabelType, 
                           vertex_label_is_const>;
                        
    template <typename              ContainerType_,
              bool                       is_const_,
              IteratorDepthType             depth_,
              IteratorDepthType       begin_depth_,
              TupleIdxType         edge_label_idx_,
              TupleIdxType            dst_ptr_idx_,
              TupleIdxType            edge_id_idx_,
              TupleIdxType edge_attribute_ptr_idx_>
    class EdgeContentIterator_;

    template <typename              ContainerType_, 
              bool                       is_const_,
              IteratorDepthType             depth_,
              IteratorDepthType       begin_depth_, 
              TupleIdxType         edge_label_idx_,
              TupleIdxType            dst_ptr_idx_, 
              TupleIdxType            edge_id_idx_,
              TupleIdxType edge_attribute_ptr_idx_>
    class EdgeContentIteratorSpecifiedEdgeLabel_;

    template <typename              ContainerType_, 
              bool                       is_const_,
              IteratorDepthType             depth_,
              IteratorDepthType       begin_depth_, 
              TupleIdxType         edge_label_idx_,
              TupleIdxType            dst_ptr_idx_, 
              TupleIdxType            edge_id_idx_,
              TupleIdxType edge_attribute_ptr_idx_>
    class EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_;
    
    template <typename      ContainerType_, 
              bool               is_const_, 
              IteratorDepthType     depth_,
              TupleIdxType edge_label_idx_>
    class EdgeLabelContentIterator_;

   public:
    /// would also be used in Graph
    template <typename      ContainerType_, 
              bool               is_const_, 
              IteratorDepthType     depth_,
              TupleIdxType vertex_ptr_idx_>
    class VertexContentIterator_;

    using VertexAttributeType 
          // = WithAttribute_<VertexStaticAttributeType, 
          //                  vertex_attribute_is_const,
          //                  vertex_has_dynamic_attribute, 
          //                  VertexAttributeKeyType,
          //                  vertex_attribute_container_type,
          //                  vertex_attribute_container_sort_type>;
              = Attribute_<vertex_attribute_store_type,
                           vertex_attribute_is_const,
                           std::pair<VertexLabelType, bool>,
                           VertexAttributeKeyType,
                           vertex_attribute_container_type,
                           vertex_attribute_container_sort_type>;
                           

    using   EdgeAttributeType
          // = WithAttribute_<EdgeStaticAttributeType, 
          //                  edge_attribute_is_const,
          //                  edge_has_dynamic_attribute, EdgeAttributeKeyType,
          //                  edge_attribute_container_type,
          //                  edge_attribute_container_sort_type>;
          = Attribute_<edge_attribute_store_type,
                       edge_attribute_is_const,
                       std::pair<EdgeLabelType, bool>,
                       EdgeAttributeKeyType,
                       edge_attribute_container_type,
                       edge_attribute_container_sort_type>;
                           
      // public WithAttribute_<VertexStaticAttributeType, 
      //                       vertex_attribute_is_const,
      //                       vertex_has_dynamic_attribute, 
      //                       VertexAttributeKeyType,
      //                       vertex_attribute_container_type,
      //                       vertex_attribute_container_sort_type> 

    template <bool is_const_>
    class VertexPtr_ {
     private:
     // const pointer can convert from non-constant pointer
      friend class VertexPtr_<true>; 

      friend class Graph;
      // more precisely, it should be:
      // template<bool ptr_is_const_>
      // inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
      //       EdgeCBegin(enum EdgeDirection direction,
      //                 const EdgeLabelContainerType& edge_label_container,
      //                 const EdgeLabelType& edge_label,
      //                 const VertexPtr_<ptr_is_const_>& vertex_ptr);
      // template<bool ptr_is_const_>
      // inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
      //       EdgeBegin(enum EdgeDirection direction,
      //                 const EdgeLabelContainerType& edge_label_container,
      //                 const EdgeLabelType& edge_label,
      //                 const VertexPtr_<ptr_is_const_>& vertex_ptr);
      // template<bool ptr_is_const_>
      // inline typename VertexPtrContainerType::size_type CountEdge(
      //           const EdgeLabelContainerType&    edge_label_container,
      //           const EdgeLabelType&             edge_label,
      //           const VertexPtr_<ptr_is_const_>& vertex_ptr)

      using VertexPtrType =
          typename std::conditional<is_const_, 
                           const InnerVertex_*,
                                 InnerVertex_*>::type;

      template <typename      _ContainerType_, 
                bool      _iterator_is_const_, 
                IteratorDepthType     _depth_,
                TupleIdxType _vertex_ptr_idx_>
      using FriendVertexContentIterator =
                  VertexContentIterator_<_ContainerType_, 
                                     _iterator_is_const_, 
                                                 _depth_,
                                        _vertex_ptr_idx_>;

      template <typename      _ContainerType_, 
                bool      _iterator_is_const_, 
                IteratorDepthType     _depth_,
                TupleIdxType _vertex_ptr_idx_>
      using FriendVertexIterator = Iterator_<
                FriendVertexContentIterator<_ContainerType_, 
                                        _iterator_is_const_, 
                                                    _depth_,
                                           _vertex_ptr_idx_>>;

      template <typename      _ContainerType_, 
                bool      _iterator_is_const_, 
                IteratorDepthType     _depth_,
                TupleIdxType _vertex_ptr_idx_>
      inline void Construct(
          const FriendVertexIterator<_ContainerType_, 
                                 _iterator_is_const_,
                                             _depth_,
                                    _vertex_ptr_idx_>& vertex_ptr_iterator) {
        const void* const ptr = &vertex_ptr_iterator;

        this->ptr_ = (static_cast<const FriendVertexContentIterator<
                              _ContainerType_, 
                          _iterator_is_const_, 
                                      _depth_, 
                             _vertex_ptr_idx_>*>(ptr)
                     )->const_vertex_ptr().ptr_;
        return;
      }

      /// to be called in the EdgeBegin/EdgeCBegin/CountEdge method
      inline VertexPtrType ptr() const {
        return this->ptr_;
      }

     public:
      VertexPtr_() : ptr_(nullptr) { 
        return; 
      }

      VertexPtr_(VertexPtrType const ptr) 
                              : ptr_(ptr) { 
        return; 
      }

      template <typename      _ContainerType_,
                bool      _iterator_is_const_, 
                IteratorDepthType     _depth_,
                TupleIdxType _vertex_ptr_idx_,
                std::enable_if_t<_iterator_is_const_  // can convert from iterator with same cv qualifier
                                        == is_const_
                             || !_iterator_is_const_, // or can convert from non-constant iterator
                                 bool> = false>
      VertexPtr_(
          const FriendVertexIterator<_ContainerType_, 
                                 _iterator_is_const_,
                                             _depth_,
                                    _vertex_ptr_idx_>& vertex_ptr_iterator) {
        this->Construct(vertex_ptr_iterator);
        return;
      }

      template <typename      _ContainerType_, 
                bool      _iterator_is_const_, 
                IteratorDepthType     _depth_,
                TupleIdxType _vertex_ptr_idx_,
                std::enable_if_t<_iterator_is_const_  // can convert from iterator with same cv qualifier
                                        == is_const_
                             || !_iterator_is_const_, // or can convert from non-constant iterator
                                 bool> = false>
      inline VertexPtr_& operator=(
          const FriendVertexIterator<_ContainerType_, 
                                 _iterator_is_const_,
                                             _depth_,
                                    _vertex_ptr_idx_>& vertex_ptr_iterator) {
        this->Construct(vertex_ptr_iterator);
        return *this;
      }

      template <bool input_is_const_, 
                std::enable_if_t<input_is_const_  // can convert from pointer with same cv qualifier
                                    == is_const_
                             || !input_is_const_, // or can convert from non-constant pointer
                                 bool> = false>
      VertexPtr_(const VertexPtr_<input_is_const_>& vertex_ptr)
          : ptr_(vertex_ptr.ptr_) {
        return;
      }

      template <bool input_is_const_, 
                std::enable_if_t<input_is_const_  // can convert from pointer with same cv qualifier
                                    == is_const_
                             || !input_is_const_, // or can convert from non-constant pointer
                                 bool> = false>
      inline VertexPtr_& operator=(
          const VertexPtr_<input_is_const_>& vertex_ptr) {
        this->ptr_ = vertex_ptr.ptr_;
        return *this;
      }

      inline operator bool() const {
        return !this->IsNull();
      }

      inline bool operator==(const VertexPtr_& ptr) const {
        assert(this->ptr_ != ptr.ptr_ || this->ptr_->id() == ptr.ptr_->id());
        return this->ptr_ == ptr.ptr_;
      }
      inline bool operator!=(const VertexPtr_& ptr) const {
        return this->ptr_ != ptr.ptr_;
      }
      inline bool operator<(const VertexPtr_& ptr) const {
        return this->ptr_ < ptr.ptr_;
      }
      inline bool operator>(const VertexPtr_& ptr) const {
        return this->ptr_ > ptr.ptr_;
      }
      inline bool operator<=(const VertexPtr_& ptr) const {
        return this->ptr_ <= ptr.ptr_;
      }
      inline bool operator>=(const VertexPtr_& ptr) const {
        return this->ptr_ >= ptr.ptr_;
      }

      inline VertexPtr_& operator=(const VertexPtr_& ptr) {
        this->ptr_ = ptr.ptr_;
        return *this;
      }
      
      inline VertexPtrType operator->() const {
        return this->ptr_;
      }

      template <const bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline VertexPtrType operator->() {
        static_assert(judge == is_const_, "Illegal usage of this method");
        return this->ptr_;
      }

      inline void Release() const{
        delete this->ptr_;
        return;
      }

      inline bool IsNull() const { 
        return this->ptr_ == nullptr; 
      }

     private:
      VertexPtrType ptr_;
    };
    
    template <typename      ContainerType_, 
              bool               is_const_, 
              IteratorDepthType     depth_,
              TupleIdxType vertex_ptr_idx_>
    class VertexContentIterator_
      : protected InnerIterator_<ContainerType_, 
                                      is_const_, 
                                         depth_> {
     private:
      using VertexIterator = typename Graph::VertexIterator;

      friend typename InnerVertex_::template VertexPtr_<is_const_>;
      // const pointer can convert from non-constant iterator
      friend typename InnerVertex_::template VertexPtr_<true>;

      friend VertexIterator Graph::EraseVertex(VertexIterator& vertex_iterator);

      using VertexPtr      = typename InnerVertex_::VertexPtr;
      using VertexConstPtr = typename InnerVertex_::VertexConstPtr;
      using VertexPtrType  = typename std::conditional<is_const_, 
                                                  VertexConstPtr,
                                                       VertexPtr>::type;

      using InnerIteratorType = InnerIterator_<ContainerType_,
                                                    is_const_,
                                                       depth_>;

      template <bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline VertexPtr& vertex_ptr() {
        return InnerIteratorType::template get<vertex_ptr_idx_,
                                               depth_ - 1>();
      }

      template <bool judge = is_const_,
                typename std::enable_if<judge, bool>::type = false>
      inline VertexConstPtr vertex_ptr() const {
        auto temp_vertex_ptr =
            InnerIteratorType::template get_const<vertex_ptr_idx_,
                                                  depth_ - 1>();
        return ContentPtr(temp_vertex_ptr);
      }

      template <bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline const VertexPtr& const_vertex_ptr() const {
        return InnerIteratorType::template get_const<vertex_ptr_idx_,
                                                     depth_ - 1>();
      }

      template <bool judge = is_const_,
                typename std::enable_if<judge, bool>::type = false>
      inline VertexConstPtr const_vertex_ptr() const {
        auto temp_vertex_ptr =
            InnerIteratorType::template get_const<vertex_ptr_idx_,
                                                  depth_ - 1>();
        return VertexConstPtr(temp_vertex_ptr);
      }

     protected:
      using InnerIteratorType::IsDone;
      using InnerIteratorType::ToNext;
      using ContentPtr = VertexPtrType;
      static constexpr bool kIsConst_ = is_const_;

      template <bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline ContentPtr& content_ptr() {
        assert(!this->IsDone());
        return this->vertex_ptr();
      }

      template <bool judge = is_const_,
                typename std::enable_if<judge, bool>::type = false>
      inline ContentPtr content_ptr() const {
        assert(!this->IsDone());
        return this->vertex_ptr();
      }

     public:
      using InnerIteratorType::InnerIteratorType;
    };

   private:
    template <typename              ContainerType_,
              bool                       is_const_,
              IteratorDepthType             depth_,
              IteratorDepthType       begin_depth_, 
              TupleIdxType         edge_label_idx_,
              TupleIdxType            dst_ptr_idx_, 
              TupleIdxType            edge_id_idx_,
              TupleIdxType edge_attribute_ptr_idx_>
    class EdgeContentIterator_
        : protected InnerIterator_<ContainerType_,
                                        is_const_,
                                           depth_> {
    private:
      using EdgeIterator 
        = typename InnerVertex_::EdgeIterator;
      using EdgeIteratorSpecifiedEdgeLabel 
        = typename InnerVertex_::EdgeIteratorSpecifiedEdgeLabel;
                        
      friend typename InnerVertex_::template EdgePtr_<is_const_>;
      
      friend EdgeIterator InnerVertex_::EraseEdge(
             EdgeIterator& edge_iterator);

      friend EdgeIteratorSpecifiedEdgeLabel InnerVertex_::EraseEdge(
             EdgeIteratorSpecifiedEdgeLabel& edge_iterator);

    protected:
      using InnerIteratorType = InnerIterator_<ContainerType_, 
                                                    is_const_, 
                                                       depth_>;
      using VertexPtr      = typename InnerVertex_::VertexPtr;
      using VertexConstPtr = typename InnerVertex_::VertexConstPtr;
      using VertexPtrType  = typename std::conditional<is_const_,
                                                  VertexConstPtr,
                                                  VertexPtr>::type;
      using VertexPtrIteratorType =
          typename InnerVertex_::VertexPtrContainerType::iterator;
      using DecomposedEdgeIteratorType =
          typename InnerVertex_::DecomposedEdgeIteratorType;

      inline enum EdgeDirection const_direction() const {
        return this->direction_;
      }

      inline const VertexPtrType& const_vertex_ptr() const {
        return this->vertex_ptr_;
      }
      inline VertexPtrType& vertex_ptr() {
        return this->vertex_ptr_; 
      }

      inline EdgeAttributeType& _attribute() {
        assert(!this->IsDone());
        return *(
            InnerIteratorType::template get<edge_attribute_ptr_idx_, 
                                            begin_depth_ + 2>());
      }

      inline const EdgeAttributeType& _const_attribute() const {
        assert(!this->IsDone());
        return *(
            InnerIteratorType::template get_const<edge_attribute_ptr_idx_, 
                                                  begin_depth_ + 2>());
      }

      inline VertexPtrType VertexPtrContainerElement() {
        assert(!this->IsDone());
        return InnerIteratorType
             ::template get_const<dst_ptr_idx_,
                                  begin_depth_ + 1>();
      }

      inline VertexConstPtr VertexPtrContainerConstElement() const {
        assert(!this->IsDone());
        VertexPtr temp_vertex_ptr =
            InnerIteratorType::template get_const<dst_ptr_idx_,
                                                  begin_depth_ + 1>();
        return VertexConstPtr(temp_vertex_ptr);
      }

    protected:
      using InnerIteratorType::IsDone;
      using InnerIteratorType::ToNext;
      using ContentPtr = typename std::conditional<is_const_, 
                                  const EdgeContentIterator_*,
                                        EdgeContentIterator_*>::type;
      static constexpr bool kIsConst_ = is_const_;

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
      EdgeContentIterator_() : InnerIteratorType(),
                                      direction_(),
                                     vertex_ptr_(){
        return;
      }

      template <typename... ParameterTypes>
      EdgeContentIterator_(const enum EdgeDirection direction,
                           const VertexPtrType&     vertex_ptr,
                           const ParameterTypes&... parameters)
                                : InnerIteratorType(parameters...),
                                         direction_(direction),
                                        vertex_ptr_(vertex_ptr) {
        return;
      }
      
      inline VertexPtrIteratorType& vertex_ptr_iterator() {
        assert(!this->IsDone());
        return this->template get_iterator<begin_depth_ + 1>();
      }

      inline DecomposedEdgeIteratorType& decomposed_edge_iterator() {
        assert(!this->IsDone());
        return this->template get_iterator<begin_depth_ + 2>();
      }

      inline VertexPtrType src_ptr() {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->vertex_ptr_;
        /// this->direction_ == EdgeDirection::InputEdge
        return this->VertexPtrContainerElement();
      }

      inline VertexConstPtr src_ptr() const {
        return this->const_src_ptr();
      }

      inline VertexConstPtr const_src_ptr() const {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->vertex_ptr_;
        /// this->direction_ == EdgeDirection::InputEdge
        return this->VertexPtrContainerConstElement();
      }
      
      inline VertexPtrType dst_ptr() {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->VertexPtrContainerElement();
        /// this->direction_ == EdgeDirection::InputEdge
        return this->vertex_ptr_;
      }
      
      inline VertexConstPtr dst_ptr() const {
        return this->const_dst_ptr();
      }
      
      inline VertexConstPtr const_dst_ptr() const {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->VertexPtrContainerConstElement();
        /// this->direction_ == EdgeDirection::InputEdge
        return this->vertex_ptr_;
      }

      inline const EdgeLabelType& label() const {
        assert(!this->IsDone());
        return InnerIteratorType::template get_const<edge_label_idx_, 
                                                     begin_depth_>();
      }

      inline const EdgeIDType& id() const {
        assert(!this->IsDone());
        return InnerIteratorType::template get_const<edge_id_idx_,
                                                     begin_depth_ + 2>();
      }

      template <typename ConcreteDataType>
      inline ConcreteDataType& attribute(const EdgeAttributeKeyType& key) {
        assert(!this->IsDone());
        return this->_attribute().template attribute<ConcreteDataType>(key);
      }

      template <typename ConcreteDataType>
      inline const ConcreteDataType& const_attribute(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsDone());
        return this->_const_attribute()
           .template _const_attribute<ConcreteDataType>(key);
      }

      inline EdgeAttributeIterator AttributeBegin() {
        assert(!this->IsDone());
        return this->_attribute().AttributeBegin();
      }

      inline EdgeAttributeConstIterator AttributeCBegin() const {
        assert(!this->IsDone());
        return this->_const_attribute().AttributeCBegin();
      }

      inline EdgeAttributePtr FindAttributePtr(const EdgeAttributeKeyType& key) {
        assert(!this->IsDone());
        return this->_attribute().FindAttributePtr(key);
      }

      inline EdgeAttributeConstPtr FindConstAttributePtr(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsDone());
        return this->_const_attribute().FindConstAttributePtr(key);
      }

      inline const std::string attribute_value_type_name(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsDone());
        return this->_const_attribute().attribute_value_type_name(key);
      }

      inline const enum BasicDataType attribute_value_type(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsDone());
        return this->_const_attribute().attribute_value_type(key);
      }

      template <typename ConcreteDataType>
      inline std::pair<EdgeAttributePtr, bool> AddAttribute(
          const EdgeAttributeKeyType& key, const ConcreteDataType& value) {
        assert(!this->IsDone());
        return this->_attribute().AddAttribute(key, value);
      }
      
      inline std::pair<EdgeAttributePtr, bool> AddAttribute(
          const EdgeAttributeKeyType& key, 
          const enum BasicDataType& data_type,
          const std::string& value_str) {
        assert(!this->IsDone());
        return this->_attribute().AddAttribute(key, 
                                               data_type, 
                                               value_str);
      }

      template <typename ConcreteDataType>
      inline std::pair<EdgeAttributePtr, bool> SetAttribute(
          const EdgeAttributeKeyType& key, const ConcreteDataType& value) {
        assert(!this->IsDone());
        return this->_attribute().SetAttribute(key, value);
      }

      inline EdgeAttributeIterator EraseAttribute(
          const EdgeAttributeIterator& attribute_iterator) {
        return this->_attribute().EraseAttribute(attribute_iterator);
      }

      inline bool EraseAttribute(const EdgeAttributeKeyType& key) {
        return this->_attribute().EraseAttribute(key);
      }

     private:
      enum EdgeDirection direction_;
      VertexPtrType vertex_ptr_;
    };
    
    template <typename              ContainerType_, 
              bool                       is_const_, 
              IteratorDepthType             depth_,
              IteratorDepthType       begin_depth_, 
              TupleIdxType         edge_label_idx_,
              TupleIdxType            dst_ptr_idx_, 
              TupleIdxType            edge_id_idx_,
              TupleIdxType edge_attribute_ptr_idx_>
    class EdgeContentIteratorSpecifiedEdgeLabel_
        : public EdgeContentIterator_<ContainerType_, 
                                           is_const_,
                                              depth_,
                                        begin_depth_ - 1, 
                                     edge_label_idx_,
                                        dst_ptr_idx_, 
                                        edge_id_idx_,
                             edge_attribute_ptr_idx_> {
    private:
      using EdgeIteratorSpecifiedEdgeLabel 
        = typename InnerVertex_::EdgeIteratorSpecifiedEdgeLabel;

      friend typename InnerVertex_::template EdgePtr_<is_const_>;

      friend EdgeIteratorSpecifiedEdgeLabel
      InnerVertex_::EraseEdge(
             EdgeIteratorSpecifiedEdgeLabel& edge_iterator);

      using InnerIteratorType
        = EdgeContentIterator_<ContainerType_,
                                    is_const_,
                                       depth_,
                                 begin_depth_ - 1,
                              edge_label_idx_,
                                 dst_ptr_idx_,
                                 edge_id_idx_,
                      edge_attribute_ptr_idx_>;
      using VertexPtr = typename InnerVertex_::VertexPtr;
      //   const EdgeLabelType edge_label_;
      using EdgeLabelIterator = typename std::conditional<is_const_,
      typename InnerVertex_::EdgeLabelContainerType::const_iterator,
      typename InnerVertex_::EdgeLabelContainerType::iterator>::type;

    protected:
      using InnerIteratorType::IsDone;
      using InnerIteratorType::ToNext;
      using ContentPtr = typename std::conditional<is_const_, 
                const EdgeContentIteratorSpecifiedEdgeLabel_*,
                      EdgeContentIteratorSpecifiedEdgeLabel_*>::type;
      static constexpr bool kIsConst_ = is_const_;

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

      /// to be called in the friend classes and friend methods
      inline const EdgeLabelIterator& const_edge_label_iterator() const {
        return this->edge_label_iterator_;
      }
      inline EdgeLabelIterator& edge_label_iterator(){
        return this->edge_label_iterator_;
      }

    public:
      EdgeContentIteratorSpecifiedEdgeLabel_()
                         : InnerIteratorType(), 
                        edge_label_iterator_() {
        return;
      }

      template <typename... ParameterTypes>
      EdgeContentIteratorSpecifiedEdgeLabel_(
          const EdgeLabelIterator& edge_label_iterator,
          const ParameterTypes&... parameters)
               : InnerIteratorType(parameters...),
              edge_label_iterator_(edge_label_iterator) {
        return;
      }

      inline const EdgeLabelType& label() const {
        assert(!this->IsDone());
        return this->edge_label_iterator_
                    .template get_const<edge_label_idx_>();
      }

     private:
      EdgeLabelIterator edge_label_iterator_;
    };
    
    template <typename              ContainerType_,
              bool                       is_const_,
              IteratorDepthType             depth_,
              IteratorDepthType       begin_depth_, 
              TupleIdxType         edge_label_idx_,
              TupleIdxType            dst_ptr_idx_, 
              TupleIdxType            edge_id_idx_,
              TupleIdxType edge_attribute_ptr_idx_>
    class EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_
            : public EdgeContentIteratorSpecifiedEdgeLabel_<
                                             ContainerType_,
                                                  is_const_,
                                                     depth_,
                                               begin_depth_ - 1,
                                            edge_label_idx_,
                                               dst_ptr_idx_,
                                               edge_id_idx_,
                                    edge_attribute_ptr_idx_> {
    private:
      using EdgeIteratorSpecifiedEdgeLabelVertexPtr
        = typename InnerVertex_::EdgeIteratorSpecifiedEdgeLabelVertexPtr;

      friend typename InnerVertex_::template EdgePtr_<is_const_>;

      friend EdgeIteratorSpecifiedEdgeLabelVertexPtr
      InnerVertex_::EraseEdge(EdgeIteratorSpecifiedEdgeLabelVertexPtr& edge_iterator);

      using InnerIteratorType
        = EdgeContentIteratorSpecifiedEdgeLabel_<
                                  ContainerType_,
                                       is_const_,
                                          depth_,
                                    begin_depth_ - 1,
                                 edge_label_idx_,
                                    dst_ptr_idx_,
                                    edge_id_idx_,
                         edge_attribute_ptr_idx_>;

      using VertexPtr      = typename InnerVertex_::VertexPtr;
      using VertexConstPtr = typename InnerVertex_::VertexConstPtr;
      
      using VertexPtrType  = typename std::conditional<is_const_,
                                                  VertexConstPtr,
                                                  VertexPtr>::type;
      using VertexPtrIterator = typename std::conditional<is_const_,
      typename InnerVertex_::VertexPtrContainerType::const_iterator,
      typename InnerVertex_::VertexPtrContainerType::      iterator>::type;

      inline const VertexPtrIterator& cosnt_vertex_ptr_iterator() const{
        return this->vertex_ptr_iterator_;
      }

      inline VertexPtrType VertexPtrContainerElement() {
        assert(!this->IsDone());
        return this->vertex_ptr_iterator_.template get_const<InnerVertex_::kVertexPtrIdx>();
      }

      inline VertexConstPtr VertexPtrContainerConstElement() const {
        assert(!this->IsDone());
        VertexPtr temp_vertex_ptr =
                 this->vertex_ptr_iterator_.template get_const<InnerVertex_::kVertexPtrIdx>();
        return VertexConstPtr(temp_vertex_ptr);
      }

    protected:
      using InnerIteratorType::IsDone;
      using InnerIteratorType::ToNext;
      using ContentPtr = typename std::conditional<is_const_, 
                const EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_*,
                      EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_*>::type;
      static constexpr bool kIsConst_ = is_const_;

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
      EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_()
                                  : InnerIteratorType(),
                                 vertex_ptr_iterator_() {
        return;
      }

      template <typename... ParameterTypes>
      EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_(
          const VertexPtrIterator& vertex_ptr_iterator,
          const ParameterTypes&... parameters)
               : InnerIteratorType(parameters...),
              vertex_ptr_iterator_(vertex_ptr_iterator) {
        return;
      }

      inline VertexPtrType src_ptr() {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return InnerIteratorType::vertex_ptr();
        /// this->direction_ == EdgeDirection::InputEdge
        return this->VertexPtrContainerElement();
      }
      
      inline VertexConstPtr src_ptr() const {
        return this->const_src_ptr();
      }

      inline VertexConstPtr const_src_ptr() const {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return InnerIteratorType::const_vertex_ptr();
        /// this->direction_ == EdgeDirection::InputEdge
        return this->VertexPtrContainerConstElement();
      }
      
      inline VertexPtrType dst_ptr() {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->VertexPtrContainerElement();
        /// this->direction_ == EdgeDirection::InputEdge
        return InnerIteratorType::vertex_ptr();
      }
      
      inline VertexConstPtr dst_ptr() const {
        return this->const_dst_ptr();
      }
      
      inline VertexConstPtr const_dst_ptr() const {
        assert(!this->IsDone());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->VertexPtrContainerConstElement();
        /// this->direction_ == EdgeDirection::InputEdge
        return InnerIteratorType::const_vertex_ptr();
      }

     private:
      VertexPtrIterator vertex_ptr_iterator_;
    };

   public:
    static constexpr TupleIdxType kEdgeIDIdx = 0;
    static constexpr TupleIdxType kEdgeAttributePtrIdx = 1;
    using DecomposedEdgeContainerType
                      = Container<decomposed_edge_container_type,
                                  decomposed_edge_container_sort_type, 
                                  EdgeIDType,
                                  EdgeAttributeType*>;
    using DecomposedEdgeIteratorType =
        typename DecomposedEdgeContainerType::iterator;
    using EdgeSizeType = typename DecomposedEdgeContainerType::size_type;

    static constexpr TupleIdxType kVertexPtrIdx = 0;
    static constexpr TupleIdxType kDecomposedEdgeContainerIdx = 1;
    using VertexPtrContainerType
                 = Container<vertex_ptr_container_type, 
                             vertex_ptr_container_sort_type,
                             VertexPtr, 
                             DecomposedEdgeContainerType>;
    using VertexPtrIteratorType = typename VertexPtrContainerType::iterator;
    using VertexPtrSizeType     = typename VertexPtrContainerType::size_type;

   private:
    template<typename CounterType_>
    class Counter_{
     private:
      CounterType_ counter_;

     public:
      using CounterType = CounterType_;
      
      Counter_():counter_(0){
        return;
      }
      
      Counter_(const Counter_&) = default;
      Counter_(Counter_&&) = default;

      Counter_& operator=(const Counter_&) = default;	  
      Counter_& operator=(Counter_&&) = default;	

      inline const CounterType_& counter() const{
        return this->counter_;
      }

      inline void AddOne(){
        ++this->counter_;
        return;
      }

      inline void ReduceOne(){
        --this->counter_;
        return;
      }
    };

    using EdgeCounterType 
            = Counter_<EdgeSizeType>;

    using VertexCounterType
              = Counter_<VertexPtrSizeType>;          

   public:
    static constexpr TupleIdxType kEdgeLabelIdx = 0;
    static constexpr TupleIdxType kVertexPtrContainerIdx = 1;
    static constexpr TupleIdxType kEdgeLabelEdgeCounterIdx = 2;
    using EdgeLabelContainerType
                 = Container<edge_label_container_type, 
                             edge_label_container_sort_type,
                             EdgeLabelType,
                             VertexPtrContainerType, 
                             EdgeCounterType>;
    using EdgeLabelIteratorType = typename EdgeLabelContainerType::iterator;

    //    static constexpr TupleIdxType kEdgeIsOutputIdx = 0;
    //    static constexpr TupleIdxType kEdgeLabelContainerIdx = 1;
    //    using EdgeContainerType =
    //        Container<ContainerType::Vector,
    //                  SortType::Default,
    //                  bool,
    //                  EdgeLabelContainerType>;

    template <bool is_const_, 
              bool with_const_methods_,
              bool meaning_less_ = true>
    class EdgePtrContent_ {
     private:
      /// trick the compiler, equivalent to static_assert(false)
      static_assert( is_const_, "Illegal configuration");
      static_assert(!is_const_, "Illegal configuration");
    };

    /// content pointer with only constant method
    template <bool is_const_, 
              bool meaning_less_>
    class EdgePtrContent_<is_const_, false, meaning_less_> {
      static_assert(meaning_less_, "Illegal usage of this method");

     public:
     protected:
      using EdgeLabelIteratorType = typename std::conditional<is_const_, 
                        typename EdgeLabelContainerType::const_iterator,
                        typename EdgeLabelContainerType::iterator>::type;
      using VertexPtrIteratorType = typename std::conditional<is_const_, 
                        typename VertexPtrContainerType::const_iterator,
                        typename VertexPtrContainerType::iterator>::type;
      using DecomposedEdgeIteratorType = typename std::conditional<is_const_, 
                        typename DecomposedEdgeContainerType::const_iterator,
                        typename DecomposedEdgeContainerType::iterator>::type;

      using VertexPtrType = typename std::conditional<is_const_,
                                                 VertexConstPtr,
                                                 VertexPtr>::type;

     public:
      EdgePtrContent_()
          : direction_(),
            vertex_ptr_(),
            edge_label_iterator_(),
            vertex_ptr_iterator_(),
            decomposed_edge_iterator_() {
        return;
      }

      EdgePtrContent_(
          const enum EdgeDirection& direction, 
          const VertexPtrType& vertex_ptr,
          const EdgeLabelIteratorType& edge_label_iterator,
          const VertexPtrIteratorType& vertex_ptr_iterator,
          const DecomposedEdgeIteratorType& decomposed_edge_iterator)
          : direction_(direction),
            vertex_ptr_(vertex_ptr),
            edge_label_iterator_(edge_label_iterator),
            vertex_ptr_iterator_(vertex_ptr_iterator),
            decomposed_edge_iterator_(decomposed_edge_iterator) {
        return;
      }

      inline const VertexConstPtr VertexPtrContainerConstElement() const {
        assert(!this->IsNull());
        return this->vertex_ptr_iterator_.template get_const<kVertexPtrIdx>();
      }

      inline const EdgeAttributeType& const_attribute() const {
        assert(!this->IsNull());
        return *(this->decomposed_edge_iterator_
                      .template get_const<kEdgeAttributePtrIdx>());
      }

      inline bool IsNull() const {
        return this->vertex_ptr_.IsNull(); 
      }

      /// public methods same to ContentEdgeIterator_
      inline const EdgeLabelType& label() const {
        assert(!this->IsNull());
        return this->edge_label_iterator_.template get_const<kEdgeLabelIdx>();
      }

      inline const EdgeIDType& id() const {
        assert(!this->IsNull());
        return this->decomposed_edge_iterator_.template get_const<kEdgeIDIdx>();
      }

      inline VertexConstPtr const_src_ptr() const {
        assert(!this->IsNull());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->vertex_ptr_;
        return this->VertexPtrContainerConstElement();
      }

      inline VertexConstPtr src_ptr() const {
        return this->const_src_ptr();
      }

      inline VertexConstPtr const_dst_ptr() const {
        assert(!this->IsNull());
        if (this->direction_ == EdgeDirection::OutputEdge)
          return this->VertexPtrContainerConstElement();
        return this->vertex_ptr_;
      }

      inline VertexConstPtr dst_ptr() const {
        return this->const_dst_ptr();
      }

      template <typename ConcreteDataType>
      inline const ConcreteDataType& const_attribute(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsNull());
        return this->const_attribute()
           .template const_attribute<ConcreteDataType>(key);
      }

      inline EdgeAttributeConstIterator AttributeCBegin() const {
        assert(!this->IsNull());
        return this->const_attribute().AttributeCBegin();
      }

      inline EdgeAttributeConstPtr FindConstAttributePtr(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsNull());
        return this->const_attribute().FindConstAttributePtr(key);
      }

      inline const std::string attribute_value_type_name(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsNull());
        return this->const_attribute().attribute_value_type_name(key);
      }

      inline const enum BasicDataType attribute_value_type(
          const EdgeAttributeKeyType& key) const {
        assert(!this->IsNull());
        return this->const_attribute().attribute_value_type(key);
      }

     protected:
      enum EdgeDirection direction_;
      VertexPtrType vertex_ptr_;

           EdgeLabelIteratorType      edge_label_iterator_;
           VertexPtrIteratorType      vertex_ptr_iterator_;
      DecomposedEdgeIteratorType decomposed_edge_iterator_;
    };

    /// content pointer with non-constant method
    template <bool meaning_less_>
    class EdgePtrContent_<false, true, meaning_less_>
        : public EdgePtrContent_<false, false, meaning_less_> {
      static_assert(meaning_less_, "Illegal usage of this method");

     private:
      using EdgePtrContent = EdgePtrContent_<false, false, meaning_less_>;

      inline VertexPtr VertexPtrContainerElement() {
        assert(!this->IsNull());
        return EdgePtrContent::vertex_ptr_iterator_
                              .template get_const<kVertexPtrIdx>();
      }

      inline EdgeAttributeType& _attribute() {
        assert(!this->IsNull());
        return *(EdgePtrContent::decomposed_edge_iterator_
                                .template get<kEdgeAttributePtrIdx>());
      }

     public:
      using EdgePtrContent::EdgePtrContent;

      inline VertexPtr src_ptr() {
        assert(!this->IsNull());
        if (EdgePtrContent::direction_ == EdgeDirection::OutputEdge)
          return EdgePtrContent::vertex_ptr_;
        return this->VertexPtrContainerElement();
      }

      inline VertexPtr dst_ptr() {
        assert(!this->IsNull());
        if (EdgePtrContent::direction_ == EdgeDirection::OutputEdge)
          return this->VertexPtrContainerElement();
        return EdgePtrContent::vertex_ptr_;
      }

      template <typename ConcreteDataType>
      inline ConcreteDataType& attribute(const EdgeAttributeKeyType& key) {
        assert(!this->IsNull());
        return this->_attribute().template attribute<ConcreteDataType>(key);
      }

      inline EdgeAttributeIterator AttributeBegin() {
        assert(!this->IsNull());
        return this->_attribute().AttributeBegin();
      }

      inline EdgeAttributePtr FindAttributePtr(
          const EdgeAttributeKeyType& key) {
        assert(!this->IsNull());
        return this->_attribute().FindAttributePtr(key);
      }

      template <typename ConcreteDataType>
      inline std::pair<EdgeAttributePtr, bool> AddAttribute(
          const EdgeAttributeKeyType& key, 
          const     ConcreteDataType& value) {
        assert(!this->IsNull());
        return this->_attribute().AddAttribute(key, value);
      }
      
      inline std::pair<EdgeAttributePtr, bool> AddAttribute(
          const EdgeAttributeKeyType& key, 
          const enum BasicDataType& data_type,
          const std::string& value_str) {
        assert(!this->IsNull());
        return this->_attribute().AddAttribute(key, 
                                               data_type, 
                                               value_str);
      }

      template <typename ConcreteDataType>
      inline std::pair<EdgeAttributePtr, bool> SetAttribute(
          const EdgeAttributeKeyType& key, const ConcreteDataType& value) {
        assert(!this->IsNull());
        return this->_attribute().SetAttribute(key, value);
      }

      inline EdgeAttributeIterator EraseAttribute(
          const EdgeAttributeIterator& attribute_iterator) {
        assert(!this->IsNull());
        return this->_attribute().EraseAttribute(attribute_iterator);
      }

      inline bool EraseAttribute(const EdgeAttributeKeyType& key) {
        assert(!this->IsNull());
        return this->_attribute().EraseAttribute(key);
      }
    };

   public:
    template <bool is_const_>
    class EdgePtr_ : protected EdgePtrContent_<is_const_, !is_const_> {
     private:
      friend class EdgePtr_<!is_const_>;

      using EdgePtrContentType = EdgePtrContent_<is_const_, !is_const_>;

      using EdgePtrContentTypePtrType = typename std::conditional<is_const_,
                                                  const EdgePtrContentType*,
                                                        EdgePtrContentType*>::type;

      // add by wangyj
      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      using FriendEdgeContentIterator
                = EdgeContentIterator_<ContainerType_, 
                                            is_const_,
                                               depth_, 
                                         begin_depth_,
                                      edge_label_idx_, 
                                         dst_ptr_idx_, 
                                         edge_id_idx_,
                              edge_attribute_ptr_idx_>;

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      using FriendEdgeIterator 
                    = Iterator_<FriendEdgeContentIterator<
                                            ContainerType_,
                                                    depth_, 
                                              begin_depth_, 
                                           edge_label_idx_, 
                                              dst_ptr_idx_,
                                              edge_id_idx_, 
                                   edge_attribute_ptr_idx_>>;

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      inline void Construct(
          const FriendEdgeIterator<ContainerType_,
                                           depth_, 
                                     begin_depth_,
                                  edge_label_idx_, 
                                     dst_ptr_idx_, 
                                     edge_id_idx_,
                          edge_attribute_ptr_idx_>& edge_iter) {
        using FriendEdgeContentIteratorType
            = FriendEdgeContentIterator<ContainerType_,
                                                depth_, 
                                          begin_depth_,
                                       edge_label_idx_, 
                                          dst_ptr_idx_,
                                          edge_id_idx_, 
                               edge_attribute_ptr_idx_>;

        const FriendEdgeContentIteratorType& edge_content_iter =
            *static_cast<const FriendEdgeContentIteratorType*>(
                static_cast<const void*>(&edge_iter));

        this->direction_ = edge_content_iter.const_direction();

        this->vertex_ptr_ = edge_content_iter.const_vertex_ptr();

        this->edge_label_iterator_ =
            edge_content_iter.template get_const_iterator<0>();

        this->vertex_ptr_iterator_ =
            edge_content_iter.template get_const_iterator<1>();

        this->decomposed_edge_iterator_ =
            edge_content_iter.template get_const_iterator<2>();

        return;
      }

      // add by HBN
      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      using FriendEdgeContentIteratorSpecifiedEdgeLabel
                = EdgeContentIteratorSpecifiedEdgeLabel_<
                                          ContainerType_, 
                                               is_const_,
                                                  depth_, 
                                            begin_depth_, 
                                         edge_label_idx_,
                                            dst_ptr_idx_, 
                                            edge_id_idx_, 
                                 edge_attribute_ptr_idx_>;

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_,
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_,
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      using FriendEdgeIteratorSpecifiedEdgeLabel 
                    = Iterator_<FriendEdgeContentIteratorSpecifiedEdgeLabel<
                                                              ContainerType_,
                                                                      depth_,
                                                                begin_depth_, 
                                                             edge_label_idx_,
                                                                dst_ptr_idx_, 
                                                                edge_id_idx_, 
                                                     edge_attribute_ptr_idx_>>;

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      inline void Construct(
          const FriendEdgeIteratorSpecifiedEdgeLabel<
                                       ContainerType_, 
                                               depth_, 
                                         begin_depth_, 
                                      edge_label_idx_,
                                         dst_ptr_idx_, 
                                         edge_id_idx_, 
                              edge_attribute_ptr_idx_>& edge_iter) {
        using FriendEdgeContentIteratorSpecifiedEdgeLabelType
           =  FriendEdgeContentIteratorSpecifiedEdgeLabel<
                                            ContainerType_,
                                                    depth_, 
                                              begin_depth_, 
                                           edge_label_idx_,
                                              dst_ptr_idx_, 
                                              edge_id_idx_, 
                                   edge_attribute_ptr_idx_>;

        const FriendEdgeContentIteratorSpecifiedEdgeLabelType&
            edge_content_iter = *static_cast<
                const FriendEdgeContentIteratorSpecifiedEdgeLabelType*>(
                static_cast<const void*>(&edge_iter));

        this->direction_ = edge_content_iter.const_direction();

        this->vertex_ptr_ = edge_content_iter.const_vertex_ptr();

        this->edge_label_iterator_ = edge_content_iter.const_edge_label_iterator();

        this->vertex_ptr_iterator_ =
            edge_content_iter.template get_const_iterator<0>();

        this->decomposed_edge_iterator_ =
                 edge_content_iter.template get_const_iterator<1>();

        return;
      }

     public:
      using EdgePtrContentType::IsNull;

      // by HBN
      // template <typename... ParameterTypes>
      // EdgePtr_(const ParameterTypes&... parameters)
      //    : EdgePtrContentType(parameters...) {}

      EdgePtr_() = default;

      using EdgePtrContentType::EdgePtrContentType;

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      EdgePtr_(const FriendEdgeIterator<ContainerType_,
                                                depth_, 
                                          begin_depth_,
                                       edge_label_idx_, 
                                          dst_ptr_idx_, 
                                          edge_id_idx_,
                               edge_attribute_ptr_idx_>& edge_iterator) {
        this->Construct(edge_iterator);
        return;
      }

      inline operator bool() const {
        return !this->IsNull();
      }

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      EdgePtr_(const FriendEdgeIteratorSpecifiedEdgeLabel<
                                            ContainerType_,
                                                    depth_, 
                                              begin_depth_, 
                                           edge_label_idx_,
                                              dst_ptr_idx_, 
                                              edge_id_idx_,
                                   edge_attribute_ptr_idx_>& edge_iterator) {
        this->Construct(edge_iterator);
        return;
      }

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      inline EdgePtr_& operator=(const FriendEdgeIterator<
                                            ContainerType_,
                                                    depth_, 
                                              begin_depth_,
                                           edge_label_idx_, 
                                              dst_ptr_idx_, 
                                              edge_id_idx_,
                                   edge_attribute_ptr_idx_>& edge_iterator) {
        this->Construct(edge_iterator);
        return *this;
      }

      template <typename              ContainerType_,
                IteratorDepthType             depth_,
                IteratorDepthType       begin_depth_, 
                TupleIdxType         edge_label_idx_,
                TupleIdxType            dst_ptr_idx_, 
                TupleIdxType            edge_id_idx_,
                TupleIdxType edge_attribute_ptr_idx_>
      inline EdgePtr_& operator=(const FriendEdgeIteratorSpecifiedEdgeLabel<
                                                              ContainerType_,
                                                                      depth_, 
                                                                begin_depth_, 
                                                             edge_label_idx_,
                                                                dst_ptr_idx_, 
                                                                edge_id_idx_, 
                                                     edge_attribute_ptr_idx_>&
                                                                edge_iterator) {
        this->Construct(edge_iterator);
        return *this;
      }

      template <bool input_is_const_>
      inline bool operator==(const EdgePtr_<input_is_const_>& edge_ptr) const {
        using InputEdgePtrType = EdgePtr_<input_is_const_>;
        using InputEdgePtrContentType =
            typename InputEdgePtrType::EdgePtrContentType;
        assert((edge_ptr.InputEdgePtrContentType::id()
                           != EdgePtrContentType::id()) 
          || !((edge_ptr.InputEdgePtrContentType::label()
                           != EdgePtrContentType::label()) 
            || (edge_ptr.InputEdgePtrContentType::const_src_ptr()
                           != EdgePtrContentType::const_src_ptr()) 
            || (edge_ptr.InputEdgePtrContentType::const_dst_ptr() 
                           != EdgePtrContentType::const_dst_ptr())));
        return  edge_ptr.InputEdgePtrContentType::id()
                           == EdgePtrContentType::id();
      }

      inline EdgePtrContentTypePtrType operator->() const {
        assert(!this->IsNull());
        EdgePtrContentTypePtrType const temp_ptr = this;
        return temp_ptr;
      }

      template <bool judge = is_const_,
                typename std::enable_if<!judge, bool>::type = false>
      inline EdgePtrContentTypePtrType operator->() {
        assert(!this->IsNull());
        static_assert(judge == is_const_, "Illegal usage of this method");
        EdgePtrContentTypePtrType const temp_ptr = this;
        return temp_ptr;
      }
    };

    using EdgePtr      = EdgePtr_<false>;
    using EdgeConstPtr = EdgePtr_< true>;

   private:
    template<enum StoreData store_data, 
             bool meaning_less>
    class Edges;

    template<bool meaning_less>
    class Edges<StoreData::OutEdge, meaning_less> {
     private:
      static_assert(meaning_less, "Illegal configuration");

      /// container for all output edges
      EdgeLabelContainerType out_edges_;

      // counters, in order to accelerate the count methods
      using   EdgeInnerCounterType = typename   EdgeCounterType
                                                  ::CounterType;
      using VertexInnerCounterType = typename VertexCounterType
                                                  ::CounterType;
        EdgeCounterType   out_edge_counter_;
      VertexCounterType out_vertex_counter_;

     public:
      Edges():out_edges_(),
              out_edge_counter_(),
              out_vertex_counter_(){
        return;
      }

      inline const EdgeLabelContainerType&
             const_out_edges() const {
        return this->out_edges_;
      }

      inline EdgeLabelContainerType& out_edges() { 
        return this->out_edges_; 
      }

      inline void OutEdgeAddOne(){
        this->out_edge_counter_.AddOne();
        return;
      }

      inline void OutEdgeReduceOne(){
        this->out_edge_counter_.ReduceOne();
        return;
      }

      inline const EdgeInnerCounterType& 
                OutEdgeCounter() const{
        return this->out_edge_counter_.counter();
      }

      inline void OutVertexAddOne(){
        this->out_vertex_counter_.AddOne();
        return;
      }
      
      inline void OutVertexReduceOne(){
        this->out_vertex_counter_.ReduceOne();
        return;
      }

      inline const VertexInnerCounterType& 
                OutVertexCounter() const{
        return this->out_vertex_counter_.counter();
      }
    };

    template<bool meaning_less>
    class Edges<StoreData::InOutEdge, meaning_less>
        : public Edges<StoreData::OutEdge, meaning_less> {
     private:
      static_assert(meaning_less, "Illegal configuration");

      /// container for all input edges
      EdgeLabelContainerType in_edges_;

      // counters, in order to accelerate the count methods
      using   EdgeInnerCounterType = typename   EdgeCounterType
                                                  ::CounterType;
      using VertexInnerCounterType = typename VertexCounterType
                                                  ::CounterType;
        EdgeCounterType   in_edge_counter_;
      VertexCounterType in_vertex_counter_;

      using InnerEdgeType 
               = Edges<StoreData::OutEdge, meaning_less>;

     public:
      Edges():InnerEdgeType(),
              in_edges_(),
              in_edge_counter_(),
              in_vertex_counter_(){
        return;
      }

      inline const EdgeLabelContainerType& const_in_edges() const {
        return this->in_edges_;
      }
      
      inline EdgeLabelContainerType& in_edges() { 
        return this->in_edges_; 
      }

      inline void InEdgeAddOne(){
        this->in_edge_counter_.AddOne();
        return;
      }

      inline void InEdgeReduceOne(){
        this->in_edge_counter_.ReduceOne();
        return;
      }

      inline const EdgeInnerCounterType& 
                 InEdgeCounter() const{
        return this->in_edge_counter_.counter();
      }
      
      inline void InVertexAddOne(){
        this->in_vertex_counter_.AddOne();
        return;
      }

      inline void InVertexReduceOne(){
        this->in_vertex_counter_.ReduceOne();
        return;
      }

      inline const VertexInnerCounterType& 
                 InVertexCounter() const{
        return this->in_vertex_counter_.counter();
      }
    };

    inline void AddInEdge(
        const VertexPtr& dst_ptr,
        const EdgeLabelType& edge_label,
        const EdgeIDType& edge_id,
              EdgeAttributeType* edge_attribute_ptr) {
      /// maintain the vertex counter all input vetexes
      if (!this->FindInVertex(dst_ptr)){
        /// this vertex has no input edge point to dst_ptr
        this->edges_.InVertexAddOne();
      }
      /// maintain the edge counter for all input edges
      this->edges_.InEdgeAddOne();
      auto edge_label_ret = this->edges_.in_edges()
                                        .Insert(edge_label, 
                                                std::move(VertexPtrContainerType()), 
                                                std::move(EdgeCounterType()));
      /// maintain the edge counter for edge_label
      edge_label_ret.first.template get<kEdgeLabelEdgeCounterIdx>().AddOne();
      auto vertex_ptr_ret
         = edge_label_ret.first
                         .template get<kVertexPtrContainerIdx>()
                         .Insert(dst_ptr, 
                                 std::move(DecomposedEdgeContainerType()));
      auto edge_ret = vertex_ptr_ret.first
                                    .template get<kDecomposedEdgeContainerIdx>()
                                    .Insert(edge_id,
                                            std::move(edge_attribute_ptr));
      assert(edge_ret.second);
      return;
    }

    inline std::pair<EdgePtr, bool> AddOutEdge(
        const VertexPtr& dst_ptr, 
        const EdgeLabelType& edge_label,
        const EdgeIDType& edge_id,
              EdgeAttributeType* edge_attribute_ptr) {
      /// maintain the vertex counter for all output vertexes
      if (!this->FindOutVertex(dst_ptr)){
        /// this vertex has no output edge point to dst_ptr
        this->edges_.OutVertexAddOne();
      }
      /// maintain the edge counter for all output edges
      this->edges_.OutEdgeAddOne();
      auto edge_label_it = this->edges_.out_edges()
                                       .Insert(edge_label, 
                                               VertexPtrContainerType(), 
                                               EdgeCounterType())
                                       .first;
      /// maintain the edge counter for edge_label
      edge_label_it.template get<kEdgeLabelEdgeCounterIdx>()
                   .AddOne();
      auto vertex_ptr_it
         = edge_label_it.template get<kVertexPtrContainerIdx>()
                        .Insert(dst_ptr, 
                                DecomposedEdgeContainerType())
                        .first;
      auto edge_it = vertex_ptr_it
                    .template get<kDecomposedEdgeContainerIdx>()
                    .Insert(edge_id,
                            std::move(edge_attribute_ptr))
                    .first;
      InnerVertex_* const temp_this_ptr = this;
      VertexPtr temp_vertex_ptr(temp_this_ptr);
      assert(allow_duplicate_edge);
      /// otherwise, needs to decide the second element
      /// in the pair
      return std::pair<EdgePtr, bool>(
          EdgePtr(EdgeDirection::OutputEdge,
                  temp_vertex_ptr,  /// src_ptr
                  edge_label_it, 
                  vertex_ptr_it, 
                        edge_it),
          true);
    }

    /*
    inline bool EraseOutEdge(const VertexPtr& dst_ptr,
                             const EdgeLabelType& edge_label,
                             const EdgeIDType& edge_id) {
      auto& out_edges_container = this->edges_.out_edges();
      auto edge_label_it = out_edges_container.Find(edge_label).first;
      auto& vertex_ptr_container =
          std::get<kVertexPtrContainerIdx>(*edge_label_it);
      auto vertex_ptr_it = vertex_ptr_container.Find(dst_ptr).first;
      auto& edge_id_container =
          std::get<kDecomposedEdgeContainerIdx>(*vertex_ptr_it);
      auto edge_it = edge_id_container.Find(edge_id).first;
      // erase edge_attr
      delete std::get<kEdgeAttributePtrIdx>(*edge_it);
      // erase edge_id
      edge_id_container.Erase(edge_it);
      // erase dst_ptr
      if (edge_id_container.empty()) {
        vertex_ptr_container.Erase(vertex_ptr_it);
      }
      // erase edge_label
      if (vertex_ptr_container.empty()) {
        out_edges_container.Erase(edge_label_it);
      }
      return true;
    }
    */

   public:
    InnerVertex_(const VertexIDType& id, 
                 const VertexLabelType& label)
                     : VertexWithIDType(id),
                       VertexWithLabelType(label),
                       VertexAttributeType(std::make_pair(label, true)) {
      return;
    }

    ~InnerVertex_() {
      for (auto edge_label_it  = this->edges_.out_edges().begin();
                edge_label_it != this->edges_.out_edges(). end ();
                edge_label_it++) {
        for (auto vertex_ptr_it  = edge_label_it.template get<kVertexPtrContainerIdx>().begin();
                  vertex_ptr_it != edge_label_it.template get<kVertexPtrContainerIdx>(). end ();
                  vertex_ptr_it++) {
          for (auto decomposed_edge_it
                       = vertex_ptr_it.template get<kDecomposedEdgeContainerIdx>().begin();
                    decomposed_edge_it
                      != vertex_ptr_it.template get<kDecomposedEdgeContainerIdx>(). end ();
                    decomposed_edge_it++) {
            delete decomposed_edge_it.template get<kEdgeAttributePtrIdx>();
          }
        }
      }
      return;
    }
    /*
    inline bool EraseEdge(VertexPtr& dst_ptr, const EdgeLabelType& edge_label,
                          const EdgeIDType& edge_id) {
      auto ret = this->FindOutEdge(edge_id);
      if (ret.IsNull()) {
        return false;
      }
      InnerVertex_* const temp_this_ptr = this;
      VertexPtr temp_vertex_ptr = VertexPtr(temp_this_ptr);
      dst_ptr->EraseInEdge(temp_vertex_ptr, edge_label, edge_id);
      this->EraseOutEdge(dst_ptr, edge_label, edge_id);
      return true;
    }*/

   private:
    inline void EraseEdge(
          const enum EdgeDirection& edge_direction,
               const EdgeLabelType& edge_label,
               const VertexPtr&   vertex_ptr,
               const EdgeIDType&    edge_id) {
      // first level container, decide which container to remove
      EdgeLabelContainerType* edge_label_container_ptr = nullptr;
      bool has_vertex = false;
      if (edge_direction == EdgeDirection::OutputEdge) {
        edge_label_container_ptr = &this->edges_.out_edges();
        /// maintain the edge counter for all output edges
        this->edges_.OutEdgeReduceOne();
      }
      else{
        edge_label_container_ptr = &this->edges_.in_edges();
        /// maintain the edge counter for all input edges
        this->edges_.InEdgeReduceOne();
      }
      assert(edge_label_container_ptr != nullptr);
      /// <iterator of EdgeLabelContainer, bool>
      auto edge_label_ret = edge_label_container_ptr
                     ->Find(edge_label);
      assert(edge_label_ret.second);
      /// maintain the edge counter for edge_label
      edge_label_ret.first
                    .template get<kEdgeLabelEdgeCounterIdx>()
                    .ReduceOne();
      // second level container
      VertexPtrContainerType& vertex_ptr_container
            = edge_label_ret.first.template get<kVertexPtrContainerIdx>();
      /// <iterator of VertexPtrContainer, bool>
      auto vertex_ptr_ret = vertex_ptr_container.Find(vertex_ptr);
      assert(vertex_ptr_ret.second);
      // third level container
      DecomposedEdgeContainerType& decomposed_edge_container
                                 = vertex_ptr_ret.first
                                  .template get<kDecomposedEdgeContainerIdx>();
      /// <iterator of DecomposedEdgeContainer, bool>
      auto decomposed_edge_ret = decomposed_edge_container
                                .Find(edge_id);
      assert(decomposed_edge_ret.second);

      decomposed_edge_container.Erase(decomposed_edge_ret.first);
      if (!decomposed_edge_container.empty()) {
        // no further modificaiton is required
        return;
      }
      // the decomposed_edge_container is empty,
      // therefore, there are no edge point to vertex_ptr
      // remove it from the vertex_ptr_container
      vertex_ptr_container.Erase(vertex_ptr_ret.first);
      /// there are no longer any edge with label of edge_label
      /// point to vertex_ptr, test whether there are edges
      /// with other edge_label points to vertex_ptr
      if (!this->FindVertexExceptEdgeLabel(
                *edge_label_container_ptr,
                 vertex_ptr_ret.first
                               .template get_const<kVertexPtrIdx>(),
                 edge_label)){
        /// does not have other edges connect to vertex_ptr
        /// maintain the vertex counter for in/out vertex
        if (edge_direction == EdgeDirection::OutputEdge) {
          this->edges_.OutVertexReduceOne();
        }
        else{
          this->edges_.InVertexReduceOne();
        }
      }
      if (!vertex_ptr_container.empty()){
        // no further modificaiton is required
        return;
      }
      // the vertex_ptr_container is empty,
      // therefore, there are no edge has Label edge_label
      // remove it from the edge_label_container
      edge_label_container_ptr->Erase(edge_label_ret.first);
      return;
    }

    inline bool EraseEdge(
          EdgeLabelContainerType& edge_label_container,
        const enum EdgeDirection& edge_direction,
        const      EdgeIDType&    edge_id) {
      for (auto edge_label_it  = edge_label_container.begin();
                edge_label_it != edge_label_container.end();
                edge_label_it++) {
        VertexPtrContainerType& vertex_ptr_container
            = edge_label_it.template get<kVertexPtrContainerIdx>();
        for (auto vertex_ptr_it  = vertex_ptr_container.begin();
                  vertex_ptr_it != vertex_ptr_container.end();
                  vertex_ptr_it++) {
          DecomposedEdgeContainerType& decomposed_edge_container
              = vertex_ptr_it.template get<kDecomposedEdgeContainerIdx>();
          auto edge_id_ret = decomposed_edge_container.Find(edge_id);
          if (!edge_id_ret.second) {
            // does not find edge_id in this container
            continue;
          }
          // find edge_id in this container, erase it
          delete edge_id_ret.first
                .template get<kEdgeAttributePtrIdx>();
          decomposed_edge_container.Erase(edge_id_ret.first);
          /// maintain the edge counter of all in/out edges
          if (edge_direction == EdgeDirection::OutputEdge)
            this->edges_.OutEdgeReduceOne();
          else
            this->edges_.InEdgeReduceOne();
          /// maintain the edge counter of edge_label
          edge_label_it.template get<kEdgeLabelEdgeCounterIdx>()
                       .ReduceOne();
          InnerVertex_* temp_this_ptr = this;
          VertexPtr temp_vertex_ptr(temp_this_ptr);
          vertex_ptr_it.template get<kVertexPtrIdx>()
              ->EraseEdge(edge_direction
                       == EdgeDirection:: InputEdge
                        ? EdgeDirection::OutputEdge
                        : EdgeDirection:: InputEdge,
                          edge_label_it.template get<kEdgeLabelIdx>(),
                          temp_vertex_ptr, edge_id);
          if (!decomposed_edge_container.empty()) {
            return true;
          }
          /// to be used in FindVertexExceptEdgeLabel method
          const VertexPtr temp_dst_vertex_ptr 
                                 = vertex_ptr_it
                                  .template get<kVertexPtrIdx>();
          // the decomposed_edge_container is empty,
          // therefore, there are no edge point to vertex_ptr
          // remove it from the vertex_ptr_container
          vertex_ptr_container.Erase(vertex_ptr_it);
          /// there are no longer any edge with label of
          /// edge_label point to vertex_ptr, test whether
          /// there are edges with other edge_label points
          /// to vertex_ptr
          if (!this->FindVertexExceptEdgeLabel(
                     edge_label_container,
                     temp_dst_vertex_ptr,
                     edge_label_it.template get<kEdgeLabelIdx>())){
            /// does not have other edges connect to vertex_ptr
            /// maintain the vertex counter for in/out vertex
            if (edge_direction == EdgeDirection::OutputEdge)
              this->edges_.OutVertexReduceOne();
            else
              this->edges_.InVertexReduceOne();
          }
          if (!vertex_ptr_container.empty()) {
            // no further modificaiton is required
            return true;
          }
          // the vertex_ptr_container is empty,
          // therefore, there are no edge has Label
          // edge_label remove it from the
          // edge_label_container
          edge_label_container.Erase(edge_label_it);
          return true;
        }
      }
      return false;
    }

   public:
    inline bool EraseEdge(const EdgeIDType& edge_id) {
      if (this->EraseEdge(this->edges_.out_edges(), 
                          EdgeDirection::OutputEdge,
                          edge_id))
        return true;
      return this->EraseEdge(this->edges_.in_edges(), 
                             EdgeDirection::InputEdge,
                             edge_id);
    }
    /*
    inline EdgeIterator EraseEdge(EdgeIterator& edge_iterator) {
      // check IsDone
      if (!edge_iterator.IsDone()) {
        return edge_iterator;
      }
      std::pair<EdgeIterator, bool> erase_res = this->EraseEdge(
          this->edges_.out_edges(), EdgeDirection::OutputEdge, edge_iterator);
      if (erase_res.second) return erase_res.first;
      erase_res = this->EraseEdge(this->edges_.in_edges(),
                                  EdgeDirection::InputEdge, edge_iterator);
      return erase_res.first;
    }
    */
    /*
    inline bool EraseOutEdge(const EdgeIDType& edge_id) {
      return this->EraseEdge(this->edges_.out_edges(), edge_id);
    }
    inline bool EraseInEdge(const EdgeIDType& edge_id) {
      return this->EraseEdge(this->edges_.in_edges(), edge_id);
    }*/

    inline std::pair<EdgePtr, bool> AddEdge(VertexPtr& dst_ptr,
                                  const EdgeLabelType& edge_label,
                                  const EdgeIDType&    edge_id) {
      auto ret = this->FindOutEdge(edge_id);
      if (!ret.IsNull()) {
        /// this Edge has already been existed
        return std::pair<EdgePtr, bool>(ret, false);
      }
      EdgeAttributeType* edge_attribute_ptr 
                   = new EdgeAttributeType(
                            std::make_pair(edge_label, false));
      InnerVertex_* const temp_this_ptr = this;
      VertexPtr temp_vertex_ptr = VertexPtr(temp_this_ptr);
      dst_ptr->AddInEdge(temp_vertex_ptr, 
                         edge_label,
                         edge_id,
                         edge_attribute_ptr);
      return this->AddOutEdge(dst_ptr, 
                              edge_label, 
                              edge_id, 
                              edge_attribute_ptr);
    }
    /// possible extension:
    ///     AddEdge(dst_ptr,edge_label);
    ///         if duplicate edge is not allowed

    // public:
    /// access auxiliary data, will not be implement in this beggar version
    //    const VertexAuxiliaryType& const_auxiliary() const;
    //    VertexAuxiliaryType& auxiliary();

    /// access element, will not be implement in this beggar version
    //    EdgeLabelConstPtr FindConstOutEdgeLabel(const EdgeLabelType&
    //    edge_label) const; EdgeLabelConstPtr
    //    FindConstInEdgeLabel(const EdgeLabelType& edge_label) const;

   private:
    VertexPtr FindVertexExceptEdgeLabel(
      const EdgeLabelContainerType& edge_label_container,
                   const VertexPtr& vertex_ptr,
               const EdgeLabelType& edge_label) const {
      for (auto edge_label_cit  = edge_label_container.cbegin();
                edge_label_cit != edge_label_container.cend();
                edge_label_cit++) {
        if (edge_label_cit.template get_const<kEdgeLabelIdx>()
         == edge_label){
          continue;
        }
        // <const_iterator, bool>
        auto ret = edge_label_cit
                  .template get_const<kVertexPtrContainerIdx>()
                  .FindConst(vertex_ptr);
        if (!ret.second){
          /// does not found that vertex
          continue; /// continue to the next edge_label
        }
        /// found that!
        return ret.first
                  .template get_const<kVertexPtrIdx>();
      }
      /// does not find that value
      return VertexPtr();
    }

    VertexPtr FindVertex(
                EdgeLabelContainerType& edge_label_container,
                       const VertexPtr& dst_ptr) {
      for (auto edge_label_it  = edge_label_container.begin();
                edge_label_it != edge_label_container.end();
                edge_label_it++) {
        /// <iterator, bool>
        auto ret = edge_label_it
                  .template get<kVertexPtrContainerIdx>()
                  .Find(dst_ptr);
        if (ret.second) {
          /// found it
          return ret.first.template get_const<kVertexPtrIdx>();
        }
      }
      return VertexPtr();
    }

    VertexConstPtr FindConstVertex(
        const EdgeLabelContainerType& edge_label_container,
        const VertexPtr& dst_ptr) const {
      for (auto edge_label_it  = edge_label_container.cbegin();
                edge_label_it != edge_label_container.cend(); 
              ++edge_label_it) {
        /// <iterator, bool>
        auto ret = edge_label_it
                  .template get<kVertexPtrContainerIdx>()
                  .FindConst(dst_ptr);
        if (ret.second) {
          /// found it
          return ret.first.template get<kVertexPtrIdx>();
        }
      }
      return VertexConstPtr();
    }

    VertexPtr FindVertex(EdgeLabelContainerType& edge_label_container,
                         const VertexIDType& dst_id) {
      for (auto edge_label_it  = edge_label_container.begin();
                edge_label_it != edge_label_container. end ();
                edge_label_it++) {
        for (auto vertex_it  = edge_label_it.template get<kVertexPtrContainerIdx>().begin();
                  vertex_it != edge_label_it.template get<kVertexPtrContainerIdx>(). end ();
                  vertex_it++) {
          VertexPtr vertex_ptr = vertex_it.template get<kVertexPtrIdx>();
          if (vertex_ptr->id() == dst_id) 
            return vertex_ptr;  /// found it
        }
      }
      /// not found
      return VertexPtr();
    }

    VertexConstPtr FindConstVertex(
        const EdgeLabelContainerType& edge_label_container,
        const VertexIDType& dst_id) const {
      for (auto edge_label_it  = edge_label_container.cbegin();
                edge_label_it != edge_label_container. cend ();
                edge_label_it++) {
        for (auto vertex_it  = edge_label_it.template get<kVertexPtrContainerIdx>().cbegin();
                  vertex_it != edge_label_it.template get<kVertexPtrContainerIdx>(). cend ();
                  vertex_it++) {
          VertexPtr vertex_ptr = vertex_it.template get<kVertexPtrIdx>();
          if (vertex_ptr->id() == dst_id) return vertex_ptr;  /// found it
        }
      }
      /// not found
      return VertexConstPtr();
    }

    inline VertexPtr FindVertex(EdgeLabelContainerType& edge_label_container,
                                const EdgeLabelType& edge_label,
                                const VertexPtr& dst_ptr) {
      /// <iterator of EdgeLabelContainer, bool>
      auto edge_label_ret = edge_label_container.Find(edge_label);
      if (!edge_label_ret.second)  /// does not have edge_label
        return VertexPtr();
      /// <iterator of VertexPtrContainer, bool>
      auto vertex_ptr_ret
         = edge_label_ret
          .first
          .template get<kVertexPtrContainerIdx>().Find(dst_ptr);
      if (!vertex_ptr_ret.second) 
        return VertexPtr();
      /// found it
      return vertex_ptr_ret
            .first
            .template get<kVertexPtrIdx>();
    }

    inline VertexConstPtr FindConstVertex(
        const EdgeLabelContainerType& edge_label_container,
                 const EdgeLabelType& edge_label, 
                     const VertexPtr& dst_ptr) const {
      /// <iterator of EdgeLabelContainer, bool>
      const auto edge_label_ret   
               = edge_label_container.FindConst(edge_label);
      if (!edge_label_ret.second)  /// does not have edge_label
        return VertexPtr();
      /// <iterator of VertexContainer, bool>
      auto vertex_ptr_ret
         = edge_label_ret.first.template get<kVertexPtrContainerIdx>().FindConst(dst_ptr);
      if (!vertex_ptr_ret.second) return VertexPtr();
      /// found it
      return vertex_ptr_ret.first.template get<kVertexPtrIdx>();
    }

    VertexPtr FindVertex(EdgeLabelContainerType& edge_label_container,
                         const EdgeLabelType& edge_label,
                         const VertexIDType& dst_id) {
      /// <iterator of EdgeLabelContainer, bool>
      auto edge_label_ret = edge_label_container.Find(edge_label);
      if (!edge_label_ret.second)  /// does not have edge_label
        return VertexPtr();
      for (auto vertex_it  = edge_label_ret.first
                            .template get<kVertexPtrContainerIdx>().begin();
                vertex_it != edge_label_ret.first
                            .template get<kVertexPtrContainerIdx>(). end ();
                vertex_it++) {
        VertexPtr vertex_ptr = vertex_it.template get<kVertexPtrIdx>();
        if (vertex_ptr->id() == dst_id) 
          return vertex_ptr;  /// found it
      }
      /// not found
      return VertexPtr();
    }

    VertexConstPtr FindConstVertex(
        const EdgeLabelContainerType& edge_label_container,
        const EdgeLabelType& edge_label, 
        const VertexIDType& dst_id) const {
      /// <iterator of EdgeLabelContainer, bool>
      auto edge_label_ret = edge_label_container
                           .FindConst(edge_label);
      if (!edge_label_ret.second)  /// does not have edge_label
        return VertexConstPtr();
      for (auto vertex_cit  = edge_label_ret.first.template get_const<kVertexPtrContainerIdx>().cbegin();
                vertex_cit != edge_label_ret.first.template get_const<kVertexPtrContainerIdx>(). cend ();
                vertex_cit++) {
        VertexConstPtr vertex_const_ptr = vertex_cit.template get_const<kVertexPtrIdx>();
        if (vertex_const_ptr->id() == dst_id)
          return vertex_const_ptr;  /// found it
      }
      /// not found
      return VertexConstPtr();
    }

   public:
    /// out vertex
    inline VertexPtr FindOutVertex(const VertexPtr& dst_ptr) {
      return this->FindVertex(this->edges_.out_edges(), dst_ptr);
    }
    inline VertexPtr FindOutVertex(const VertexIDType& dst_id) {
      return this->FindVertex(this->edges_.out_edges(), dst_id);
    }
    inline VertexPtr FindOutVertex(const EdgeLabelType& edge_label,
                                   const VertexPtr& dst_ptr) {
      return this->FindVertex(this->edges_.out_edges(), edge_label, dst_ptr);
    }
    inline VertexPtr FindOutVertex(const EdgeLabelType& edge_label,
                                   const VertexIDType& dst_id) {
      return this->FindVertex(this->edges_.out_edges(), edge_label, dst_id);
    }
    /// out vertex const method overload 
    inline VertexConstPtr FindOutVertex(const VertexPtr& dst_ptr) const {
      return this->FindConstVertex(dst_ptr);
    }
    inline VertexConstPtr FindOutVertex(const VertexIDType& dst_id) const {
      return this->FindConstVertex(dst_id);
    }
    inline VertexConstPtr FindOutVertex(const EdgeLabelType& edge_label,
                                        const VertexPtr& dst_ptr)  const{
      return this->FindConstVertex(edge_label, dst_ptr);
    }
    inline VertexConstPtr FindOutVertex(const EdgeLabelType& edge_label,
                                        const VertexIDType& dst_id) const {
      return this->FindConstVertex(edge_label, dst_id);
    }
    /// const out vertex
    inline VertexConstPtr FindConstOutVertex(const VertexPtr& dst_ptr) const {
      return this->FindConstVertex(this->edges_.const_out_edges(), dst_ptr);
    }
    inline VertexConstPtr FindConstOutVertex(const VertexIDType& dst_id) const {
      return this->FindConstVertex(this->edges_.const_out_edges(), dst_id);
    }
    inline VertexConstPtr FindConstOutVertex(const EdgeLabelType& edge_label,
                                             const VertexPtr& dst_ptr) const {
      return this->FindConstVertex(this->edges_.const_out_edges(), edge_label,
                                   dst_ptr);
    }
    inline VertexConstPtr FindConstOutVertex(const EdgeLabelType& edge_label,
                                             const  VertexIDType& dst_id) const {
      return this->FindConstVertex(this->edges_.const_out_edges(), 
                                         edge_label,
                                         dst_id);
    }
    /// in vertex
    inline VertexPtr FindInVertex(const VertexPtr& dst_ptr) {
      return this->FindVertex(this->edges_.in_edges(), dst_ptr);
    }
    inline VertexPtr FindInVertex(const VertexIDType& dst_id) {
      return this->FindVertex(this->edges_.in_edges(), dst_id);
    }
    inline VertexPtr FindInVertex(const EdgeLabelType& edge_label,
                                  const VertexPtr& dst_ptr) {
      return this->FindVertex(this->edges_.in_edges(), edge_label, dst_ptr);
    }
    inline VertexPtr FindInVertex(const EdgeLabelType& edge_label,
                                  const VertexIDType& dst_id) {
      return this->FindVertex(this->edges_.in_edges(), edge_label, dst_id);
    }
    /// in vertex const method overload 
    inline VertexConstPtr FindInVertex(const VertexPtr& dst_ptr) const {
      return this->FindConstInVertex(dst_ptr);
    }
    inline VertexConstPtr FindInVertex(const VertexIDType& dst_id) const {
      return this->FindConstInVertex(dst_id);
    }
    inline VertexConstPtr FindInVertex(const EdgeLabelType& edge_label,
                                  const VertexPtr& dst_ptr) const {
      return this->FindConstInVertex(edge_label, dst_ptr);
    }
    inline VertexConstPtr FindInVertex(const EdgeLabelType& edge_label,
                                  const VertexIDType& dst_id) const {
      return this->FindConstInVertex(edge_label, dst_id);
    }
    /// const in vertex
    inline VertexConstPtr FindConstInVertex(const VertexPtr& dst_ptr) const {
      return this->FindConstVertex(this->edges_.const_in_edges(), dst_ptr);
    }
    inline VertexConstPtr FindConstInVertex(const VertexIDType& dst_id) const {
      return this->FindConstVertex(this->edges_.const_in_edges(), dst_id);
    }
    inline VertexConstPtr FindConstInVertex(const EdgeLabelType& edge_label,
                                            const VertexPtr& dst_ptr) const {
      return this->FindConstVertex(this->edges_.const_in_edges(), 
                                         edge_label,
                                         dst_ptr);
    }
    inline VertexConstPtr FindConstInVertex(const EdgeLabelType& edge_label,
                                            const  VertexIDType& dst_id) const {
      return this->FindConstVertex(this->edges_.const_in_edges(), 
                                         edge_label,
                                         dst_id);
    }
    /// const in vertex:
    ///    unimplemented:
    ///    inline VertexConstPtr FindConstInVertex(const VertexPtr& dst_ptr)
    ///    const; inline VertexConstPtr FindConstInVertex(const VertexIDType&
    ///    dst_id ) const; inline VertexConstPtr FindConstInVertex(const
    ///    EdgeLabelType& edge_label,
    ///                                            const VertexPtr& dst_ptr)
    ///                                            const;
    ///    inline VertexConstPtr FindConstInVertex(const EdgeLabelType&
    ///    edge_label,
    ///                                            const VertexIDType& dst_id)
    ///                                            const;
   private:
    inline typename VertexPtrContainerType::size_type CountVertex(
        const VertexPtrContainerType& vertex_ptr_container) const {
      return vertex_ptr_container.size();
    }
    
    inline typename VertexPtrContainerType::size_type CountVertex(
        const EdgeLabelContainerType& edge_label_container) const {
      if (allow_duplicate_edge) {
        std::set<VertexConstPtr> erase_multi_result;
        for (auto edge_label_cit  = edge_label_container.cbegin();
                  edge_label_cit != edge_label_container.cend();
                  edge_label_cit++) {
          const VertexPtrContainerType& vertex_ptr_container_ref 
              = edge_label_cit.template get_const<kVertexPtrContainerIdx>();
          for (auto vertex_ptr_cit  = vertex_ptr_container_ref.cbegin();
                    vertex_ptr_cit != vertex_ptr_container_ref.cend();
                    vertex_ptr_cit++) {
            erase_multi_result.insert(vertex_ptr_cit
                                     .template get_const<kVertexPtrIdx>());
          }
        }
        return erase_multi_result.size();
      }
      typename VertexPtrContainerType::size_type out_vertex_num = 0;
      for (auto edge_label_cit  = edge_label_container.cbegin();
                edge_label_cit != edge_label_container.cend(); edge_label_cit++) {
        const VertexPtrContainerType& vertex_ptr_container_ref 
            = edge_label_cit.template get_const<kVertexPtrContainerIdx>();
        out_vertex_num += this->CountVertex(vertex_ptr_container_ref);
      }
      return out_vertex_num;
    }
    
    inline typename VertexPtrContainerType::size_type CountVertex(
        const EdgeLabelContainerType& edge_label_container,
        const EdgeLabelType&          edge_label) const {
      /// <iterator, bool>
      auto ret = edge_label_container.FindConst(edge_label);
      if (!ret.second){
        /// not found this edge label=
        return 0;
      }
      return this->CountVertex(ret.first.template get_const<kVertexPtrContainerIdx>());
    }

    inline typename VertexPtrContainerType::size_type CountEdge(
         const DecomposedEdgeContainerType& decomposed_edge_container) const {
      return decomposed_edge_container.size();
    }

    inline typename VertexPtrContainerType::size_type CountEdge(
              const VertexPtrContainerType& vertex_ptr_container) const {
      typename VertexPtrContainerType::size_type out_edge_num = 0;
      for (auto vertex_ptr_cit  = vertex_ptr_container.cbegin();
                vertex_ptr_cit != vertex_ptr_container.cend();
                vertex_ptr_cit++) {
        out_edge_num += this->CountEdge(vertex_ptr_cit
                             .template get_const<kDecomposedEdgeContainerIdx>());
      }
      return out_edge_num;
    }

    inline typename VertexPtrContainerType::size_type CountEdge(
              const EdgeLabelContainerType& edge_label_container) const {
      typename VertexPtrContainerType::size_type out_edge_num = 0;
      for (auto edge_label_cit  = edge_label_container.cbegin();
                edge_label_cit != edge_label_container.cend();
                edge_label_cit++) {
        out_edge_num += this->CountEdge(edge_label_cit
                             .template get_const<kVertexPtrContainerIdx>());
      }
      return out_edge_num;
    }

    inline typename VertexPtrContainerType::size_type CountEdge(
              const EdgeLabelContainerType& edge_label_container,
              const EdgeLabelType&          edge_label) const {
      /// <iterator, bool>
      auto ret = edge_label_container.FindConst(edge_label);
      if (!ret.second){
        /// not found this edge label=
        return 0;
      }
      assert(this->CountEdge(ret.first.template get_const<kVertexPtrContainerIdx>())
          == ret.first
                .template get_const<kEdgeLabelEdgeCounterIdx>()
                .counter());
      return ret.first
                .template get_const<kEdgeLabelEdgeCounterIdx>()
                .counter();
    }

    template<bool ptr_is_const_>
    inline typename VertexPtrContainerType::size_type CountEdge(
              const EdgeLabelContainerType&    edge_label_container,
              const EdgeLabelType&             edge_label,
              const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      /// <iterator, bool>
      auto edge_label_ret = edge_label_container.FindConst(edge_label);
      if (!edge_label_ret.second){
        /// does not have edge with this label
        return 0;
      }
      VertexPtr temp_vertex_ptr(const_cast<InnerVertex_*>(vertex_ptr.ptr()));
      /// <iterator, bool>
      auto vertex_ptr_ret = edge_label_ret
                           .first
                           .template get_const<kVertexPtrContainerIdx>()
                           .FindConst(temp_vertex_ptr);
      if (!vertex_ptr_ret.second){
        /// does not have edge connected to this vertex
        return 0;
      }
      return this->CountEdge(vertex_ptr_ret
                            .first
                            .template get_const<kDecomposedEdgeContainerIdx>());
    }

   public:
    inline typename EdgeLabelContainerType::size_type CountOutEdgeLabel()
        const {
      return this->edges_.const_out_edges().size();
    }
    inline typename EdgeLabelContainerType::size_type CountInEdgeLabel() const {
      return this->edges_.const_in_edges().size();
    }

    inline typename VertexPtrContainerType::size_type CountOutVertex() const {
      assert(this->CountVertex(this->edges_.const_out_edges())
          == this->edges_.OutVertexCounter());
      return this->edges_.OutVertexCounter();
    }
    inline typename VertexPtrContainerType::size_type CountInVertex() const {
      assert(this->CountVertex(this->edges_.const_in_edges())
          == this->edges_.InVertexCounter());
      return this->edges_.InVertexCounter();
    }
    inline typename VertexPtrContainerType::size_type CountOutVertex(
                       const EdgeLabelType& edge_label) const {
      return this->CountVertex(this->edges_.const_out_edges(),
                               edge_label);
    }
    inline typename VertexPtrContainerType::size_type CountInVertex(
                       const EdgeLabelType& edge_label) const {
      return this->CountVertex(this->edges_.const_in_edges(),
                               edge_label);
    }

    inline typename VertexPtrContainerType::size_type CountOutEdge() const {
      assert(this->CountEdge(this->edges_.const_out_edges())
          == this->edges_.OutEdgeCounter());
      return this->edges_.OutEdgeCounter();
    }
    inline typename VertexPtrContainerType::size_type CountOutEdge(
        const EdgeLabelType& edge_label) const {
      return this->CountEdge(this->edges_.const_out_edges(), 
                             edge_label);
    }
    template<bool ptr_is_const_>
    inline typename VertexPtrContainerType::size_type CountOutEdge(
        const EdgeLabelType edge_label,
        const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      return this->CountEdge(this->edges_.const_out_edges(), 
                             edge_label, 
                             vertex_ptr);
    }
    inline typename VertexPtrContainerType::size_type CountInEdge() const {
      assert(this->CountEdge(this->edges_.const_in_edges())
          == this->edges_.InEdgeCounter());
      return this->edges_.InEdgeCounter();
    }
    inline typename VertexPtrContainerType::size_type CountInEdge(
        const EdgeLabelType edge_label) const {
      return this->CountEdge(this->edges_.const_in_edges(), 
                             edge_label);
    }
    template<bool ptr_is_const_>
    inline typename VertexPtrContainerType::size_type CountInEdge(
        const EdgeLabelType edge_label,
        const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      return this->CountEdge(this->edges_.const_in_edges(), 
                             edge_label, 
                             vertex_ptr);
    }

    ///    unimplemented:
    ///    inline typename VertexPtrContainerType::size_type
    ///           CountOutVertex(const   EdgeLabelType&   edge_label,
    ///                          const VertexLabelType& vertex_label) const;
    ///    inline typename VertexPtrContainerType::size_type
    ///           CountInVertex(const   EdgeLabelType&   edge_label,
    ///                         const VertexLabelType& vertex_label) const;

    ///    unimplemented:
    ///    inline typename DecomposedEdgeContainerType::size_type
    ///           CountOutEdge() const;
    ///    inline typename DecomposedEdgeContainerType::size_type
    ///           CountInEdge() const;
    ///    inline typename DecomposedEdgeContainerType::size_type
    ///           CountOutEdge(const EdgeLabelType& edge_label) const;
    ///    inline typename DecomposedEdgeContainerType::size_type
    ///           CountInEdge(const EdgeLabelType& edge_label) const;
    ///    inline typename DecomposedEdgeContainerType::size_type
    ///           CountOutEdge(const   EdgeLabelType&   edge_label,
    ///                        const VertexLabelType& vertex_label) const;
    ///    inline typename DecomposedEdgeContainerType::size_type
    ///           CountInEdge(const   EdgeLabelType&   edge_label,
    ///                       const VertexLabelType& vertex_label) const;

   private:
    /// Find Edge methods:
    template<typename _EdgeLabelContainerType,
             typename _VertexPtrType>
    inline auto FindEdge(_EdgeLabelContainerType& edge_label_container,
                            const _VertexPtrType& this_vertex_ptr,
                            const enum EdgeDirection& edge_direction,
                            const EdgeIDType& edge_id) const {
      static_assert(// non-constant method
                    (std::is_same_v<_EdgeLabelContainerType, 
                                     EdgeLabelContainerType>
                  && std::is_same_v<_VertexPtrType, 
                                     VertexPtr>)
                    // constant method
                  ||(std::is_same_v<_EdgeLabelContainerType, 
                               const EdgeLabelContainerType>
                  && std::is_same_v<_VertexPtrType, 
                                     VertexConstPtr>),
                          "illegal type!");
      
      using EdgePtrType = typename std::conditional<
                                   std::is_const_v<_EdgeLabelContainerType>,
                                   EdgeConstPtr,
                                   EdgePtr>::type;

      for (auto edge_label_it  = edge_label_container.begin();
                edge_label_it != edge_label_container.end();
              ++edge_label_it) {
        for (auto vertex_ptr_it  = edge_label_it.template get<kVertexPtrContainerIdx>().begin();
                  vertex_ptr_it != edge_label_it.template get<kVertexPtrContainerIdx>().end();
                ++vertex_ptr_it) {
          auto decomposed_edge_ret = vertex_ptr_it
                                    .template get<kDecomposedEdgeContainerIdx>()
                                    .Find(edge_id);
          if (!decomposed_edge_ret.second) {
            /// not found
            continue;
          }
          return EdgePtrType(edge_direction, 
                             this_vertex_ptr, 
                             edge_label_it,
                             vertex_ptr_it, 
                             decomposed_edge_ret.first);
        }
      }
      return EdgePtrType();
    }

    template<typename _EdgeLabelContainerType,
             typename _VertexPtrType>
    inline auto FindEdge(_EdgeLabelContainerType& edge_label_container,
                            const _VertexPtrType& this_vertex_ptr,
                        const enum EdgeDirection& edge_direction,
                        const EdgeLabelType& edge_label,
                        const EdgeIDType& edge_id) const {
      static_assert(// non-constant method
                    (std::is_same_v<_EdgeLabelContainerType, 
                                     EdgeLabelContainerType>
                  && std::is_same_v<_VertexPtrType, 
                                     VertexPtr>)
                    // constant method
                  ||(std::is_same_v<_EdgeLabelContainerType, 
                               const EdgeLabelContainerType>
                  && std::is_same_v<_VertexPtrType, 
                                     VertexConstPtr>),
                          "illegal type!");
      
      using EdgePtrType = typename std::conditional<
                                   std::is_const_v<_EdgeLabelContainerType>,
                                   EdgeConstPtr,
                                   EdgePtr>::type;

      /// <iterator of EdgeLabelContainer, bool>
      auto edge_label_ret = edge_label_container.Find(edge_label);
      if (!edge_label_ret.second) {
        /// not found this edge label
        return EdgePtrType();
      }
      /// found this edge label
      for (auto vertex_ptr_it  = edge_label_ret.first
                                               .template get<kVertexPtrContainerIdx>()
                                               .begin();
                vertex_ptr_it != edge_label_ret.first
                                               .template get<kVertexPtrContainerIdx>()
                                               .end();
              ++vertex_ptr_it) {
        auto decomposed_edge_ret
          = vertex_ptr_it.template get<kDecomposedEdgeContainerIdx>()
                         .Find(edge_id);
        if (!decomposed_edge_ret.second) {
          /// not found
          continue;
        }
        return EdgePtrType(edge_direction, 
                           this_vertex_ptr, 
                           edge_label_ret.first,
                           vertex_ptr_it, 
                decomposed_edge_ret.first);
      }
      return EdgePtrType();
    }

    template<typename _EdgeLabelContainerType,
             typename _VertexPtrType>
    inline auto FindEdge(_EdgeLabelContainerType& edge_label_container,
                            const _VertexPtrType& this_vertex_ptr,
                        const enum EdgeDirection& edge_direction,
                        const EdgeLabelType& edge_label,
                        const VertexPtr& dst_ptr,
                        const EdgeIDType& edge_id) const {
      static_assert(// non-constant method
                    (std::is_same_v<_EdgeLabelContainerType, 
                                     EdgeLabelContainerType>
                  && std::is_same_v<_VertexPtrType, 
                                     VertexPtr>)
                    // constant method
                  ||(std::is_same_v<_EdgeLabelContainerType, 
                               const EdgeLabelContainerType>
                  && std::is_same_v<_VertexPtrType, 
                                     VertexConstPtr>),
                          "illegal type!");
      
      using EdgePtrType = typename std::conditional<
                                   std::is_const_v<_EdgeLabelContainerType>,
                                   EdgeConstPtr,
                                   EdgePtr>::type;

      /// <iterator of EdgeLabelContainer, bool>
      auto edge_label_ret = edge_label_container.Find(edge_label);
      if (!edge_label_ret.second)  /// does not have this edge label
        return EdgePtrType();
      /// <iterator of VertexContainer, bool>
      auto vertex_ptr_ret = edge_label_ret.first
                                          .template get<kVertexPtrContainerIdx>()
                                          .Find(dst_ptr);
      if (!vertex_ptr_ret.second)  /// does not have this VertexPtr
        return EdgePtrType();
      /// <iterator of DecomposedEdgeContainer, bool>
      auto decomposed_edge_ret = vertex_ptr_ret.first
                                               .template get<kDecomposedEdgeContainerIdx>()
                                               .Find(edge_id);
      if (!decomposed_edge_ret.second)  /// does not find it
        return EdgePtrType();
      return EdgePtrType(edge_direction, 
                         this_vertex_ptr, 
                         edge_label_ret.first,
                       vertex_ptr_ret.first, 
              decomposed_edge_ret.first);
    }

    inline VertexPtr this_vertex_ptr(){
      InnerVertex_* const temp_this_ptr = this;
      return VertexPtr(temp_this_ptr);
    }
    
    inline VertexConstPtr this_vertex_ptr() const {
      const InnerVertex_* const temp_this_ptr = this;
      return VertexConstPtr(temp_this_ptr);
    }

   public:
    // find out edge methods
    inline EdgePtr FindOutEdge(const EdgeIDType& edge_id) {
      return this->FindEdge(this->edges_.out_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::OutputEdge,
                            edge_id);
    }
    inline EdgePtr FindOutEdge(const EdgeLabelType& edge_label,
                               const EdgeIDType& edge_id) {
      return this->FindEdge(this->edges_.out_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::OutputEdge,
                            edge_label, edge_id);
    }
    inline EdgePtr FindOutEdge(const EdgeLabelType& edge_label,
                               const VertexPtr& dst_ptr,
                               const EdgeIDType& edge_id) {
      return this->FindEdge(this->edges_.out_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::OutputEdge,
                            edge_label, dst_ptr, edge_id);
    }
    
    // find out edge methods constant override
    inline EdgeConstPtr FindOutEdge(const EdgeIDType& edge_id) const  {
      return this->FindConstOutEdge(edge_id);
    }
    inline EdgeConstPtr FindOutEdge(const EdgeLabelType& edge_label,
                                    const EdgeIDType& edge_id) const {
      return this->FindConstOutEdge(edge_label, 
                                    edge_id);
    }
    inline EdgeConstPtr FindOutEdge(const EdgeLabelType& edge_label,
                                    const VertexPtr& dst_ptr,
                                    const EdgeIDType& edge_id) const {
      return this->FindConstOutEdge(edge_label, 
                                     dst_ptr,
                                    edge_id);
    }

    // find const out edge methods
    inline EdgeConstPtr FindConstOutEdge(const EdgeIDType& edge_id) const {
      return this->FindEdge(this->edges_.const_out_edges(),
                            this->this_vertex_ptr(),
                            EdgeDirection::OutputEdge, 
                            edge_id);
    }
    inline EdgeConstPtr FindConstOutEdge(const EdgeLabelType& edge_label,
                                         const EdgeIDType& edge_id) const {
      return this->FindEdge(this->edges_.const_out_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::OutputEdge,
                            edge_label, 
                            edge_id);
    }
    inline EdgeConstPtr FindConstOutEdge(const EdgeLabelType& edge_label,
                                         const VertexPtr& dst_ptr,
                                         const EdgeIDType& edge_id) const {
      return this->FindEdge(this->edges_.const_out_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::OutputEdge,
                            edge_label, 
                            dst_ptr, 
                            edge_id);
    }

    ///    unimplemented:
    ///    inline EdgeConstPtr FindConstOutEdge(const EdgeIDType& edge_id);
    ///    inline EdgeConstPtr FindConstOutEdge(const EdgeLabelType&
    ///    edge_label,
    ///                                         const EdgeIDType& edge_id);
    ///    inline EdgeConstPtr FindConstOutEdge(const EdgeLabelType&
    ///    edge_label,
    ///                                         const VertexPtr& dst_ptr,
    ///                                         const EdgeIDType& edge_id);
    /// possible extension:
    ///     FindOutEdge(edge_label,dst_ptr)
    ///         when there are not duplicate edge
    ///     FindOutEdge(dst_ptr)
    ///         when there are not duplicate edge
    ///         and only has one edge label type

    // find in edge methods
    inline EdgePtr FindInEdge(const EdgeIDType& edge_id) {
      return this->FindEdge(this->edges_.in_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::InputEdge,
                            edge_id);
    }
    inline EdgePtr FindInEdge(const EdgeLabelType& edge_label,
                              const EdgeIDType& edge_id) {
      return this->FindEdge(this->edges_.in_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::InputEdge,
                            edge_label, 
                            edge_id);
    }
    inline EdgePtr FindInEdge(const EdgeLabelType& edge_label,
                              const VertexPtr& src_ptr,
                              const EdgeIDType& edge_id) {
      return this->FindEdge(this->edges_.in_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::InputEdge,
                            edge_label, 
                            src_ptr, 
                            edge_id);
    }
    
    // find in edge methods constant override
    inline EdgeConstPtr FindInEdge(const EdgeIDType& edge_id) const {
      return this->FindConstInEdge(edge_id);
    }
    inline EdgeConstPtr FindInEdge(const EdgeLabelType& edge_label,
                                   const EdgeIDType& edge_id) const {
      return this->FindConstInEdge(edge_label, 
                                   edge_id);
    }
    inline EdgeConstPtr FindInEdge(const EdgeLabelType& edge_label,
                                   const VertexPtr& src_ptr,
                                   const EdgeIDType& edge_id) const {
      return this->FindConstInEdge(edge_label, 
                                   src_ptr, 
                                   edge_id);
    }
    
    // find const in edge methods
    inline EdgeConstPtr FindConstInEdge(const EdgeIDType& edge_id) const {
      return this->FindEdge(this->edges_.const_in_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::InputEdge,
                            edge_id);
    }
    inline EdgeConstPtr FindConstInEdge(const EdgeLabelType& edge_label,
                                        const EdgeIDType& edge_id) const {
      return this->FindEdge(this->edges_.const_in_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::InputEdge,
                            edge_label, 
                            edge_id);
    }
    inline EdgeConstPtr FindConstInEdge(const EdgeLabelType& edge_label,
                                        const VertexPtr& src_ptr,
                                        const EdgeIDType& edge_id) const {
      return this->FindEdge(this->edges_.const_in_edges(), 
                            this->this_vertex_ptr(),
                            EdgeDirection::InputEdge,
                            edge_label, 
                            src_ptr, 
                            edge_id);
    }

    ///    unimplemented:
    ///    inline EdgeConstPtr FindConstInEdge(const EdgeIDType& edge_id);
    ///    inline EdgeConstPtr FindConstInEdge(const EdgeLabelType&
    ///    edge_label,
    ///                                        const EdgeIDType& edge_id);
    ///    inline EdgeConstPtr FindConstInEdge(const EdgeLabelType&
    ///    edge_label,
    ///                                        const VertexPtr& dst_ptr,
    ///                                        const EdgeIDType& edge_id);
    /// possible extension:
    ///     FindInEdge(edge_label,dst_ptr)
    ///         when there are not duplicate edge
    ///     FindInEdge(dst_ptr)
    ///         when there are not duplicate edge
    ///         and only has one edge label type

    using EdgeLabelIterator
                 = Iterator_<EdgeLabelContentIterator_<
                             EdgeLabelContainerType, true, 1,
                            kEdgeLabelIdx>>;
    /// non-constant vertex iterator
    using VertexIterator
              = Iterator_<VertexContentIterator_<
                          EdgeLabelContainerType, false, 2,
                         kVertexPtrIdx>>;
    using VertexConstIterator 
              = Iterator_<VertexContentIterator_<
                          EdgeLabelContainerType,  true, 2, 
                         kVertexPtrIdx>>;
    using VertexIteratorSpecifiedEdgeLabel
              = Iterator_<VertexContentIterator_<
                          VertexPtrContainerType, false, 1,
                         kVertexPtrIdx>>;
    using VertexConstIteratorSpecifiedEdgeLabel 
              = Iterator_<VertexContentIterator_<
                          VertexPtrContainerType,  true, 1, 
                         kVertexPtrIdx>>;

    template<bool is_const_>
    using EdgeContentIterator
        = EdgeContentIterator_<EdgeLabelContainerType, 
                               is_const_, 3, 0, 
                              kEdgeLabelIdx,
                              kVertexPtrIdx, 
                              kEdgeIDIdx, 
                              kEdgeAttributePtrIdx>;
    using EdgeIterator 
            = Iterator_<EdgeContentIterator<false>>;
    using EdgeConstIterator 
            = Iterator_<EdgeContentIterator< true>>;

    template<bool is_const_>
    using EdgeContentIteratorSpecifiedEdgeLabel
        = EdgeContentIteratorSpecifiedEdgeLabel_<
                      VertexPtrContainerType, 
                      is_const_, 2, 0, 
                     kEdgeLabelIdx, 
                     kVertexPtrIdx,
                     kEdgeIDIdx, 
                     kEdgeAttributePtrIdx>;
    using EdgeIteratorSpecifiedEdgeLabel 
            = Iterator_<EdgeContentIteratorSpecifiedEdgeLabel<false>>;
    using EdgeConstIteratorSpecifiedEdgeLabel
            = Iterator_<EdgeContentIteratorSpecifiedEdgeLabel< true>>;

    template<bool is_const_>
    using EdgeContentIteratorSpecifiedEdgeLabelVertexPtr
        = EdgeContentIteratorSpecifiedEdgeLabelVertexPtr_<
                      DecomposedEdgeContainerType, 
                      is_const_, 1, 0, 
                     kEdgeLabelIdx, 
                     kVertexPtrIdx,
                     kEdgeIDIdx, 
                     kEdgeAttributePtrIdx>;
    using EdgeIteratorSpecifiedEdgeLabelVertexPtr
            = Iterator_<EdgeContentIteratorSpecifiedEdgeLabelVertexPtr<false>>;
    using EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
            = Iterator_<EdgeContentIteratorSpecifiedEdgeLabelVertexPtr< true>>;
    
   public:
    inline EdgeIteratorSpecifiedEdgeLabelVertexPtr EraseEdge(
           EdgeIteratorSpecifiedEdgeLabelVertexPtr& edge_iterator) {
      using EdgeContentIteratorSpecifiedEdgeLabelVertexPtrType
          = EdgeContentIteratorSpecifiedEdgeLabelVertexPtr<false>;
      void* ptr = &edge_iterator;
      EdgeContentIteratorSpecifiedEdgeLabelVertexPtrType* edge_it_ptr =
          static_cast<EdgeContentIteratorSpecifiedEdgeLabelVertexPtrType*>(ptr);

      auto edge_label_iterator = edge_it_ptr->edge_label_iterator();
      auto vertex_ptr_iterator = edge_it_ptr->vertex_ptr_iterator();
      DecomposedEdgeContainerType& decomposed_edge_container
        = vertex_ptr_iterator.template get<kDecomposedEdgeContainerIdx>();
      auto decomposed_edge_iterator = edge_it_ptr->decomposed_edge_iterator();

      assert(false);
      return EdgeIteratorSpecifiedEdgeLabelVertexPtr();
    }
    
    inline EdgeIteratorSpecifiedEdgeLabel EraseEdge(
           EdgeIteratorSpecifiedEdgeLabel& edge_iterator) {
      using EdgeContentIteratorSpecifiedEdgeLabelType
          = EdgeContentIteratorSpecifiedEdgeLabel<false>;
      void* ptr = &edge_iterator;
      EdgeContentIteratorSpecifiedEdgeLabelType* edge_it_ptr =
          static_cast<EdgeContentIteratorSpecifiedEdgeLabelType*>(ptr);
      enum EdgeDirection edge_direction = edge_it_ptr->direction();
      auto edge_label_iterator = edge_it_ptr->get_iterator();
      VertexPtrContainerType& vertex_ptr_container
        = edge_label_iterator.template get<kVertexPtrContainerIdx>();
      auto vertex_ptr_iterator = edge_it_ptr->vertex_ptr_iterator();
      DecomposedEdgeContainerType& decomposed_edge_container
        = vertex_ptr_iterator.template get<kDecomposedEdgeContainerIdx>();
      auto decomposed_edge_iterator = edge_it_ptr->decomposed_edge_iterator();
      // erase dst_ptr iterator
      EdgeLabelType edge_label = edge_iterator->label();
      EdgeIDType edge_id = edge_iterator->id();
      InnerVertex_* temp_this_ptr = this;
      VertexPtr temp_vertex_ptr = VertexPtr(temp_this_ptr);
      VertexPtr dst_ptr;
      if (edge_direction == EdgeDirection::OutputEdge) {
        dst_ptr = edge_it_ptr->dst_ptr();
      } else {
        dst_ptr = edge_it_ptr->src_ptr();
      }
      enum EdgeDirection reverse_edge_direction;
      if (edge_direction == EdgeDirection::OutputEdge) {
        reverse_edge_direction = EdgeDirection::InputEdge;
      } else {
        reverse_edge_direction = EdgeDirection::OutputEdge;
      }
      dst_ptr->EraseEdge(reverse_edge_direction, edge_label, temp_vertex_ptr,
                         edge_id);
      EdgeIteratorSpecifiedEdgeLabel ret_iterator = edge_iterator;
      void* ret_ptr = &ret_iterator;
      EdgeContentIteratorSpecifiedEdgeLabelType* ret_it_ptr =
          static_cast<EdgeContentIteratorSpecifiedEdgeLabelType*>(ret_ptr);
      auto& ret_vertex_ptr_iterator = ret_it_ptr->vertex_ptr_iterator();
      auto& ret_decomposed_edge_iterator =
          ret_it_ptr->decomposed_edge_iterator();
      // erase decomposed edge
      decomposed_edge_iterator =
          decomposed_edge_container.Erase(decomposed_edge_iterator);
      /// maintain the edge counter
      if (edge_direction == EdgeDirection::OutputEdge) {
        this->edges_.OutEdgeReduceOne();
      } else {
        this->edges_.InEdgeReduceOne();
      }
      edge_label_iterator.template get<kEdgeLabelEdgeCounterIdx>()
                         .ReduceOne();
      if (!decomposed_edge_container.empty()) {
        // decomposed_edge_container is not empty and iter is not end
        // return new iterator
        if (decomposed_edge_iterator != decomposed_edge_container.end()) {
          ret_decomposed_edge_iterator = decomposed_edge_iterator;
          return ret_iterator;
        }
        // else add vertex_ptr to next iter
        vertex_ptr_iterator++;
      } else {
        // decomposed_edge_container is empty
        // erase vertex_ptr_iterator
        vertex_ptr_iterator = vertex_ptr_container.Erase(vertex_ptr_iterator);
      }
      if ((!vertex_ptr_container.empty()) &&
          (vertex_ptr_iterator != vertex_ptr_container.end())) {
        auto& decomposed_edge_container_ref
          = vertex_ptr_iterator.template get<kDecomposedEdgeContainerIdx>();
        ret_vertex_ptr_iterator = vertex_ptr_iterator;
        ret_decomposed_edge_iterator = decomposed_edge_container_ref.begin();
        /// the end iterator might be invalid since an element is erased
        /// from the vertex ptr container
        ret_it_ptr->iterator_end() = vertex_ptr_container.end();
        return ret_iterator;
      }
      return EdgeIteratorSpecifiedEdgeLabel();
    }

    inline EdgeIterator EraseEdge(EdgeIterator& edge_iterator) {
      using EdgeContentIteratorType
          = EdgeContentIterator<false>;
      void* ptr = &edge_iterator;
      EdgeContentIteratorType* edge_content_iterator_ptr =
          static_cast<EdgeContentIteratorType*>(ptr);

      // get direction
      enum EdgeDirection edge_direction
                       = edge_content_iterator_ptr->const_direction();
      // get container and iterator
      EdgeLabelContainerType* edge_label_container_ptr = nullptr;
      if (edge_direction == EdgeDirection::OutputEdge)
        edge_label_container_ptr = &this->edges_.out_edges();
      else
        edge_label_container_ptr = &this->edges_.in_edges();
      VertexPtrContainerType& vertex_ptr_container =
          edge_content_iterator_ptr->template get<kVertexPtrContainerIdx, 0>();
      DecomposedEdgeContainerType& decomposed_edge_container =
          edge_content_iterator_ptr->template get<kDecomposedEdgeContainerIdx, 1>();
      auto edge_label_iterator
       = edge_content_iterator_ptr
         ->template get_iterator<0>();
      auto vertex_ptr_iterator
       = edge_content_iterator_ptr
              ->template get_iterator<1>();
      auto decomposed_edge_iterator
            = edge_content_iterator_ptr
              ->template get_iterator<2>();

      // erase dst_ptr iterator
      EdgeLabelType edge_label = edge_iterator->label();
      EdgeIDType    edge_id    = edge_iterator->id();
      InnerVertex_* temp_this_ptr = this;
      VertexPtr temp_vertex_ptr = VertexPtr(temp_this_ptr);
      VertexPtr dst_ptr;
      enum EdgeDirection reverse_edge_direction;
      if (edge_direction == EdgeDirection::OutputEdge) {
        reverse_edge_direction = EdgeDirection::InputEdge;
        dst_ptr = edge_content_iterator_ptr->dst_ptr();
      } else {
        reverse_edge_direction = EdgeDirection::OutputEdge;
        dst_ptr = edge_content_iterator_ptr->src_ptr();
      }
      dst_ptr->EraseEdge(reverse_edge_direction, 
                                 edge_label, 
                                 temp_vertex_ptr,
                                 edge_id);
      // build return iterator
      EdgeIterator ret_iterator = edge_iterator;
      void* ret_ptr = &ret_iterator;
      EdgeContentIteratorType* ret_it
       = static_cast<EdgeContentIteratorType*>(ret_ptr);
      auto& ret_edge_label_iterator 
          = ret_it->template get_iterator<0>();
      auto& ret_vertex_ptr_iterator 
          = ret_it->template get_iterator<1>();
      auto& ret_decomposed_edge_iterator 
          = ret_it->template get_iterator<2>();
      // erase decomposed edge
      decomposed_edge_iterator =
      decomposed_edge_container.Erase(decomposed_edge_iterator);
      /// maintain the edge counter
      if (edge_direction == EdgeDirection::OutputEdge) {
        this->edges_.OutEdgeReduceOne();
      } else {
        this->edges_.InEdgeReduceOne();
      }
      edge_label_iterator.template get<kEdgeLabelEdgeCounterIdx>()
                         .ReduceOne();
      if (!decomposed_edge_container.empty()) {
        // decomposed_edge_container is not empty and iter is not end
        // return new iterator
        if (decomposed_edge_iterator != decomposed_edge_container.end()) {
          ret_decomposed_edge_iterator = decomposed_edge_iterator;
          return ret_iterator;
        }
        // else add vertex_ptr to next iter
        vertex_ptr_iterator++;
      } else {
        // decomposed_edge_container is empty
        // erase vertex_ptr_iterator
        vertex_ptr_iterator = vertex_ptr_container.Erase(vertex_ptr_iterator);
      }
      if (!vertex_ptr_container.empty()) {
        if (vertex_ptr_iterator != vertex_ptr_container.end()) {
          auto& decomposed_edge_container_ref
            = vertex_ptr_iterator.template get<kDecomposedEdgeContainerIdx>();
          ret_vertex_ptr_iterator = vertex_ptr_iterator;
          ret_decomposed_edge_iterator = decomposed_edge_container_ref.begin();
          return ret_iterator;
        }
        edge_label_iterator++;
      } else {
        edge_label_iterator = edge_label_container_ptr->Erase(edge_label_iterator);
      }
      if (!edge_label_container_ptr->empty() &&
           edge_label_iterator != edge_label_container_ptr->end()) {
        vertex_ptr_iterator
          = (edge_label_iterator.template get<kVertexPtrContainerIdx>()).begin();
        decomposed_edge_iterator
          = vertex_ptr_iterator.template get<kDecomposedEdgeContainerIdx>()
                               .begin();
        ret_edge_label_iterator = edge_label_iterator;
        ret_vertex_ptr_iterator = vertex_ptr_iterator;
        ret_it->template get_iterator<2>() =
            decomposed_edge_iterator;
        /// the end iterator might be invalid since an element is erased
        /// from the edge label container
        ret_it->iterator_end() = edge_label_container_ptr->end();
        return ret_iterator;
      }
      /// is done
      return EdgeIterator();
    }

   private:
    inline EdgeLabelIterator EdgeLabelBegin(
        const EdgeLabelContainerType& edge_label_container) const {
      return EdgeLabelIterator(edge_label_container.cbegin(),
                               edge_label_container.cend());
    }

    inline VertexIterator VertexBegin(
        EdgeLabelContainerType& edge_label_container) {
      return VertexIterator(edge_label_container.begin(),
                            edge_label_container.end());
    }

    inline VertexConstIterator VertexCBegin(
        const EdgeLabelContainerType& edge_label_container) const {
      return VertexConstIterator(edge_label_container.cbegin(),
                                 edge_label_container.cend());
    }

    inline VertexIteratorSpecifiedEdgeLabel VertexBegin(
        EdgeLabelContainerType& edge_label_container,
        const EdgeLabelType& edge_label) {
      auto ret = edge_label_container.Find(edge_label);
      if (!ret.second) {
        /// not find
        return VertexIteratorSpecifiedEdgeLabel();
      }
      return VertexIteratorSpecifiedEdgeLabel(
          (ret.first.template get<kVertexPtrContainerIdx>()).begin(),
          (ret.first.template get<kVertexPtrContainerIdx>()).end());
    }

    inline VertexConstIteratorSpecifiedEdgeLabel VertexCBegin(
        const EdgeLabelContainerType& edge_label_container,
        const EdgeLabelType& edge_label) const {
      auto ret = edge_label_container.FindConst(edge_label);
      if (!ret.second) {
        /// not find
        return VertexConstIteratorSpecifiedEdgeLabel();
      }
      return VertexConstIteratorSpecifiedEdgeLabel(
          (ret.first.template get_const<kVertexPtrContainerIdx>()).cbegin(),
          (ret.first.template get_const<kVertexPtrContainerIdx>()).cend());
    }

    inline EdgeIterator EdgeBegin(
        enum EdgeDirection direction,
        EdgeLabelContainerType& edge_label_container) {
      InnerVertex_* const temp_this_ptr = this;
      VertexPtr temp_this_vertex_ptr(temp_this_ptr);
      return EdgeIterator(direction, temp_this_vertex_ptr,
                          edge_label_container.begin(),
                          edge_label_container.end());
    }

    inline EdgeConstIterator EdgeCBegin(
        enum EdgeDirection direction,
        const EdgeLabelContainerType& edge_label_container) const {
      const InnerVertex_* const temp_this_ptr = this;
      const VertexConstPtr temp_this_vertex_ptr(temp_this_ptr);
      return EdgeConstIterator(direction, temp_this_vertex_ptr,
                               edge_label_container.cbegin(),
                               edge_label_container.cend());
    }
    
    inline EdgeIteratorSpecifiedEdgeLabel EdgeBegin(
        enum EdgeDirection direction,
        EdgeLabelContainerType& edge_label_container,
        const EdgeLabelType& edge_label) {
      /// <iterator of EdgeLabelContainerType, bool>
      auto ret = edge_label_container.Find(edge_label);
      if (!ret.second) {
        /// not found
        return EdgeIteratorSpecifiedEdgeLabel();
      }
      /// found
      InnerVertex_* const temp_this_ptr = this;
      const VertexPtr temp_this_vertex_ptr(temp_this_ptr);
      return EdgeIteratorSpecifiedEdgeLabel(
          ret.first, direction, temp_this_vertex_ptr,
          ret.first.template get<kVertexPtrContainerIdx>().begin(),
          ret.first.template get<kVertexPtrContainerIdx>().end());
    }

    inline EdgeConstIteratorSpecifiedEdgeLabel EdgeCBegin(
        enum EdgeDirection direction,
        const EdgeLabelContainerType& edge_label_container,
        const EdgeLabelType& edge_label) const {
      /// <iterator of EdgeLabelContainerType, bool>
      auto ret = edge_label_container.FindConst(edge_label);
      if (!ret.second) {
        /// not found
        return EdgeConstIteratorSpecifiedEdgeLabel();
      }
      /// found
      const InnerVertex_* const temp_this_ptr = this;
      VertexConstPtr temp_this_vertex_ptr(temp_this_ptr);
      return EdgeConstIteratorSpecifiedEdgeLabel(
          ret.first, direction, temp_this_vertex_ptr,
          ret.first.template get_const<kVertexPtrContainerIdx>().cbegin(),
          ret.first.template get_const<kVertexPtrContainerIdx>().cend());
    }

    template<bool ptr_is_const_>
    inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
           EdgeBegin(enum EdgeDirection direction,
                    const EdgeLabelContainerType& edge_label_container,
                    const EdgeLabelType& edge_label,
                    const VertexPtr_<ptr_is_const_>& vertex_ptr){
      auto edge_label_ret = edge_label_container.Find(edge_label);
      if (!edge_label_ret.second) {
        /// not found
        return EdgeIteratorSpecifiedEdgeLabelVertexPtr();
      }
      VertexPtr temp_vertex_ptr(const_cast<InnerVertex_*>(vertex_ptr.ptr()));
      auto vertex_ptr_ret = edge_label_ret
                           .first
                           .template get_const<kVertexPtrContainerIdx>()
                           .Find(temp_vertex_ptr);
      if (!vertex_ptr_ret.second) {
        /// not found
        return EdgeIteratorSpecifiedEdgeLabelVertexPtr();
      }
      /// found
      const InnerVertex_* const temp_this_ptr = this;
      VertexConstPtr temp_this_vertex_ptr(temp_this_ptr);
      return EdgeIteratorSpecifiedEdgeLabelVertexPtr(
          vertex_ptr_ret.first, 
          edge_label_ret.first, direction, temp_this_vertex_ptr,
          vertex_ptr_ret.first.template get<kDecomposedEdgeContainerIdx>().begin(),
          vertex_ptr_ret.first.template get<kDecomposedEdgeContainerIdx>().end());
    }

    template<bool ptr_is_const_>
    inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
           EdgeCBegin(enum EdgeDirection direction,
                     const EdgeLabelContainerType& edge_label_container,
                     const EdgeLabelType& edge_label,
                     const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      auto edge_label_ret = edge_label_container.FindConst(edge_label);
      if (!edge_label_ret.second) {
        /// not found
        return EdgeConstIteratorSpecifiedEdgeLabelVertexPtr();
      }
      VertexPtr temp_vertex_ptr(const_cast<InnerVertex_*>(vertex_ptr.ptr()));
      auto vertex_ptr_ret = edge_label_ret
                           .first
                           .template get_const<kVertexPtrContainerIdx>()
                           .FindConst(temp_vertex_ptr);
      if (!vertex_ptr_ret.second) {
        /// not found
        return EdgeConstIteratorSpecifiedEdgeLabelVertexPtr();
      }
      /// found
      const InnerVertex_* const temp_this_ptr = this;
      VertexConstPtr temp_this_vertex_ptr(temp_this_ptr);
      return EdgeConstIteratorSpecifiedEdgeLabelVertexPtr(
          vertex_ptr_ret.first,
          edge_label_ret.first, direction, temp_this_vertex_ptr,
          vertex_ptr_ret.first.template get_const<kDecomposedEdgeContainerIdx>().cbegin(),
          vertex_ptr_ret.first.template get_const<kDecomposedEdgeContainerIdx>().cend());
    }

   public:
    /// EdgeLabel Iterators:
    inline EdgeLabelIterator OutEdgeLabelBegin() const {
      return this->EdgeLabelBegin(this->edges_.const_out_edges());
    }
    inline EdgeLabelIterator InEdgeLabelBegin() const {
      return this->EdgeLabelBegin(this->edges_.const_in_edges());
    }

    /// Vertex Iterators:
    /// output edge:
    inline VertexIterator OutVertexBegin() {
      return this->VertexBegin(this->edges_.out_edges());
    }
    inline VertexConstIterator OutVertexCBegin() const {
      return this->VertexCBegin(this->edges_.const_out_edges());
    }
    inline VertexIteratorSpecifiedEdgeLabel OutVertexBegin(
        const EdgeLabelType& edge_label) {
      return this->VertexBegin(this->edges_.out_edges(), edge_label);
    }
    inline VertexConstIteratorSpecifiedEdgeLabel OutVertexCBegin(
        const EdgeLabelType& edge_label) const {
      return this->VertexCBegin(this->edges_.const_out_edges(), edge_label);
    }
    /// possible extension:
    ///     OutVertexBegin (edge_label,vertex_label)
    ///     OutVertexCBegin(edge_label,vertex_label)

    /// input edge:
    inline VertexIterator InVertexBegin() {
      return this->VertexBegin(this->edges_.in_edges());
    }
    inline VertexConstIterator InVertexCBegin() const {
      return this->VertexCBegin(this->edges_.const_in_edges());
    }
    inline VertexIteratorSpecifiedEdgeLabel InVertexBegin(
        const EdgeLabelType& edge_label) {
      return this->VertexBegin(this->edges_.in_edges(), edge_label);
    }
    inline VertexConstIteratorSpecifiedEdgeLabel InVertexCBegin(
        const EdgeLabelType& edge_label) const {
      return this->VertexCBegin(this->edges_.const_in_edges(), edge_label);
    }
    /// possible extension:
    ///     InVertexBegin (edge_label,vertex_label)
    ///     InVertexCBegin(edge_label,vertex_label)

    /// Edge Iterators:
    /// output edge:
    inline EdgeIterator OutEdgeBegin() {
      return this->EdgeBegin(EdgeDirection::OutputEdge,
                             this->edges_.out_edges());
    }
    inline EdgeConstIterator OutEdgeBegin() const {
      return this->OutEdgeCBegin();
    }
    inline EdgeConstIterator OutEdgeCBegin() const {
      return this->EdgeCBegin(EdgeDirection::OutputEdge,
                              this->edges_.const_out_edges());
    }
    inline EdgeIteratorSpecifiedEdgeLabel OutEdgeBegin(
        const EdgeLabelType& edge_label) {
      return this->EdgeBegin(EdgeDirection::OutputEdge,
                             this->edges_.out_edges(), edge_label);
    }
    inline EdgeConstIteratorSpecifiedEdgeLabel OutEdgeBegin(
        const EdgeLabelType& edge_label) const {
      return this->EdgeCBegin(edge_label);
    }
    inline EdgeConstIteratorSpecifiedEdgeLabel OutEdgeCBegin(
        const EdgeLabelType& edge_label) const {
      return this->EdgeCBegin(EdgeDirection::OutputEdge,
                              this->edges_.const_out_edges(), edge_label);
    }
    template<bool ptr_is_const_>
    inline EdgeIteratorSpecifiedEdgeLabelVertexPtr
        OutEdgeBegin(const   EdgeLabelType&   edge_label,
                     const VertexPtr_<ptr_is_const_>& vertex_ptr){
      return this->EdgeBegin(EdgeDirection::OutputEdge,
                             this->edges_.out_edges(), 
                             edge_label,
                             vertex_ptr);
    }
    template<bool ptr_is_const_>
    inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
        OutEdgeBegin(const   EdgeLabelType&   edge_label,
                     const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      return this->EdgeCBegin(edge_label,
                              vertex_ptr);
    }
    template<bool ptr_is_const_>
    inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
        OutEdgeCBegin(const   EdgeLabelType&   edge_label,
                      const VertexPtr_<ptr_is_const_>& vertex_ptr) const{
      return this->EdgeCBegin(EdgeDirection::OutputEdge,
                              this->edges_.const_out_edges(),
                              edge_label,
                              vertex_ptr);
    }
    /// possible extension:
    ///     OutEdgeBegin (edge_label,vertex_label)
    ///     OutEdgeCBegin(edge_label,vertex_label)

    /// input edge:
    inline EdgeIterator InEdgeBegin() {
      return this->EdgeBegin(EdgeDirection::InputEdge, this->edges_.in_edges());
    }
    inline EdgeConstIterator InEdgeBegin() const {
      return this->InEdgeCBegin();
    }
    inline EdgeConstIterator InEdgeCBegin() const {
      return this->EdgeCBegin(EdgeDirection::InputEdge,
                              this->edges_.const_in_edges());
    }
    inline EdgeIteratorSpecifiedEdgeLabel InEdgeBegin(
        const EdgeLabelType& edge_label) {
      return this->EdgeBegin(EdgeDirection::InputEdge, this->edges_.in_edges(),
                             edge_label);
    }
    inline EdgeConstIteratorSpecifiedEdgeLabel InEdgeBegin(
        const EdgeLabelType& edge_label) const {
      return this->EdgeCBegin(edge_label);
    }
    inline EdgeConstIteratorSpecifiedEdgeLabel InEdgeCBegin(
        const EdgeLabelType& edge_label) const {
      return this->EdgeCBegin(EdgeDirection::InputEdge,
                              this->edges_.const_in_edges(), edge_label);
    }
    template<bool ptr_is_const_>
    inline EdgeIteratorSpecifiedEdgeLabelVertexPtr
         InEdgeBegin(const   EdgeLabelType&   edge_label,
                       const VertexPtr_<ptr_is_const_>& vertex_ptr){
      return this->EdgeBegin(EdgeDirection::InputEdge,
                             this->edges_.in_edges(), 
                             edge_label,
                             vertex_ptr);
    }
    template<bool ptr_is_const_>
    inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
         InEdgeBegin(const   EdgeLabelType&   edge_label,
                       const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      return this->EdgeCBegin(edge_label,
                              vertex_ptr);
    }
    template<bool ptr_is_const_>
    inline EdgeConstIteratorSpecifiedEdgeLabelVertexPtr
         InEdgeCBegin(const   EdgeLabelType&   edge_label,
                      const VertexPtr_<ptr_is_const_>& vertex_ptr) const {
      return this->EdgeCBegin(EdgeDirection::InputEdge,
                              this->edges_.const_in_edges(), 
                              edge_label,
                              vertex_ptr);
    }
    /// possible extension:
    ///     InEdgeBegin (edge_label,vertex_label)
    ///     InEdgeCBegin(edge_label,vertex_label)

    /// unimplemented:
    /// inline VertexIterator EraseVertex(const VertexIterator& iterator);
    /// inline VertexIteratorSpecifiedEdgeLabel
    ///                       EraseVertex(const
    ///                       VertexIteratorSpecifiedEdgeLabel& iterator);
    /// inline EdgeIterator EraseEdge(const EdgeIterator& iterator);
    /// inline EdgeIteratorSpecifiedEdgeLabel
    ///                     EraseEdge(const EdgeIteratorSpecifiedEdgeLabel&
    ///                     iterator);

    /// inline EdgePtr SetLabel(EdgePtr&       edge_ptr,
    ///                   const EdgeLabelType& edge_label);
    /// inline EdgePtr SetLabel(const EdgeIDType&    edge_id,
    ///                         const EdgeLabelType& edge_label);
    /// inline EdgePtr SetSrc(EdgePtr&     edge_ptr,
    ///                 const VertexPtr& vertex_ptr);
    /// inline EdgePtr SetSrc(const EdgeIDType& edge_id,
    ///                       const VertexPtr&  vertex_ptr);
    /// inline EdgePtr SetDst(EdgePtr&     edge_ptr,
    ///                 const VertexPtr& vertex_ptr);
    /// inline EdgePtr SetDst(const EdgeIDType& edge_id,
    ///                       const VertexPtr&  vertex_ptr);
    
   private:
    Edges<store_data, true> edges_;
  };

 private:
  inline void Deconstruct() {
    for (auto vertex_label_it  = this->vertexes_.begin();
              vertex_label_it != this->vertexes_.end();
              vertex_label_it++) {
      for (auto vertex_ptr_it  = vertex_label_it
                                .template get<kVertexIDContainerIdx>().begin();
                vertex_ptr_it != vertex_label_it
                                .template get<kVertexIDContainerIdx>(). end ();
                vertex_ptr_it++) {
        vertex_ptr_it.template get<kVertexPtrIdx>().Release();
      }
      vertex_label_it.template get<kVertexIDContainerIdx>().clear();
    }
    this->vertexes_.clear();
    return;
  }

  template <template <typename...> class GraphType_, typename... configures_>
  inline void CanConvertTo(const GraphType_<configures_...>& graph) const {
    static_assert(std::is_base_of<Graph<configures_...>,
                                  GraphType_<configures_...>>::value,
                  "Illegal input parameter, should be a graph");
    using ConvertToGraphType = GraphType_<configures_...>;
    static_assert(Graph::allow_duplicate_edge ||
                      !ConvertToGraphType::allow_duplicate_edge,
                  "Cannot convert from a graph allows duplicate edge\
                 to a graph does not allow duplicate edge");
    /// wenzhi ToDo:
    ///     complete the checking process of the parameters,
    ///     such as the above statement, if this graph type does
    ///     not allow duplicate edges but the InputGraphType allows
    ///     that, then this convert would be illegal
    return;
  }

  template <template <typename...> class GraphType_, typename... configures_>
  inline void Construct(const GraphType_<configures_...>& graph) {
    using GraphType = GraphType_<configures_...>;
    for (auto vertex_cit = graph.VertexCBegin(); 
             !vertex_cit.IsDone(); 
              vertex_cit++){
      /// <VertexPtr, bool>
      auto vertex_ret = this->AddVertex(vertex_cit->id(), 
                                        vertex_cit->label());
      assert(vertex_ret.second);
      for (auto attr_cit = vertex_cit->AttributeCBegin();
               !attr_cit.IsDone();attr_cit++){
        switch(attr_cit->value_type()){
        case BasicDataType::kTypeString:
          vertex_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<std::string>());
          continue;
        case BasicDataType::kTypeInt:
          vertex_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<int>());
          continue;
        case BasicDataType::kTypeInt64:
          vertex_ret.first->AddAttribute(attr_cit->key(),
                               attr_cit->template const_value<int64_t>());
          continue;
        case BasicDataType::kTypeFloat:
          vertex_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<float>());
          continue;
        case BasicDataType::kTypeDouble:
          vertex_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<double>());
          continue;
        case BasicDataType::kTypeDateTime:
          vertex_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<DateTime>());
          continue;
        default:
          /// unknown data type
          assert(false);
        }
      }
    }
    for (auto vertex_cit = graph.VertexCBegin(); 
             !vertex_cit.IsDone(); 
              vertex_cit++){
      auto vertex_ptr = this->FindVertex(vertex_cit->id());
      for (auto edge_cit = vertex_cit->OutEdgeCBegin(); 
               !edge_cit.IsDone(); 
                edge_cit++) {
        assert(vertex_ptr->id() == edge_cit->const_src_ptr()->id());
        auto dst_ptr = this->FindVertex(edge_cit->const_dst_ptr()->id());
        /// <EdgePtr, bool>
        auto edge_ret = vertex_ptr->AddEdge(dst_ptr,
                                            edge_cit->label(), 
                                            edge_cit->id());
        assert(edge_ret.second);
        for (auto attr_cit = edge_cit->AttributeCBegin();
                 !attr_cit.IsDone(); attr_cit++){
          switch(attr_cit->value_type()){
          case BasicDataType::kTypeString:
            edge_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<std::string>());
            continue;
          case BasicDataType::kTypeInt:
            edge_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<int>());
            continue;
          case BasicDataType::kTypeInt64:
            edge_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<int64_t>());
            continue;
          case BasicDataType::kTypeFloat:
            edge_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<float>());
            continue;
          case BasicDataType::kTypeDouble:
            edge_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<double>());
            continue;
          case BasicDataType::kTypeDateTime:
            edge_ret.first->AddAttribute(
                               attr_cit->key(),
                               attr_cit->template const_value<DateTime>());
            continue;
          default:
            /// unknown data type
            assert(false);
          }
        }
      }
    }
    return;
  }

 public:
  Graph() = default;

  ~Graph() {
    this->Deconstruct();
    return;
  }
  
  Graph(const Graph& graph) {
    this->CanConvertTo(graph);
    this->Construct(graph);
    return;
  }

  template<typename GraphType>
  Graph(const GraphType& graph) {
    this->CanConvertTo(graph);
    this->Construct(graph);
    return;
  }

  Graph& operator=(const Graph& graph) {
    this->CanConvertTo(graph);
    this->Deconstruct();
    this->Construct(graph);
    return *this;
  }

  template<typename GraphType>
  Graph& operator=(const GraphType& graph) {
    this->CanConvertTo(graph);
    this->Deconstruct();
    this->Construct(graph);
    return *this;
  }
  
  template<typename GraphType>
  void CopyGraphTopo(const GraphType& graph) {
    this->Deconstruct();
    for (auto vertex_cit = graph.VertexCBegin(); 
             !vertex_cit.IsDone(); 
              vertex_cit++){
      /// <VertexPtr, bool>
      auto vertex_ret = this->AddVertex(vertex_cit->id(), 
                                        vertex_cit->label());
    }
    for (auto vertex_cit = graph.VertexCBegin(); 
             !vertex_cit.IsDone(); 
              vertex_cit++){
      auto vertex_ptr = this->FindVertex(vertex_cit->id());
      for (auto edge_cit = vertex_cit->OutEdgeCBegin(); 
               !edge_cit.IsDone(); 
                edge_cit++) {
        assert(vertex_ptr->id() == edge_cit->const_src_ptr()->id());
        auto dst_ptr = this->FindVertex(edge_cit->const_dst_ptr()->id());
        /// <EdgePtr, bool>
        auto edge_ret = vertex_ptr->AddEdge(dst_ptr,
                                            edge_cit->label(), 
                                            edge_cit->id());
      }
    }
    return;
  }


  inline void Clear() {
    this->Deconstruct();
    return;
  }

  inline std::pair<VertexPtr, bool> AddVertex(
      const typename VertexType::IDType& id,
      const typename VertexType::LabelType& label) {
    const VertexPtr ret = this->FindVertex(id);
    if (!ret.IsNull()) {
      /// already exist
      return std::pair<VertexPtr, bool>(ret, false);
    }
    InnerVertex_* const temp_inner_vertex_ptr = new InnerVertex_(id, label);
    VertexPtr temp_vertex_ptr(temp_inner_vertex_ptr);
    /// vertex label iterator
    // <iterator, bool>
    auto vertex_label_ret = this->vertexes_.Insert(label, VertexIDContainerType());
    /// vertex ID iterator
    // <iterator, bool>
    auto vertex_id_ret
       = vertex_label_ret.first
                         .template get<kVertexIDContainerIdx>()
                         .Insert(id, std::move(temp_vertex_ptr));
    assert(vertex_id_ret.second);
    return std::pair<VertexPtr, bool>(vertex_id_ret.first
                                                   .template get<kVertexPtrIdx>(),
                                      true);
  }
  /// possible variant:
  ///     AddVertex(id)
  ///     AddVertex(id,label)
  ///     AddVertex(id,label,attribute)
  ///     AddVertex(id,label,attribute0,attribute1,...)
  ///     AddVertex(id,attribute)
  ///     AddVertex(id,attribute0,attribute1,...)
  ///     ...

  /// return whether has removed that vertex successfully
  inline bool EraseVertex(const VertexConstPtr& vertex_const_ptr){
    /// <iterator, bool>
    auto vertex_label_ret = this->vertexes_.Find(vertex_const_ptr->label());
    if (!vertex_label_ret.second){
      /// does not have a vertex with that label
      return false;
    }
    /// found that vertex
    /// erase out edge
    for (auto edge_it = vertex_label_ret.first
                       .template get<kVertexPtrIdx>()
                      ->OutEdgeBegin();
             !edge_it.IsDone();){
      edge_it = vertex_label_ret.first
                                .template get<kVertexPtrIdx>()
                               ->EraseEdge(edge_it);
    }
    /// erase in edge
    for (auto edge_it = vertex_label_ret.first
                       .template get<kVertexPtrIdx>()
                      ->InEdgeBegin();
             !edge_it.IsDone();){
      edge_it = vertex_label_ret.first
                                .template get<kVertexPtrIdx>()
                               ->EraseEdge(edge_it);
    }
    bool ret = vertex_label_ret.first
                               .template get<kVertexIDContainerIdx>()
                               .Erase(vertex_const_ptr->id());
    if (vertex_label_ret.first
                        .template get<kVertexIDContainerIdx>()
                        .size() == 0){
      this->vertexes_.Erase(vertex_label_ret.first);
    }
    return ret;
  }
  
  inline size_t EraseVertex(const VertexIDType& vertex_id){
    for (auto vertex_label_it  = this->vertexes_.begin();
              vertex_label_it != this->vertexes_.end();
            ++vertex_label_it) {
      /// <iterator of ID container, bool>
      const auto ret = vertex_label_it.template get<kVertexIDContainerIdx>()
                                      .Find(vertex_id);
      if (ret.second) {
        /// erase out edge
        for (auto edge_it = ret.first.template get<kVertexPtrIdx>()
                              ->OutEdgeBegin();
                 !edge_it.IsDone();){
          edge_it = ret.first.template get<kVertexPtrIdx>()
                            ->EraseEdge(edge_it);
        }
        /// erase in edge
        for (auto edge_it = ret.first.template get<kVertexPtrIdx>()
                              ->InEdgeBegin();
                 !edge_it.IsDone();){
          edge_it = ret.first.template get<kVertexPtrIdx>()
                            ->EraseEdge(edge_it);
        }
        vertex_label_it.template get<kVertexIDContainerIdx>()
                       .Erase(vertex_id);
        if (vertex_label_it.template get<kVertexIDContainerIdx>()
                           .size() == 0){
          this->vertexes_.Erase(vertex_label_it);
        }
        /// successfully erased vertex
        return 1;
      }
    }
    /// not found
    return 0;
  }

  inline VertexIterator EraseVertex(VertexIterator& vertex_iterator) {
    /// depth of each level
    static constexpr IteratorDepthType vertex_id_container_depth = 0;
    static constexpr IteratorDepthType vertex_ptr_depth = 1;
    /// type of each level
    using VertexLabelIteratorType =
        typename VertexLabelContainerType::iterator;
    using VertexIDIteratorType =
        typename VertexIDContainerType::iterator;
        
    /// erase out edge
    for (auto edge_it = vertex_iterator->OutEdgeBegin();
             !edge_it.IsDone();){
      edge_it = vertex_iterator->EraseEdge(edge_it);
    }
    /// erase in edge
    for (auto edge_it = vertex_iterator->InEdgeBegin();
             !edge_it.IsDone();){
      edge_it = vertex_iterator->EraseEdge(edge_it);
    }

    VertexIterator ret_iterator = vertex_iterator;
    void* ret_ptr = &ret_iterator;
    /// upper cast
    VertexContentIterator* ret_content_it
      = static_cast<VertexContentIterator*>(ret_ptr);
    /// reference of the containers
    VertexLabelContainerType& vertex_label_container_ref = this->vertexes_;
    VertexIDContainerType& vertex_id_container_ref
                         = ret_content_it->template get<kVertexIDContainerIdx,
                                                         vertex_id_container_depth>();
    /// reference of the iterators
    VertexLabelIteratorType& ret_vertex_label_iterator =
           ret_content_it->template get_iterator<vertex_id_container_depth>();
    VertexIDIteratorType& ret_vertex_id_iterator =
           ret_content_it->template get_iterator<vertex_ptr_depth>();
    ret_vertex_id_iterator = vertex_id_container_ref.Erase(ret_vertex_id_iterator);
    if (!vertex_id_container_ref.empty()) {
      /// vertex id container is not empty
      if (ret_vertex_id_iterator != vertex_id_container_ref.end()) {
        /// need not move to the next vertex_id_container
        return ret_iterator;
      }
      /// move to the next vertex_id_container
      ret_vertex_label_iterator++;
    } else {
      /// this container is empty, remove this one
      ret_vertex_label_iterator 
        = vertex_label_container_ref.Erase(ret_vertex_label_iterator);
    }

    /// vertex label container is not empty
    if (!vertex_label_container_ref.empty()
      &&(vertex_label_container_ref.end() != ret_vertex_label_iterator)) {
      /// need not move to the next vertex_id_container
      ret_vertex_id_iterator = ret_vertex_label_iterator
                              .template get<kVertexIDContainerIdx>()
                              .begin();
      /// the end iterator might be invalid since an element is erased
      /// from the vertex label container
      ret_content_it->iterator_end() = vertex_label_container_ref.end();
      return ret_iterator;
    }
    /// reach the end of vertex label container
    /// return an End iterator
    return VertexIterator();
  }

  // inline bool EraseItem(const GraphItem<Graph>& graph_item){
  //   switch(graph_item.type()){
  //   case ItemType::Vertex:
  //     assert(!graph_item.vertex_const_ptr().IsNull());
  //     return this->EraseVertex(graph_item.vertex_const_ptr());
  //   case ItemType::VertexAttr:
  //     assert(!graph_item.vertex_const_ptr().IsNull());
  //     return graph_item.vertex_ptr()
  //                     ->EraseAttribute(graph_item
  //                                     .vertex_attribute_key());
  //   default:
  //     assert(false);
  //   }
  //   return false;
  // }

  // inline bool EraseItem(const GraphItem<const Graph>& graph_item){
  //   VertexPtr vertex_ptr;
  //   switch(graph_item.type()){
  //   case ItemType::Vertex:
  //     assert(!graph_item.vertex_const_ptr().IsNull());
  //     return this->EraseVertex(graph_item.vertex_const_ptr());
  //   case ItemType::VertexAttr:
  //     assert(!graph_item.vertex_const_ptr().IsNull());
  //     vertex_ptr = this->FindVertex(graph_item.vertex_const_ptr()->id(),
  //                                   graph_item.vertex_const_ptr()->label());
  //     assert(!vertex_ptr.IsNull());
  //     return vertex_ptr->EraseAttribute(graph_item
  //                                      .vertex_attribute_key());
  //   default:
  //     assert(false);
  //   }
  //   return false;
  // }

  inline bool EraseEdge(const typename EdgeType::IDType& edge_id) {
    for (auto vertex_it = this->VertexBegin(); !vertex_it.IsDone();
         vertex_it++) {
      if (vertex_it->EraseEdge(edge_id)) return true;
    }
    return false;
  }
  /*
  bool find_flag = false;

      for (auto vertex_ptr_it = this->VertexBegin(); !vertex_ptr_it.IsDone();
           ++vertex_ptr_it) {
        EdgePtr edge_ptr = vertex_ptr_it->FindOutEdge(edge_id);
        VertexPtr src_ptr = vertex_ptr_it;
        VertexPtr dst_ptr = edge_ptr->dst_ptr();
        EdgeLabelType edge_label = edge_ptr->label();
        if (!edge_ptr.IsNull()) {
          find_flag = true;
          src_ptr->EraseEdge(dst_ptr, edge_label, edge_id);
          break;
        }
      }
      return find_flag;*/

  inline std::pair<EdgePtr, bool> AddEdge(
      const typename VertexType::IDType& src_id,
      const typename VertexType::IDType& dst_id,
      const typename   EdgeType::LabelType& edge_label,
      const typename   EdgeType::IDType&    edge_id) {
    VertexPtr src_ptr = this->FindVertex(src_id);
    VertexPtr dst_ptr = this->FindVertex(dst_id);
    if (src_ptr.IsNull() || dst_ptr.IsNull()) {
      /// the src vertex or the dst vertex does not exist
      return std::pair<EdgePtr, bool>(EdgePtr(), false);
    }
//    for (auto vertex_ptr_it = this->VertexBegin();
//             !vertex_ptr_it.IsDone();
//            ++vertex_ptr_it) {
//      EdgePtr const edge_ptr = vertex_ptr_it->FindOutEdge(edge_id);
//      if (!edge_ptr.IsNull()) {
//        /// the edge with this edge_id has already existed
//        return std::pair<EdgePtr, bool>(edge_ptr, false);
//      }
//    }
    /// assume that the duplicate edge is allowed
    return src_ptr->AddEdge(dst_ptr, edge_label, edge_id);
  }
  /// possible variant:
  ///     AddEdge(src_id,dst_id)
  ///     AddEdge(src_id,dst_id,edge_label)
  ///     AddEdge(src_id,dst_id,edge_label,edge_id)
  ///     AddEdge(src_id,dst_id,edge_label,edge_id,edge_attribute)
  ///     ...

  inline VertexPtr FindVertex(const typename VertexType::IDType& id) {
    for (auto vertex_label_it  = this->vertexes_.begin();
              vertex_label_it != this->vertexes_.end();
            ++vertex_label_it) {
      /// <iterator of ID container, bool>
      const auto ret = vertex_label_it.template get<kVertexIDContainerIdx>().Find(id);
      if (ret.second) {
        /// found it
        return ret.first.template get_const<kVertexPtrIdx>();
      }
    }
    /// not found
    return VertexPtr();
  }
  
  inline VertexConstPtr FindVertex(const typename VertexType
                                                    ::IDType& id) const{
    return this->FindConstVertex(id);
  }

  inline VertexConstPtr FindConstVertex(
      const typename VertexType::IDType& id) const {
    for (auto vertex_label_cit  = this->vertexes_.cbegin();
              vertex_label_cit != this->vertexes_.cend();
            ++vertex_label_cit) {
      /// <iterator of ID container, bool>
      const auto ret = vertex_label_cit.template get_const<kVertexIDContainerIdx>()
                                       .FindConst(id);
      if (ret.second) {
        /// found it
        return ret.first.template get_const<kVertexPtrIdx>();
      }
    }
    /// not found
    return VertexConstPtr();
  }

  inline VertexPtr FindVertex(const typename VertexType::IDType& id,
                              const typename VertexType::LabelType& label) {
    /// <iterator of VertexLabelContainer, bool>
    auto vertex_label_ret = this->vertexes_.Find(label);
    if (!vertex_label_ret.second) {
      /// not have this vertex label
      return VertexPtr();
    }
    auto vertex_id_ret
       = vertex_label_ret.first.template get<kVertexIDContainerIdx>().Find(id);
    if (!vertex_id_ret.second) {
      /// not have this vertex id
      return VertexPtr();
    }
    return vertex_id_ret.first.template get<kVertexPtrIdx>();
  }

  inline VertexConstPtr FindConstVertex(
      const typename VertexType::IDType& id,
      const typename VertexType::LabelType& label) const {
    /// <iterator of VertexLabelContainer, bool>
    auto vertex_label_ret = this->vertexes_.FindConst(label);
    if (!vertex_label_ret.second) {
      /// not have this vertex label
      return VertexPtr();
    }
    auto vertex_id_ret
       = vertex_label_ret.first.template get_const<kVertexIDContainerIdx>()
                               .FindConst(id);
    if (!vertex_id_ret.second) {
      /// not have this vertex id
      return VertexConstPtr();
    }
    return vertex_id_ret.first.template get_const<kVertexPtrIdx>();
  }

  inline EdgePtr FindEdge(const typename EdgeType::IDType& edge_id) {
    for (auto vertex_ptr_it = this->VertexBegin(); !vertex_ptr_it.IsDone();
         vertex_ptr_it++) {
      EdgePtr const edge_ptr = vertex_ptr_it->FindOutEdge(edge_id);
      if (!edge_ptr.IsNull()) {
        /// the edge with this edge_id has already existed
        return edge_ptr;
      }
    }
    return EdgePtr();
  }

  ///  unimplemented:
  ///  inline EdgeConstPtr FindConstEdge(
  ///           const typename EdgeType::IDType& edge_id) const;
  ///  inline EdgeConstPtr FindConstEdge(
  ///           const typename EdgeType::   IDType& edge_id,
  ///           const typename EdgeType::LabelType& edge_label) const;
  ///  inline EdgeConstPtr FindConstEdge(
  ///                const typename EdgeType::IDType& edge_id) const;
  ///  inline EdgeConstPtr FindConstEdge(
  ///                const typename EdgeType::   IDType& edge_id,
  ///                const typename EdgeType::LabelType& edge_label) const;

  EdgeSizeType CountEdge() const {
    EdgeSizeType edge_num = 0;
    for (auto vertex_cit = this->VertexCBegin();
             !vertex_cit.IsDone();vertex_cit++){
      edge_num += vertex_cit->CountOutEdge();
    }
    return edge_num;
  }

  VertexSizeType CountVertex() const {
    VertexSizeType vertex_num = 0;
    for (auto vertex_label_it  = this->vertexes_.cbegin();
              vertex_label_it != this->vertexes_.cend();
              vertex_label_it++) {
      vertex_num += vertex_label_it.template get_const<kVertexIDContainerIdx>().size();
    }
    return vertex_num;
  }

  inline VertexSizeType CountVertex(
      const typename VertexType::LabelType& label) const {
    /// <iterator of VertexLabelContainer, bool>
    auto ret = this->vertexes_.FindConst(label);
    if (!ret.second) {
      /// does not have this vertex label
      return 0;
    }
    return ret.first.template get_const<kVertexIDContainerIdx>().size();
  }

  inline VertexIterator VertexBegin() {
    return VertexIterator(this->vertexes_.begin(), this->vertexes_.end());
  }

  inline VertexConstIterator VertexBegin() const {
    return this->VertexCBegin();
  }

  inline VertexConstIterator VertexCBegin() const {
    return VertexConstIterator(this->vertexes_.cbegin(),
                               this->vertexes_.cend());
  }

  inline VertexIteratorSpecifiedLabel VertexBegin(
      typename VertexType::LabelType label) {
    /// <iterator of VertexLabelContainer, bool>
    auto ret = this->vertexes_.Find(label);
    if (!ret.second)  /// does not have this vertex label
      return VertexIteratorSpecifiedLabel();
    return VertexIteratorSpecifiedLabel(
        ret.first.template get<kVertexIDContainerIdx>().begin(),
        ret.first.template get<kVertexIDContainerIdx>().end());
  }
  
  inline VertexConstIteratorSpecifiedLabel VertexBegin(
      typename VertexType::LabelType label) const {
    return this->VertexCBegin(label);
  }
  
  inline VertexConstIteratorSpecifiedLabel VertexCBegin(
      typename VertexType::LabelType label) const {
    /// <iterator of VertexLabelContainer, bool>
    auto ret = this->vertexes_.FindConst(label);
    if (!ret.second)  /// does not have this vertex label
      return VertexConstIteratorSpecifiedLabel();
    return VertexConstIteratorSpecifiedLabel(
        ret.first.template get_const<kVertexIDContainerIdx>().cbegin(),
        ret.first.template get_const<kVertexIDContainerIdx>().cend());
  }

  /// unimplemented:
  /// inline VertexIterator EraseVertex(const VertexIterator& iterator);
  /// inline VertexIteratorSpecifiedLabel
  ///                       EraseVertex(const VertexIteratorSpecifiedLabel&
  ///                       iterator);

  /// unimplemented:
  /// inline VertexPtr SetLabel(VertexPtr&       vertex_ptr,
  ///                     const VertexLabelType& vertex_label);
  /// inline VertexPtr SetLabel(const VertexIDType&    vertex_id,
  ///                           const VertexLabelType& vertex_label);
  /// inline   EdgePtr SetLabel(EdgePtr&       edge_ptr,
  ///                     const EdgeLabelType& edge_label);
  /// inline   EdgePtr SetLabel(const   EdgeIDType&      edge_id,
  ///                           const   EdgeLabelType&   edge_label);
  /// inline   EdgePtr SetSrc  (  EdgePtr&   edge_ptr,
  ///                           VertexPtr& vertex_ptr);
  /// inline   EdgePtr SetDst  (  EdgePtr&   edge_ptr,
  ///                           VertexPtr& vertex_ptr);
  /// inline   EdgePtr SetSrc (const EdgeIDType& edge_id,
  ///                          VertexPtr& vertex_ptr);
  /// inline   EdgePtr SetDst (const EdgeIDType& edge_id,
  ///            G             VertexPtr& vertex_ptr);

 public:
  std::string vertex_id_type() const { return TypeToString<VertexIDType>(); }
  std::string vertex_label_type() const {
    return TypeToString<VertexLabelType>();
  }
  std::string edge_id_type() const { return TypeToString<EdgeIDType>(); }
  std::string edge_label_type() const { return TypeToString<EdgeLabelType>(); }

 private:
  /// data member
  VertexLabelContainerType vertexes_;
};

}  // namespace GUNDAM
#endif  // _GRAPH_H
