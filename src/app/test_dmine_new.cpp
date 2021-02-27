#include <glog/logging.h>

#include <iostream>
#include <string>

#include "gpar_template/gpar_discover.h"
#include "gpar_template/gpar_new.h"
#include "gundam/large_graph.h"
int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  char config_file_path[1500] = "dmine_config.txt";
  if (argc == 2) {
    strcpy(config_file_path, argv[1]);
  }
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO, "./");
  LOG(INFO) << config_file_path;
  std::ifstream config_file(config_file_path);
  if (!config_file.is_open()) {
    std::cout << "cannot open config file." << std::endl;
    return 0;
  }
  int x, y, q, k, d;
  std::string v_file_path, e_file_path, output_dir;
  std::getline(config_file, v_file_path);
  if (v_file_path.back() == '\r') v_file_path.pop_back();
  std::getline(config_file, e_file_path);
  if (e_file_path.back() == '\r') e_file_path.pop_back();
  std::getline(config_file, output_dir);
  if (output_dir.back() == '\r') output_dir.pop_back();
  int32_t supp_limit;
  config_file >> x >> y >> q;
  config_file >> supp_limit;
  config_file >> k;
  config_file >> d;
  config_file.close();

  LOG(INFO) << "node_file: " << v_file_path;
  LOG(INFO) << "edge_file: " << e_file_path;
  LOG(INFO) << "output_dir: " << output_dir;
  LOG(INFO) << "x: " << x;
  LOG(INFO) << "y: " << y;
  LOG(INFO) << "q: " << q;
  LOG(INFO) << "supp_limit = " << supp_limit;
  LOG(INFO) << "k: " << k;
  LOG(INFO) << "d: " << d;

  std::cout << "node_file: " << v_file_path << std::endl;
  std::cout << "edge_file: " << e_file_path << std::endl;
  std::cout << "output_dir: " << output_dir << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
  std::cout << "q: " << q << std::endl;
  std::cout << "supp_limit = " << supp_limit << std::endl;
  std::cout << "k: " << k << std::endl;
  std::cout << "d: " << d << std::endl;
  std::cout << std::endl;

  // using GraphType = GUNDAM::Graph<>;
  using GraphType = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  gmine_new::DmineNew<GraphType, DataGraph>(
      v_file_path.c_str(), e_file_path.c_str(), x, y, q, supp_limit, d, k,
      output_dir.c_str());
  google::ShutdownGoogleLogging();
  return 0;
}