#ifndef _GPARDFSCODE_H
#define _GPARDFSCODE_H
#include "gpar_new.h"

namespace gmine_new {
template <class GPAR>
class GPARDFSCode {
 public:
  using VertexLabelType = typename GPAR::VertexLabelType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  using VertexIDType = typename GPAR::VertexIDType;

 public:
  GPARDFSCode(int src_vertex_script, int dst_vertex_script,
              VertexLabelType src_label, EdgeLabelType edge_label,
              VertexLabelType dst_label)
      : src_vertex_script_(src_vertex_script),
        dst_vertex_script_(dst_vertex_script),
        src_label_(src_label),
        edge_label_(edge_label),
        dst_label_(dst_label) {}
  GPARDFSCode(const GPARDFSCode<GPAR> &b)
      : src_vertex_script_(b.src_vertex_script_),
        dst_vertex_script_(b.dst_vertex_script_),
        src_label_(b.src_label_),
        edge_label_(b.edge_label_),
        dst_label_(b.dst_label_) {}
  bool operator<(const GPARDFSCode<GPAR> &b) const {
    if (this->src_vertex_script_ != b.src_vertex_script_)
      return this->src_vertex_script_ < b.src_vertex_script_;
    if (this->dst_vertex_script_ != b.dst_vertex_script_)
      return this->dst_vertex_script_ < b.dst_vertex_script_;
    if (this->src_label_ != b.src_label_)
      return this->src_label_ < b.src_label_;
    if (this->edge_label_ != b.edge_label_)
      return this->edge_label_ < b.edge_label_;
    return this->dst_label_ < b.dst_label_;
  }
  bool operator==(const GPARDFSCode<GPAR> &b) const {
    if ((this->src_vertex_script_ == b.src_vertex_script_) &&
        (this->dst_vertex_script_ == b.dst_vertex_script_) &&
        (this->src_label_ == b.src_label_) &&
        (this->edge_label_ == b.edge_label_) &&
        (this->dst_label_ == b.dst_label_))
      return true;
    return false;
  }
  void Print() {
    std::cout << src_vertex_script_ << " " << dst_vertex_script_ << " "
              << src_label_ << " " << edge_label_ << " " << dst_label_
              << std::endl;
  }

