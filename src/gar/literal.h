#ifndef _LITERAL_H
#define _LITERAL_H

#include <gundam/datatype.h>
#include <gundam/geneator.h>

#include <map>
#include <set>

namespace gmine_new {

template <class Pattern, class DataGraph>
class Literal {
 public:
  using PatternVertexType = typename Pattern::VertexType;
  using PatternVertexPtr = typename Pattern::VertexPtr;
  using PatternVertexIDType = typename Pattern::VertexType::IDType;
  using PatternVertexConstPtr = typename Pattern::VertexConstPtr;

  using DataGraphIDType = typename DataGraph::VertexType::IDType;
  using DataGraphVertexType = typename DataGraph::VertexType;
  using DataGraphVertexIDType = typename DataGraph::VertexType::IDType;
  using DataGraphVertexPtr = typename DataGraph::VertexPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  using DataGraphEdgeType = typename DataGraph::EdgeType;
  using DataGraphEdgeIDType = typename DataGraph::EdgeType::IDType;
  using DataGraphEdgeLabelType = typename DataGraphEdgeType::LabelType;
  using DataGraphAttributeConstPtr =
      typename DataGraph::VertexType::AttributeConstPtr;
  using DataGraphAttributePtr = typename DataGraph::VertexType::AttributePtr;
  using DataGraphVertexAttributeKeyType =
      typename DataGraph::VertexType::AttributeKeyType;
  using DataGraphEdgePtr = typename DataGraph::EdgeConstPtr;
  virtual ~Literal() {}
  virtual bool Satisfy(const std::map<PatternVertexConstPtr,
                                      DataGraphVertexConstPtr> &) const = 0;
  virtual bool Update(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result,
      DataGraph &data_graph,
      GUNDAM::SimpleArithmeticIDGenerator<DataGraphEdgeIDType> &edge_id_gen,
      std::set<DataGraphIDType> *diff_vertex_attr_set = nullptr,
      std::set<DataGraphEdgeIDType> *diff_edge_set = nullptr) const = 0;
  virtual Literal<Pattern, DataGraph> *Copy(Pattern &pattern) const = 0;
  virtual void Print(std::ostream &out) const = 0;
  virtual void CalPivot(std::set<PatternVertexConstPtr> &pivot) const = 0;
};

// x.A
template <class Pattern, class DataGraph>
class AttributeLiteral : public Literal<Pattern, DataGraph> {
 private:
  using BaseLiteralType = Literal<Pattern, DataGraph>;
  using PatternVertexConstPtr = typename BaseLiteralType::PatternVertexConstPtr;
  using PatternVertexIDType = typename BaseLiteralType::PatternVertexIDType;
  using DataGraphVertexConstPtr =
      typename BaseLiteralType::DataGraphVertexConstPtr;
  using DataGraphVertexPtr = typename BaseLiteralType::DataGraphVertexPtr;
  using DataGraphVertexAttributeKeyType =
      typename BaseLiteralType::DataGraphVertexAttributeKeyType;
  using DataGraphEdgeIDType = typename BaseLiteralType::DataGraphEdgeIDType;
  using DataGraphEdgePtr = typename DataGraph::EdgeConstPtr;
  using DataGraphVertexIDType = typename BaseLiteralType::DataGraphIDType;
  PatternVertexConstPtr vertex_ptr_;
  DataGraphVertexAttributeKeyType attr_key_;

