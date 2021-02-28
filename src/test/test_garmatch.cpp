#include <gundam/graph_type/large_graph.h>
#include <gundam/graph_type/small_graph.h>
#include <gundam/io/csvgraph.h>
#include <gundam/match/matchresult.h>

#include <cstring>
#include <fstream>
#include <iostream>

#include "../gpar/gpar.h"
#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar/gar_match.h"
#include "yaml-cpp/yaml.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  char config_file_path[1500] = "./gar_match_config.yaml";
  if (argc == 2) {
    strcpy(config_file_path, argv[1]);
  }
  std::ifstream config_file(config_file_path);
  if (!config_file.is_open()) {
    std::cout << "cannot open config file." << std::endl;
    return 0;
  }
  YAML::Node config = YAML::LoadFile(config_file_path);

  // get graph
  if (!config["Graph"]) {
    std::cout << "cannot get graph!" << std::endl;
    return 0;
  }
  if (!config["Graph"]["Dir"]) {
    std::cout << " cannot get graph dir!" << std::endl;
    return 0;
  }
  std::string graph_dir = config["Graph"]["Dir"].as<std::string>();
  if (!config["Graph"]["VertexFile"]) {
    std::cout << "cannot get vertex file!" << std::endl;
  }
  std::vector<std::string> vertex_file_list;
  for (const auto& it : config["Graph"]["VertexFile"]) {
    std::string v_file = graph_dir + it.as<std::string>();
    vertex_file_list.emplace_back(std::move(v_file));
  }
  if (!config["Graph"]["EdgeFile"]) {
    std::cout << "cannot get edge file!" << std::endl;
  }
  std::vector<std::string> edge_file_list;
  for (const auto& it : config["Graph"]["EdgeFile"]) {
    std::string e_file = graph_dir + it.as<std::string>();
    edge_file_list.emplace_back(std::move(e_file));
  }

  // get gar
  if (!config["GAR"]) {
    std::cout << "cannot get GAR!" << std::endl;
    return 0;
  }
  auto gar_node = config["GAR"];
  if (!gar_node["Dir"]) {
    std::cout << "cannot get GAR dir!" << std::endl;
    return 0;
  }
  std::string gar_dir = gar_node["Dir"].as<std::string>();

  if (!gar_node["Name"]) {
    std::cout << "cannot get GAR name!" << std::endl;
    return 0;
  }
  std::string gar_name = gar_node["Name"].as<std::string>();
  if (!gar_node["VertexFile"]) {
    std::cout << "cannot get gar vertex file!" << std::endl;
    return 0;
  }
  std::string gar_vertex_file =
      gar_dir + gar_node["VertexFile"].as<std::string>();

  if (!gar_node["EdgeFile"]) {
    std::cout << "cannot get gar edge file!" << std::endl;
    return 0;
  }
  std::string gar_edge_file = gar_dir + gar_node["EdgeFile"].as<std::string>();

  if (!gar_node["LiteralX"]) {
    std::cout << "cannot get gar literal x!" << std::endl;
    return 0;
  }
  std::string gar_literal_x_file =
      gar_dir + gar_node["LiteralX"].as<std::string>();

  if (!gar_node["LiteralY"]) {
    std::cout << "cannot get gar literal y!" << std::endl;
    return 0;
  }
  std::string gar_literal_y_file =
      gar_dir + gar_node["LiteralY"].as<std::string>();

  // get dir
  if (!config["ResultDir"]) {
    std::cout << "cannot get result dir!" << std::endl;
    return 0;
  }
  std::string result_dir = config["ResultDir"].as<std::string>();

  using Pattern = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                     uint32_t, std::string>;
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  using PatternVertexConstPtr = typename Pattern::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  gmine_new::GraphAssociationRule<Pattern, DataGraph> gar;
  gmine_new::ReadGAR(gar, gar_vertex_file.c_str(), gar_edge_file.c_str(),
                     gar_literal_x_file.c_str(), gar_literal_y_file.c_str());
  DataGraph data_graph;
  GUNDAM::ReadCSVGraph(data_graph, vertex_file_list, edge_file_list);
  using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  MatchResultList match_result;
  std::map<PatternVertexConstPtr, DataGraphVertexConstPtr> match_state;
  match_state.emplace(gar.pattern().FindConstVertex(1),
                      data_graph.FindConstVertex(6));
  match_state.emplace(gar.pattern().FindConstVertex(2),
                      data_graph.FindConstVertex(7));
  int count =
      gmine_new::GARMatch<true>(gar, data_graph, match_state, match_result);
  if (count < 0) {
    std::cout << "Match Error!" << std::endl;
    return count;
  }
  std::cout << "Match count = " << count << std::endl;
  std::string result = result_dir + gar_name + "_match.csv";
  std::ofstream result_file(result);
  MatchResultToFile<gmine_new::VertexIDType, gmine_new::VertexIDType>(
      match_result, result_file);
  std::cout << "out end!" << std::endl;
  return 0;
}
