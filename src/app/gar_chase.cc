#include "gar/gar_chase.h"

#include <gundam/csvgraph.h>
#include <gundam/large_graph.h>
#include <gundam/large_graph2.h>
#include <gundam/matchresult.h>
#include <gundam/small_graph.h>
#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar/gar_match.h"
#include "gar_config.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  std::string config_file_path = "gar_chase.yaml";
  if (argc == 2) {
    config_file_path = argv[1];
  }

  int res;

  gar::GARChaseInfo gar_chase_info;
  res = ReadGARChaseInfoYAML(config_file_path, gar_chase_info);
  if (res < 0) return res;

  using Pattern = GUNDAM::SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;
  using DataGraph = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string,
                                        uint64_t, uint32_t, std::string>;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using DataGraphVertexID = typename DataGraph::VertexType::IDType;
  using PatternVertexConstPtr = typename Pattern::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  using GAR = gmine_new::GraphAssociationRule<Pattern, DataGraph>;
  using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;

  std::vector<GAR> gar_list;
  for (const auto& gar_info : gar_chase_info.gar_list) {
    GAR gar;
    res = gmine_new::ReadGAR(gar, gar_info.vertex_file, gar_info.edge_file,
                             gar_info.literal_x_file, gar_info.literal_y_file);
    if (res < 0) {
      std::cout << "Read GAR error: " << res << std::endl;
      return res;
    }
    gar_list.emplace_back(std::move(gar));
  }

  DataGraph data_graph;
  GUNDAM::SimpleArithmeticIDGenerator<typename DataGraph::VertexType::IDType>
      vertex_id_gen;
  GUNDAM::SimpleArithmeticIDGenerator<typename DataGraph::EdgeType::IDType>
      edge_id_gen;
  res = GUNDAM::ReadCSVGraph(data_graph, gar_chase_info.graph.vertex_file_list,
                             gar_chase_info.graph.edge_file_list, vertex_id_gen,
                             edge_id_gen);
  if (res < 0) {
    std::cout << "Read CSV graph error: " << res << std::endl;
    return res;
  }
  std::set<typename DataGraph::VertexType::IDType> diff_vertex_attr_set;
  std::set<typename DataGraph::EdgeType::IDType> diff_edge_set;

  auto time_begin = clock();
  MatchResultList match_result;
  res = gmine_new::GARChase(gar_list, data_graph, edge_id_gen,
                            &diff_vertex_attr_set, &diff_edge_set);
  auto time_end = clock();
  std::cout << "Chase time: " << (1.0 * time_end - time_begin) / CLOCKS_PER_SEC
            << std::endl;
  if (res < 0) {
    std::cout << "Chase error: " << res << std::endl;
    return res;
  }
  std::cout << "Chase count = " << res << std::endl;

  res = GUNDAM::WriteCSVGraph(data_graph,
                              gar_chase_info.result_graph.vertex_file_list[0],
                              gar_chase_info.result_graph.edge_file_list[0]);
  std::string diff_vertex_csv =
      gar_chase_info.result_graph.dir + "diff_vertex.csv";
  std::string diff_edge_csv = gar_chase_info.result_graph.dir + "diff_edge.csv";
  GUNDAM::WriteCSVParticalGraph(data_graph, diff_vertex_csv, diff_edge_csv,
                                diff_vertex_attr_set, diff_edge_set);
  /*
  std::ofstream diff_vertex_stream(diff_vertex_csv);
  std::ofstream diff_edge_stream(diff_edge_csv);
  diff_vertex_stream << "id,key,value" << std::endl;
  for (const auto& it : diff_vertex_attr_set) {
    diff_vertex_stream << it.first->id() << "," << it.second << ","
                       << it.first->FindConstAttributePtr(it.second)
                              ->template const_value<std::string>()
                       << std::endl;
  }
  diff_edge_stream << "id,src_id,dst_id,label" << std::endl;
  for (const auto& it : diff_edge_set) {
    diff_edge_stream << it->id() << "," << it->src_id() << "," << it->dst_id()
                     << "," << it->label() << std::endl;
  }
  */
  if (res < 0) {
    std::cout << "Write CSV graph error: " << res << std::endl;
    return res;
  }

  return 0;
}
