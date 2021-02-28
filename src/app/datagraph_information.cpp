#include <algorithm>

#include "gundam/graph_type/large_graph.h"
#include "gundam/io/csvgraph.h"
int main() {
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  DataGraph data_graph;
  std::map<unsigned int, double> sum_in_edge, sum_out_edge;
  std::map<unsigned int, size_t> max_in_edge, max_out_edge;
  std::map<unsigned int, size_t> total_in_vertex, total_out_vertex;
  GUNDAM::ReadCSVGraph(data_graph,
                       "/Users/apple/Desktop/buaa/data/liantong/"
                       "liantong_n.csv",
                       "/Users/apple/Desktop/buaa/data/liantong/"
                       "liantong_e.csv");
  size_t vertex_count = data_graph.CountVertex();
  double sum_in = 0, sum_out = 0;
  size_t min_visit = 1000000;
  for (auto it = data_graph.VertexCBegin(); !it.IsDone(); it++) {
    if (it->id() > 4999) continue;
    std::cout << "id = " << it->id() << std::endl;
    for (unsigned int i = 0; i <= 2; i++) {
      sum_in += it->CountInEdge(i);
      sum_out += it->CountOutEdge(i);
      sum_in_edge[i] += it->CountInEdge(i);
      sum_out_edge[i] += it->CountOutEdge(i);
      max_in_edge[i] = std::max(max_in_edge[i], it->CountInEdge(i));
      max_out_edge[i] = std::max(max_out_edge[i], it->CountOutEdge(i));
      min_visit = std::min(min_visit, it->CountOutEdge(2));
      int vertex_num = 0;
      std::set<typename DataGraph::VertexConstPtr> unique_vertex;
      for (auto edge_it = it->OutEdgeCBegin(i); !edge_it.IsDone(); edge_it++) {
        unique_vertex.insert(edge_it->const_dst_ptr());
      }
      total_out_vertex[i] += unique_vertex.size();
      std::cout << "label = " << i << " out edge = " << it->CountOutEdge(i)
                << " vertex = " << unique_vertex.size() << std::endl;
      unique_vertex.clear();
      for (auto edge_it = it->InEdgeCBegin(i); !edge_it.IsDone(); edge_it++) {
        unique_vertex.insert(edge_it->const_src_ptr());
      }
      std::cout << "label = " << i << " in edge = " << it->CountInEdge(i)
                << " vertex = " << unique_vertex.size() << std::endl;
      total_in_vertex[i] += unique_vertex.size();
    }
  }
  std::cout << sum_in / vertex_count << " " << sum_out / vertex_count
            << std::endl;
  std::cout << "min visit =" << min_visit << std::endl;
  for (int i = 0; i <= 2; i++) {
    std::cout << "total = " << sum_in_edge[i] << " " << (int)sum_out_edge[i]
              << std::endl;
    std::cout << "max = " << max_in_edge[i] << " " << max_out_edge[i]
              << std::endl;
    std::cout << "total vertex = " << total_out_vertex[i] << " "
              << total_in_vertex[i] << std::endl;
  }

  return 0;
}