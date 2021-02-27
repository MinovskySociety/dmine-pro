#include <iostream>
#include <string>

#include "gundam/datatype.h"
#include "gundam/large_graph.h"
#include "yaml-cpp/yaml.h"
int main() {
  YAML::Node config = YAML::LoadFile("./test.yaml");

  std::string node_file = config["NodeFile"].as<std::string>();
  std::string edge_file = config["EdgeFile"].as<std::string>();
  if (config["OutputDir"])
    std::string output_dir = config["OutputDir"].as<std::string>();
  int32_t x = config["NodeXLabel"].as<int32_t>();
  int32_t y = config["NodeYLabel"].as<int32_t>();
  int32_t q = config["EdgeQLabel"].as<int32_t>();
  int32_t supp_limit = config["SuppRLimit"].as<int32_t>();
  int32_t max_edge = config["Radius"].as<int32_t>();
  int32_t top_k = config["TopK"].as<int32_t>();
  std::cout << "node_file = " << node_file << std::endl;
  std::cout << config["test"].size() << std::endl;
  for (const auto &it : config["test"]) {
    std::cout << it["a"].as<std::string>() << std::endl;
  }
  return 0;
}