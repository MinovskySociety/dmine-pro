#ifndef GAR_CHASE_H_
#define GAR_CHASE_H_

#include <map>
#include <vector>

#include "gar.h"
#include "gar_match.h"
//#include "literalset.h"

namespace gmine_new {

template <class GAR, class DataGraph, class EdgeIDGen>
int GARChase(
    const std::vector<GAR> &gar_list, DataGraph &data_graph,
    EdgeIDGen &edge_id_gen,
    std::set<typename DataGraph::VertexType::IDType> *diff_vertex_set = nullptr,
    std::set<typename DataGraph::EdgeType::IDType> *diff_edge_set = nullptr) {
  using PatternVertexConstPtr = typename GAR::PatternVertexConstPtr;
  using DataGraphVertexConstPtr = typename GAR::DataGraphVertexConstPtr;
  using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;

  int count = 0;
  bool update_flag;
  do {
    update_flag = false;
    for (auto &gar : gar_list) {
      MatchResultList match_result;
      int res = GARMatch<false>(gar, data_graph, match_result);
      if (res < 0) return res;

      auto &y_literal_set = gar.y_literal_set();
      for (auto &single_literal_ptr : y_literal_set) {
        for (auto &single_match_result : match_result) {
          if (single_literal_ptr->Satisfy(single_match_result)) {
            continue;
          }
          bool r = single_literal_ptr->Update(single_match_result, data_graph,
                                              edge_id_gen, diff_vertex_set,
                                              diff_edge_set);

          update_flag |= r;
          if (r) ++count;
        }
      }
    }
  } while (update_flag);

  return count;
}
};      // namespace gmine_new
#endif  // GAR_CHASE_H_