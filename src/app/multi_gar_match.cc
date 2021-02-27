#include <gundam/csvgraph.h>
#include <gundam/large_graph.h>
#include <gundam/matchresult.h>
#include <gundam/small_graph.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar/gar_match.h"
#include "gar_config.h"

int main() {
  //需要匹配的GAR的数量
  constexpr int gar_num = 1000;
  using Pattern = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                     uint32_t, std::string>;
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using DataGraphVertexID = typename DataGraph::VertexType::IDType;
  using PatternVertexConstPtr = typename Pattern::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  using GAR = gmine_new::GraphAssociationRule<Pattern, DataGraph>;
  using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  DataGraph data_graph;
  // read csv data graph
  auto res = GUNDAM::ReadCSVGraph(data_graph, "", "");
  if (res < 0) {
    std::cout << "Read CSV graph error: " << res << std::endl;
    return res;
  }
  for (int i = 0; i < gar_num; i++) {
    MatchResultList match_result;
    GAR gar;
    std::string gar_vertex_file = "";
    std::string gar_edge_file = "";
    std::string literal_x_file = "";
    std::string literal_y_file = "";
    auto res = gmine_new::ReadGAR(gar, gar_vertex_file, gar_edge_file,
                                  literal_x_file, literal_y_file);
    std::cout << "GAR " << i << " begin cal!" << std::endl;
    // cal satisfy Q[x] and X's match
    res = gmine_new::GARMatch<false>(gar, data_graph, match_result);
    std::cout << "satisfy Q[x] and X's match number is " << match_result.size()
              << std::endl;
    // cal satisfy Q[x] ,X and Y's match
    match_result.clear();
    res = gmine_new::GARMatch<true>(gar, data_graph, match_result);
    std::cout << "satisfy Q[x] , X and Y's match number is "
              << match_result.size() << std::endl;
  }
  return 0;
}