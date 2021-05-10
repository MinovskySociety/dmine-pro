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
    auto a_dst_handle =
        is_out ? a.second->const_dst_handle() : a.second->const_src_handle();
    auto b_dst_handle =
        is_out ? b.second->const_dst_handle() : b.second->const_src_handle();
    if (is_out) {
      if (a.second->label() != b.second->label())
        return a.second->label() < b.second->label();
      if (a_dst_handle->label() != b_dst_handle->label())
        return a_dst_handle->label() < b_dst_handle->label();
    } else {
      if (a_dst_handle->label() != b_dst_handle->label())
        return a_dst_handle->label() < b_dst_handle->label();
      if (a.second->label() != b.second->label())
        return a.second->label() < b.second->label();
    }
    if (a_dst_handle->CountOutVertex() != b_dst_handle->CountOutVertex())
      return a_dst_handle->CountOutVertex() < b_dst_handle->CountOutVertex();
    if (a_dst_handle->CountInVertex() != b_dst_handle->CountInVertex())
      return a_dst_handle->CountInVertex() < b_dst_handle->CountInVertex();
    if (a_dst_handle->CountOutEdge() != b_dst_handle->CountOutEdge())
      return a_dst_handle->CountOutEdge() < b_dst_handle->CountOutEdge();
    if (a_dst_handle->CountInEdge() != b_dst_handle->CountInEdge())
      return a_dst_handle->CountInEdge() < b_dst_handle->CountInEdge();
    return a.second->id() < b.second->id();
  }
};
template <class GPAR, class VertexPtr, class DFSCodeContainer>
void DFS(VertexPtr now_vertex_ptr, std::set<VertexPtr> &used_vertex,
         std::set<typename GPAR::EdgeIDType> &used_edge, int32_t &max_script,
         std::map<VertexPtr, int32_t> &vertex_to_script,
         DFSCodeContainer &dfs_code_container) {
  using EdgePtr = typename GPAR::EdgePtr;
  // using GPARVertexPtr = typename GPAR::VertexPtr;
  using ComPareEdgeData = std::pair<int, EdgePtr>;
  used_vertex.insert(now_vertex_ptr);
  constexpr int32_t script_max = INT32_MAX;
  constexpr bool out = true;
  constexpr bool in = false;
  std::vector<ComPareEdgeData> sort_edge_container;
  for (auto out_edge_it = now_vertex_ptr->OutEdgeBegin(); !out_edge_it.IsDone();
       out_edge_it++) {
    EdgePtr edge_ptr = out_edge_it;
    if (used_edge.count(edge_ptr->id())) continue;
    auto it = vertex_to_script.find(out_edge_it->dst_handle());
    int32_t dst_script =
        (it == vertex_to_script.end()) ? script_max : it->second;
    sort_edge_container.emplace_back(std::make_pair(dst_script, edge_ptr));
  }

  std::stable_sort(sort_edge_container.begin(), sort_edge_container.end(),
                   EdgeCMP<out, EdgePtr>());
  int32_t now_script = vertex_to_script[now_vertex_ptr];
  for (auto &it : sort_edge_container) {
    if (used_edge.count(it.second->id())) continue;
    auto dst_handle = it.second->dst_handle();
    auto find_it = vertex_to_script.find(dst_handle);
    int32_t script =
        (find_it == vertex_to_script.end()) ? ++max_script : find_it->second;
    if (find_it == vertex_to_script.end())
      vertex_to_script.emplace(dst_handle, script);
    dfs_code_container.emplace_back(
        GPARDFSCode<GPAR>(now_script, script, now_vertex_ptr->label(),
                          it.second->label(), dst_handle->label()));
    used_edge.insert(it.second->id());
    if (!used_vertex.count(dst_handle))
      DFS<GPAR>(dst_handle, used_vertex, used_edge, max_script,
                vertex_to_script, dfs_code_container);
  }
  sort_edge_container.clear();
  for (auto out_edge_it = now_vertex_ptr->InEdgeBegin(); !out_edge_it.IsDone();
       out_edge_it++) {
    EdgePtr edge_ptr = out_edge_it;
    if (used_edge.count(edge_ptr->id())) continue;
    auto it = vertex_to_script.find(out_edge_it->src_handle());
    int32_t dst_script =
        (it == vertex_to_script.end()) ? script_max : it->second;
    sort_edge_container.emplace_back(std::make_pair(dst_script, edge_ptr));
  }
  std::stable_sort(sort_edge_container.begin(), sort_edge_container.end(),
                   EdgeCMP<in, EdgePtr>());
  for (auto &it : sort_edge_container) {
    if (used_edge.count(it.second->id())) continue;
    VertexPtr src_handle = it.second->src_handle();
    auto find_it = vertex_to_script.find(src_handle);
    int32_t script =
        (find_it == vertex_to_script.end()) ? ++max_script : find_it->second;
    if (find_it == vertex_to_script.end())
      vertex_to_script.emplace(src_handle, script);
    dfs_code_container.emplace_back(
        GPARDFSCode<GPAR>(script, now_script, src_handle->label(),
                          it.second->label(), now_vertex_ptr->label()));
    used_edge.insert(it.second->id());
    if (!used_vertex.count(src_handle))
      DFS<GPAR>(src_handle, used_vertex, used_edge, max_script,
                vertex_to_script, dfs_code_container);
  }
}
template <class GPAR, class DFSCodeContainer>
inline void GetGPARDFSCode(GPAR &gpar, DFSCodeContainer &dfs_code_container) {
  using EdgePtr = typename GPAR::EdgePtr;
  using VertexPtr = typename GPAR::VertexPtr;
  using EdgeIDType = typename GPAR::EdgeIDType;
  std::set<VertexPtr> used_vertex;
  std::set<EdgeIDType> used_edge;
  std::map<VertexPtr, int> vertex_to_script;
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