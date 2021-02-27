#include "gundam/large_graph.h"

int main() {
  using namespace GUNDAM;

  LargeGraph<uint32_t, uint32_t, std::string, uint32_t, uint32_t, std::string> g;

  g.AddVertex(1, 1);
  g.AddVertex(2, 1);
  g.AddEdge(1, 2, 1, 0);
  g.AddEdge(1, 2, 1, 1);
  g.AddEdge(1, 2, 2, 2);
  std::cout << "count = " << g.FindConstVertex(1)->CountOutVertex(1)
            << std::endl;
  return 0;
}