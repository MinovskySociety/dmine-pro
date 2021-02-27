#include <glog/logging.h>

#include <iostream>
#include <string>

#include "gpar_template/gpar_discover.h"
#include "gpar_template/gpar_new.h"
#include "gundam/large_graph.h"
#include "gundam/large_graph2.h"
#include "gundam/simple_small_graph.h"
#include "gundam/small_graph.h"
#include "yaml-cpp/yaml.h"
int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  std::string config_file_path = "dmine_pro.yaml";
  if (argc == 2) {
    config_file_path = argv[1];
  }
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO, "./");
  LOG(INFO) << config_file_path;
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
  LOG(INFO) << "node_file: " << node_file;
  LOG(INFO) << "edge_file: " << edge_file;
  LOG(INFO) << "output_dir: " << output_dir;
  LOG(INFO) << "x: " << x;
  LOG(INFO) << "y: " << y;
  LOG(INFO) << "q: " << q;
  LOG(INFO) << "supp_limit = " << supp_limit;
  LOG(INFO) << "k: " << top_k;
  LOG(INFO) << "d: " << max_edge;

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
  std::vector<gmine_new::DiscoverdGPAR<Pattern, DataGraph>> root_gpars;
  // read root gpar
  if (config["Root"]) {
    for (const auto& config_root : config["Root"]) {
      if ((!config_root["NodeFile"]) || (!config_root["EdgeFile"])) {
        std::cout << "cannot read root gpar!" << std::endl;
        return 0;
      }
      std::string vertex_file = config_root["NodeFile"].as<std::string>();
      std::string edge_file = config_root["EdgeFile"].as<std::string>();
      Pattern root_pattern;
      GUNDAM::ReadCSVGraph(root_pattern, vertex_file, edge_file);
      typename Pattern::VertexType::IDType x_id = 1, y_id = 2;
      if (config_root["XID"]) {
        x_id = config_root["XID"].as<typename Pattern::VertexType::IDType>();
      }
      if (config_root["YID"]) {
        x_id = config_root["XID"].as<typename Pattern::VertexType::IDType>();
      }
      gmine_new::DiscoverdGPAR<Pattern, DataGraph> root_gpar(root_pattern, x_id,
                                                             y_id, q);
      root_gpars.push_back(root_gpar);
    }
  }
  if (root_gpars.size() != 0)
    gmine_new::DMinePro<Pattern, DataGraph>(
        node_file.c_str(), edge_file.c_str(), x, y, q, supp_limit, max_edge,
        top_k, output_dir.c_str(), &root_gpars);
  else {
    gmine_new::DMinePro<Pattern, DataGraph>(
        node_file.c_str(), edge_file.c_str(), x, y, q, supp_limit, max_edge,
        top_k, output_dir.c_str(), nullptr);
  }
  google::ShutdownGoogleLogging();
  return 0;
}