#ifndef _BOOSTISO_H
#define _BOOSTISO_H
//#include "gundam/graph_type/graph.h"
template <class VertexPtr>
void GetKStepAdj(VertexPtr vertex_ptr, int k, std::set<VertexPtr> &adj_list) {
  if (k == 0) {
    adj_list.insert(vertex_ptr);
    return;
  }
  for (auto it = vertex_ptr->OutEdgeCBegin(); !it.IsDone(); it++) {
    VertexPtr next_vertex_ptr = it->const_dst_ptr();
    GetKStepAdj(next_vertex_ptr, k - 1, adj_list);
  }
  for (auto it = vertex_ptr->InEdgeCBegin(); !it.IsDone(); it++) {
    VertexPtr next_vertex_ptr = it->const_src_ptr();
    GetKStepAdj(next_vertex_ptr, k - 1, adj_list);
  }
}
template <class GraphType, class VertexPtr>
bool SyntacticContainment(VertexPtr query_vertex_ptr,
                          VertexPtr target_vertex_ptr) {
  if (query_vertex_ptr->label() != target_vertex_ptr->label()) return false;
  using EdgeLabelType = typename GraphType::EdgeType::LabelType;
  std::map<std::pair<EdgeLabelType, VertexPtr>, int> query_map, target_map;
  std::map<EdgeLabelType, int> query_num, target_num;
  for (auto it = query_vertex_ptr->OutEdgeCBegin(); !it.IsDone(); it++) {
    VertexPtr dst_ptr = it->const_dst_ptr();
    if (dst_ptr == target_vertex_ptr) {
      query_num[it->label()]++;
    } else {
      query_map[std::make_pair(it->label(), dst_ptr)]++;
    }
  }
  for (auto it = target_vertex_ptr->OutEdgeCBegin(); !it.IsDone(); it++) {
    VertexPtr dst_ptr = it->const_dst_ptr();
    if (dst_ptr == query_vertex_ptr) {
      target_num[it->label()]++;
    } else {
      target_map[std::make_pair(it->label(), dst_ptr)]++;
    }
  }
  for (auto &it : query_num) {
    if (target_num.find(it.first)->second < it.second) return false;
  }
  for (auto &it : query_map) {
    if (target_map.find(it.first)->second < it.second) return false;
  }
  query_num.clear();
  query_map.clear();
  target_num.clear();
  target_map.clear();
  for (auto it = query_vertex_ptr->InEdgeCBegin(); !it.IsDone(); it++) {
    VertexPtr src_ptr = it->const_src_ptr();
    if (src_ptr == target_vertex_ptr) {
      query_num[it->label()]++;
    } else {
      query_map[std::make_pair(it->label(), src_ptr)]++;
    }
  }
  for (auto it = target_vertex_ptr->InEdgeCBegin(); !it.IsDone(); it++) {
    VertexPtr src_ptr = it->const_src_ptr();
    if (src_ptr == query_vertex_ptr) {
      target_num[it->label()]++;
    } else {
      target_map[std::make_pair(it->label(), src_ptr)]++;
    }
  }
  for (auto &it : query_num) {
    if (target_num.find(it.first)->second < it.second) return false;
  }
  for (auto &it : query_map) {
    if (target_map.find(it.first)->second < it.second) return false;
  }
  return true;
}
template <class GraphType, class VertexPtr>
bool SyntacticEquivalence(VertexPtr query_vertex_ptr,
                          VertexPtr target_vertex_ptr) {
  return SyntacticContainment<GraphType>(query_vertex_ptr, target_vertex_ptr) &&
         SyntacticContainment<GraphType>(target_vertex_ptr, query_vertex_ptr);
}
template <class DataGraph>
void ComputeAdaptedGraph(
    DataGraph &data_graph, DataGraph &ret_graph,
    std::map<typename DataGraph::VertexType::IDType,
             std::vector<typename DataGraph::VertexConstPtr>> &map_vertex_list,
    std::map<typename DataGraph::VertexType::IDType,
             std::map<typename DataGraph::EdgeType::LabelType, int>>
        &is_clique) {
  using VertexConstPtr = typename DataGraph::VertexConstPtr;
  std::set<VertexConstPtr> used_vertex;
  using VertexIDType = typename DataGraph::VertexType::IDType;
  VertexIDType vertex_num = 0;
  std::map<VertexConstPtr, VertexIDType> h;
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    std::cout << "id = " << vertex_it->id() << std::endl;
    VertexConstPtr vertex_ptr = vertex_it;
    if (used_vertex.count(vertex_ptr)) continue;
    used_vertex.insert(vertex_ptr);
    ret_graph.AddVertex(++vertex_num, vertex_ptr->label());
    h.emplace(vertex_ptr, vertex_num);
    map_vertex_list[vertex_num].push_back(vertex_ptr);
    std::set<VertexConstPtr> adj_list;
    GetKStepAdj(vertex_ptr, 1, adj_list);
    for (auto next_ptr : adj_list) {
      if (used_vertex.count(next_ptr)) continue;
      if (SyntacticEquivalence<DataGraph>(vertex_ptr, next_ptr)) {
        h.emplace(next_ptr, vertex_num);
        map_vertex_list[vertex_num].push_back(next_ptr);
        used_vertex.insert(next_ptr);
        std::map<typename DataGraph::EdgeType::LabelType, int>
            is_clique_value_map;
        for (auto edge_it = next_ptr->OutEdgeCBegin(); !edge_it.IsDone();
             edge_it++) {
          if (edge_it->const_dst_ptr() == vertex_ptr) {
            is_clique_value_map[edge_it->label()]++;
          }
        }
        for (auto &value_it : is_clique_value_map) {
          is_clique[vertex_num][value_it.first] =
              std::max(is_clique[vertex_num][value_it.first], value_it.second);
        }
      }
    }
    adj_list.clear();
    GetKStepAdj(vertex_ptr, 2, adj_list);
    for (auto next_ptr : adj_list) {
      if (used_vertex.count(next_ptr)) continue;
      if (SyntacticEquivalence<DataGraph>(vertex_ptr, next_ptr)) {
        h.emplace(next_ptr, vertex_num);
        map_vertex_list[vertex_num].push_back(next_ptr);
        used_vertex.insert(next_ptr);
      }
    }
  }
  std::set<VertexConstPtr> add_vertex;
  for (auto &it : map_vertex_list) {
    add_vertex.insert(it.second[0]);
  }
  int edge_id = 0;
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    VertexConstPtr vertex_ptr = vertex_it;
    if (!add_vertex.count(vertex_ptr)) continue;
    for (auto edge_it = vertex_ptr->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      ret_graph.AddEdge(h[vertex_ptr], h[edge_it->const_dst_ptr()],
                        edge_it->label(), ++edge_id);
    }
  }
  std::cout << "vertex num = " << vertex_num << std::endl;
  std::cout << "edge num = " << edge_id << std::endl;
}
#endif