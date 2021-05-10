#ifndef _GUNDAM_MATCH_MATCH_H
#define _GUNDAM_MATCH_MATCH_H
#pragma once
#include <iostream>
#include <set>
#include <vector>

#include <string>
#include <sstream>
#include <fstream>

#include "gundam/component/container.h"
#include "gundam/component/iterator.h"

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/data_type/datatype.h"

namespace GUNDAM {

template <typename SrcGraphType,
          typename DstGraphType,
          enum ContainerType map_container_type = ContainerType::Vector,
          enum SortType map_container_sort_type = SortType::Default>
class Match;

/// can be configured to much different type
///     constant src graph to     constant dst graph
///     constant src graph to non-constant dst graph
/// non-constant src graph to     constant dst graph
/// non-constant src graph to non-constant dst graph
template <typename SrcGraphType,
          typename DstGraphType,
          enum ContainerType map_container_type,
          enum SortType map_container_sort_type>
class Match {
 private:
  template <typename _SrcGraphType,
            typename _DstGraphType,
            enum ContainerType _map_container_type,
            enum      SortType _map_container_sort_type>
  friend class Match;

  using SrcVertexIDType     = typename SrcGraphType ::VertexType ::IDType;
  using SrcVertexHandleType = typename VertexHandle<SrcGraphType>::type;

  using DstVertexIDType     = typename DstGraphType ::VertexType ::IDType;
  using DstVertexHandleType = typename VertexHandle<DstGraphType>::type;

  // using SrcVertexConstHandle = typename SrcGraphType ::VertexConstPtr;
  // using DstVertexConstHandle = typename DstGraphType ::VertexConstPtr;

  static constexpr TupleIdxType kSrcVertexPtrIdx = 0;
  static constexpr TupleIdxType kDstVertexPtrIdx = 1;

  using MapContainerType =
      Container<map_container_type,
                map_container_sort_type,
                SrcVertexHandleType,
                DstVertexHandleType>;

  template <typename ContainerType_,
            bool is_const_,
            IteratorDepthType depth_,
            TupleIdxType src_idx_,
            TupleIdxType dst_idx_>
  class MapContentIterator_
      : protected InnerIterator_<ContainerType_, is_const_, depth_> {
   private:
    using InnerIteratorType = InnerIterator_<ContainerType_, is_const_, depth_>;

   protected:
    using ContentPtr =
        typename std::conditional<is_const_,
                  const MapContentIterator_*,
                        MapContentIterator_*>::type;

    using InnerIteratorType::IsDone;
    using InnerIteratorType::ToNext;
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
    using InnerIteratorType::InnerIteratorType;

    // inline SrcVertexConstHandle const_src_handle() const {
    //   return InnerIteratorType::template get_const<
    //            src_idx_, depth_ - 1>();
    //   auto temp_vertex_ptr =
    //       InnerIteratorType::template get_const<
    //            src_idx_, depth_ - 1>();
    //   return SrcVertexConstHandle(temp_vertex_ptr);
    // }

    inline SrcVertexHandleType src_handle() const{
      return InnerIteratorType::template get_const<
               src_idx_, depth_ - 1>();
    }

    // inline SrcVertexConstHandle src_handle() const{
    //   return this->const_src_handle();
    // }

    // inline DstVertexConstHandle const_dst_handle() const {
    //   auto temp_vertex_ptr =
    //       InnerIteratorType::template get_const<
    //            dst_idx_, depth_ - 1>();
    //   return DstVertexConstHandle(temp_vertex_ptr);
    // }

    inline DstVertexHandleType dst_handle() const{
      return InnerIteratorType::template get_const<
               dst_idx_, depth_ - 1>();
    }

    // inline DstVertexConstHandle dst_handle() const{
    //   return this->const_dst_handle();
    // }
  };

  MapContainerType match_container_;

 public:
  using MapIterator =
      Iterator_<MapContentIterator_<MapContainerType, false, 1, kSrcVertexPtrIdx,
                                                                kDstVertexPtrIdx>>;
  using MapConstIterator =
      Iterator_<MapContentIterator_<MapContainerType,  true, 1, kSrcVertexPtrIdx,
                                                                kDstVertexPtrIdx>>;

