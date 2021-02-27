#include "fold_unfold.h"

#include <set>
#include <vector>
#include <gundam/attribute.h>

#include "select.h"

namespace gptools {

using namespace graphpackage;

// template <class Vertex>
int FoldAttribute(const AttributeDict &attr_dict, const TypeDict &type_dict,
                  const std::set<AttributeKey> &fold_attr_list,
                  // Vertex &vertex,
                  Graph::VertexPtr &vertex, std::vector<ID> &remove_vertex_list,
                  std::vector<ID> &remove_edge_list) {
  // T1 : x.a0(c0) ==> x -has-> a0_a.value(c0)|A|
  // T2 : x.a0(c0) ==> x -has-> a0_a_c0|V|
  // T3 : x.a0(c0) ==> x -has-> a0_a|V| -is-> t0_v.value(c0)|V|
  // T4 : x.a0(c0) ==> x -has-> a0_a|A| -is-> t0_v_c0|V|
  // T5 : x.a0(c0) ==> x -has-> a0_a|A| -is-> t0_v.value(c0)|V|
  // T6 : x.a0(c0) ==> x -has-> a0_a|V| -is-> t0_v|V| -equal-> t0_c_c0|V|
  // T7 : x.a0(c0) ==> x -has-> a0_a|A| -is-> t0_v|V| -equal-> t0_c_c0|V|

  int count = 0;
  for (auto has_e_iter = vertex->OutEdgeCBegin(ReservedLabel::kHas);
       !has_e_iter.IsDone(); ++has_e_iter) {
    auto attr_v = has_e_iter->const_dst_ptr();
    auto attr_iter = attr_dict.Find(attr_v->label());
    if (attr_iter == attr_dict.end()) return -1;

    if (!fold_attr_list.empty() &&
        fold_attr_list.count(attr_iter->attr_key) == 0)
      continue;

    std::string value_str;
    if (attr_iter->attr_label_id == attr_v->label()) {
      // T1, T3-T7
      auto is_e_iter = attr_v->OutEdgeCBegin(ReservedLabel::kIs);
      if (is_e_iter.IsDone()) {
        // T1
        auto attr_ptr = attr_v->FindConstAttributePtr("value");
        if (attr_ptr.IsNull()) return -1;
        value_str = attr_ptr->value_str();

        remove_vertex_list.emplace_back(attr_v->id());

      } else {
        // T3-T7
        auto value_v = is_e_iter->const_dst_ptr();
        auto type_iter = type_dict.Find(attr_iter->type_name);
        if (type_iter == type_dict.end()) return -1;

        if (type_iter->value_label_id == value_v->label()) {
          // T3, T5-T7
          auto equal_e_iter = value_v->OutEdgeCBegin(ReservedLabel::kEqual);
          if (is_e_iter.IsDone()) {
            // T3, T5
            auto attr_ptr = value_v->FindConstAttributePtr("value");
            if (attr_ptr.IsNull()) return -1;
            value_str = attr_ptr->value_str();

          } else {
            // T6, T7
            auto constant_v = equal_e_iter->const_dst_ptr();
            auto type_value_iter = type_iter->Find(constant_v->label());
            if (type_value_iter == type_iter->end()) return -1;
            value_str = type_value_iter->value_str;
          }
        } else {
          // T4
          auto type_value_iter = type_iter->Find(value_v->label());
          if (type_value_iter == type_iter->end()) return -1;
          value_str = type_value_iter->value_str;
        }

        auto attr_value_iter = attr_iter->Find(value_str);
        if (attr_value_iter == attr_iter->end()) return -1;
        if (attr_value_iter->shared_attr_vertex_id != attr_v->id()) {
          // T4, T5, T7
          remove_vertex_list.emplace_back(attr_v->id());
        } else {
          // T3, T6
          remove_edge_list.emplace_back(has_e_iter->id());
        }
      }
    } else {
      // T2
      auto attr_value_iter = attr_iter->Find(attr_v->label());
      if (attr_value_iter == attr_iter->end()) return -1;
      value_str = attr_value_iter->value_str;

      remove_edge_list.emplace_back(has_e_iter->id());
    }

    auto ret = vertex->AddAttribute(attr_iter->attr_key,
                                    attr_iter->value_data_type, value_str);
    if (!ret.second) return -1;
    ++count;
  }

  return count;
}

int FoldAttribute(GraphPackage &gp, const std::set<Label> &vertex_label_list,
                  const std::set<AttributeKey> &fold_attr_list) {
  int res;

  Graph &g = gp.graph();
  const LabelDict &label_dict = gp.label_dict();
  const AttributeDict &attr_dict = gp.attr_dict();
  const TypeDict &type_dict = gp.type_dict();

  std::vector<ID> remove_vertex_list, remove_edge_list;
  int count_attr = 0;
  if (vertex_label_list.empty()) {
    for (auto v_iter = g.VertexBegin(); !v_iter.IsDone(); ++v_iter) {
      auto label_type = label_dict.GetLabelType(v_iter->label());
      if (label_type != LabelType::kEntity &&
          label_type != LabelType::kRelation)
        continue;

      Graph::VertexPtr v_ptr = v_iter;
      res = FoldAttribute(attr_dict, type_dict, fold_attr_list, v_ptr,
                          remove_vertex_list, remove_edge_list);
      if (res < 0) return res;

      count_attr += res;
    }
  } else {
    for (const auto &v_label : vertex_label_list) {
      for (auto v_iter = g.VertexBegin(v_label); !v_iter.IsDone(); ++v_iter) {
        Graph::VertexPtr v_ptr = v_iter;
        res = FoldAttribute(attr_dict, type_dict, fold_attr_list, v_ptr,
                            remove_vertex_list, remove_edge_list);
        if (res < 0) return res;

        count_attr += res;
      }
    }
  }

  std::cout << "added attribute count - " << count_attr << std::endl;

  std::cout << "Remove Attribute vertices:" << std::endl;
  RemoveVertexByIDList(g, remove_vertex_list);

  std::cout << "Remove 'Has' edges:" << std::endl;
  RemoveEdgeByIDList(g, remove_edge_list);

  return count_attr;
}

int UnfoldAttribute(GraphPackage &gp, const std::set<Label> &vertex_label_list,
                    const std::set<AttributeKey> &unfold_attr_list,
                    int unfold_type) {
  using VertexPtr = typename Graph::VertexPtr;
  using EdgePtr = typename Graph::EdgePtr;

  // T1 : x.a0(c0) ==> x -has-> a0_a.value(c0)|A|
  // T2 : x.a0(c0) ==> x -has-> a0_a_c0|V|
  // T3 : x.a0(c0) ==> x -has-> a0_a|V| -is-> t0_v.value(c0)|V|
  // T4 : x.a0(c0) ==> x -has-> a0_a|A| -is-> t0_v_c0|V|
  // T5 : x.a0(c0) ==> x -has-> a0_a|A| -is-> t0_v.value(c0)|V|
  // T6 : x.a0(c0) ==> x -has-> a0_a|V| -is-> t0_v|V| -equal-> t0_c_c0|V|
  // T7 : x.a0(c0) ==> x -has-> a0_a|A| -is-> t0_v|V| -equal-> t0_c_c0|V|

  Graph &g = gp.graph();
  IDGen &vidgen = gp.vertex_id_gen();
  IDGen &eidgen = gp.edge_id_gen();
  const LabelDict &label_dict = gp.label_dict();
  AttributeDict &attr_dict = gp.attr_dict();
  TypeDict &type_dict = gp.type_dict();

  bool r;

  if (unfold_type < 1 || unfold_type > 7) {
    return -1;
  }

  if (unfold_attr_list.empty()) {
    return 0;
  }

  int count_v_attr_value = 0;
  int count_v_attr = 0;
  int count_v_value_constant = 0;
  int count_v_value = 0;
  int count_v_constant = 0;
  int count_e_has = 0;
  int count_e_is = 0;
  int count_e_equal = 0;

  for (const auto &attr_key : unfold_attr_list) {
    auto attr_iter = attr_dict.Find(attr_key);
    if (attr_iter == attr_dict.end()) return -1;
    auto &attr_info = *attr_iter;

    for (auto &attr_value_info : attr_info) {
      if (unfold_type == 2) {
        // Add "AttributeValueConstant" vertex
        if (attr_value_info.shared_attr_vertex_id == 0) {
          if (attr_value_info.attr_value_label_id == 0) {
            return -1;
          }
          auto v_iter = g.VertexBegin(attr_value_info.attr_value_label_id);
          if (!v_iter.IsDone()) {
            return -1;
          }
          VertexPtr attr_value_v;
          std::tie(attr_value_v, r) =
              g.AddVertex(vidgen.GetID(), attr_value_info.attr_value_label_id);
          if (!r) {
            return -1;
          }
          if (!attr_value_v
                   ->AddAttribute<std::string>(
                       "name",
                       attr_info.attr_key + "_a_" + attr_value_info.value_str)
                   .second) {
            return -1;
          }
          ++count_v_attr_value;
          attr_value_info.shared_attr_vertex_id = attr_value_v->id();
        }
      } else if (unfold_type == 3 || unfold_type == 4 || unfold_type == 5 ||
                 unfold_type == 6 || unfold_type == 7) {
        bool added_attr_v = false;
        bool added_value_v = false;
        if (unfold_type == 3 || unfold_type == 6) {
          if (attr_value_info.shared_attr_vertex_id == 0) {
            // Add "Attribute" vertex (shared)
            if (attr_info.attr_label_id == 0) {
              return -1;
            }
            VertexPtr v_attr;
            std::tie(v_attr, r) =
                g.AddVertex(vidgen.GetID(), attr_info.attr_label_id);
            if (!r) {
              return -1;
            }
            if (!v_attr->AddAttribute<std::string>("name", attr_info.attr_key)
                     .second) {
              return -1;
            }
            attr_value_info.shared_attr_vertex_id = v_attr->id();
            ++count_v_attr;

            added_attr_v = true;
          }
        }

        auto &type_info = *type_dict.Find(attr_info.type_name);
        auto &type_value_info = *type_info.Find(attr_value_info.value_str);
        if (unfold_type == 4) {
          if (type_value_info.value_vertex_id == 0) {
            // Add "ValueConstant" vertex
            if (type_value_info.value_constant_label_id == 0) {
              return -1;
            }
            VertexPtr v_value_constant;
            std::tie(v_value_constant, r) = g.AddVertex(
                vidgen.GetID(), type_value_info.value_constant_label_id);
            if (!r) {
              return -1;
            }
            if (!v_value_constant
                     ->AddAttribute<std::string>("name",
                                                 type_info.type_name + "_v_" +
                                                     type_value_info.value_str)
                     .second) {
              return -1;
            }
            type_value_info.value_vertex_id = v_value_constant->id();
            ++count_v_value_constant;

            added_value_v = true;
          }
        } else {
          assert(unfold_type == 3 || unfold_type == 5 || unfold_type == 6 ||
                 unfold_type == 7);
          if (type_value_info.value_vertex_id == 0) {
            // Add "Value" vertex
            if (type_info.value_label_id == 0) {
              return -1;
            }
            VertexPtr v_value;
            std::tie(v_value, r) =
                g.AddVertex(vidgen.GetID(), type_info.value_label_id);
            if (!r) {
              return -1;
            }
            if (!v_value
                     ->AddAttribute<std::string>("name",
                                                 type_info.type_name + "_v")
                     .second) {
              return -1;
            }
            type_value_info.value_vertex_id = v_value->id();
            ++count_v_value;

            added_value_v = true;

            if (unfold_type == 3 || unfold_type == 5) {
              if (!v_value
                       ->AddAttribute("value", type_info.value_data_type,
                                      type_value_info.value_str)
                       .second) {
                return -1;
              }
            } else {
              assert(unfold_type == 6 || unfold_type == 7);
              // Add "Constant" vertex
              if (type_value_info.constant_label_id == 0) {
                return -1;
              }
              VertexPtr v_constant;
              std::tie(v_constant, r) = g.AddVertex(
                  vidgen.GetID(), type_value_info.constant_label_id);
              if (!r) {
                return -1;
              }
              if (!v_constant
                       ->AddAttribute<std::string>(
                           "name", type_info.type_name + "_c_" +
                                       type_value_info.value_str)
                       .second) {
                return -1;
              }
              ++count_v_constant;

              // Add "equal" edge
              EdgePtr e_equal;
              std::tie(e_equal, r) =
                  g.AddEdge(type_value_info.value_vertex_id, v_constant->id(),
                            ReservedLabel::kEqual, eidgen.GetID());
              if (!r) {
                return -1;
              }
              ++count_e_equal;
            }
          }
        }

        if ((added_attr_v || added_value_v) &&
            attr_value_info.shared_attr_vertex_id != 0 &&
            type_value_info.value_vertex_id != 0) {
          assert(unfold_type == 3 || unfold_type == 6);
          // Add "is" edge
          EdgePtr e_is;
          std::tie(e_is, r) = g.AddEdge(attr_value_info.shared_attr_vertex_id,
                                        type_value_info.value_vertex_id,
                                        ReservedLabel::kIs, eidgen.GetID());
          if (!r) {
            return -1;
          }
          ++count_e_is;
        }
      } else {
        assert(unfold_type == 1);
      }
    }
  }

  for (auto v_iter = g.VertexBegin(); !v_iter.IsDone(); ++v_iter) {
    if (!vertex_label_list.empty() &&
        vertex_label_list.count(v_iter->label()) == 0)
      continue;

    for (const auto &attr_key : unfold_attr_list) {
      auto attr = v_iter->FindConstAttributePtr(attr_key);
      if (attr.IsNull()) {
        continue;
      }
      auto attr_iter = attr_dict.Find(attr_key);
      if (attr_iter == attr_dict.end()) {
        continue;
      }

      auto value_str = attr->value_str();
      const auto &attr_info = *attr_iter;
      const auto &attr_value_info = *attr_info.Find(value_str);
      const auto &type_info = *type_dict.Find(attr_info.type_name);
      const auto &type_value_info = *type_info.Find(value_str);

      if (unfold_type == 1 || unfold_type == 4 || unfold_type == 5 ||
          unfold_type == 7) {
        // Add "Attribute" vertex
        VertexPtr v_attr;
        std::tie(v_attr, r) =
            g.AddVertex(vidgen.GetID(), attr_info.attr_label_id);
        if (!r) {
          return -1;
        }
        if (!v_attr->AddAttribute<std::string>("name", attr_info.attr_key)
                 .second) {
          return -1;
        }
        ++count_v_attr;
        if (unfold_type == 1) {
          v_attr->AddAttribute("value", attr->value_type(), attr->value_str());

        } else {
          assert(unfold_type == 4 || unfold_type == 5 || unfold_type == 7);
          // Add "is" edge
          assert(type_value_info.value_vertex_id != 0);
          EdgePtr e_is;
          std::tie(e_is, r) =
              g.AddEdge(v_attr->id(), type_value_info.value_vertex_id,
                        ReservedLabel::kIs, eidgen.GetID());
          if (!r) {
            return -1;
          }
          ++count_e_is;
        }
        // Add "has" edge
        EdgePtr e_has;
        std::tie(e_has, r) = g.AddEdge(v_iter->id(), v_attr->id(),
                                       ReservedLabel::kHas, eidgen.GetID());
        if (!r) {
          return -1;
        }
        ++count_e_has;
      } else {
        assert(unfold_type == 2 || unfold_type == 3 || unfold_type == 6);
        // Add "has" edge
        EdgePtr e_has;
        std::tie(e_has, r) =
            g.AddEdge(v_iter->id(), attr_value_info.shared_attr_vertex_id,
                      ReservedLabel::kHas, eidgen.GetID());
        if (!r) {
          return -1;
        }
        ++count_e_has;
      }

      v_iter->EraseAttribute(attr_key);
    }
  }

  std::cout << "Added " << count_v_attr << " 'Attribute' vertices."
            << std::endl;
  std::cout << "Added " << count_v_value << " 'Value' vertices." << std::endl;
  std::cout << "Added " << count_v_constant << " 'Constant' vertices."
            << std::endl;
  std::cout << "Added " << count_v_attr_value
            << " 'AttributeValueConstant' vertices." << std::endl;
  std::cout << "Added " << count_v_value_constant
            << " 'ValueConstant' vertices." << std::endl;
  std::cout << "Added " << count_e_has << " 'has' edges." << std::endl;
  std::cout << "Added " << count_e_is << " 'is' edges." << std::endl;
  std::cout << "Added " << count_e_equal << " 'equal' edges." << std::endl;
  std::cout << std::endl;

  return count_e_has;
}

int FoldRelation(GraphPackage &gp, const std::set<Label> &fold_relation_list) {
  int res = FoldAttribute(gp, fold_relation_list, {});
  if (res < 0) return res;

  auto &g = gp.graph();
  IDGen &eidgen = gp.edge_id_gen();

  int count_e = 0;

  std::vector<ID> remove_list;

  for (const auto &v_label : fold_relation_list) {
    for (auto it_v = g.VertexCBegin(v_label); !it_v.IsDone(); ++it_v) {
      auto it_e_source = it_v->InEdgeCBegin(ReservedLabel::kSource);
      auto it_e_target = it_v->OutEdgeCBegin(ReservedLabel::kTarget);
      if (it_e_source.IsDone() || it_e_target.IsDone()) return -1;

      const auto &src = it_e_source->src_id();
      const auto &dst = it_e_target->dst_id();

      auto [e_r, r] = g.AddEdge(src, dst, v_label, eidgen.GetID());
      if (!r) return -1;
      ++count_e;

      res = CopyAllAttributes(it_v, e_r);
      if (res < 0) return res;

      remove_list.emplace_back(it_v->id());
    }
  }

  RemoveVertexByIDList(g, remove_list);

  std::cout << "added Edge count - " << count_e << std::endl;

  return count_e;
}

int UnfoldRelation(GraphPackage &gp,
                   const std::set<Label> &unfold_relation_list) {
  auto &g = gp.graph();
  auto &vidgen = gp.vertex_id_gen();
  auto &eidgen = gp.edge_id_gen();

  if (unfold_relation_list.empty()) return 0;

  int count_v = 0;
  int count_e = 0;

  std::vector<ID> remove_list;

  for (auto it_e = g.EdgeCBegin(); !it_e.IsDone(); ++it_e) {
    auto label = it_e->label();
    if (unfold_relation_list.find(label) == unfold_relation_list.end())
      continue;

    auto ret1 = g.AddVertex(vidgen.GetID(), label);
    if (!ret1.second) return -1;
    auto &v_r = ret1.first;
    ++count_v;

    int res = CopyAllAttributes(it_e, v_r);
    if (res < 0) return res;

    auto ret2 = g.AddEdge(it_e->src_id(), v_r->id(), ReservedLabel::kSource,
                          eidgen.GetID());
    if (!ret2.second) return -1;
    ++count_e;

    auto ret3 = g.AddEdge(v_r->id(), it_e->dst_id(), ReservedLabel::kTarget,
                          eidgen.GetID());
    if (!ret3.second) return -1;
    ++count_e;

    remove_list.emplace_back(it_e->id());
  }

  RemoveEdgeByIDList(g, remove_list);

  std::cout << "added Relation vertex count - " << count_v << std::endl;
  std::cout << "added Source/Target edge count - " << count_e << std::endl;

  return count_v;
}

}  // namespace gptools
