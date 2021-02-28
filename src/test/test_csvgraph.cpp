#include <ctime>

#include "gundam/io/csvgraph.h"
//#include "gundam/graph.h"
#include "gundam/graph_type/large_graph.h"
int main() {
  using namespace GUNDAM;
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  DataGraph g;
  time_t t_begin = clock();

  std::vector<std::string> v_file_list, e_file_list;
  v_file_list.push_back("D:\\Work\\cu4999\\liantong_v.csv");
  e_file_list.push_back("D:\\Work\\cu4999\\liantong_e.csv");
  ReadCSVGraph(g, v_file_list, e_file_list);
  time_t t_end = clock();
  std::cout << "read graph time is :"
            << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC << std::endl;
  size_t Max = 0;
  for (auto it = g.VertexCBegin(4); !it.IsDone(); it++) {
    Max += it->CountOutVertex(2);
  }
  std::cout << "Max = " << Max << std::endl;
  return 0;
}
