#ifndef GAR_MATCH_H_
#define GAR_MATCH_H_

#include <map>
#include <vector>

#include "gar.h"
#include "gundam/algorithm/dp_iso.h"

namespace gmine_new {
template <bool check_y_flag, class QueryVertexPtr, class DataGraphVertexPtr,
          class ResultContainer, class LiteralList, class SuppContainer>
inline bool MatchCallbackSaveResultWithCheckXY(
    const std::map<QueryVertexPtr, DataGraphVertexPtr> &m, const LiteralList &x,
    const LiteralList &y, std::set<QueryVertexPtr> &pivot, SuppContainer *supp,
    ResultContainer *r) {
  using ResultType = typename ResultContainer::value_type;
  // must a map!
  if (r) {
    for (const auto &literal_ptr : x) {
      bool satisfy_flag = literal_ptr->Satisfy(m);
      if (!satisfy_flag) return true;
    }
    if (check_y_flag) {
      for (const auto &literal_ptr : y) {
        bool satisfy_flag = literal_ptr->Satisfy(m);
        if (!satisfy_flag) return true;
      }
    }
    ResultType tmp;
    for (const auto &it : pivot) {
      auto find_it = m.find(it);
      tmp.emplace(*find_it);
    }
    supp->emplace(std::move(tmp));
    // GUNDAM::_vf2::CopyMap(tmp, std::move(m));
    // r->emplace_back(std::move(tmp));
    r->emplace_back(m);
  }
  return true;
}

template <class QueryVertexPtr, class DataGraphVertexPtr, class SuppContainer,
          class LiteralList>
inline bool GARPruneCallBack(
    const std::map<QueryVertexPtr, DataGraphVertexPtr> &m,
    std::set<QueryVertexPtr> &pivot, const LiteralList &y, SuppContainer *r) {
  if (r) {
    if (pivot.empty()) return false;
    std::map<QueryVertexPtr, DataGraphVertexPtr> temp;
    for (const auto &it : pivot) {
      auto find_it = m.find(it);
      if (find_it == m.end()) {
        return false;
      }
      temp.insert(std::make_pair(find_it->first, find_it->second));
    }
    // std::cout << "count = " << r->count(temp) << std::endl;
    if (r->count(temp)) return true;
    return false;
  }
  return false;
}

template <class CandidateSetContainer>
inline bool UpdateCallBackEmpty(CandidateSetContainer &candidate_set) {
  return true;
}

template <class CandidateSetContainer, class Pivot>
inline bool GARUpdateCallBack(CandidateSetContainer &candidate_set,
                              Pivot &pivot) {
  for (auto it = candidate_set.begin(); it != candidate_set.end();) {
    if (pivot.count(it->first)) {
      it++;
    } else {
      it = candidate_set.erase(it);
    }
  }
  return true;
}
// cal node in X and Y match
template <bool check_y_flag, class GAR, class DataGraph, class ResultContainer>
int GenXYParticalMatch(const GAR &gar, const DataGraph &target,
                       ResultContainer &match_result) {
  using PatternVertexConstPtr = typename GAR::PatternType::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  // using ResultType = typename ResultContainer::value_type;
  using LiteralSetType = typename GAR::LiteralSetType;
  std::set<PatternVertexConstPtr> pivot;
  using SuppContainerType =
      std::set<std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>>;
  using CandidateSetContainerType =
      std::map<PatternVertexConstPtr, std::vector<DataGraphVertexConstPtr>>;
  std::set<std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>> supp;
  auto &x_literal_set = gar.x_literal_set();
  auto &y_literal_set = gar.y_literal_set();
  for (const auto &literal_ptr : x_literal_set) {
    literal_ptr->CalPivot(pivot);
  }
  for (const auto &literal_ptr : y_literal_set) {
    literal_ptr->CalPivot(pivot);
  }
  // std::cout << "pivot size = " << pivot.size() << std::endl;
  auto user_call_back = std::bind(
      MatchCallbackSaveResultWithCheckXY<
          check_y_flag, PatternVertexConstPtr, DataGraphVertexConstPtr,
          ResultContainer, LiteralSetType, SuppContainerType>,
      std::placeholders::_1, std::ref(x_literal_set), std::ref(y_literal_set),
      std::ref(pivot), &supp, &match_result);
  auto prune_call_back = std::bind(
      GARPruneCallBack<PatternVertexConstPtr, DataGraphVertexConstPtr,
                       SuppContainerType, LiteralSetType>,
      std::placeholders ::_1, std::ref(pivot), std::ref(y_literal_set), &supp);
  auto update_call_back =
      std::bind(GARUpdateCallBack<CandidateSetContainerType,
                                  std::set<PatternVertexConstPtr>>,
                std::placeholders::_1, std::ref(pivot));
  GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      gar.pattern(), target, user_call_back, prune_call_back, update_call_back,
      -1.0);
  std::cout << "xy size = " << match_result.size() << std::endl;
  return 0;
}

