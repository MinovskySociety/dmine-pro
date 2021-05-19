#include <fstream>
#include <map>

#include "gundam/graph_type/large_graph.h"
#include "gundam/io/csvgraph.h"
#include "gundam/io/rapidcsv.h"
#include "gundam/type_getter/vertex_handle.h"
#include "yaml-cpp/yaml.h"
template <class Graph, class LabelDict>
bool GetGraphAndDict(YAML::Node &config, Graph &graph, LabelDict &label_dict) {
  if (!config["Graph"]) {
    return false;
  }
  auto config_graph = config["Graph"];
  if (!config_graph["Name"] || !config_graph["Dir"] ||
      !config_graph["VertexFile"] || !config_graph["EdgeFile"] ||
      !config_graph["Dict"]) {
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
  std::string dict_file = config_graph["Dict"].as<std::string>();
  rapidcsv::Document doc((csv_dir + dict_file).c_str(),
                         rapidcsv::LabelParams(0, -1));
  size_t col_size = doc.GetRowCount();
  using LabelType = typename Graph::VertexType::LabelType;
  for (size_t i = 0; i < col_size; i++) {
    // std::cout << doc.GetCell<LabelType>(0, i) << " "
    //          << doc.GetCell<std::string>(1, i) << std::endl;
    LabelType label = doc.GetCell<LabelType>(0, i);
    std::string dict = doc.GetCell<std::string>(1, i);
    label_dict.emplace(label, dict);
  }
  return true;
}
template <class Graph, class LabelDict>
bool ConvertGraphToRDF(YAML::Node &config, Graph &graph,
                       LabelDict &label_dict) {
  if (!config["RDF"]) {
    return false;
  }
  auto rdf_config = config["RDF"];
  if (!rdf_config["Dir"] || !rdf_config["Name"]) {
    return false;
  }
  std::string rdf_dir = rdf_config["Dir"].as<std::string>();
  std::string rdf_file_name = rdf_config["Name"].as<std::string>();
  std::ofstream output_rdf((rdf_dir + rdf_file_name).c_str());
  output_rdf << "# filename: " << rdf_file_name << std::endl;
  output_rdf << std::endl;
  output_rdf << "@prefix pre: <liantongdata/> ." << std::endl;
  output_rdf << std::endl;
  using VertexType = typename Graph::VertexType;
  using VertexIDType = typename VertexType::IDType;
  using VertexLabelType = typename VertexType::LabelType;
  using EdgeLabelType = typename Graph::EdgeType::LabelType;
  using VertexConstPtr = typename GUNDAM::VertexHandle<Graph>::type;
  for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone(); vertex_it++) {
    // std::cout << "id = " << vertex_it->id() << std::endl;
    // std::cout << "label = " << vertex_it->label() << std::endl;
    VertexIDType src_id = vertex_it->id();
    VertexLabelType src_label = vertex_it->label();
    // std::cout << "flag = " << label_dict.count(src_label) << std::endl;
    std::string src_name = "pre:vertex_" + std::to_string(src_id);
    output_rdf << src_name << " "
               << "pre:" + label_dict.find(src_label)->second << " "
               << "\""
               << "has label"
               << "\""
               << " ." << std::endl;
    for (auto edge_it = vertex_it->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      EdgeLabelType edge_label = edge_it->label();
      VertexConstPtr dst_ptr = edge_it->dst_handle();
      VertexIDType dst_id = dst_ptr->id();
      VertexLabelType dst_label = dst_ptr->label();
      std::string dst_name = "pre:vertex_" + std::to_string(dst_id);
      std::string edge_label_name = label_dict.find(edge_label)->second;
      output_rdf << src_name << " " << dst_name << " "
                 << "\"" << edge_label_name << "\""
                 << " ." << std::endl;
    }
  }
  output_rdf.close();
  return true;
}
int main() {
  const char *config_file_path = "./csv_to_rdf.yaml";
  YAML::Node config = YAML::LoadFile(config_file_path);
  using Graph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                   uint32_t, std::string>;
  using VertexLabelType = typename Graph::VertexType::LabelType;
  using EdgeLabelType = typename Graph::EdgeType::LabelType;
  using LabelDict = std::map<VertexLabelType, std::string>;
  Graph graph;
  LabelDict label_dict;
  // read csv graph and label dict
  bool get_graph_flag = GetGraphAndDict(config, graph, label_dict);
  if (!get_graph_flag) return 0;
  std::cout << "read end!" << std::endl;
  ConvertGraphToRDF(config, graph, label_dict);
  std::cout << "convert end!" << std::endl;
  return 0;
}