#include <ctime>
#include <fstream>
#include <iostream>

#include "gpar/datagraph_information.cpp"
#include "gpar/discover_gpar.cpp"
#include "gpar/gpar.h"
#include "gpar/graph_pattern_association_rules.cpp"
#include "gpar/test.cpp"

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  char config_file_path[1500] = "dmine_config.txt";
  if (argc == 2) {
    strcpy(config_file_path, argv[1]);
  }
  std::cout << config_file_path << std::endl;
  std::ifstream config_file(config_file_path);
  if (!config_file.is_open()) {
    std::cout << "cannot open config file." << std::endl;
    return 0;
  }
  int x, y, q, k, d;
  char v_file_path[1500], e_file_path[1500], output_dir[1500];
  config_file >> v_file_path;
  config_file >> e_file_path;
  config_file >> output_dir;
  config_file >> x >> y >> q;
  config_file >> k;
  config_file >> d;
  config_file.close();

  std::cout << "node_file: " << v_file_path << std::endl;
  std::cout << "edge_file: " << e_file_path << std::endl;
  std::cout << "output_dir: " << output_dir << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
  std::cout << "q: " << q << std::endl;
  std::cout << "k: " << k << std::endl;
  std::cout << "d: " << d << std::endl;
  std::cout << std::endl;

  gmine_new::DiscoverGPAR g;
  g.SetQEdge(x, y, q);
  g.SetTopK(k);
  g.SetUpEdgeNum(d);

  g.SetOutputDir(output_dir);

  clock_t tot_begin = clock();
  int res = g.DminePro(v_file_path, e_file_path);
  clock_t tot_end = clock();
  std::cout << "tot time=" << (1.0 * tot_end - tot_begin) / CLOCKS_PER_SEC
            << std::endl;

  return 0;
}