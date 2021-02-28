#include "gundam/algorithm/dp_iso.h"
#include "gundam/graph_type/graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/match/match.h"
int main() {
  std::cout << "111" << std::endl;
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  using Pattern = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                     uint32_t, std::string>;
  using VertexConstPtr = typename DataGraph::VertexConstPtr;
  DataGraph data_graph;
  data_graph.AddVertex(1, 1);
  data_graph.AddVertex(2, 2);
  data_graph.AddEdge(1, 2, 1, 1);
  DataGraph query;
  query.AddVertex(1, 1);
  query.AddVertex(2, 2);
  query.AddEdge(1, 2, 1, 1);
  GUNDAM::Match<const DataGraph, const DataGraph> partical_match;
  GUNDAM::MatchSet<const DataGraph, const DataGraph> res;
  GUNDAM::DPISO_UsingPatricalMatchAndMatchSet(query, data_graph, partical_match,
                                              res);
  std::cout << "size = " << res.size() << std::endl;
  return 0;
}