 public:
  AttributeLiteral(Pattern &pattern, PatternVertexIDType vertex_id,
                   DataGraphVertexAttributeKeyType attr_key) {
    this->vertex_ptr_ = pattern.FindConstVertex(vertex_id);
    this->attr_key_ = attr_key;
  }
  ~AttributeLiteral() {}
  virtual bool Satisfy(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result) const override {
    DataGraphVertexConstPtr match_ptr =
        match_result.find(this->vertex_ptr_)->second;
    if (match_ptr->FindConstAttributePtr(this->attr_key_).IsNull()) {
      return false;
    }
    return true;
  }
  virtual bool Update(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result,
      DataGraph &data_graph,
      GUNDAM::SimpleArithmeticIDGenerator<DataGraphEdgeIDType> &edge_id_gen,
      std::set<DataGraphVertexIDType> *diff_vertex_attr_set = nullptr,
      std::set<DataGraphEdgeIDType> *diff_edge_set = nullptr) const override {
    DataGraphVertexPtr data_graph_vertex_ptr = data_graph.FindVertex(
        match_result.find(this->vertex_ptr_)->second->id());
    if (!(data_graph_vertex_ptr->FindAttributePtr(this->attr_key_)).IsNull()) {
      return false;
    }
    data_graph_vertex_ptr->AddAttribute(this->attr_key_, std::string{"#"});
    if (diff_vertex_attr_set != nullptr) {
      diff_vertex_attr_set->insert(data_graph_vertex_ptr->id());
    }
    return true;
  }
  virtual Literal<Pattern, DataGraph> *Copy(Pattern &pattern) const override {
    PatternVertexIDType vertex_id = this->vertex_ptr_->id();
    Literal<Pattern, DataGraph> *literal_ptr =
        new AttributeLiteral<Pattern, DataGraph>(pattern, vertex_id,
                                                 this->attr_key_);
    return literal_ptr;
  }
  virtual void Print(std::ostream &out) const override {
    out << "Attribute," << this->vertex_ptr_->id() << "," << this->attr_key_
        << ",,,," << std::endl;
  }
  virtual void CalPivot(std::set<PatternVertexConstPtr> &pivot) const override {
    pivot.insert(vertex_ptr_);
  }
};

// x -l-> y
template <class Pattern, class DataGraph>
class EdgeLiteral : public Literal<Pattern, DataGraph> {
 private:
  using BaseLiteralType = Literal<Pattern, DataGraph>;
  using PatternVertexConstPtr = typename BaseLiteralType::PatternVertexConstPtr;
  using PatternVertexIDType = typename BaseLiteralType::PatternVertexIDType;
  using DataGraphVertexIDType = typename BaseLiteralType::DataGraphVertexIDType;
  using DataGraphVertexConstPtr =
      typename BaseLiteralType::DataGraphVertexConstPtr;
  using DataGraphVertexPtr = typename BaseLiteralType::DataGraphVertexPtr;
  using DataGraphEdgeLabelType =
      typename BaseLiteralType::DataGraphEdgeLabelType;
  using DataGraphEdgeIDType = typename BaseLiteralType::DataGraphEdgeIDType;
  using DataGraphEdgePtr = typename DataGraph::EdgeConstPtr;
  using DataGraphVertexAttributeKeyType =
      typename DataGraph::VertexType::AttributeKeyType;

 public:
  EdgeLiteral(Pattern &pattern, PatternVertexIDType src_id,
              PatternVertexIDType dst_id, DataGraphEdgeLabelType edge_label)
      : src_ptr_{pattern.FindConstVertex(src_id)},
        dst_ptr_{pattern.FindConstVertex(dst_id)},
        edge_label_{edge_label} {
    assert(src_ptr_);
    assert(dst_ptr_);
  }

  virtual bool Satisfy(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result) const override {
    auto &match_src_ptr = match_result.find(this->src_ptr_)->second;
    auto &match_dst_ptr = match_result.find(this->dst_ptr_)->second;
    for (auto it = match_src_ptr->OutEdgeCBegin(this->edge_label_);
         !it.IsDone(); ++it) {
      if (it->const_dst_ptr() == match_dst_ptr) return true;
    }
    return false;
  }

  virtual bool Update(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result,
      DataGraph &data_graph,
      GUNDAM::SimpleArithmeticIDGenerator<DataGraphEdgeIDType> &edge_id_gen,
      std::set<DataGraphVertexIDType> *diff_vertex_attr_set = nullptr,
      std::set<DataGraphEdgeIDType> *diff_edge_set = nullptr) const override {
    auto &match_src_ptr = match_result.find(this->src_ptr_)->second;
    auto &match_dst_ptr = match_result.find(this->dst_ptr_)->second;
    auto [edge_ptr, r] =
        data_graph.AddEdge(match_src_ptr->id(), match_dst_ptr->id(),
                           this->edge_label_, edge_id_gen.GetID());
    if (!r) return false;
    if (diff_edge_set) {
      diff_edge_set->insert(edge_ptr->id());
    }
    return true;
  }

