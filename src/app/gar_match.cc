#include "gar/gar_match.h"

#include <gundam/graph_type/large_graph.h>
#include <gundam/graph_type/small_graph.h>
#include <gundam/io/csvgraph.h>
#include <gundam/match/matchresult.h>
#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar_config.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  std::string config_file_path = "gar_match_config.yaml";
  if (argc == 2) {
    config_file_path = argv[1];
  }

  int res;

  gar::GARMatchInfo gar_match_info;
  res = ReadGARMatchInfoYAML(config_file_path, gar_match_info);
  if (res < 0) return res;

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

  GAR gar;
  res = gmine_new::ReadGAR(gar, gar_match_info.gar_list[0].vertex_file,
                           gar_match_info.gar_list[0].edge_file,
                           gar_match_info.gar_list[0].literal_x_file,
                           gar_match_info.gar_list[0].literal_y_file);
  if (res < 0) {
    std::cout << "Read GAR error: " << res << std::endl;
    return res;
  }

  DataGraph data_graph;
  res = GUNDAM::ReadCSVGraph(data_graph, gar_match_info.graph.vertex_file_list,
                             gar_match_info.graph.edge_file_list);
  if (res < 0) {
    std::cout << "Read CSV graph error: " << res << std::endl;
    return res;
  }

  std::cout << "Matching..." << std::endl;
  auto time_begin = clock();
  MatchResultList match_result;
  res = gmine_new::GARMatch<false>(gar, data_graph, match_result);
  auto time_end = clock();
  std::cout << "Match time: " << (1.0 * time_end - time_begin) / CLOCKS_PER_SEC
            << std::endl;
  if (res < 0) {
    std::cout << "Match error: " << res << std::endl;
    return res;
  }
  std::cout << "Match count = " << res << std::endl;

  std::string result = gar_match_info.result_dir +
                       gar_match_info.gar_list[0].name + "_match.csv";
  std::ofstream result_file(result);
  MatchResultToFile<PatternVertexID, DataGraphVertexID>(match_result,
                                                        result_file);
  std::cout << "out end!" << std::endl;

  return 0;
}