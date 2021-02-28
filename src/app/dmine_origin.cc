#include <yaml-cpp/yaml.h>

#include "gpar_template/gpar_origin.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/small_graph.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  std::string config_file_path = "dmine_origin.yaml";
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
  if (!config["NodeFile"]) {
    std::cout << "cannot get node file!" << std::endl;
    return 0;
  }
  std::string node_file = config["NodeFile"].as<std::string>();
  if (!config["EdgeFile"]) {
    std::cout << "cannot get edge file!" << std::endl;
    return 0;
  }
  std::string edge_file = config["EdgeFile"].as<std::string>();
  if (!config["OutputDir"]) {
    std::cout << "cannot get output dir!" << std::endl;
    return 0;
  }
  std::string output_dir = config["OutputDir"].as<std::string>();
  if (!config["NodeXLabel"]) {
    std::cout << "cannot get x!" << std::endl;
    return 0;
  }
  PatternVertexLabelType x = config["NodeXLabel"].as<PatternVertexLabelType>();
  if (!config["NodeYLabel"]) {
    std::cout << "cannot get y!" << std::endl;
    return 0;
  }
  PatternVertexLabelType y = config["NodeYLabel"].as<PatternVertexLabelType>();
  if (!config["EdgeQLabel"]) {
    std::cout << "cannot get q!" << std::endl;
    return 0;
  }
  PatternEdgeLabelType q = config["EdgeQLabel"].as<PatternEdgeLabelType>();
  if (!config["SuppRLimit"]) {
    std::cout << "cannot get supp_r_limit!" << std::endl;
    return 0;
  }
  int32_t supp_limit = config["SuppRLimit"].as<int32_t>();
  if (!config["Radius"]) {
    std::cout << "cannot get d!" << std::endl;
    return 0;
  }
  int32_t max_edge = config["Radius"].as<int32_t>();
  if (!config["TopK"]) {
    std::cout << "cannot get k!" << std::endl;
    return 0;
  }
  int32_t top_k = config["TopK"].as<int32_t>();

  std::cout << "node_file: " << node_file << std::endl;
  std::cout << "edge_file: " << edge_file << std::endl;
  std::cout << "output_dir: " << output_dir << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
  std::cout << "q: " << q << std::endl;
  std::cout << "supp_limit = " << supp_limit << std::endl;
  std::cout << "k: " << top_k << std::endl;
  std::cout << "d: " << max_edge << std::endl;
  std::cout << std::endl;

  gmine_new::DMineOrigin<Pattern, DataGraph>(
      node_file.c_str(), edge_file.c_str(), x, y, q, supp_limit, top_k,
      max_edge, output_dir.c_str());
  return 0;
}