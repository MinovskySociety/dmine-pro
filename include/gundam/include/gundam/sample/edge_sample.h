#ifndef _GUNDAM_SAMPLE_EDGE_SAMPLE_H
#define _GUNDAM_SAMPLE_EDGE_SAMPLE_H

#include <vector>
#include "gundam/tool/remove_isolate_vertex.h"

namespace GUNDAM{

template<typename GraphType>
GraphType EdgeSample(const GraphType& graph, double sample_ratio){
  GraphType sampled_graph(graph);
  std::vector<typename GraphType::EdgeType::IDType> remove_edge_id_set;
  for (auto vertex_it = graph.VertexBegin();
           !vertex_it.IsDone(); 
            vertex_it++){
    for (auto edge_it = vertex_it->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++){
      const double kR = ((double) rand() / (RAND_MAX));
      if (kR <= sample_ratio) {
        /// this vertex should to be preserved
        continue;
      }
      remove_edge_id_set.emplace_back(edge_it->id());
    }
  }
  for (const auto& remove_edge_id : remove_edge_id_set){
    sampled_graph.EraseEdge(remove_edge_id);
  }
  GUNDAM::RemoveIsolateVertex(sampled_graph);
  return sampled_graph;
}

};

#endif // _GUNDAM_SAMPLE_EDGE_SAMPLE_H