template <bool check_y_flag, class GAR, class DataGraph, class ResultContainer>
int CalCompleteMatch(const GAR &gar, const DataGraph &target,
                     ResultContainer &xy_result,
                     ResultContainer &match_result) {
  using PatternVertexConstPtr = typename GAR::PatternType::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  // using ResultType = typename ResultContainer::value_type;
  using LiteralSetType = typename GAR::LiteralSetType;
  std::set<PatternVertexConstPtr> pivot;
  using SuppContainerType =
      std::set<std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>>;
  using CandidateSetContainerType =
      std::map<PatternVertexConstPtr, std::vector<DataGraphVertexConstPtr>>;
  using Pattern = typename GAR::PatternType;
  CandidateSetContainerType candidate_set;
  if (!GUNDAM::_dp_iso::InitCandidateSet<GUNDAM::MatchSemantics::kIsomorphism>(
          gar.pattern(), target, candidate_set)) {
    return 0;
  }
  if (!GUNDAM::_dp_iso::RefineCandidateSet(gar.pattern(), target,
                                           candidate_set)) {
    return 0;
  }
  for (auto &match_state : xy_result) {
    size_t result_count = 0;
    int max_result = 1;
    auto user_callback = std::bind(
        GUNDAM::_vf2::MatchCallbackSaveResult<
            PatternVertexConstPtr, DataGraphVertexConstPtr, ResultContainer>,
        std::placeholders::_1, &max_result, &match_result);
    auto prune_callback =
        std::bind(GUNDAM::_dp_iso::PruneCallbackEmpty<PatternVertexConstPtr,
                                                      DataGraphVertexConstPtr>,
                  std::placeholders::_1);
    auto temp_candidate_set = candidate_set;
    GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
        gar.pattern(), target, temp_candidate_set, match_state, user_callback,
        prune_callback, 600.0);
  }
  return 1;
}

template <bool check_y_flag, class GAR, class DataGraph, class ResultContainer,
          class = typename std::enable_if<std::is_convertible<
              std::pair<typename GAR::PatternType::VertexConstPtr,
                        typename DataGraph::VertexConstPtr>,
              typename ResultContainer::value_type::value_type>::value>::type>
inline int GARMatch(const GAR &gar, const DataGraph &target,
                    ResultContainer &match_result) {
  ResultContainer xy_result;
  GenXYParticalMatch<check_y_flag>(gar, target, xy_result);
  CalCompleteMatch<check_y_flag>(gar, target, xy_result, match_result);
  return static_cast<int>(match_result.size());
}

// using partical match
template <bool check_y_flag, class GAR, class DataGraph, class ResultContainer,
          class = typename std::enable_if<std::is_convertible<
              std::pair<typename GAR::PatternType::VertexConstPtr,
                        typename DataGraph::VertexConstPtr>,
              typename ResultContainer::value_type::value_type>::value>::type>
inline int GARMatch(const GAR &gar, const DataGraph &target,
                    typename ResultContainer::value_type &match_state,
                    ResultContainer &match_result) {
  using PatternVertexConstPtr = typename GAR::PatternType::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  // using ResultType = typename ResultContainer::value_type;
  using LiteralSetType = typename GAR::LiteralSetType;
  match_result.clear();
  std::set<PatternVertexConstPtr> pivot;
  using SuppContainerType =
      std::set<std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>>;
  using CandidateSetContainerType =
      std::map<PatternVertexConstPtr, std::vector<DataGraphVertexConstPtr>>;
  std::set<std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>> supp;
  auto &x_literal_set = gar.x_literal_set();
  auto &y_literal_set = gar.y_literal_set();
  for (const auto &it : y_literal_set) {
    it->CalPivot(pivot);
  }
  auto user_call_back = std::bind(
      MatchCallbackSaveResultWithCheckXY<
          check_y_flag, PatternVertexConstPtr, DataGraphVertexConstPtr,
          ResultContainer, LiteralSetType, SuppContainerType>,
      std::placeholders::_1, std::ref(x_literal_set), std::ref(y_literal_set),
      pivot, &supp, &match_result);
  auto prune_call_back =
      std::bind(GARPruneCallBack<PatternVertexConstPtr, DataGraphVertexConstPtr,
                                 SuppContainerType, LiteralSetType>,
                std::placeholders ::_1, pivot, std::ref(y_literal_set), &supp);
  auto update_call_back = std::bind(
      UpdateCallBackEmpty<CandidateSetContainerType>, std::placeholders::_1);

  CandidateSetContainerType candidate_set;
  bool flag =
      GUNDAM::_dp_iso::InitCandidateSet<GUNDAM::MatchSemantics::kIsomorphism,
                                        typename GAR::PatternType, DataGraph>(
          gar.pattern(), target, candidate_set);
  if (!flag) return 0;
  flag =
      GUNDAM::_dp_iso::RefineCandidateSet(gar.pattern(), target, candidate_set);
  if (!flag) return 0;
  std::set<DataGraphVertexConstPtr> target_matched;
  for (const auto &match_pair : match_state) {
    target_matched.insert(match_pair.second);
  }
  size_t result_count = 0;
  GUNDAM::_dp_iso::_DPISO<GUNDAM::MatchSemantics::kIsomorphism,
                          typename GAR::PatternType, DataGraph>(
      candidate_set, match_state, target_matched, result_count, user_call_back,
      prune_call_back, clock());
  // GUNDAM::VF2_Label_Equal<GUNDAM::MatchSemantics::kIsomorphism>(
  //    gar.pattern(), target, user_call_back, prune_call_back,
  //    update_call_back);
  // GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(gar.pattern(), target,
  // -1);

  return static_cast<int>(match_result.size());
}
};  // namespace gmine_new

#endif  // GAR_MATCH_H_