  virtual Literal<Pattern, DataGraph> *Copy(Pattern &pattern) const override {
    PatternVertexIDType src_id = this->src_ptr_->id();
    PatternVertexIDType dst_id = this->dst_ptr_->id();
    Literal<Pattern, DataGraph> *literal_ptr =
        new EdgeLiteral<Pattern, DataGraph>(pattern, src_id, dst_id,
                                            this->edge_label_);
    return literal_ptr;
  }

  virtual void Print(std::ostream &out) const override {
    out << "Edge," << this->src_ptr_->id() << ",," << this->dst_ptr_->id()
        << ",," << this->edge_label_ << "," << std::endl;
  }

  virtual void CalPivot(std::set<PatternVertexConstPtr> &pivot) const override {
    pivot.insert(this->src_ptr_);
    pivot.insert(this->dst_ptr_);
  }

 private:
  PatternVertexConstPtr src_ptr_, dst_ptr_;
  DataGraphEdgeLabelType edge_label_;
};

// x.A = y.B
template <class Pattern, class DataGraph>
class VariableLiteral : public Literal<Pattern, DataGraph> {
 private:
  using BaseLiteralType = Literal<Pattern, DataGraph>;
  using PatternVertexConstPtr = typename BaseLiteralType::PatternVertexConstPtr;
  using PatternVertexIDType = typename BaseLiteralType::PatternVertexIDType;
  using DataGraphVertexConstPtr =
      typename BaseLiteralType::DataGraphVertexConstPtr;
  using DataGraphVertexPtr = typename BaseLiteralType::DataGraphVertexPtr;
  using DataGraphVertexAttributeKeyType =
      typename BaseLiteralType::DataGraphVertexAttributeKeyType;
  using DataGraphAttributeConstPtr =
      typename BaseLiteralType::DataGraphAttributeConstPtr;
  using DataGraphAttributePtr = typename BaseLiteralType::DataGraphAttributePtr;
  using DataGraphEdgeIDType = typename BaseLiteralType::DataGraphEdgeIDType;
  using DataGraphEdgePtr = typename DataGraph::EdgeConstPtr;
  using DataGraphVertexIDType = typename BaseLiteralType::DataGraphIDType;
  std::pair<PatternVertexConstPtr, DataGraphVertexAttributeKeyType> x_, y_;

