#pragma once

//#include <cstring>
#include <iostream>

#include "gundam/csvgraph.h"
#include "gundam/graph.h"

#ifndef IN
#define IN
#endif  // !IN
#ifndef OUT
#define OUT
#endif  // !OUT
#ifndef INOUT
#define INOUT
#endif  // !INOUT

#define ECS_OK 0
#define ECS_E_UNKNOWN -1
#define ECS_E_FILE_NOT_EXISTS -2
#define ECS_E_FILE_FORMAT_ERROR -3
#define ECS_E_FOLDER_NOT_EXISTS -4
#define ECS_E_VERTEX_NOT_FOUND -5
#define ECS_E_EDGE_NOT_FOUND -6
#define ECS_E_ADD_EDGE_ERROR -7

namespace ECS {

using namespace std;
using namespace GUNDAM;

/*
  <summary>
    Get the max edge id number.
  </summary>
  <param name="graph">inited graph</param>
  <returns>max id</returns>
*/
template <typename... graph_configures>
int64_t get_max_edge_id(IN GUNDAM::Graph<graph_configures...>& graph) {
  int64_t max_id = 0;
  for (auto v_it = graph.VertexBegin(); !v_it.IsDone(); v_it++) {
    for (auto e_it = v_it->OutEdgeBegin(); !e_it.IsDone(); e_it++)
      if (max_id < static_cast<int64_t>(e_it->id())) {
        max_id = e_it->id();
      }
  }
  return max_id;
}

/*
  <summary>
    Get the max veretex id number.
  </summary>
  <param name="graph">inited graph</param>
  <returns>max id</returns>
*/
template <typename... graph_configures>
int64_t get_max_vertex_id(IN GUNDAM::Graph<graph_configures...>& graph) {
  int64_t max_id = 0;
  for (auto v_it = graph.VertexBegin(); !v_it.IsDone(); v_it++) {
    if (max_id < v_it->id()) max_id = v_it->id();
  }
  return max_id;
}

/*
  <summary>
    Print graph on terminal.
  </summary>
  <param name="graph">inited graph</param>
  <returns>void </returns>
*/
template <typename... graph_configures>
void print_graph(IN const GUNDAM::Graph<graph_configures...>& graph) {
  //// temp
  // using GraphType = GUNDAM::Graph<>;
  // GraphType graph;
  //// end_temp
  int64_t v_count = 0;
  for (auto v_it = graph.VertexCBegin(); !v_it.IsDone(); v_it++) {
    v_count++;
    cout << "Vertex id: " << v_it->id() << ":" << v_it->label() << endl;
    for (auto va_it = v_it->AttributeCBegin(); !va_it.IsDone(); va_it++) {
      cout << "  attr key:" << va_it->key()
           << ", attr value: " << va_it->template value<std::string>() << endl;
    }
    int64_t e_in_count = 0;
    int64_t e_out_count = 0;
    for (auto e_it = v_it->InEdgeCBegin(); !e_it.IsDone(); e_it++) {
      e_in_count++;
      cout << "  In edge id: " << e_it->id() << ":" << e_it->label()
           << " -- soruce vertex id: " << to_string(e_it->const_src_ptr()->id())
           << ":" << e_it->const_src_ptr()->label() << endl;
      for (auto ea_it = e_it->AttributeCBegin(); !ea_it.IsDone(); ea_it++) {
        // cout << "    attr key:" << ea_it->key()
        //      << ", attr value: " << ea_it->template value<std::string>()
        //      << endl;
      }
    }
    for (auto e_it = v_it->OutEdgeCBegin(); !e_it.IsDone(); e_it++) {
      e_out_count++;
      cout << "  Out edge id: " << e_it->id() << ":" << e_it->label()
           << " -- target vertex id: " << to_string(e_it->const_dst_ptr()->id())
           << ":" << e_it->const_dst_ptr()->label() << endl;
      // for (auto ea_it = e_it->AttributeCBegin(); !ea_it.IsDone(); ea_it++) {
      // cout << "    attr key:" << ea_it->key()
      //     << ", attr value: " << ea_it->template value<std::string>()
      //     << endl;
      //}
    }
    cout << "Count vertex " << v_it->id()
         << ", out edge number: " << e_out_count
         << ", in edge number: " << e_in_count << endl;
  }
  cout << "Graph vertex number: " << v_count << endl;
}

/*
<summary>
  count the number of calls per user.
</summary>
<param name="graph">inited graph</param>
<param name="is_rewrite_graph">bool, count result is write back to graph as a
attribute if this arg is true, otherwise it's simply print on terminal</param>
<returns> ECS_OK means running correctly, a negative number indicates has error
</returns>
*/
template <typename... graph_configures>
int64_t count_user_call_nums(IN GUNDAM::Graph<graph_configures...>& graph,
                             bool is_rewrite_graph = false) {
  // temp
  // using GraphType = GUNDAM::Graph<>;
  // GraphType graph;
  // end_temp

  map<int64_t, tuple<string, string>> user_calls;
  using GraphType = GUNDAM::Graph<graph_configures...>;
  // Temporary solution: set 99 as call_out_num attribute, set 100 as
  // call_in_num attribute
  typename GraphType::VertexType::AttributeKeyType attr_key_out_num(
      "call_out_num");
  typename GraphType::VertexType::AttributeKeyType attr_key_in_num(
      "call_in_num");
  typename GraphType::VertexType::LabelType label_user(
      "User");  // user's label is 4
  typename GraphType::EdgeType::LabelType label_call(
      "CALL");  // CALL label is 0

  // count result
  for (auto vertex_it = graph.VertexBegin(label_user); !vertex_it.IsDone();
       vertex_it++) {
    int64_t out_num = 0;
    int64_t in_num = 0;
    for (auto e_it = vertex_it->OutEdgeCBegin(label_call); !e_it.IsDone();
         e_it++)
      out_num++;

    for (auto e_it = vertex_it->InEdgeCBegin(label_call); !e_it.IsDone();
         e_it++)
      in_num++;

    user_calls.insert(pair<int64_t, tuple<string, string>>(
        vertex_it->id(), make_tuple(to_string(out_num), to_string(in_num))));
  }
  // print result
  for (auto calls = user_calls.begin(); calls != user_calls.end(); calls++) {
    cout << "user id: " << calls->first
         << ", call out: " << std::get<0>(calls->second)
         << ", call in: " << std::get<1>(calls->second) << endl;
    if (is_rewrite_graph) {
      auto vertex_it = graph.FindVertex(calls->first);
      if (vertex_it != nullptr) {
        vertex_it->AddAttribute(attr_key_out_num, std::get<0>(calls->second));
        vertex_it->AddAttribute(attr_key_in_num, std::get<1>(calls->second));
      } else {
        return ECS_E_VERTEX_NOT_FOUND;
      }
    }
  }
  return ECS_OK;
}
/*
<summary>
  remove the duplicate CALL edge in graph
</summary>
<param name="graph">inited graph</param>
<returns>
        ECS_OK means running correctly, a negative number indicates has error
</returns>
*/
template <typename... graph_configures>
int64_t remove_dup_calls(INOUT GUNDAM::Graph<graph_configures...>& graph) {
  // temp
  // using graphtype = gundam::graph<>;
  // graphtype graph;
  // end_temp

  using GraphType = GUNDAM::Graph<graph_configures...>;
  typename GraphType::EdgeType::IDType max_edge_id = get_max_edge_id(graph);
  cout << "----------max edge id:" << max_edge_id << endl << endl << endl;
  typename GraphType::VertexType::AttributeKeyType attr_key_out_num(
      "call_out_num");
  typename GraphType::VertexType::AttributeKeyType attr_key_in_num(
      "call_in_num");
  typename GraphType::VertexType::LabelType label_user(
      "User");  // user's label is 4
  typename GraphType::EdgeType::LabelType label_call(
      "CALL");  // CALL label is 0

  for (auto vertex_it = graph.VertexBegin(label_user); !vertex_it.IsDone();
       vertex_it++) {
    for (auto vertex_dst_it = vertex_it->OutVertexBegin(label_call);
         !vertex_dst_it.IsDone(); vertex_dst_it++) {
      int64_t out_num = 0;
      int64_t in_num = 0;
      typename GraphType::VertexPtr vertex_dst_ptr = vertex_dst_it;
      for (auto e_it = vertex_it->OutEdgeBegin(); !e_it.IsDone(); e_it++) {
        if (e_it->dst_ptr() == vertex_dst_ptr) {
          out_num++;
          continue;
        }
      }
      for (auto e_it = vertex_it->InEdgeBegin(); !e_it.IsDone(); e_it++) {
        if (e_it->src_ptr() == vertex_dst_ptr) {
          in_num++;
        }
      }
      auto e_add_pair = graph.AddEdge(vertex_it->id(), vertex_dst_ptr->id(),
                                      label_call, ++max_edge_id);
      if (!e_add_pair.second) {
        return ECS_E_ADD_EDGE_ERROR;
      }
      e_add_pair.first->AddAttribute(attr_key_out_num, to_string(out_num));
      e_add_pair.first->AddAttribute(attr_key_in_num, to_string(in_num));

      for (auto e_it = vertex_it->OutEdgeBegin(); !e_it.IsDone();) {
        if (e_it->id() != max_edge_id && e_it->dst_ptr() == vertex_dst_ptr) {
          // TODO erase edge
          e_it = vertex_it->EraseEdge(e_it);
          continue;
        }
        e_it++;
      }
      for (auto e_it = vertex_it->InEdgeBegin(); !e_it.IsDone();) {
        if (e_it->id() != max_edge_id && e_it->src_ptr() == vertex_dst_ptr) {
          // TODO erase edge
          e_it = vertex_it->EraseEdge(e_it);
          continue;
        }
        e_it++;
      }
    }
  }
  return ECS_OK;
}
}  // namespace ECS