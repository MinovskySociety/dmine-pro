#ifndef _SELECT_H
#define _SELECT_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

#include "gptdef.h"
#include "gundam/type_getter/vertex_handle.h"

namespace gptools {

template <class IDList>
inline int RemoveVertexByIDList(Graph &g, const IDList &remove_list) {
  std::cout << "ID list count - " << remove_list.size() << std::endl;

  size_t count = 0;
  for (const auto &id : remove_list) {
    count += g.EraseVertex(id);
  }
  std::cout << "removed vertex/edge count - " << count << std::endl;

  return static_cast<int>(count);
}

template <class LabelList>
inline int RemoveVertexByLabelList(Graph &g,
                                   const LabelList &vertex_label_list) {
  std::cout << "Label list count - " << vertex_label_list.size() << std::endl;

  std::vector<ID> remove_list;
  for (const Label &label : vertex_label_list) {
    for (auto it_v = g.VertexBegin(label); !it_v.IsDone(); ++it_v) {
      remove_list.emplace_back(it_v->id());
    }
  }

  return RemoveVertexByIDList(g, remove_list);
}

template <class IDList>
inline int RemoveEdgeByIDList(Graph &g, const IDList &remove_list) {
  std::cout << "ID list count - " << remove_list.size() << std::endl;

  size_t count = 0;
  for (const auto &id : remove_list) {
    count += g.EraseEdge(id);
  }
  std::cout << "removed edge count - " << count << std::endl;

  return static_cast<int>(count);
}

#ifdef USE_GRAPH
inline int RemoveEdgeByLabel(Graph &g, const Label &edge_label,
                             const std::set<Label> &source_label_list,
                             const std::set<Label> &target_label_list) {
  std::cout << "edge label - " << edge_label << std::endl;
  std::cout << "source label count - " << source_label_list.size() << std::endl;
  std::cout << "target label count - " << target_label_list.size() << std::endl;

  int counter = 0;
  for (auto it_v = g.VertexBegin(); !it_v.IsDone(); it_v++) {
    // std::cout<<"checking vertex id: "<<it_v->id()<<std::endl;
    for (auto it_e = it_v->OutEdgeBegin(); !it_e.IsDone();) {
      // std::cout<<" wenzhi in erase edge: "<<std::endl;
      if (edge_label != 0 && it_e->label() != edge_label) {
        // std::cout<<" wenzhi here #1# "<<std::endl;
        ++it_e;
        continue;
      }
      // std::cout<<" wenzhi here #2# "<<std::endl;
      if (!source_label_list.empty() &&
          source_label_list.count(it_e->const_src_handle()->label()) == 0) {
        // std::cout<<" wenzhi here #2# "<<std::endl;
        ++it_e;
        continue;
      }
      // std::cout<<" wenzhi here #3# "<<std::endl;
      if (!target_label_list.empty() &&
          target_label_list.count(it_e->const_dst_handle()->label()) == 0) {
        // std::cout<<" wenzhi here #3# "<<std::endl;
        ++it_e;
        continue;
      }
      // std::cout<<"remove edge id: "<<it_e->id()<<std::endl;
      counter++;
      it_e = it_v->EraseEdge(it_e);
      // std::cout<<" wenzhi here "<<std::endl;
      // std::cout<<" edge iterator is done: "<<it_e.IsDone()<<std::endl;
    }
    // std::cout<<" wenzhi here "<<std::endl;
    // std::cout<<"checked vertex id: "<<it_v->id()<<std::endl;
  }
  // std::cout<<"edges removed"<<std::endl;
  std::cout << "removed edge count - " << counter << std::endl;
  return counter;
}
#else
inline int RemoveEdgeByLabel(Graph &g, const Label &edge_label,
                             const std::set<Label> &source_label_list,
                             const std::set<Label> &target_label_list) {
  std::cout << "edge label - " << edge_label << std::endl;
  std::cout << "source label count - " << source_label_list.size() << std::endl;
  std::cout << "target label count - " << target_label_list.size() << std::endl;

  std::vector<ID> remove_list;
  /// for (auto it_e = g.EdgeBegin(); !it_e.IsDone(); ++it_e) {
  /// modified by wenzhi
  for (auto it_v = g.VertexBegin(); !it_v.IsDone(); it_v++) {
    for (auto it_e = it_v->OutEdgeBegin(); !it_e.IsDone(); it_e++) {
      if (edge_label != 0 && it_e->label() != edge_label) continue;
      if (!source_label_list.empty() &&
          source_label_list.count(it_e->const_src_handle()->label()) == 0)
        continue;
      if (!target_label_list.empty() &&
          target_label_list.count(it_e->const_dst_handle()->label()) == 0)
        continue;

      remove_list.emplace_back(it_e->id());
    }
  }
  return RemoveEdgeByIDList(g, remove_list);
}
#endif  // USE_GRAPH

template <class LabelList, class AttributeKeyList>
inline int RemoveVertexAttribute(Graph &g, const LabelList &vertex_label_list,
                                 const AttributeKeyList &attr_key_list) {
  std::cout << "vertex label count - " << vertex_label_list.size() << std::endl;
  std::cout << "attribute key count - " << attr_key_list.size() << std::endl;

  size_t count = 0;
  for (const Label &label : vertex_label_list) {
    for (auto it_v = g.VertexBegin(label); !it_v.IsDone(); ++it_v) {
      for (const AttributeKey &attr_key : attr_key_list) {
        count += it_v->EraseAttribute(attr_key);
      }
    }
  }

  std::cout << "removed vertex attribute count - " << count << std::endl;
  return static_cast<int>(count);
}

inline int FindConnectedVertex(const Graph &g, std::set<ID> &connected_list) {
  std::queue<GUNDAM::VertexHandle<const Graph>::type> q;
  for (const auto &id : connected_list) {
    auto v = g.FindVertex(id);
    if (!v) return -1;
    q.emplace(v);
  }
  int count = 0;
  while (!q.empty()) {
    auto v = q.front();
    for (auto it_e = v->OutEdgeBegin(); !it_e.IsDone(); ++it_e) {
      if (connected_list.emplace(it_e->const_dst_handle()->id()).second) {
        q.emplace(it_e->const_dst_handle());
        ++count;
      }
    }
    for (auto it_e = v->InEdgeBegin(); !it_e.IsDone(); ++it_e) {
      if (connected_list.emplace(it_e->const_src_handle()->id()).second) {
        q.emplace(it_e->const_src_handle());
        ++count;
      }
    }
    q.pop();
  }

  std::cout << "count - " << count << std::endl;
  return count;
}

template <class IDList, class RemoveJudgeCallback>
inline int RemoveDisconnected(Graph &g, IDList &&start_vertex_list,
                              RemoveJudgeCallback rm_callback) {
  int res;

  std::cout << "Find connected vertex:" << std::endl;
  std::set<ID> connected_list{std::forward<IDList>(start_vertex_list)};
  res = FindConnectedVertex(g, connected_list);
  if (res < 0) return res;

  std::vector<ID> remove_list;
  for (auto it_v = g.VertexBegin(); !it_v.IsDone(); ++it_v) {
    auto id = it_v->id();
    if (connected_list.find(id) == connected_list.end() && rm_callback(it_v)) {
      remove_list.emplace_back(id);
    }
  }

  std::cout << "Remove vertex:" << std::endl;
  return static_cast<int>(RemoveVertexByIDList(g, remove_list));
}

template <class LabelList>
inline int GetVertexList(const Graph &g, const LabelList &label_list,
                         std::set<ID> &vertex_list) {
  vertex_list.clear();
  for (const auto &label : label_list) {
    for (auto it_v = g.VertexBegin(label); !it_v.IsDone(); ++it_v) {
      vertex_list.emplace(it_v->id());
    }
  }
  std::cout << "count - " << vertex_list.size() << std::endl;
  return static_cast<int>(vertex_list.size());
}

template <class LabelNameList>
inline int GetLabelList(const graphpackage::LabelDict &label_dict,
                        const LabelNameList &name_list,
                        std::set<Label> &label_list) {
  label_list.clear();
  for (const auto &label_name : name_list) {
    std::cout << "  " << label_name;
    auto label = label_dict.GetLabelID(label_name);
    if (label == -1) {
      std::cout << " not found." << std::endl;
      return -1;
    }
    std::cout << " (" << label << ")" << std::endl;
    label_list.emplace(label);
  }
  return static_cast<int>(label_list.size());
}

template <class IDList>
inline int RemoveDisconnectedEntityRelation(
    Graph &g, IDList &&start_vertex_list,
    const graphpackage::LabelDict &label_dict) {
  std::set<Label> remove_label_list;
  for (const auto &label_info : label_dict) {
    if (label_info.label_type == LabelType::kEntity ||
        label_info.label_type == LabelType::kRelation) {
      remove_label_list.emplace(label_info.label_id);
    }
  }

  return RemoveDisconnected(g, std::forward<IDList>(start_vertex_list),
                            [&remove_label_list](auto &vertex) {
                              return remove_label_list.find(vertex->label()) !=
                                     remove_label_list.end();
                            });
}

int ReadIDList(const std::filesystem::path &list_file, std::set<ID> &id_list);

int SamplingVertexByIDList(Graph &g, const Label &label,
                           const std::set<ID> &id_list);

int SamplingVertexRandom(Graph &g, const Label &label, size_t num);

}  // namespace gptools

#endif