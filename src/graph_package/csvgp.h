#ifndef _CSVGP_H
#define _CSVGP_H

#include <gundam/component/container2.h>
#include <gundam/io/csvgraph.h>
#include <gundam/io/rapidcsv.h>

#include <chrono>
#include <filesystem>

#include "graph_package.h"

namespace graphpackage {
namespace csvgp {

using LabelSet = GUNDAM::TreeSet<Label>;
using GroupDict = GUNDAM::TreeDict<std::string, LabelSet>;

std::filesystem::path GetLabelFilename(const std::string &graph_name);

std::filesystem::path GetAttributeFilename(const std::string &graph_name);

std::filesystem::path GetTypeFilename(const std::string &graph_name);

std::filesystem::path GetVertexFilename(const std::string &graph_name);

std::filesystem::path GetVertexFilename(const std::string &graph_name,
                                        const std::string &group_name);

std::filesystem::path GetEdgeFilename(const std::string &graph_name);

std::filesystem::path GetEdgeFilename(const std::string &graph_name,
                                      const std::string &group_name);

bool CheckLabelFilename(const std::filesystem::path &filename,
                        const std::string &graph_name);

bool CheckAttributeFilename(const std::filesystem::path &filename,
                            const std::string &graph_name);

bool CheckTypeFilename(const std::filesystem::path &filename,
                       const std::string &graph_name);

std::pair<std::string, bool> CheckVertexFilename(
    const std::filesystem::path &filename, const std::string &graph_name);

std::pair<std::string, bool> CheckEdgeFilename(
    const std::filesystem::path &filename, const std::string &graph_name);

int ReadLabelFile(const std::filesystem::path &label_file,
                  LabelDict &label_dict);

int WriteLabelFile(const LabelDict &label_dict,
                   const std::filesystem::path &label_file);

int ReadAttributeFile(const std::filesystem::path &attr_file,
                      AttributeDict &attr_dict);

int WriteAttributeFile(const AttributeDict &attr_dict,
                       const std::filesystem::path &attr_file);

int ReadTypeFile(const std::filesystem::path &type_file, TypeDict &type_dict);

int WriteTypeFile(const TypeDict &type_dict,
                  const std::filesystem::path &type_file);

bool CheckDict(const LabelDict &label_dict, const AttributeDict &attr_dict,
               const TypeDict &type_dict);

int BuildVertexEdgeGroups(const LabelDict &label_dict,
                          const AttributeDict &attr_dict,
                          const TypeDict &type_dict, GroupDict &vertex_groups,
                          GroupDict &edge_groups);

template <class Graph>
bool CheckGraph(const Graph &graph, const LabelDict &label_dict,
                AttributeDict &attr_dict, TypeDict &type_dict) {
  std::cout << "Check Vertex/Edge labels:" << std::endl;
  std::set<Label> all_label_set;
  for (auto v_iter = graph.VertexBegin(); !v_iter.IsDone(); ++v_iter) {
    all_label_set.insert(v_iter->label());
  }
  // for (auto e_iter = graph.EdgeBegin(); !e_iter.IsDone(); ++e_iter) {
  // modified by wenzhi
  for (auto v_iter = graph.VertexBegin(); !v_iter.IsDone(); v_iter++) {
    for (auto e_iter = v_iter->OutEdgeBegin(); !e_iter.IsDone(); ++e_iter) {
      all_label_set.insert(e_iter->label());
    }
  }

  for (const auto &label_id : all_label_set) {
    if (label_dict.Find(label_id) == label_dict.end()) {
      std::cout << label_id << "isn't found in Label Dictionary." << std::endl;
      return false;
    }
  }

  // Attribute Dict
  for (auto &attr_info : attr_dict) {
    for (auto &attr_value_info : attr_info) {
      attr_value_info.shared_attr_vertex_id = 0;
    }

    bool shared = false;

    auto attr_v_iter = graph.VertexBegin(attr_info.attr_label_id);
    if (attr_v_iter.IsDone()) {
      // AttributeValueConstant
      for (auto &attr_value_info : attr_info) {
        auto attr_value_v_iter =
            graph.VertexBegin(attr_value_info.attr_value_label_id);

        if (!attr_value_v_iter.IsDone()) {
          shared = true;

          attr_value_info.shared_attr_vertex_id = attr_value_v_iter->id();

          ++attr_value_v_iter;
          if (!attr_value_v_iter.IsDone()) return false;
        }
      }
    } else {
      // Attribute
      for (auto &attr_value_info : attr_info) {
        if (graph.CountVertex(attr_value_info.attr_value_label_id) > 0)
          return false;
      }

      while (!attr_v_iter.IsDone()) {
        if (attr_v_iter->CountInEdge(ReservedLabel::kHas) != 1) shared = true;

        auto attr_value_iter = attr_info.end();

        auto is_e_iter = attr_v_iter->OutEdgeBegin(ReservedLabel::kIs);
        if (is_e_iter.IsDone()) {
          auto attr_ptr = attr_v_iter->FindAttribute("value");
          if (attr_ptr.IsNull()) return false;

          attr_value_iter = attr_info.Find(attr_ptr->value_str());

        } else {
          auto value_ptr = is_e_iter->const_dst_handle();
          auto equal_e_iter = value_ptr->OutEdgeBegin(ReservedLabel::kEqual);
          if (equal_e_iter.IsDone()) {
            auto attr_ptr = value_ptr->FindAttribute("value");
            if (attr_ptr.IsNull()) return false;

            attr_value_iter = attr_info.Find(attr_ptr->value_str());

          } else {
            auto constant_ptr = equal_e_iter->const_dst_handle();
            auto type_iter = type_dict.Find(attr_info.type_name);
            if (type_iter == type_dict.end()) return false;
            auto type_value_iter = type_iter->Find(constant_ptr->label());
            if (type_value_iter == type_iter->end()) return false;

            attr_value_iter = attr_info.Find(type_value_iter->value_str);
          }
        }

        if (attr_value_iter == attr_info.end()) return false;

        if (shared && attr_value_iter->shared_attr_vertex_id != 0 &&
            attr_value_iter->shared_attr_vertex_id != attr_v_iter->id())
          return false;

        attr_value_iter->shared_attr_vertex_id = attr_v_iter->id();

        ++attr_v_iter;
      }
    }

    if (shared) {
      for (auto &attr_value_info : attr_info) {
        if (attr_value_info.shared_attr_vertex_id == 0) return false;
      }
    } else {
      for (auto &attr_value_info : attr_info) {
        attr_value_info.shared_attr_vertex_id = 0;
      }
    }
  }

  // Type dict
  for (auto &type_info : type_dict) {
    for (auto &type_value_info : type_info) {
      type_value_info.value_vertex_id = 0;
    }

    // std::cout << type_info.type_name << std::endl;
    // std::cout << type_info.value_label_id << std::endl;
    // std::cout << type_info.value_data_type << std::endl;

    auto value_v_iter = graph.VertexBegin(type_info.value_label_id);
    if (value_v_iter.IsDone()) {
      for (auto &type_value_info : type_info) {
        // std::cout << type_value_info.value_str << std::endl;
        // std::cout << type_value_info.constant_label_id << std::endl;
        // std::cout << type_value_info.value_constant_label_id << std::endl;

        auto value_constant_v_iter =
            graph.VertexBegin(type_value_info.value_constant_label_id);
        if (!value_constant_v_iter.IsDone()) {
          // std::cout << value_constant_v_iter->id() << std::endl;
          // std::cout << value_constant_v_iter->label() << std::endl;

          type_value_info.value_vertex_id = value_constant_v_iter->id();

          ++value_constant_v_iter;
          if (!value_constant_v_iter.IsDone()) {
            // std::cout << value_constant_v_iter->id() << std::endl;
            // std::cout << value_constant_v_iter->label() << std::endl;
            return false;
          }
        }
      }
    } else {
      do {
        auto type_value_iter = type_info.end();

        auto equal_e_iter = value_v_iter->OutEdgeBegin(ReservedLabel::kEqual);
        if (equal_e_iter.IsDone()) {
          auto attr_ptr = value_v_iter->FindAttribute("value");
          if (attr_ptr.IsNull()) return false;

          type_value_iter = type_info.Find(attr_ptr->value_str());
        } else {
          auto constant_v_ptr = equal_e_iter->const_dst_handle();

          type_value_iter = type_info.Find(constant_v_ptr->label());
        }

        if (type_value_iter == type_info.end()) return false;
        if (type_value_iter->value_vertex_id != 0 &&
            type_value_iter->value_vertex_id != value_v_iter->id())
          return false;

        type_value_iter->value_vertex_id = value_v_iter->id();

        // std::cout << type_value_iter->value_str << std::endl;
        // std::cout << type_value_iter->value_vertex_id << std::endl;

        ++value_v_iter;
      } while (!value_v_iter.IsDone());
    }
  }

  return true;
}

template <bool has_attr, class Graph>
int ReadGraph(const std::filesystem::path &dir,
              const std::vector<std::filesystem::path> &vertex_files,
              const std::vector<std::filesystem::path> &edge_files,
              Graph &graph, IDGen &vertex_id_gen, IDGen &edge_id_gen) {
  using namespace GUNDAM;

  int res;

  graph.Clear();
  vertex_id_gen.Reset();
  edge_id_gen.Reset();

  std::cout << "Vertex file:" << std::endl;
  IDGen &vidgen = vertex_id_gen;
  int count_v = 0;
  auto rv_callback = [&vidgen, &count_v](auto &vertex) -> bool {
    vidgen.UseID(vertex->id());
    ++count_v;
    return true;
  };

  for (const auto &v_file : vertex_files) {
    res = ReadCSVVertexFileWithCallback<has_attr>((dir / v_file).string(),
                                                  graph, rv_callback);
    if (res < 0) return res;
  }

  std::cout << "Edge file:" << std::endl;
  IDGen &eidgen = edge_id_gen;
  int count_e = 0;
  auto re_callback = [&eidgen, &count_e](auto &edge) -> bool {
    eidgen.UseID(edge->id());
    ++count_e;
    return true;
  };

  for (const auto &e_file : edge_files) {
    res = ReadCSVEdgeFileWithCallback<has_attr>((dir / e_file).string(), graph,
                                                re_callback);
    if (res < 0) return res;
  }

  std::cout << "Vertex: " << count_v << std::endl;
  std::cout << "Edge: " << count_e << std::endl;

  return count_v + count_e;
}

template <bool has_attr, class Graph>
int WriteGraph(const Graph &graph, const std::filesystem::path &dir,
               const std::string &name, const GroupDict &vertex_groups,
               const GroupDict &edge_groups,
               std::vector<std::filesystem::path> &vertex_files,
               std::vector<std::filesystem::path> &edge_files) {
  using namespace GUNDAM;

  if (!std::filesystem::is_directory(dir)) return -1;
  if (name.empty()) return -1;

  std::cout << "Vertex file:" << std::endl;
  vertex_files.clear();
  int count_v = 0;
  for (const auto &p : vertex_groups) {
    auto v_file = GetVertexFilename(name, p.first);
    vertex_files.emplace_back(v_file);

    const auto &label_set = p.second;
    auto wv_callback = [&label_set](auto &vertex) -> bool {
      return label_set.Find(vertex->label()) != label_set.end();
    };

    int res = WriteCSVVertexFileWithCallback<has_attr>(
        graph, (dir / v_file).string(), wv_callback);

    if (res < 0) return res;

    count_v += res;
  }

  std::cout << "Edge file:" << std::endl;
  edge_files.clear();
  int count_e = 0;
  for (const auto &p : edge_groups) {
    auto e_file = GetEdgeFilename(name, p.first);
    edge_files.emplace_back(e_file);

    const auto &label_set = p.second;
    auto we_callback = [&label_set](auto &edge) -> bool {
      return label_set.Find(edge->label()) != label_set.end();
    };

    int res = WriteCSVEdgeFileWithCallback<has_attr>(
        graph, (dir / e_file).string(), we_callback);
    if (res < 0) return res;

    count_e += res;
  }

  std::cout << "Vertex: " << count_v << std::endl;
  std::cout << "Edge: " << count_e << std::endl;

  return count_e + count_v;
}

template <bool has_attr, class Graph>
int WriteGraph(const Graph &graph, const std::filesystem::path &v_file,
               const std::filesystem::path &e_file) {
  using namespace GUNDAM;

  int res;

  res =
      WriteCSVVertexFileWithCallback<has_attr>(graph, v_file.string(), nullptr);
  if (res < 0) return res;
  int count_v = res;

  res = WriteCSVEdgeFileWithCallback<has_attr>(graph, e_file.string(), nullptr);
  if (res < 0) return res;
  int count_e = res;

  std::cout << "Vertex: " << count_v << std::endl;
  std::cout << "Edge: " << count_e << std::endl;

  return count_e + count_v;
}

template <bool has_attr, class Graph>
int WriteGraph(const Graph &graph, const std::filesystem::path &dir,
               const std::string &name) {
  if (!std::filesystem::is_directory(dir)) return -1;
  if (name.empty()) return -1;

  auto v_file = GetVertexFilename(name);
  auto e_file = GetEdgeFilename(name);

  return WriteGraph<has_attr>(graph, dir / v_file, dir / e_file);
}

int CompleteReadGraphPackageInfo(GraphPackageInfo &info);

int CompleteWriteGraphPackageInfo(GraphPackageInfo &info);

}  // namespace csvgp

template <class Graph>
int ReadDict(GraphPackage<Graph> &gp) {
  using namespace csvgp;

  int res;

  const auto &info = gp.info();

  std::cout << "Label file:" << std::endl;
  res = ReadLabelFile(info.dict_dir / info.label_file, gp.label_dict());
  if (res < 0) return res;

  std::cout << "Attribue file:" << std::endl;
  res = ReadAttributeFile(info.dict_dir / info.attr_file, gp.attr_dict());
  if (res < 0) return res;

  std::cout << "Type file:" << std::endl;
  res = ReadTypeFile(info.dict_dir / info.type_file, gp.type_dict());
  if (res < 0) return res;

  std::cout << "Check Label/Attribute/Type dictionaries: ";
  if (!CheckDict(gp.label_dict(), gp.attr_dict(), gp.type_dict())) {
    std::cout << "failed" << std::endl;
    return -1;
  }
  std::cout << "ok" << std::endl;

  return 0;
}

template <class Graph>
int ReadGraph(GraphPackage<Graph> &gp) {
  using namespace csvgp;

  int res;

  if (gp.info().has_attr) {
    res = ReadGraph<true>(gp.info().graph_dir, gp.info().vertex_files,
                          gp.info().edge_files, gp.graph(), gp.vertex_id_gen(),
                          gp.edge_id_gen());
  } else {
    res = ReadGraph<false>(gp.info().graph_dir, gp.info().vertex_files,
                           gp.info().edge_files, gp.graph(), gp.vertex_id_gen(),
                           gp.edge_id_gen());
  }
  if (res < 0) return res;

  if (gp.info().check_graph) {
    if (!CheckGraph(gp.graph(), gp.label_dict(), gp.attr_dict(),
                    gp.type_dict())) {
      return -1;
    }
  }

  return res;
}

template <class Graph>
int ReadGraphPackage(const GraphPackageInfo &info, GraphPackage<Graph> &gp) {
  int res;

  auto begin_time = std::chrono::steady_clock::now();

  std::cout << "-------------------------" << std::endl;
  std::cout << "   Read graph package" << std::endl;
  std::cout << "-------------------------" << std::endl;

  gp.info() = info;
  res = csvgp::CompleteReadGraphPackageInfo(gp.info());
  if (res < 0) return res;

  std::cout << "Name: " << gp.info().name << std::endl;
  std::cout << "Read dictionary: " << (gp.info().has_dict ? "true" : "false")
            << std::endl;
  if (gp.info().has_dict) {
    std::cout << "- Dictionary directory: " << gp.info().dict_dir.string()
              << std::endl;
  }
  std::cout << "Read graph: " << (gp.info().has_graph ? "true" : "false")
            << std::endl;
  if (gp.info().has_graph) {
    std::cout << "- Graph directory: " << gp.info().graph_dir.string()
              << std::endl;
    std::cout << "- Read attribute: " << (gp.info().has_attr ? "true" : "false")
              << std::endl;
    std::cout << "- Is grouping: " << (gp.info().is_grouping ? "true" : "false")
              << std::endl;
    std::cout << "- Check graph: " << (gp.info().check_graph ? "true" : "false")
              << std::endl;
  }

  if (gp.info().has_dict) {
    res = ReadDict(gp);
    if (res < 0) return res;
  }

  if (gp.info().has_graph) {
    res = ReadGraph(gp);
    if (res < 0) return res;
  }

  auto end_time = std::chrono::steady_clock::now();
  std::cout << "Time: "
            << std::chrono::duration<double>(end_time - begin_time).count()
            << " s" << std::endl;
  std::cout << std::endl;

  return res;
}

template <class Graph>
int WriteDict(GraphPackage<Graph> &gp) {
  using namespace csvgp;

  int res;

  auto &info = gp.info();

  std::cout << "Check Label/Attribute/Type dictionaries: ";
  if (!CheckDict(gp.label_dict(), gp.attr_dict(), gp.type_dict())) {
    std::cout << "failed" << std::endl;
    return -1;
  }
  std::cout << "ok" << std::endl;

  std::cout << "Label file:" << std::endl;
  if (info.label_file.empty()) {
    if (info.name.empty() || info.dict_dir.empty()) return -1;
    info.label_file = GetLabelFilename(info.name);
  }
  res = WriteLabelFile(gp.label_dict(), info.dict_dir / info.label_file);
  if (res < 0) return res;

  std::cout << "Attribute file:" << std::endl;
  if (info.attr_file.empty()) {
    if (info.name.empty() || info.dict_dir.empty()) return -1;
    info.attr_file = GetAttributeFilename(info.name);
  }
  res = WriteAttributeFile(gp.attr_dict(), info.dict_dir / info.attr_file);
  if (res < 0) return res;

  std::cout << "Type file:" << std::endl;
  if (info.type_file.empty()) {
    if (info.name.empty() || info.dict_dir.empty()) return -1;
    info.type_file = GetTypeFilename(info.name);
  }
  res = WriteTypeFile(gp.type_dict(), info.dict_dir / info.type_file);
  if (res < 0) return res;

  return 0;
}

template <class Graph>
int WriteGraph(GraphPackage<Graph> &gp) {
  using namespace csvgp;

  int res;

  if (gp.info().is_grouping) {
    GroupDict vertex_groups;
    GroupDict edge_groups;

    std::cout << "Build Vertex/Edge groups: ";
    res = BuildVertexEdgeGroups(gp.label_dict(), gp.attr_dict(), gp.type_dict(),
                                vertex_groups, edge_groups);
    if (res < 0) {
      std::cout << "failed" << std::endl;
      return res;
    }
    std::cout << "ok" << std::endl;

    if (gp.info().has_attr) {
      res = WriteGraph<true>(gp.graph(), gp.info().graph_dir, gp.info().name,
                             vertex_groups, edge_groups, gp.info().vertex_files,
                             gp.info().edge_files);
    } else {
      res = WriteGraph<false>(gp.graph(), gp.info().graph_dir, gp.info().name,
                              vertex_groups, edge_groups,
                              gp.info().vertex_files, gp.info().edge_files);
    }
    if (res < 0) return res;

    if (gp.info().has_skeleton) {
      res = WriteGraph<false>(gp.graph(), gp.info().graph_dir, gp.info().name);
      if (res < 0) return res;
    }

  } else {
    if (gp.info().has_attr) {
      res = WriteGraph<true>(gp.graph(), gp.info().graph_dir, gp.info().name);
    } else {
      res = WriteGraph<false>(gp.graph(), gp.info().graph_dir, gp.info().name);
    }
    if (res < 0) return res;
  }

  return res;
}

template <class Graph>
int WriteGraphPackage(const GraphPackageInfo &info, GraphPackage<Graph> &gp) {
  int res;

  auto begin_time = std::chrono::steady_clock::now();

  std::cout << "-------------------------" << std::endl;
  std::cout << "   Write graph package" << std::endl;
  std::cout << "-------------------------" << std::endl;

  gp.info() = info;
  res = csvgp::CompleteWriteGraphPackageInfo(gp.info());
  if (res < 0) return res;

  std::cout << "Name: " << gp.info().name << std::endl;
  std::cout << "Write dictionary: " << (gp.info().has_dict ? "true" : "false")
            << std::endl;
  if (gp.info().has_dict) {
    std::cout << "- Dictionary directory: " << gp.info().dict_dir.string()
              << std::endl;
  }
  std::cout << "Write graph: " << (gp.info().has_graph ? "true" : "false")
            << std::endl;
  if (gp.info().has_graph) {
    std::cout << "- Graph directory: " << gp.info().graph_dir.string()
              << std::endl;
    std::cout << "- Write attribute: "
              << (gp.info().has_attr ? "true" : "false") << std::endl;
    std::cout << "- Is grouping: " << (gp.info().is_grouping ? "true" : "false")
              << std::endl;
    std::cout << "- Write skeleton: "
              << (gp.info().has_skeleton ? "true" : "false") << std::endl;
  }

  if (gp.info().has_dict) {
    res = WriteDict(gp);
    if (res < 0) return res;
  }

  if (gp.info().has_graph) {
    res = WriteGraph(gp);
    if (res < 0) return res;
  }

  auto end_time = std::chrono::steady_clock::now();

  std::cout << "Time: "
            << std::chrono::duration<double>(end_time - begin_time).count()
            << " s" << std::endl;
  std::cout << std::endl;

  return res;
}

}  // namespace graphpackage

#endif