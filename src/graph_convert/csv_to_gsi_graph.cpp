#include <fstream>
#include <map>
#include <set>

#include "gundam/graph_type/large_graph.h"
#include "gundam/io/csvgraph.h"
#include "gundam/io/rapidcsv.h"
#include "yaml-cpp/yaml.h"
template <class Graph>
bool GetGraphAndDict(YAML::Node &config, Graph &graph) {
  if (!config["Graph"]) {
    return false;
  }
  auto config_graph = config["Graph"];
  if (!config_graph["Name"] || !config_graph["Dir"] ||
      !config_graph["VertexFile"] || !config_graph["EdgeFile"]) {
    return false;
  }
  std::string csv_dir = config_graph["Dir"].as<std::string>();
  std::vector<std::string> vertex_file_list, edge_file_list;
  for (const auto &vertex_file : config_graph["VertexFile"]) {
    std::string v_file = csv_dir + vertex_file.as<std::string>();
    vertex_file_list.emplace_back(std::move(v_file));
  }
  for (const auto &edge_file : config_graph["EdgeFile"]) {
    std::string e_file = csv_dir + edge_file.as<std::string>();
    edge_file_list.emplace_back(std::move(e_file));
  }
  GUNDAM::ReadCSVGraph(graph, vertex_file_list, edge_file_list);
  return true;
}
template <class DataGraph>
bool ConvertGraphToGSIGraph(YAML::Node &config, DataGraph &data_graph) {
  if (!config["GSIGraph"]) {
    return false;
  }
  auto gsi_config = config["GSIGraph"];
  if (!gsi_config["Dir"] || !gsi_config["Name"]) {
    return false;
  }
  std::string gsi_dir = gsi_config["Dir"].as<std::string>();
  std::string gsi_file_name = gsi_config["Name"].as<std::string>();
  std::ofstream output_gsi((gsi_dir + gsi_file_name).c_str());
  std::ofstream output_vertex_label_map((gsi_dir + "vertexmap.txt").c_str());
  std::ofstream output_edge_label_map((gsi_dir + "edgemap.txt").c_str());
  using VertexLabelType = typename DataGraph::VertexType::LabelType;
  using VertexIDType = typename DataGraph::VertexType::IDType;
  using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
  using VertexLabelMap = std::map<VertexLabelType, VertexLabelType>;
  using EdgeLabelMap = std::map<EdgeLabelType, EdgeLabelType>;
  using VertexLabelContainer = std::set<VertexLabelType>;
  using EdgeLabelContainer = std::set<EdgeLabelType>;
  VertexLabelContainer vertex_label_set;
  EdgeLabelContainer edge_label_set;
  VertexLabelMap vertex_label_map;
  EdgeLabelMap edge_label_map;
  std::set<VertexIDType> vertex_id_set;
  VertexIDType vertex_id_counter = 0;
  std::map<VertexIDType, VertexIDType> vertex_id_map;
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    vertex_label_set.insert(vertex_it->label());
    vertex_id_set.insert(vertex_it->id());
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      edge_label_set.insert(edge_it->label());
    }
  }
  VertexLabelType vertex_label_counter = 0;
  EdgeLabelType edge_label_counter = 0;
  for (const auto &vertex_label : vertex_label_set) {
    vertex_label_map.emplace(vertex_label, ++vertex_label_counter);
    output_vertex_label_map << vertex_label << " " << vertex_label_counter
                            << std::endl;
  }
  for (const auto &edge_label : edge_label_set) {
    edge_label_map.emplace(edge_label, ++edge_label_counter);
    output_edge_label_map << edge_label << " " << edge_label_counter
                          << std::endl;
  }
  for (const auto &vertex_id : vertex_id_set) {
    vertex_id_map.emplace(vertex_id, vertex_id_counter++);
  }
  std::set<std::tuple<int, int, int>> edge_set;
  int edge_sum = 0;
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_set.count(
              {edge_it->src_id(), edge_it->dst_id(), edge_it->label()})) {
        continue;
      }
      edge_sum++;
      edge_set.insert({edge_it->src_id(), edge_it->dst_id(), edge_it->label()});
    }
  }
  output_gsi << "t # 0\n";
  output_gsi << data_graph.CountVertex() << " " << edge_sum << " "
             << vertex_label_counter << " " << edge_label_counter << std::endl;
  edge_set.clear();
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    output_gsi << "v " << vertex_id_map[vertex_it->id()] << " "
               << vertex_label_map[vertex_it->label()] << std::endl;
  }

  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      if (edge_set.count(
              {edge_it->src_id(), edge_it->dst_id(), edge_it->label()})) {
        continue;
      }
      output_gsi << "e " << vertex_id_map[edge_it->src_id()] << " "
                 << vertex_id_map[edge_it->dst_id()] << " "
                 << edge_label_map[edge_it->label()] << std::endl;
      edge_set.insert({edge_it->src_id(), edge_it->dst_id(), edge_it->label()});
    }
  }

  output_gsi << "t # -1\n";
  return true;
}
int main() {
  const char *config_file_path = "./csv_to_gsi_graph.yaml";
  YAML::Node config = YAML::LoadFile(config_file_path);
  using Graph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                   uint32_t, std::string>;
  using VertexLabelType = typename Graph::VertexType::LabelType;
  using EdgeLabelType = typename Graph::EdgeType::LabelType;
  using LabelDict = std::map<VertexLabelType, std::string>;
  Graph graph;
  // read csv graph
  bool get_graph_flag = GetGraphAndDict(config, graph);
  if (!get_graph_flag) return 0;
  std::cout << "read end!" << std::endl;
  ConvertGraphToGSIGraph(config, graph);
  std::cout << "convert end!" << std::endl;
  return 0;
}