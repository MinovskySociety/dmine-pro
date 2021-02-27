#ifndef _LITERALSEL_H
#define _LITERALSEL_H

#include <vector>

#include "literal.h"
#include "ml_literal.h"

namespace gmine_new {
template <class BaseClass>
class BaseContainer {
 private:
  using BaseContainerType = std::vector<BaseClass *>;
  using BaseContainerIterator = typename BaseContainerType::iterator;
  using BaseContainerConstIterator = typename BaseContainerType::const_iterator;
  BaseContainerType base_container_;

 public:
  BaseContainer() { this->base_container_.clear(); }
  ~BaseContainer() {
    for (auto &ptr : this->base_container_) {
      delete ptr;
    }
    this->base_container_.clear();
  }
  BaseContainer(const BaseContainer &b) {
    this->base_container_ = b.base_container_;
  }
  BaseContainerIterator begin() { return this->base_container_.begin(); }
  BaseContainerIterator end() { return this->base_container_.end(); }
  BaseContainerConstIterator begin() const {
    return this->base_container_.cbegin();
  }
  BaseContainerConstIterator end() const {
    return this->base_container_.cend();
  }
  template <class DerivedClass, typename... Args>
  void Add(Args &&... args) {
    BaseClass *base_ptr = new DerivedClass(args...);
    this->base_container_.push_back(base_ptr);
  }
};
template <class Pattern, class DataGraph>
class LiteralSet {
 private:
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
  using DataGraphAttributeConstPtr =
      typename DataGraph::VertexAttributeConstPtr;
  using DataGraphAttributePtr = typename DataGraph::VertexAttributePtr;
  using DataGraphVertexAttributeKeyType =
      typename DataGraph::VertexType::AttributeKeyType;
  using VertexAttributePair =
      std::pair<PatternVertexIDType, DataGraphVertexAttributeKeyType>;
  using LiteralListType = std::vector<Literal<Pattern, DataGraph> *>;
  using LiteralSetIterator =
      typename std::vector<Literal<Pattern, DataGraph> *>::iterator;
  using LiteralSetConstIterator =
      typename std::vector<Literal<Pattern, DataGraph> *>::const_iterator;
  std::vector<Literal<Pattern, DataGraph> *> literal_set_;

 public:
  LiteralSet() { this->literal_set_.clear(); }
  ~LiteralSet() {
    for (auto &ptr : this->literal_set_) {
      delete ptr;
    }
    this->literal_set_.clear();
  }
  LiteralSet(const LiteralSet &b) { this->literal_set_ = b.literal_set_; }
  LiteralSetIterator begin() { return this->literal_set_.begin(); }
  LiteralSetIterator end() { return this->literal_set_.end(); }
  LiteralSetConstIterator begin() const { return this->literal_set_.cbegin(); }
  LiteralSetConstIterator end() const { return this->literal_set_.cend(); }
  // Attribute Literal
  void Add(Pattern &pattern, VertexAttributePair x) {
    Literal<Pattern, DataGraph> *literal_ptr =
        new AttributeLiteral<Pattern, DataGraph>(pattern, x.first, x.second);
    this->literal_set_.push_back(literal_ptr);
    return;
  }
  // Edge Literal
  void Add(Pattern &pattern, PatternVertexIDType src_id,
           PatternVertexIDType dst_id, DataGraphEdgeLabelType edge_label) {
    Literal<Pattern, DataGraph> *literal_ptr =
        new EdgeLiteral<Pattern, DataGraph>(pattern, src_id, dst_id,
                                            edge_label);
    this->literal_set_.push_back(literal_ptr);
    return;
  }
  // ML Literal
  /*
  void Add(Pattern &pattern, PatternVertexIDType src_id,
           PatternVertexIDType dst_id, DataGraphEdgeLabelType edge_label,
           std::string module_url) {
    Literal<Pattern, DataGraph> *literal_ptr =
        new MLLiteral<Pattern, DataGraph>(pattern, src_id, dst_id, edge_label,
                                          module_url);
    this->literal_set_.push_back(literal_ptr);
    return;
  }
  */
  // Variable Literal
  void Add(Pattern &pattern, VertexAttributePair x, VertexAttributePair y) {
    Literal<Pattern, DataGraph> *literal_ptr =
        new VariableLiteral<Pattern, DataGraph>(pattern, x.first, x.second,
                                                y.first, y.second);
    this->literal_set_.push_back(literal_ptr);
    return;
  }
  // Constant Literal
  template <typename ConstantType>
  void Add(Pattern &pattern, VertexAttributePair x, ConstantType c) {
    Literal<Pattern, DataGraph> *literal_ptr =
        new ConstantLiteral<Pattern, DataGraph, ConstantType>(pattern, x.first,
                                                              x.second, c);
    this->literal_set_.push_back(literal_ptr);
    return;
  }
};
};  // namespace gmine_new
#endif