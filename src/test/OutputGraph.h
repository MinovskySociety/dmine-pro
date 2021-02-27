#include <iostream>
#include <sstream>
#include <string>

//#include "gundam/graph.h"

using ReturnType = int;

template <template <typename...> class GraphType, typename... configures>
ReturnType OutputGraph(const GraphType<configures...>& graph) {
  std::cout << "node label:\n";
  for (auto it = graph.VertexCBegin(); !it.IsDone(); it++) {
    std::cout << it->id() << " " << it->label() << std::endl;
  }
  std::cout << "src_id dst_id edge_label:\n";
  for (auto it = graph.VertexCBegin(); !it.IsDone(); it++) {
    for (auto edge_it = graph.FindConstVertex(it->id())->OutEdgeCBegin();
         !edge_it.IsDone(); edge_it++) {
      std::cout << edge_it->const_src_ptr()->id() << " "
                << edge_it->const_dst_ptr()->id() << " " << edge_it->label()
                << std::endl;
    }
  }
  return 1;
}