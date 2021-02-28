//#include "gundam/boost_iso.h"
#include "gundam/algorithm/dp_iso.h"
#include "gundam/io/csvgraph.h"
//#include "gundam/graph.h"
#include <map>
#include <vector>

#include "gundam/algorithm/vf2_boost.h"
#include "gundam/graph_type/large_graph.h"

int main() {
  using namespace GUNDAM;

  using GraphType = LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                               uint32_t, std::string>;
  GraphType pattern, data_graph;
  using VertexConstPtr = typename GraphType::VertexConstPtr;
  using VertexIDType = typename GraphType::VertexType::IDType;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  using QueryVertexPtr = VertexConstPtr;
  using TargetVertexPtr = VertexConstPtr;
  using MatchMap = std::map<QueryVertexPtr, TargetVertexPtr>;
  using MatchContainer = std::vector<MatchMap>;
  using CandidateSetContainer =
      std::map<QueryVertexPtr, std::vector<TargetVertexPtr>>;
  MatchContainer partial_match_container, complete_match_container;
  CandidateSetContainer candidate_set;
  if (!GUNDAM::_dp_iso::InitCandidateSet<GUNDAM::MatchSemantics::kIsomorphism>(
          pattern, data_graph, candidate_set)) {
    //... exit
  }
  if (!GUNDAM::_dp_iso::RefineCandidateSet(pattern, data_graph,
                                           candidate_set)) {
    //... exit
  }

  for (auto &match_state : partial_match_container) {
    auto user_callback = [&complete_match_container](MatchMap &match_state) {
      complete_match_container.emplace_back(match_state);
      return true;
    };
    auto prune_callback = [](MatchMap &match_state) { return false; };
    GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
        pattern, data_graph, candidate_set, match_state, user_callback,
        prune_callback);
  }
  return 0;
}