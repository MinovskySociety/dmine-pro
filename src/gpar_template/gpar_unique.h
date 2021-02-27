#ifndef _GPARUNIQUE_H
#define _GPARUNIQUE_H
#include "gpar_dfs_code.h"
#include "gpar_new.h"
#include "gundam/vf2.h"
namespace gmine_new {
template <class GPAR>
bool CheckIsIso(const GPAR &query, const GPAR &target) {
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using VertexIDType = typename GPAR::VertexIDType;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  MatchResultList match_result;
  VertexIDType query_x_id = query.x_node_ptr()->id();
  VertexIDType target_x_id = query.x_node_ptr()->id();
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query.pattern, target.pattern, query_x_id, target_x_id,
      GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
      GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1, match_result);
  if (match_result.size() < 1) return false;
  match_result.clear();
  MatchResultList().swap(match_result);
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      target.pattern, query.pattern, target_x_id, query_x_id,
      GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
      GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1, match_result);
  if (match_result.size() < 1) return false;
  return true;
}
template <class GPARList>
inline void UniqueGPARList(GPARList &gpar_list) {
  using GPAR = typename GPARList::value_type;
  GPARList temp_gpar_list;
  for (size_t gpar_list_iter = 0; gpar_list_iter < gpar_list.size();
       gpar_list_iter++) {
    bool is_find_iso_gpar = false;
    for (size_t temp_gpar_list_iter = 0;
         temp_gpar_list_iter < temp_gpar_list.size(); temp_gpar_list_iter++) {
      if (CheckIsIso(gpar_list[gpar_list_iter],
                     temp_gpar_list[temp_gpar_list_iter])) {
        is_find_iso_gpar = true;
        break;
      }
    }
    if (!is_find_iso_gpar) {
      temp_gpar_list.push_back(gpar_list[gpar_list_iter]);
    }
  }
  std::swap(gpar_list, temp_gpar_list);
}
int now_id = 0;
template <class GPARList>
inline void UniqueGPARListUsingDFSCode(GPARList &gpar_list) {
  using GPAR = typename GPARList::value_type;
  using DFSCodeList = std::vector<GPARDFSCode<GPAR>>;
  using SuppQ = typename GPAR::SuppType;
  std::map<DFSCodeList, int32_t> all_gpar;
  GPARList temp_gpar_list;
  for (size_t i = 0; i < gpar_list.size(); i++) {
    DFSCodeList dfs_code_container;
    GetGPARDFSCode(gpar_list[i], dfs_code_container);
    auto it = all_gpar.find(dfs_code_container);
    if (it != all_gpar.end()) {
      // std::cout << "same:" << std::endl;
      // TransposePattern(gpar_list[i].pattern);
      // TransposePattern(temp_gpar_list[it->second].pattern);
      SuppQ intersection_result;
      std::set_intersection(
          gpar_list[i].supp_Q().begin(), gpar_list[i].supp_Q().end(),
          temp_gpar_list[it->second].supp_Q().begin(),
          temp_gpar_list[it->second].supp_Q().end(),
          inserter(intersection_result, intersection_result.begin()));
      temp_gpar_list[it->second].supp_Q() = intersection_result;
      intersection_result.clear();

      std::set_intersection(
          gpar_list[i].supp_R().begin(), gpar_list[i].supp_R().end(),
          temp_gpar_list[it->second].supp_R().begin(),
          temp_gpar_list[it->second].supp_R().end(),
          inserter(intersection_result, intersection_result.begin()));
      temp_gpar_list[it->second].supp_R() = intersection_result;

      continue;
    } else {
      temp_gpar_list.push_back(gpar_list[i]);
      all_gpar.emplace(std::move(dfs_code_container),
                       temp_gpar_list.size() - 1);
    }
  }
  for (auto &gpar : temp_gpar_list) {
    gpar.Setid(++now_id);
  }
  std::swap(gpar_list, temp_gpar_list);
}
}  // namespace gmine_new

#endif