  using size_type = typename MapContainerType::size_type;

  Match() = default;

  Match(const Match&) = default;
  Match(Match&&) = default;

  Match& operator=(const Match&) = default;
  Match& operator=(Match&&) = default;

  Match(SrcGraphType& src_graph,
        DstGraphType& dst_graph){
    return;
  }

  Match(SrcGraphType& src_graph,
        DstGraphType& dst_graph,
        std::string construct_method) : match_container_(){
    assert(construct_method == "same_id_map");
    for (auto src_vertex_it = src_graph.VertexBegin();
             !src_vertex_it.IsDone();
              src_vertex_it++){
      auto dst_ptr = dst_graph.FindVertex(src_vertex_it->id());
      if (!dst_ptr){
        // dst_ptr is null 
        continue;
      }
      this->AddMap(src_vertex_it, dst_ptr);
    }
    return;
  }

  inline size_type size() const { return this->match_container_.size(); }

  inline bool empty() const { return this->match_container_.empty(); }

  inline bool HasMap(const SrcVertexHandleType& src_ptr) const {
    /// <iterator, bool>
    auto ret = this->match_container_.FindConst(src_ptr);
    return ret.second;
  }

  inline bool HasMap(const SrcVertexIDType& src_id) const {
    for (auto cit  = this->match_container_.cbegin();
              cit != this->match_container_.cend();cit++){
      if (cit.template get_const<kSrcVertexPtrIdx>()->id() == src_id)
        return true;
    }
    return false;
  }

  /// if such a match does not exist, add this src-dst
  /// pair to current match then return true
  /// else if the match for this src vertex has already
  /// exisited then return false
  inline bool AddMap(const SrcVertexHandleType& src_ptr,
                     const DstVertexHandleType& dst_ptr) {
    // assert(src_ptr != nullptr
    //     && dst_ptr != nullptr);
    /// <iterator, bool>
    auto temp_dst_ptr = dst_ptr;
    auto ret = this->match_container_.Insert(src_ptr, std::move(temp_dst_ptr));
    if (!ret.second) {
      /// has already existed in the current match
      return false;
    }
    /// has not added match for that vertex
    return true;
  }

  /// constant dst
  inline DstVertexHandleType MapTo(const SrcVertexHandleType& src_ptr) const {
    assert(src_ptr);
    /// <iterator, bool>
    auto ret = this->match_container_.FindConst(src_ptr);
    if (!ret.second) {
      /// does not find src_ptr
      /// this just a partial match and does not contain
      return DstVertexHandleType();
    }
    /// found that
    return ret.first.template get_const<kDstVertexPtrIdx>();
  }

  inline Match<DstGraphType,
               SrcGraphType> Reverse() const {
    Match<DstGraphType,
          SrcGraphType> reverse_match;
    for (auto cit  = this->match_container_.cbegin();
              cit != this->match_container_.cend();cit++){
      auto ret = reverse_match.AddMap(cit.template get<kDstVertexPtrIdx>(),
                                      cit.template get<kSrcVertexPtrIdx>());
      assert(ret);
    }
    assert(this->size() == reverse_match.size());
    return std::move(reverse_match);
  }

  // /// constant dst
  // inline DstVertexConstHandle MapToConst(const SrcVertexHandleType& src_ptr) const {
  //   assert(!src_ptr.IsNull());
  //   /// <iterator, bool>
  //   auto ret = this->match_container_.FindConst(src_ptr);
  //   if (!ret.second) {
  //     /// does not find src_ptr
  //     /// this just a partial match and does not contain
  //     return DstVertexConstHandle();
  //   }
  //   /// found that
  //   return DstVertexConstHandle(ret.first.template get_const<kDstVertexPtrIdx>());
  // }

  /// constant dst
  inline DstVertexHandleType MapTo(const SrcVertexIDType& src_id) const {
    for (auto it  = this->match_container_.begin();
              it != this->match_container_.end();it++){
      if (it.template get_const<kSrcVertexPtrIdx>()->id() == src_id)
        return it.template get_const<kDstVertexPtrIdx>();
    }
    /// not found
    return DstVertexHandleType();
  }

