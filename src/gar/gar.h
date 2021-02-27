#ifndef _GAR_H
#define _GAR_H

#include "gundam/container2.h"
#include "gundam/geneator.h"
//#include "gundam/graph.h"
#include "gundam/rapidcsv.h"
#include "gundam/vf2.h"

#include "literal.h"
//#include "literalset.h"

namespace gmine_new {

template <class Pattern, class DataGraph>
class GraphAssociationRule {
 public:
  using PatternType = Pattern;
  using DataGraphType = DataGraph;

 public:
  using PatternVertexType = typename Pattern::VertexType;
  using PatternVertexPtr = typename Pattern::VertexPtr;
  using PatternVertexIDType = typename Pattern::VertexType::IDType;
  using PatternVertexConstPtr = typename Pattern::VertexConstPtr;

  using DataGraphVertexIDType = typename DataGraph::VertexType::IDType;
  using DataGraphVertexType = typename DataGraph::VertexType;
  using DataGraphVertexPtr = typename DataGraph::VertexPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  using DataGraphEdgeType = typename DataGraph::EdgeType;
  using DataGraphEdgeLabelType = typename DataGraphEdgeType::LabelType;
  using DataGraphVertexAttributeKeyType =
      typename DataGraph::VertexType::AttributeKeyType;

  using VertexAttributePair =
      std::pair<PatternVertexIDType, DataGraphVertexAttributeKeyType>;

  using LiteralSetType = GUNDAM::PointerVector<Literal<Pattern, DataGraph>>;

 public:
  GraphAssociationRule(){};

  //template <class UPattern>
  //GraphAssociationRule(UPattern &&pattern)
  //    : pattern_(std::forward<UPattern>(pattern)){};

  GraphAssociationRule(const Pattern &pattern) : pattern_(pattern){};

  GraphAssociationRule(const GraphAssociationRule &b) {
    pattern_ = b.pattern_;
    x_literal_set_ = b.x_literal_set_;
    y_literal_set_ = b.y_literal_set_;
    for (auto &ptr : x_literal_set_) {
      ptr = ptr->Copy(pattern_);
    }
    for (auto &ptr : y_literal_set_) {
      ptr = ptr->Copy(pattern_);
    }
  }

  ~GraphAssociationRule() {}

  template <class T, typename... Args>
  void AddX(Args... args) {
    x_literal_set_.template Add<T>(pattern_, args...);
    // x_literal_set_.Add<T>(pattern_, args...);
  }

  template <class T, typename... Args>
  void AddY(Args... args) {
    // std::cout << "before add y id1 ptr =" <<
    // pattern_.FindConstVertex(1)
    //          << std::endl;
    y_literal_set_.template Add<T>(pattern_, args...);
    // y_literal_set_.Add<T>(pattern_, args...);
  }

  /*
  // attribute literal
  void AddX(VertexAttributePair x) {
    x_literal_set_.Add<AttributeLiteral<Pattern, DataGraph>>(
        pattern_, x);
    return;
  }
  // edge literal
  void AddX(PatternVertexIDType src_id, PatternVertexIDType dst_id,
            DataGraphEdgeLabelType edge_label) {
    x_literal_set_.Add(pattern_, src_id, dst_id, edge_label);
    return;
  }
  // ML Literal
  void AddX(PatternVertexIDType src_id, PatternVertexIDType dst_id,
            DataGraphEdgeLabelType edge_label, std::string module_url) {
    x_literal_set_.Add(pattern_, src_id, dst_id, edge_label,
                             module_url);
    return;
  }
  // variable literal
  void AddX(VertexAttributePair x, VertexAttributePair y) {
    x_literal_set_.Add(pattern_, x, y);
    return;
  }
  // constant literal
  template <typename ConstantType>
  void AddX(VertexAttributePair x, ConstantType c) {
    x_literal_set_.Add(pattern_, x, c);
    return;
  }
  // attribute literal
  void AddY(VertexAttributePair x) {
    y_literal_set_.Add(pattern_, x);
    return;
  }
  // edge literal
  void AddY(PatternVertexIDType src_id, PatternVertexIDType dst_id,
            DataGraphEdgeLabelType edge_label) {
    y_literal_set_.Add(pattern_, src_id, dst_id, edge_label);
    return;
  }
  // ML Literal
  void AddY(PatternVertexIDType src_id, PatternVertexIDType dst_id,
            DataGraphEdgeLabelType edge_label, std::string module_url) {
    y_literal_set_.Add(pattern_, src_id, dst_id, edge_label,
                             module_url);
    return;
  }
  // variable literal
  void AddY(VertexAttributePair x, VertexAttributePair y) {
    y_literal_set_.Add(pattern_, x, y);
    return;
  }
  // constant literal
  template <typename ConstantType>
  void AddY(VertexAttributePair x, ConstantType c) {
    y_literal_set_.Add(pattern_, x, c);
    return;
  }
  */

  void Reset() {
    x_literal_set_.Clear();
    y_literal_set_.Clear();
    pattern_.Clear();
  }

  template <class UPattern>
  void SetPattern(UPattern &&pattern) {
    pattern_ = std::forward<UPattern>(pattern);
  }

  Pattern &pattern() { return pattern_; }
  const Pattern &pattern() const { return pattern_; }

  LiteralSetType &x_literal_set() { return x_literal_set_; }
  const LiteralSetType &x_literal_set() const { return x_literal_set_; }

  LiteralSetType &y_literal_set() { return y_literal_set_; }
  const LiteralSetType &y_literal_set() const { return y_literal_set_; }

 private:
  PatternType pattern_;
  LiteralSetType x_literal_set_, y_literal_set_;
};
}  // namespace gmine_new
#endif