#ifndef _GPARCONFIG_H
#define _GPARCONFIG_H
#include <set>
namespace gmine_new {
// similiar 阈值
constexpr double similiar_limit = 0.75;

// double 判断误差
constexpr double eps = 1e-7;

// Data Graph 中 y_label的节点是否只有一个（可用于加速计算supp_r)
constexpr bool y_label_count_1 = true;

// 是否采用基于拓展图的方法剪枝
constexpr bool using_prune = true;
// 设置L的size上限
constexpr size_t L_size = 2500;

// 不会出现在GPAR里的边的label
std::set<int> erase_edge_label{};
// 不会出现在GPAR里的点的label
std::set<int> erase_vertex_label{};

// 是否采用基于code的去重方法
constexpr bool using_dfs_code = true;

//是否保留全部结果
constexpr bool remain_all_result = false;

// 是否采用筛选的方法
constexpr bool using_selectk = true;
// 每轮留下的GPAR数量
constexpr int select_k = 200;

// 是否开启人工筛选
constexpr bool using_manual_check = true;
// 人工筛选GPAR(拓扑结构上)
auto manual_check = [](auto &gpar) {
  /*
  // biokg data:
  for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      auto dst_ptr = edge_it->const_dst_ptr();
      for (auto dst_edge_it = dst_ptr->OutEdgeCBegin();
  !dst_edge_it.IsDone(); dst_edge_it++) { if
  (dst_edge_it->const_dst_ptr()->id() == vertex_it->id() &&
            dst_edge_it->label() == edge_it->label()) {
          return false;
        }
      }
    }
  }
  return true;
  */

  // gmar:
  for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    if (vertex_it->id() == gpar.x_node_ptr()->id()) continue;
    // if (vertex_it->CountOutEdge(-1) > 0) return false;
  }

  // liantongdata:
  int not_fake_num = 0;
  int fake_num = 0;
  // x node cannot has q edge
  if (gpar.x_node_ptr()->CountOutEdge(gpar.q_edge_label()) > 0) return false;
  for (auto it = gpar.pattern.VertexCBegin(); !it.IsDone(); it++) {
    if (it->label() == 1000000001) not_fake_num++;
    if (it->label() == 1000000002) fake_num++;
  }
  // not fake or fake node num is less than 1
  if (not_fake_num >= 2 || fake_num >= 2) return false;
  for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    // if (vertex_it->label() == 1002 && vertex_it->CountOutEdge(3) >= 1)
    //  return false;

    int head_num = 0;
    int length_num = 0;
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      // 不允许出现由 attribute 点拓展一个新的 user/phone 点出来
      if ((vertex_it->label() == 1001 || vertex_it->label() == 1002) &&
          vertex_it->CountOutEdge() == 1 && vertex_it->CountInEdge() == 0 &&
          edge_it->label() == 3) {
        return false;
      }
      // 不允许出现由 value 点拓展一个新的 attribute 点出来
      if (vertex_it->label() == 3003 && vertex_it->CountOutEdge() == 1 &&
          vertex_it->CountInEdge() == 0 && edge_it->label() == 4) {
        return false;
      }
      // 不允许出现由 constant 点拓展一个新的 value 点出来
      if (vertex_it->label() == 4002 && vertex_it->CountOutEdge() == 1 &&
          vertex_it->CountInEdge() == 0 && edge_it->label() == 5) {
        return false;
      }
      //一个phone最多只能有一个 head 跟一个 length
      if (vertex_it->label() == 1002) {
        if (edge_it->const_dst_ptr()->label() == 3003) length_num++;
        if (edge_it->const_dst_ptr()->label() >= 1000000003 &&
            edge_it->const_dst_ptr()->label() <= 1000000373)
          head_num++;
      }
    }
    if (length_num >= 2 || head_num >= 2) return false;
  }
  return true;
};

auto select_callback = [](auto &a, auto &b) {
  // gmar:
  int num1 = 0, num2 = 0;
  for (auto vertex_it = a.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    num1 += vertex_it->CountOutEdge(-1);
  }
  for (auto vertex_it = b.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    num2 += vertex_it->CountOutEdge(-1);
  }
  if (num1 != num2) return num1 > num2;
  // liantong data
  int numa = 0, numb = 0;
  for (auto vertex_it = a.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    if (vertex_it->label() == 1002) {
      // phone
      std::set<int> adj_user_set;
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (edge_it->const_dst_ptr()->label() == 1001) {
          adj_user_set.insert(edge_it->const_dst_ptr()->id());
        }
      }
      for (auto edge_it = vertex_it->InEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (edge_it->const_src_ptr()->label() == 1001) {
          adj_user_set.insert(edge_it->const_src_ptr()->id());
        }
      }
      if (adj_user_set.size() == 1) numa++;
    }
  }
  for (auto vertex_it = b.pattern.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    if (vertex_it->label() == 1002) {
      // phone
      std::set<int> adj_user_set;
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (edge_it->const_dst_ptr()->label() == 1001) {
          adj_user_set.insert(edge_it->const_dst_ptr()->id());
        }
      }
      for (auto edge_it = vertex_it->InEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (edge_it->const_src_ptr()->label() == 1001) {
          adj_user_set.insert(edge_it->const_src_ptr()->id());
        }
      }
      if (adj_user_set.size() == 1) numb++;
    }
  }
  if (numa != numb) return numa < numb;
  // other data
  return a.id() < b.id();
};
}  // namespace gmine_new

#endif