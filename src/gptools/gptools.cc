#include "gptools.h"

#include <gflags/gflags.h>
#include <stdlib.h>

#include <iostream>

int main(int argc, char **argv) {
  if (argc > 2) {
    std::cout << "parameter num is not correct! (0 or 1)" << std::endl;
    return 0;
  }

  std::string config_file_path = "gptools_config.yaml";
  if (argc == 2) config_file_path = argv[1];

  using namespace gptools;

  int res;

  GPToolsInfo gptools_info;
  res = ReadGPToolsInfoYAML(config_file_path, gptools_info);
  if (res < 0) {
    std::cout << "Can't read yaml configure file (err: " << res << ")."
              << std::endl;
    return res;
  }

  GraphPackage gp;
  res = graphpackage::ReadGraphPackage(gptools_info.graph_info, gp);
  if (res < 0) {
    std::cout << "Can't read graph package (err: " << res << ")." << std::endl;
    return res;
  }

  std::cout << "-------------------------" << std::endl;
  std::cout << "   Run actions" << std::endl;
  std::cout << "-------------------------" << std::endl;
  for (const auto &action_ptr : gptools_info.action_list) {
    res = action_ptr->Run(gp);
    if (res < 0) {
      std::cout << "error - " << res << std::endl;
      return res;
    }
    std::cout << std::endl;
  }

  res = graphpackage::WriteGraphPackage(gptools_info.result_info, gp);
  if (res < 0) {
    std::cout << "Can't write graph package (err: " << res << ")." << std::endl;
    return res;
  }

  quick_exit(0);
  return 0;
}