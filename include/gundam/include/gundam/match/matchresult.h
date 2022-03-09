#ifndef _GUNDAM_MATCH_MATCHRESULT_H
#define _GUNDAM_MATCH_MATCHRESULT_H
#include <ostream>

#include "gundam/match/match.h"

namespace GUNDAM{

// single match_result is ptr->ptr
template <typename PatternVertexIDType, 
          typename  TargetVertexIDType,
          typename MatchResultContainer>
void MatchResultToFile(const MatchResultContainer &r, std::ostream &out) {
  //size_t total_match_num = r.size();
  std::map<PatternVertexIDType, int> vertex_id_to_index;
  std::vector<PatternVertexIDType> index_to_vertex_id;
  for (const auto &single_match_result : r) {
    for (const auto &single_pair : single_match_result) {
      index_to_vertex_id.push_back(single_pair.first->id());
      vertex_id_to_index.insert(
          std::make_pair(single_pair.first->id(),
                         static_cast<int>(index_to_vertex_id.size() - 1)));
    }
    break;
  }
  out << "match_id";
  for (const auto &it : index_to_vertex_id) {
    out << "," << it;
  }
  out << std::endl;
  int now_match_id = 0;
  for (const auto &single_match_result : r) {
    std::vector<TargetVertexIDType> match_target_result(
        index_to_vertex_id.size());
    for (const auto &single_pair : single_match_result) {
      int now_index = vertex_id_to_index[single_pair.first->id()];
      match_target_result[now_index] = single_pair.second->id();
    }
    out << (++now_match_id);
    for (const auto &it : match_target_result) {
      out << "," << it;
    }
    out << std::endl;
  }
}
  
// single match_result is ptr->ptr
template <typename SrcGraphType, 
          typename DstGraphType,
          auto... MatchSetConfigures>
void MatchSetToFile(MatchSet<SrcGraphType,
                             DstGraphType,
                             MatchSetConfigures...>& match_set, 
                    std::ostream &out) {
  if (match_set.Empty()){
    return;
  }

  using SrcVertexHandle = typename GUNDAM::VertexHandle<SrcGraphType>::type;
  using DstVertexHandle = typename GUNDAM::VertexHandle<SrcGraphType>::type;

  std::vector<SrcVertexHandle> src_vertex_handle_set;

  auto match_it = match_set.MatchBegin();
  assert(!match_it.IsDone());
  for (auto map_it = match_it->MapBegin();
           !map_it.IsDone();
            map_it++){
    src_vertex_handle_set.emplace_back(map_it->src_handle());
  }

  out << "match_id";
  for (auto &src_vertex_handle : src_vertex_handle_set) {
    out << "," << src_vertex_handle->id();
  }
  out << std::endl;
  
  int now_match_id = 0;
  for (auto match_it = match_set.MatchBegin();
           !match_it.IsDone(); match_it++){
    assert(src_vertex_handle_set.size() == match_it->size());
    out << (now_match_id++);
    for (auto &src_vertex_handle : src_vertex_handle_set) {
      auto dst_vertex_handle = match_it->MapTo(src_vertex_handle);
      assert(dst_vertex_handle);
      out << "," << dst_vertex_handle->id();
    }
    out << std::endl;
  }
}

}; // namespace GUNDAM


#endif