  // /// constant dst
  // inline DstVertexConstHandle MapToConst(const SrcVertexIDType& src_id) const {
  //   for (auto cit  = this->match_container_.cbegin();
  //             cit != this->match_container_.cend();cit++){
  //     if (cit.template get_const<kSrcVertexPtrIdx>()->id() == src_id)
  //       return cit.template get_const<kDstVertexPtrIdx>();
  //   }
  //   /// not found
  //   return DstVertexConstHandle();
  // }

  inline bool operator<(const Match& match) const{
    if (this->size() < match.size())
      return true;
    if (this->size() > match.size())
      return false;
    for (auto  this_cit  = this->match_container_.cbegin(),
              match_cit  = match.match_container_.cbegin();
               this_cit != this->match_container_.cend();
               this_cit++,
              match_cit++) {
      if (this_cit.template get_const<kSrcVertexPtrIdx>()
       < match_cit.template get_const<kSrcVertexPtrIdx>()){
        return true;
      }
      if (this_cit.template get_const<kSrcVertexPtrIdx>()->id()
       > match_cit.template get_const<kSrcVertexPtrIdx>()->id()){
        return false;
      }
      if (this_cit.template get_const<kDstVertexPtrIdx>()->id()
       < match_cit.template get_const<kDstVertexPtrIdx>()->id()){
        return true;
      }
      if (this_cit.template get_const<kDstVertexPtrIdx>()->id()
       > match_cit.template get_const<kDstVertexPtrIdx>()->id()){
        return false;
      }
    }
    return true;
  }

  bool operator==(const Match& match) const {
    for (auto cit  = this->match_container_.cbegin();
              cit != this->match_container_.cend(); cit++) {
      if (match.MapTo(cit.template get_const<kSrcVertexPtrIdx>())
                   != cit.template get_const<kDstVertexPtrIdx>()) {
        /// not equal
        return false;
      }
    }
    return true;
  }

  inline MapIterator MapBegin(){
    return MapIterator(this->match_container_.begin(),
                       this->match_container_.end());
  }

  inline MapConstIterator MapBegin() const {
    return this->MapCBegin();
  }

  inline MapConstIterator MapCBegin() const {
    return MapConstIterator(this->match_container_.cbegin(),
                            this->match_container_.cend());
  }

  /// composition
  /// match2 = match1(match0);
  /// match0: SrcSrcGraphType -> SrcGraphType
  /// match1:                    SrcGraphType -> DstGraphType
  /// match2: SrcSrcGraphType -----------------> DstGraphType
  template<typename SrcSrcGraphType>
  inline Match<SrcSrcGraphType,
                  DstGraphType> operator()(const Match<SrcSrcGraphType,
                                                          SrcGraphType>& match0) const{
    Match<SrcSrcGraphType, DstGraphType> result;
    for (auto cit  = match0.match_container_.cbegin();
              cit != match0.match_container_.cend(); cit++) {
      const auto src_src_ptr = cit.template get_const<kSrcVertexPtrIdx>();
      const auto     src_ptr = cit.template get_const<kDstVertexPtrIdx>();
      const auto     dst_ptr = this->MapTo(src_ptr);
      if (!dst_ptr){
        // "*this" does not have match for src_ptr
        continue;
      }
      result.AddMap(src_src_ptr, dst_ptr);
    }
    assert(result.size() <= match0.size());
    return result;
  }

  template<typename SrcSrcGraphType>
  inline Match<SrcSrcGraphType,
                  DstGraphType> operator()(const Match<SrcSrcGraphType,
                                                          SrcGraphType&>& match0) const{
    const Match<SrcSrcGraphType,
                   SrcGraphType>& temp_match = match0;
    return (*this)(temp_match);
  }
};

template <typename SrcGraphType,
          typename DstGraphType>
Match(SrcGraphType& src_graph,
      DstGraphType& dst_graph) 
   -> Match<SrcGraphType,
            DstGraphType,
           ContainerType::Vector,
                SortType::Default>;

template <typename SrcGraphType,
          typename DstGraphType,
          enum ContainerType match_container_type = ContainerType::Vector,
          enum SortType match_container_sort_type = SortType::Default>
class MatchSet;

template <typename SrcGraphType, 
          typename DstGraphType,
          enum ContainerType match_container_type,
          enum SortType match_container_sort_type>
class MatchSet {
 private:
  using SrcVertexHandleType = typename VertexHandle<SrcGraphType>::type;
  using DstVertexHandleType = typename VertexHandle<DstGraphType>::type;

