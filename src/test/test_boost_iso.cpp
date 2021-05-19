//#include "gundam/boost_iso.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/io/csvgraph.h"

int main() {
  using namespace GUNDAM;
  using Graph = LargeGraph<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                           std::string>;

  Graph data_graph, ret_graph;
  // using VertexConstPtr = typename Graph::VertexConstPtr;
  ReadCSVGraph(
      data_graph,
      "/Users/apple/Desktop/buaa/code/dmine-pro/testdata/"
      "liantong_n1.csv",
      "/Users/apple/Desktop/buaa/code/dmine-pro/testdata/liantong_e1.csv");
  // ComputeAdaptedGraph(data_graph, ret_graph);
}