#ifndef _MATCH_HELPER_H
#define _MATCH_HELPER_H
#include <map>
#include <string>
#include <vector>
namespace GUNDAM {
namespace match_helper {
// user call back of cal match number
template <class QueryVertexPtr, class TargetVertexPtr>
inline bool MatchCallbackLimit(
    const std::map<QueryVertexPtr, TargetVertexPtr> &, int *max_result) {
  if (max_result && *max_result > 0) {
    (*max_result)--;
    if (*max_result == 0) return false;
  }
  return true;
}
// user call back of cal max_result match
template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool MatchCallbackSaveResult(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, int *max_result,
    ResultContainer *r) {
  if (r) {
    r->emplace_back(m);
  }

  return MatchCallbackLimit(m, max_result);
}
// each supp save complete match
template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool SuppCallbackSaveCompleteMatch(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, ResultContainer *r) {
  if (r) {
    r->emplace_back(m);
  }
  return true;
}
// when supp only contain one vertex,save match vertex
template <class QueryVertexPtr, class TargetVertexPtr, class ResultContainer>
inline bool SuppCallbackSaveSuppPtr(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, QueryVertexPtr supp_ptr,
    ResultContainer *r) {
  if (r) {
    r->emplace_back(m.find(supp_ptr)->second);
  }
  return true;
}
// user call back on save match which contains supp vertex
template <class QueryVertexPtr, class TargetVertexPtr, class SuppList,
          class ResultContainer>
inline bool SuppCallbackSaveSuppMatch(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m, SuppList *supp_list,
    ResultContainer *r) {
  std::map<QueryVertexPtr, TargetVertexPtr> supp_match;
  if (*supp_list) {
    for (const auto &supp : (*supp_list)) {
      supp_match.emplace(supp, m.find(supp)->second);
    }
    if (*r) {
      r->emplace_back(std::move(supp_match));
    }
  }
  return true;
}
// empty update call back
template <class QueryVertexPtr, class TargetVertexPtr>
bool UpdateCandidateCallbackEmpty(
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set) {
  return true;
}
// update call back on erase vertex not on pivot
template <class QueryVertexPtr, class TargetVertexPtr, class Pivot>
inline bool UpdateCallbackEraseNotMatchPatternVertex(
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    Pivot *pivot) {
  for (auto &it : candidate_set) {
    if (pivot->count(it.first)) continue;
    candidate_set.erase(it.first);
  }
  return true;
}
template <class QueryVertexPtr, class TargetVertexPtr>
bool UpdateCandidateCallback(
    std::map<QueryVertexPtr, std::vector<TargetVertexPtr>> &candidate_set,
    QueryVertexPtr *query_vertex_ptr,
    const std::vector<TargetVertexPtr> &possible_candidate) {
  if (possible_candidate.empty()) {
    return true;
  }
  std::vector<TargetVertexPtr> res_candidate;
  auto &query_vertex_candidate = candidate_set[*query_vertex_ptr];
  std::set_intersection(query_vertex_candidate.begin(),
                        query_vertex_candidate.end(),
                        possible_candidate.begin(), possible_candidate.end(),
                        inserter(res_candidate, res_candidate.begin()));
  if (res_candidate.empty()) return false;
  query_vertex_candidate = res_candidate;
  return true;
}

// empty prune call back
template <class QueryVertexPtr, class TargetVertexPtr>
inline bool PruneCallbackEmpty(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m) {
  return false;
}
template <class QueryVertexPtr, class TargetVertexPtr>
inline bool PruneCallBackCheckAttribute(
    const std::map<QueryVertexPtr, TargetVertexPtr> &m) {
  for (const auto &match_pair : m) {
    for (auto attr_it = match_pair.first->AttributeCBegin(); !attr_it.IsDone();
         attr_it++) {
      auto attr_key = attr_it->key();
      auto match_vertex_attributeptr =
          match_pair.second->FindConstAttributePtr(attr_key);
      if (match_vertex_attributeptr->IsNull()) {
        return true;
      }
      if (match_vertex_attributeptr->template const_value<std::string>() !=
          attr_it->template const_value<std::string>()) {
        return true;
      }
    }
  }
  return false;
}
}  // namespace match_helper

}  // namespace GUNDAM

#endif