 private:
  int src_vertex_script_, dst_vertex_script_;
  VertexLabelType src_label_;
  EdgeLabelType edge_label_;
  VertexLabelType dst_label_;
};
template <bool is_out, class EdgePtr>
class EdgeCMP {
 public:
  bool operator()(std::pair<int32_t, EdgePtr> a,
                  std::pair<int32_t, EdgePtr> b) {
    if (a.first != b.first) return a.first < b.first;
    auto a_dst_ptr =
        is_out ? a.second->const_dst_ptr() : a.second->const_src_ptr();
    auto b_dst_ptr =
        is_out ? b.second->const_dst_ptr() : b.second->const_src_ptr();
    if (is_out) {
      if (a.second->label() != b.second->label())
        return a.second->label() < b.second->label();
      if (a_dst_ptr->label() != b_dst_ptr->label())
        return a_dst_ptr->label() < b_dst_ptr->label();
    } else {
      if (a_dst_ptr->label() != b_dst_ptr->label())
        return a_dst_ptr->label() < b_dst_ptr->label();
      if (a.second->label() != b.second->label())
        return a.second->label() < b.second->label();
    }
    if (a_dst_ptr->CountOutVertex() != b_dst_ptr->CountOutVertex())
      return a_dst_ptr->CountOutVertex() < b_dst_ptr->CountOutVertex();
    if (a_dst_ptr->CountInVertex() != b_dst_ptr->CountInVertex())
      return a_dst_ptr->CountInVertex() < b_dst_ptr->CountInVertex();
    if (a_dst_ptr->CountOutEdge() != b_dst_ptr->CountOutEdge())
      return a_dst_ptr->CountOutEdge() < b_dst_ptr->CountOutEdge();
    if (a_dst_ptr->CountInEdge() != b_dst_ptr->CountInEdge())
      return a_dst_ptr->CountInEdge() < b_dst_ptr->CountInEdge();
    return a.second->id() < b.second->id();
  }
};
template <class GPAR, class VertexPtr, class DFSCodeContainer>
void DFS(VertexPtr now_vertex_ptr, std::set<VertexPtr> &used_vertex,
         std::set<typename GPAR::EdgeIDType> &used_edge, int32_t &max_script,
         std::map<VertexPtr, int32_t> &vertex_to_script,
         DFSCodeContainer &dfs_code_container) {
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using ComPareEdgeData = std::pair<int, EdgeConstPtr>;
  used_vertex.insert(now_vertex_ptr);
  constexpr int32_t script_max = INT32_MAX;
  constexpr bool out = true;
  constexpr bool in = false;
  std::vector<ComPareEdgeData> sort_edge_container;
  for (auto out_edge_it = now_vertex_ptr->OutEdgeCBegin();
       !out_edge_it.IsDone(); out_edge_it++) {
    EdgeConstPtr edge_ptr = out_edge_it;
    if (used_edge.count(edge_ptr->id())) continue;
    auto it = vertex_to_script.find(out_edge_it->const_dst_ptr());
    int32_t dst_script =
        (it == vertex_to_script.end()) ? script_max : it->second;
    sort_edge_container.emplace_back(std::make_pair(dst_script, edge_ptr));
  }

  std::stable_sort(sort_edge_container.begin(), sort_edge_container.end(),
                   EdgeCMP<out, EdgeConstPtr>());
  int32_t now_script = vertex_to_script[now_vertex_ptr];
  for (const auto &it : sort_edge_container) {
    if (used_edge.count(it.second->id())) continue;
    VertexConstPtr dst_ptr = it.second->const_dst_ptr();
    auto find_it = vertex_to_script.find(dst_ptr);
    int32_t script =
        (find_it == vertex_to_script.end()) ? ++max_script : find_it->second;
    if (find_it == vertex_to_script.end())
      vertex_to_script.emplace(dst_ptr, script);
    dfs_code_container.emplace_back(
        GPARDFSCode<GPAR>(now_script, script, now_vertex_ptr->label(),
                          it.second->label(), dst_ptr->label()));
    used_edge.insert(it.second->id());
    if (!used_vertex.count(dst_ptr))
      DFS<GPAR>(dst_ptr, used_vertex, used_edge, max_script, vertex_to_script,
                dfs_code_container);
  }
  sort_edge_container.clear();
  for (auto out_edge_it = now_vertex_ptr->InEdgeCBegin(); !out_edge_it.IsDone();
       out_edge_it++) {
    EdgeConstPtr edge_ptr = out_edge_it;
    if (used_edge.count(edge_ptr->id())) continue;
    auto it = vertex_to_script.find(out_edge_it->const_src_ptr());
    int32_t dst_script =
        (it == vertex_to_script.end()) ? script_max : it->second;
    sort_edge_container.emplace_back(std::make_pair(dst_script, edge_ptr));
  }
  std::stable_sort(sort_edge_container.begin(), sort_edge_container.end(),
                   EdgeCMP<in, EdgeConstPtr>());
  for (const auto &it : sort_edge_container) {
    if (used_edge.count(it.second->id())) continue;
    VertexConstPtr src_ptr = it.second->const_src_ptr();
    auto find_it = vertex_to_script.find(src_ptr);
    int32_t script =
        (find_it == vertex_to_script.end()) ? ++max_script : find_it->second;
    if (find_it == vertex_to_script.end())
      vertex_to_script.emplace(src_ptr, script);
    dfs_code_container.emplace_back(
        GPARDFSCode<GPAR>(script, now_script, src_ptr->label(),
                          it.second->label(), now_vertex_ptr->label()));
    used_edge.insert(it.second->id());
    if (!used_vertex.count(src_ptr))
      DFS<GPAR>(src_ptr, used_vertex, used_edge, max_script, vertex_to_script,
                dfs_code_container);
  }
}
template <class GPAR, class DFSCodeContainer>
inline void GetGPARDFSCode(const GPAR &gpar,
                           DFSCodeContainer &dfs_code_container) {
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeIDType = typename GPAR::EdgeIDType;
  std::set<VertexConstPtr> used_vertex;
  std::set<EdgeIDType> used_edge;
  std::map<VertexConstPtr, int> vertex_to_script;
  int32_t max_script = -1;
  vertex_to_script.insert(std::make_pair(gpar.x_node_ptr(), ++max_script));
  vertex_to_script.insert(std::make_pair(gpar.y_node_ptr(), ++max_script));
  dfs_code_container.emplace_back(
      GPARDFSCode<GPAR>(0, 1, gpar.x_node_ptr()->label(), gpar.q_edge_label(),
                        gpar.y_node_ptr()->label()));
  DFS<GPAR>(gpar.x_node_ptr(), used_vertex, used_edge, max_script,
            vertex_to_script, dfs_code_container);
}
}  // namespace gmine_new

#endif