 public:
  VariableLiteral(Pattern &pattern, PatternVertexIDType x_id,
                  DataGraphVertexAttributeKeyType x_attr_key,
                  PatternVertexIDType y_id,
                  DataGraphVertexAttributeKeyType y_attr_key) {
    this->x_.first = pattern.FindConstVertex(x_id);
    this->x_.second = x_attr_key;
    this->y_.first = pattern.FindConstVertex(y_id);
    this->y_.second = y_attr_key;
  }
  ~VariableLiteral() {}
  // x.A = y.B  A.value_type 是否等于 B.value_type?
  // 目前默认两个value_type相等
  virtual bool Satisfy(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result) const override {
    DataGraphVertexConstPtr match_x_ptr =
        match_result.find(this->x_.first)->second;
    DataGraphVertexConstPtr match_y_ptr =
        match_result.find(this->y_.first)->second;
    DataGraphAttributeConstPtr x_attr_ptr =
        match_x_ptr->FindConstAttributePtr(this->x_.second);
    DataGraphAttributeConstPtr y_attr_ptr =
        match_y_ptr->FindConstAttributePtr(this->y_.second);
    if (x_attr_ptr.IsNull() || y_attr_ptr.IsNull()) return false;
    GUNDAM::BasicDataType x_value_type = x_attr_ptr->value_type();
    GUNDAM::BasicDataType y_value_type = y_attr_ptr->value_type();
    if (x_value_type != y_value_type) return false;
    switch (x_value_type) {
      case GUNDAM::BasicDataType::kTypeInt:
        if (x_attr_ptr->template const_value<int>() !=
            y_attr_ptr->template const_value<int>()) {
          return false;
        }
        break;
      case GUNDAM::BasicDataType::kTypeDouble:
        if (x_attr_ptr->template const_value<double>() !=
            y_attr_ptr->template const_value<double>()) {
          return false;
        }
        break;
      case GUNDAM::BasicDataType::kTypeString:
        if (x_attr_ptr->template const_value<std::string>() !=
            y_attr_ptr->template const_value<std::string>()) {
          return false;
        }
        break;
      default:
        break;
    }
    return true;
  }
  virtual bool Update(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result,
      DataGraph &data_graph,
      GUNDAM::SimpleArithmeticIDGenerator<DataGraphEdgeIDType> &edge_id_gen,
      std::set<DataGraphVertexIDType> *diff_vertex_attr_set = nullptr,
      std::set<DataGraphEdgeIDType> *diff_edge_set = nullptr) const override {
    DataGraphVertexPtr x_ptr =
        data_graph.FindVertex(match_result.find(this->x_.first)->second->id());
    DataGraphVertexPtr y_ptr =
        data_graph.FindVertex(match_result.find(this->y_.first)->second->id());
    DataGraphAttributePtr x_attr_ptr = x_ptr->FindAttributePtr(this->x_.second);
    DataGraphAttributePtr y_attr_ptr = y_ptr->FindAttributePtr(this->y_.second);
    if (y_attr_ptr.IsNull()) {
      y_ptr->AddAttribute(this->y_.second, std::string{"#"});
    }
    if (x_attr_ptr.IsNull()) {
      x_ptr->AddAttribute(this->x_.second, std::string{"#"});
    }
    GUNDAM::BasicDataType x_value_type = x_attr_ptr->value_type();
    GUNDAM::BasicDataType y_value_type = y_attr_ptr->value_type();
    bool x_empty_flag = (x_value_type == GUNDAM::BasicDataType::kTypeString &&
                         x_attr_ptr->template value<std::string>() == "#");
    bool y_empty_flag = (y_value_type == GUNDAM::BasicDataType::kTypeString &&
                         y_attr_ptr->template value<std::string>() == "#");
    if ((!x_empty_flag) && (!y_empty_flag)) {
      //两个非空 只会矛盾
      return false;
    }
    if (x_empty_flag) {
      //如果x为空 就把y.B赋值给x.A
      switch (y_value_type) {
        case GUNDAM::BasicDataType::kTypeInt:
          x_ptr->SetAttribute(this->x_.second,
                              y_attr_ptr->template value<int>());
          break;
        case GUNDAM::BasicDataType::kTypeDouble:
          x_ptr->SetAttribute(this->x_.second,
                              y_attr_ptr->template value<double>());
          break;
        case GUNDAM::BasicDataType::kTypeString:
          x_ptr->SetAttribute(this->x_.second,
                              y_attr_ptr->template value<std::string>());
          break;
        default:
          break;
      }
      if (diff_vertex_attr_set != nullptr) {
        diff_vertex_attr_set->insert(x_ptr->id());
      }
      return true;
    }
    //否则，将x.A赋值给y.B
    switch (x_value_type) {
      case GUNDAM::BasicDataType::kTypeInt:
        y_ptr->SetAttribute(this->y_.second, x_attr_ptr->template value<int>());
        break;
      case GUNDAM::BasicDataType::kTypeDouble:
        y_ptr->SetAttribute(this->y_.second,
                            x_attr_ptr->template value<double>());
        break;
      case GUNDAM::BasicDataType::kTypeString:
        y_ptr->SetAttribute(this->y_.second,
                            x_attr_ptr->template value<std::string>());
        break;
      default:
        break;
    }
    if (diff_vertex_attr_set != nullptr) {
      diff_vertex_attr_set->insert(y_ptr->id());
    }
    return true;
  }
  virtual Literal<Pattern, DataGraph> *Copy(Pattern &pattern) const override {
    PatternVertexIDType x_id = this->x_.first->id();
    PatternVertexIDType y_id = this->y_.first->id();
    Literal<Pattern, DataGraph> *literal_ptr =
        new VariableLiteral<Pattern, DataGraph>(pattern, x_id, this->x_.second,
                                                y_id, this->y_.second);
    return literal_ptr;
  }
  virtual void Print(std::ostream &out) const override {
    out << "Variable," << this->x_.first->id() << "," << this->x_.second << ",";
    out << this->y_.first->id() << "," << this->y_.second << ",";
    out << "," << std::endl;
  }
  virtual void CalPivot(std::set<PatternVertexConstPtr> &pivot) const override {
    pivot.insert(this->x_.first);
    pivot.insert(this->y_.first);
  }
};

// x.A = c
template <class Pattern, class DataGraph, typename ConstantType>
class ConstantLiteral : public Literal<Pattern, DataGraph> {
  using BaseLiteralType = Literal<Pattern, DataGraph>;
  using PatternVertexConstPtr = typename BaseLiteralType::PatternVertexConstPtr;
  using PatternVertexIDType = typename BaseLiteralType::PatternVertexIDType;
  using DataGraphVertexConstPtr =
      typename BaseLiteralType::DataGraphVertexConstPtr;
  using DataGraphVertexPtr = typename BaseLiteralType::DataGraphVertexPtr;
  using DataGraphVertexAttributeKeyType =
      typename BaseLiteralType::DataGraphVertexAttributeKeyType;
  using DataGraphAttributeConstPtr =
      typename BaseLiteralType::DataGraphAttributeConstPtr;
  using DataGraphAttributePtr = typename BaseLiteralType::DataGraphAttributePtr;
  using DataGraphEdgeIDType = typename BaseLiteralType::DataGraphEdgeIDType;
  using DataGraphEdgePtr = typename DataGraph::EdgeConstPtr;
  using DataGraphVertexIDType = typename BaseLiteralType::DataGraphIDType;

