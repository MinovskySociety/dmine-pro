#ifndef _GUNDAM_SAMPLE_VERTEX_SAMPLE_H
#define _GUNDAM_SAMPLE_VERTEX_SAMPLE_H

namespace GUNDAM{

template<typename GraphType>
GraphType VertexSample(const GraphType& graph, double sample_ratio){
  GraphType sampled_graph(graph);
  std::vector<typename GraphType::VertexType::IDType> remove_vertex_id_set;
  for (auto vertex_it = graph.VertexBegin();
           !vertex_it.IsDone(); 
            vertex_it++){
    const double kR = ((double) rand() / (RAND_MAX));
    if (kR <= sample_ratio) {
      /// this vertex should to be preserved
      continue;
    }
    remove_vertex_id_set.emplace_back(vertex_it->id());
  }
  for (const auto& remove_vertex_id : remove_vertex_id_set){
    sampled_graph.EraseVertex(remove_vertex_id);
  }
  return sampled_graph;
}

};

#endif // _GUNDAM_SAMPLE_VERTEX_SAMPLE_H