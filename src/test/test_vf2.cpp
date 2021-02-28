#include <ostream>

#include "gundam/algorithm/dp_iso.h"
#include "gundam/algorithm/vf2_boost.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/io/csvgraph.h"
int main() {
  using namespace GUNDAM;
  using GraphType = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  using TargetVertexPtr = typename DataGraph::VertexConstPtr;
  DataGraph data_graph;
  using VertexConstPtr = typename DataGraph::VertexConstPtr;
  using VertexIDType = typename DataGraph::VertexType::IDType;
  using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
  ReadCSVGraph(
      data_graph,
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/liantong_v.csv",
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/liantong_e.csv");
  GraphType pattern;
  ReadCSVGraph(pattern,
               "/Users/apple/Desktop/buaa/code/grape-gundam/dataset/"
               "benchmark_pattern/cal_match/pattern_v_3.csv",
               "/Users/apple/Desktop/buaa/code/grape-gundam/dataset/"
               "benchmark_pattern/cal_match/pattern_e_3.csv");
  auto t_begin = clock();
  int ans = GUNDAM::DPISO(pattern, data_graph, -1);
  auto t_end = clock();
  std::cout << "time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
            << std::endl;
  std::cout << "size = " << ans << std::endl;
  return 0;
}