  using SrcVertexIDType = typename SrcGraphType::VertexType::IDType;
  using DstVertexIDType = typename DstGraphType::VertexType::IDType;

 public:
  using MatchType = Match<SrcGraphType, DstGraphType>;

 private:
  static constexpr TupleIdxType kMatchIdx = 0;
  using MatchContainerType = Container<match_container_type, 
                                       match_container_sort_type, 
                                       MatchType>;

  template <typename ContainerType_,
            bool is_const_,
            IteratorDepthType depth_,
            TupleIdxType key_idx_>
  class MatchContentIterator_
      : protected InnerIterator_<ContainerType_, is_const_, depth_> {
   private:
    using InnerIteratorType = InnerIterator_<ContainerType_, is_const_, depth_>;

   protected:
    using ContentPtr = typename std::conditional<is_const_,
                                          const MatchType*,
                                                MatchType*>::type;
    using ContentDataType = MatchType;
    using InnerIteratorType::IsDone;
    using InnerIteratorType::ToNext;
    static constexpr bool kIsConst_ = is_const_;

    template <bool judge = is_const_,
              typename std::enable_if<judge, bool>::type = false>
    inline ContentPtr content_ptr() const {
      static_assert(judge == is_const_, "illegal usage of this method");
      assert(!this->IsDone());
      return &(InnerIteratorType::template get_const<key_idx_,
                                                     depth_ - 1>());
    }

    template <bool judge = is_const_,
              typename std::enable_if<!judge, bool>::type = false>
    inline ContentPtr content_ptr() {
      static_assert(judge == is_const_, "illegal usage of this method");
      assert(!this->IsDone());
      return &(
          InnerIteratorType::template get<key_idx_, depth_ - 1>());
    }

    template <bool judge = is_const_,
              typename std::enable_if<judge, bool>::type = false>
    inline const MatchType& instance() const {
      static_assert(judge == is_const_, "illegal usage of this method");
      assert(!this->IsDone());
      return (InnerIteratorType::template get_const<key_idx_,
                                                    depth_ - 1>());
    }

    template <bool judge = is_const_,
              typename std::enable_if<!judge, bool>::type = false>
    inline MatchType& instance() {
      static_assert(judge == is_const_, "illegal usage of this method");
      assert(!this->IsDone());
      return (
          InnerIteratorType::template get<key_idx_, depth_ - 1>());
    }

   public:
    using InnerIteratorType::InnerIteratorType;
  };

  MatchContainerType match_set_;

 public:
  using MatchIterator =
      IteratorWithInstance_<MatchContentIterator_<MatchContainerType, false, 1, kMatchIdx>>;
  using MatchConstIterator =
      IteratorWithInstance_<MatchContentIterator_<MatchContainerType,  true, 1, kMatchIdx>>;

  using size_type = typename MatchContainerType::size_type;

  MatchSet() = default;

  MatchSet(SrcGraphType& src_graph,
           DstGraphType& dst_graph){
    return;
  }