 private:
  std::pair<PatternVertexConstPtr, DataGraphVertexAttributeKeyType> x_;
  ConstantType c_;

 public:
  ConstantLiteral(Pattern &pattern, PatternVertexIDType x_id,
                  DataGraphVertexAttributeKeyType attr_key, ConstantType c) {
    this->x_.first = pattern.FindConstVertex(x_id);
    this->x_.second = attr_key;
    this->c_ = c;
  }
  ~ConstantLiteral() {}
  virtual bool Satisfy(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result) const override {
    DataGraphVertexConstPtr match_x_ptr =
        match_result.find(this->x_.first)->second;
    DataGraphAttributeConstPtr x_attr_ptr =
        match_x_ptr->FindConstAttributePtr(this->x_.second);
    if (x_attr_ptr.IsNull()) return false;
    if (x_attr_ptr->template const_value<ConstantType>() != this->c_)
      return false;
    return true;
  }
  virtual bool Update(
      const std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
          &match_result,
      DataGraph &data_graph,
      GUNDAM::SimpleArithmeticIDGenerator<DataGraphEdgeIDType> &edge_id_gen,
      std::set<DataGraphVertexIDType> *diff_vertex_attr_set = nullptr,
      std::set<DataGraphEdgeIDType> *diff_edge_set = nullptr) const override {
    DataGraphVertexPtr x_ptr =
        data_graph.FindVertex(match_result.find(this->x_.first)->second->id());
    DataGraphAttributePtr x_attr_ptr = x_ptr->FindAttributePtr(this->x_.second);
    if (x_attr_ptr.IsNull()) {
      x_ptr->AddAttribute(this->x_.second, std::string{"#"});
    }
    GUNDAM::BasicDataType x_value_type = x_attr_ptr->value_type();
    bool x_empty_flag = (x_value_type == GUNDAM::BasicDataType::kTypeString &&
                         x_attr_ptr->template value<std::string>() == "#");
    if ((!x_empty_flag) &&
        x_attr_ptr->template const_value<ConstantType>() != this->c_) {
      return false;
    }
    x_ptr->SetAttribute(this->x_.second, this->c_);
    if (diff_vertex_attr_set != nullptr) {
      diff_vertex_attr_set->insert(x_ptr->id());
    }
    return true;
  }
  virtual Literal<Pattern, DataGraph> *Copy(Pattern &pattern) const override {
    PatternVertexIDType x_id = this->x_.first->id();
    Literal<Pattern, DataGraph> *literal_ptr =
        new ConstantLiteral<Pattern, DataGraph, ConstantType>(
            pattern, x_id, this->x_.second, this->c_);
    return literal_ptr;
  }
  virtual void Print(std::ostream &out) const override {
    out << "Constant," << this->x_.first->id() << "," << this->x_.second << ",";
    out << ",,,";
    std::string value_type = GUNDAM::TypeToString(this->c_);
    out << this->c_ << ";" << value_type << std::endl;
  }
  virtual void CalPivot(std::set<PatternVertexConstPtr> &pivot) const override {
    pivot.insert(this->x_.first);
  }
};
};  // namespace gmine_new

#endif