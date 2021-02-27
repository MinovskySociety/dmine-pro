#ifndef _ML_LITERAL_H
#define _ML_LITERAL_H

#include "literal.h"
//#include <xmlrpc-c/base.hpp>
//#include <xmlrpc-c/client_simple.hpp>
//#include <xmlrpc-c/girerr.hpp>

namespace gmine_new {

template <class Pattern, class DataGraph>
class MLLiteral : public Literal<Pattern, DataGraph> {
 private:
  using BaseLiteralType = Literal<Pattern, DataGraph>;
  using PatternVertexConstPtr = typename BaseLiteralType::PatternVertexConstPtr;
  using DataGraphVertexConstPtr =
      typename BaseLiteralType::DataGraphVertexConstPtr;
  using DataGraphEdgeIDType = typename BaseLiteralType::DataGraphEdgeIDType;
  using PatternVertexIDType = typename BaseLiteralType::PatternVertexIDType;
  using DataGraphVertexIDType = typename BaseLiteralType::DataGraphVertexIDType;
  using DataGraphEdgeLabelType =
      typename BaseLiteralType::DataGraphEdgeLabelType;

 public:
  MLLiteral(Pattern &pattern, PatternVertexIDType src_id,
            PatternVertexIDType dst_id, DataGraphEdgeLabelType edge_label,
            std::string module_url) {
    this->src_ptr_ = pattern.FindConstVertex(src_id);
    this->dst_ptr_ = pattern.FindConstVertex(dst_id);
    this->edge_label_ = edge_label;
    this->module_url_ = module_url;
  }

  ~MLLiteral() {}

  virtual bool Satisfy(std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>
                           &match_result) const override {
    // xmlrpc_c::clientSimple rpc_client;
    // xmlrpc_c::value result;
    // xmlrpc_c::paramList para;

    // rpc_client.call(url_, method_, para, &result);

    // string const str = xmlrpc_c::value_string(result);
    return true;
  }

  virtual bool Update(const std::map<PatternVertexConstPtr,
                                     DataGraphVertexConstPtr> &match_result,
                      DataGraph &data_graph,
                      GUNDAM::SimpleArithmeticIDGenerator<DataGraphEdgeIDType>
                          &edge_id_gen) const override;
  virtual Literal<Pattern, DataGraph> *Copy(Pattern &pattern) const override {
    PatternVertexIDType src_id = this->src_ptr_->id();
    PatternVertexIDType dst_id = this->dst_ptr_->id();
    Literal<Pattern, DataGraph> *literal_ptr =
        new MLLiteral<Pattern, DataGraph>(pattern, src_id, dst_id,
                                          this->edge_label_, this->module_url_);
    return literal_ptr;
  }

  virtual void Print(std::ofstream &out) const override {
    // out << "ML," << this->src_ptr_->id() << ",," << this->dst_ptr_->id() <<
    // ",,"
    //    << this->edge_label_ << "," << std::endl;
  }

  virtual void CalPivot(std::set<PatternVertexConstPtr> &pivot) const override {
    pivot.insert(this->src_ptr_);
    pivot.insert(this->dst_ptr_);
  }

 private:
  std::string url_;
  std::string method_;
  PatternVertexConstPtr src_ptr_, dst_ptr_;
  DataGraphEdgeLabelType edge_label_;
};
}  // namespace gmine_new

#endif