  MatchSet(SrcGraphType& src_graph,
           DstGraphType& dst_graph,
           std::istream& match_file){
    if (!match_file.good()){
      std::cout << " match file is not good! " << std::endl;
      return;
    }

    // get the first line of the match_file
    std::string s;
    if (!std::getline( match_file, s )) {
      // cannot get the first line of the match_file
      std::cout << " cannot get the first line of the match_file! " << std::endl;
      return;
    }

    std::istringstream ss( s );
    std::vector <std::string> record;
    record.reserve(src_graph.CountVertex() + 1);
    while (ss) {
      std::string s;
      if (!std::getline( ss, s, ',' )) {
        break;
      }
      record.emplace_back( s );
    }

    if (record.size() != src_graph.CountVertex() + 1) {
      std::cout << "record.size(): "
                <<  record.size() << std::endl;
      std::cout << "src_graph.CountVertex(): "
                <<  src_graph.CountVertex() << std::endl;
      // illegal match_file
      std::cout << " illegal match file, colum mismatch! " << std::endl;
      return;
    }

    if (record[0] != "match_id") {
      // illegal match_file
      std::cout << " illegal match file, first colum of the first line should be \"match_id\"! " 
                << std::endl;
      return;
    }

    std::vector<SrcVertexHandleType> src_vertex_handle_set;

    assert(record.size() > 1);
    for (int i = 1; i < record.size(); i++){
      const SrcVertexIDType src_vertex_id = GUNDAM::StringToDataType<SrcVertexIDType>(record[i]);
      src_vertex_handle_set.emplace_back(src_graph.FindVertex(src_vertex_id));
    }
    assert(src_vertex_handle_set.size() == src_graph.CountVertex());

    while (match_file) {
      std::string s;
      if (!std::getline( match_file, s )) 
        break;

      std::istringstream ss( s );
      std::vector <std::string> record;
      record.reserve(src_graph.CountVertex() + 1);
      while (ss) {
        std::string s;
        if (!std::getline( ss, s, ',' )) {
          break;
        }
        record.emplace_back( s );
      }

      if (record.size() != src_graph.CountVertex() + 1) {
        std::cout << "record.size(): "
                  <<  record.size() << std::endl;
        std::cout << "src_graph.CountVertex(): "
                  <<  src_graph.CountVertex() << std::endl;
        // illegal match_file
        std::cout << " illegal match file, colum mismatch! " << std::endl;
        return;
      }

      auto [match_it, match_ret] = this->AddMatch(MatchType());
      assert(match_ret);

      for (auto [src_vertex_handle_cit,
                 dst_vertex_id_str_cit] = std::tuple{src_vertex_handle_set.cbegin(),
                                                     record.cbegin() + 1};
                 src_vertex_handle_cit != src_vertex_handle_set.cend()
              && dst_vertex_id_str_cit != record.cend();
                 src_vertex_handle_cit++,
                 dst_vertex_id_str_cit++){
        const DstVertexIDType kDstId
          = GUNDAM::StringToDataType<DstVertexIDType>(*dst_vertex_id_str_cit);
        DstVertexHandleType dst_vertex_handle = dst_graph.FindVertex(kDstId);

        auto src_vertex_handle = *src_vertex_handle_cit;

        bool add_map_ret = match_it->AddMap(src_vertex_handle, 
                                            dst_vertex_handle);
        assert(add_map_ret);
      }

      assert(match_it->size() == src_graph.CountVertex());
    }
    return;
  }

  inline size_type size() const { 
    return this->match_set_.size(); 
  }

  inline MatchIterator MatchBegin() {
    return MatchIterator(this->match_set_.begin(), 
                         this->match_set_. end ());
  }

  inline MatchConstIterator MatchBegin() const {
    return this->MatchCBegin();
  }

  inline bool Empty() const { 
    return this->match_set_.empty(); 
  }

  inline MatchConstIterator MatchCBegin() const {
    return MatchConstIterator(this->match_set_.cbegin(), 
                              this->match_set_. cend ());
  }

  inline std::pair<MatchIterator, bool> AddMatch(const MatchType& match) {
    /// <iterator, bool>
    auto ret = this->match_set_.Insert(match);
    assert(ret.second || ret.first == this->match_set_.end());
    return std::pair<MatchIterator, bool>(
        MatchIterator(ret.first, this->match_set_.end()), ret.second);
  }
};

template <typename SrcGraphType,
          typename DstGraphType>
MatchSet(SrcGraphType& src_graph,
         DstGraphType& dst_graph)
   -> MatchSet<SrcGraphType,
               DstGraphType,
              ContainerType::Vector,
                   SortType::Default>;

template <typename SrcGraphType,
          typename DstGraphType>
MatchSet(SrcGraphType& src_graph,
         DstGraphType& dst_graph,
        std::ifstream& supp_file)
   -> MatchSet<SrcGraphType,
               DstGraphType,
              ContainerType::Vector,
                   SortType::Default>;
                   
}  // namespace GUNDAM

#endif
