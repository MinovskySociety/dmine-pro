#include "mcts/multimgm.h"

#include <mpi.h>
#include <yaml-cpp/yaml.h>

#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/small_graph.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  std::string config_file_path = "multimgm.yaml";
  if (argc == 2) {
    config_file_path = argv[1];
  }
  using Pattern = GUNDAM::SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;
  using DataGraph = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string,
                                        uint64_t, uint32_t, std::string>;
  using PatternVertexLabelType = Pattern::VertexType::LabelType;
  using PatternEdgeLabelType = Pattern::EdgeType::LabelType;
  std::ifstream config_file(config_file_path);
  if (!config_file.is_open()) {
    std::cout << "cannot open config file." << std::endl;
    return 0;
  }
  YAML::Node config = YAML::LoadFile(config_file_path);

  // Input Graph
  if (!config["Graph"]["Name"]) {
    std::cout << "cannot get input graph name!" << std::endl;
    return 0;
  }
  std::string input_graph_name = config["Graph"]["Name"].as<std::string>();

  if (!config["Graph"]["Dir"]) {
    std::cout << "cannot get input graph dir!" << std::endl;
    return 0;
  }
  std::string input_graph_dir = config["Graph"]["Dir"].as<std::string>();

  if (!config["Graph"]["VertexFile"]) {
    std::cout << "cannot get input graph vertex file!" << std::endl;
    return 0;
  }
  std::string input_graph_vertex_file =
      config["Graph"]["VertexFile"].as<std::string>();

  if (!config["Graph"]["EdgeFile"]) {
    std::cout << "cannot get input graph edge file!" << std::endl;
    return 0;
  }
  std::string input_graph_edge_file =
      config["Graph"]["EdgeFile"].as<std::string>();

  // MGM
  if (!config["MGM"]["Supp"]) {
    std::cout << "cannot get support!" << std::endl;
    return 0;
  }
  int32_t supp = config["MGM"]["Supp"].as<int32_t>();

  if (!config["MGM"]["Conf"]) {
    std::cout << "cannot get confidence!" << std::endl;
    return 0;
  }
  double conf = config["MGM"]["Conf"].as<double>();

  if (!config["MGM"]["EdgeSize"]) {
    std::cout << "cannot get the edge size of gpar!" << std::endl;
    return 0;
  }
  int32_t edgesize = config["MGM"]["EdgeSize"].as<int32_t>();

  if (!config["MGM"]["TopK"]) {
    std::cout << "cannot get the num of top-k!" << std::endl;
    return 0;
  }
  int32_t topk = config["MGM"]["TopK"].as<int32_t>();

  if (!config["MGM"]["SrcNodeLabel"]) {
    std::cout << "cannot get the src node label!" << std::endl;
    return 0;
  }
  PatternVertexLabelType srcnodelabel =
      config["MGM"]["SrcNodeLabel"].as<PatternVertexLabelType>();

  if (!config["MGM"]["DstNodeLabel"]) {
    std::cout << "cannot get the dst node label!" << std::endl;
    return 0;
  }
  PatternVertexLabelType dstnodelabel =
      config["MGM"]["DstNodeLabel"].as<PatternVertexLabelType>();

  if (!config["MGM"]["EdgeLabel"]) {
    std::cout << "cannot get the edge label!" << std::endl;
    return 0;
  }
  PatternEdgeLabelType edgelabel =
      config["MGM"]["EdgeLabel"].as<PatternEdgeLabelType>();

  // Output Graph
  if (!config["OutputGraph"]["Dir"]) {
    std::cout << "cannot get the dir of output graph!" << std::endl;
    return 0;
  }
  std::string output_graph_dir = config["OutputGraph"]["Dir"].as<std::string>();

  std::cout << config << std::endl;
  std::string node_file = input_graph_dir + input_graph_vertex_file;
  std::string edge_file = input_graph_dir + input_graph_edge_file;

  gmine_new::MultiMGM<Pattern, DataGraph>(
      argc, argv, node_file.c_str(), edge_file.c_str(), srcnodelabel,
      dstnodelabel, edgelabel, supp, conf, topk, edgesize,
      output_graph_dir.c_str());
  return 0;
}