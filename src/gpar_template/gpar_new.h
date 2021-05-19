#ifndef _GPARNEW_H
#define _GPARNEW_H

#include <string>
#include <vector>

#include "gundam/io/csvgraph.h"
#include "gundam/type_getter/edge_handle.h"
#include "gundam/type_getter/vertex_handle.h"
//#include "gundam/graph.h"
namespace gmine_new {
template <class Pattern>
class GPAR {
 public:
  typedef Pattern value_type;
  using VertexType = typename Pattern::VertexType;
  using VertexIDType = typename VertexType::IDType;
  using VertexLabelType = typename VertexType::LabelType;
  using EdgeType = typename Pattern::EdgeType;
  using EdgeIDType = typename EdgeType::IDType;
  using EdgeLabelType = typename EdgeType::LabelType;
  using VertexPtr = typename GUNDAM::VertexHandle<Pattern>::type;
  using VertexConstPtr = typename GUNDAM::VertexHandle<const Pattern>::type;
  using EdgePtr = typename GUNDAM::EdgeHandle<Pattern>::type;
  using EdgeConstPtr = typename GUNDAM::EdgeHandle<const Pattern>::type;
  using VertexSizeType = size_t;

  GPAR() = default;
  // init x_node_id = 1 and y_node_id = 2
  GPAR(const VertexLabelType x, const VertexLabelType y,
       const EdgeLabelType q) {
    this->pattern.AddVertex(1, x);
    this->pattern.AddVertex(2, y);
    this->x_node_ptr_ = this->pattern.FindVertex(1);
    this->y_node_ptr_ = this->pattern.FindVertex(2);
    this->q_edge_label_ = q;
  }
  // another constructor
  GPAR(const Pattern& pattern, const VertexIDType x_node_id,
       const VertexIDType y_node_id, const EdgeLabelType q) {
    this->pattern = pattern;
    this->x_node_ptr_ = this->pattern.FindVertex(x_node_id);
    this->y_node_ptr_ = this->pattern.FindVertex(y_node_id);
    this->q_edge_label_ = q;
  }
  // copy onstructor
  GPAR(const GPAR<Pattern>& b) {
    this->pattern = b.pattern;
    this->x_node_ptr_ = this->pattern.FindVertex(b.x_node_ptr_->id());
    this->y_node_ptr_ = this->pattern.FindVertex(b.y_node_ptr_->id());
    this->q_edge_label_ = b.q_edge_label_;
  }
  VertexPtr x_node_ptr() const { return this->x_node_ptr_; }
  VertexPtr y_node_ptr() const { return this->y_node_ptr_; }
  EdgeLabelType q_edge_label() const { return this->q_edge_label_; }
  bool operator<(const GPAR<Pattern>& b) const { return 1; }
  const Pattern& const_pattern() const { return this->pattern; }

  GPAR& operator=(const GPAR<Pattern>& b) {
    this->pattern = b.pattern;
    this->x_node_ptr_ = this->pattern.FindConstVertex(b.x_node_ptr_->id());
    this->y_node_ptr_ = this->pattern.FindConstVertex(b.y_node_ptr_->id());
    this->q_edge_label_ = b.q_edge_label_;
    return *this;
  }
  std::string Serialization() const {
    std::string res = "";
    res += "vertex";
    for (auto vertex_it = this->pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      res += " " + std::to_string(vertex_it->id()) + " " +
             std::to_string(vertex_it->label());
    }
    res += " edge";
    for (auto vertex_it = this->pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        res += " " + std::to_string(edge_it->id()) + " " +
               std::to_string(edge_it->src_id()) + " " +
               std::to_string(edge_it->dst_id()) + " " +
               std::to_string(edge_it->label());
      }
    }
    res += " q(x,y)";
    res += " " + std::to_string(this->x_node_ptr_->id());
    res += " " + std::to_string(this->y_node_ptr_->id());
    res += " " + std::to_string(this->q_edge_label_);
    return res;
  }

 public:
  Pattern pattern;
  VertexPtr x_node_ptr_, y_node_ptr_;
  EdgeLabelType q_edge_label_;
};
template <class Pattern, class DataGraph>
class DiscoverdGPAR : public GPAR<Pattern> {
 public:
  typedef Pattern value_type;
  using VertexType = typename Pattern::VertexType;
  using VertexIDType = typename VertexType::IDType;
  using VertexLabelType = typename VertexType::LabelType;
  using EdgeType = typename Pattern::EdgeType;
  using EdgeIDType = typename EdgeType::IDType;
  using EdgeLabelType = typename EdgeType::LabelType;
  using VertexPtr = typename GUNDAM::VertexHandle<Pattern>::type;
  using VertexConstPtr = typename GUNDAM::VertexHandle<const Pattern>::type;
  using EdgePtr = typename GUNDAM::EdgeHandle<Pattern>::type;
  using EdgeConstPtr = typename GUNDAM::EdgeHandle<const Pattern>::type;
  using VertexSizeType = size_t;
  using DataGraphVertexPtr =
      typename GUNDAM::VertexHandle<const DataGraph>::type;
  using SuppType = std::vector<DataGraphVertexPtr>;
  using ConfType = double;

 private:
  SuppType supp_Q_;
  SuppType supp_R_;
  ConfType conf_;
  int GPAR_id;

 public:
  DiscoverdGPAR() = default;
  DiscoverdGPAR(const VertexLabelType x, const VertexLabelType y,
                const EdgeLabelType q)
      : GPAR<Pattern>(x, y, q) {
    this->conf_ = -1;
    this->supp_Q_.clear();
    this->supp_R_.clear();
    this->GPAR_id = -1;
  }
  DiscoverdGPAR(Pattern& pattern, const VertexIDType x_node_id,
                const VertexIDType y_node_id, const EdgeLabelType q)
      : GPAR<Pattern>(pattern, x_node_id, y_node_id, q) {
    this->conf_ = -1;
    this->supp_Q_.clear();
    this->supp_R_.clear();
    this->GPAR_id = -1;
  }
  DiscoverdGPAR(const DiscoverdGPAR<Pattern, DataGraph>& b) : GPAR<Pattern>(b) {
    this->supp_R_ = b.supp_R_;
    this->supp_Q_ = b.supp_Q_;
    this->conf_ = b.conf_;
    this->GPAR_id = b.GPAR_id;
  }
  bool operator<(const DiscoverdGPAR<Pattern, DataGraph>& b) const { return 1; }
  DiscoverdGPAR& operator=(const DiscoverdGPAR<Pattern, DataGraph>& b) {
    this->pattern = b.pattern;
    this->x_node_ptr_ = this->pattern.FindVertex(b.x_node_ptr()->id());
    this->y_node_ptr_ = this->pattern.FindVertex(b.y_node_ptr()->id());
    this->q_edge_label_ = b.q_edge_label();
    this->supp_R_ = b.supp_R_;
    this->supp_Q_ = b.supp_Q_;
    this->conf_ = b.conf_;
    this->GPAR_id = b.GPAR_id;
    return *this;
  }
  SuppType& supp_Q() { return this->supp_Q_; }
  const SuppType& supp_Q() const { return this->supp_Q_; }
  SuppType& supp_R() { return this->supp_R_; }
  const SuppType& supp_R() const { return this->supp_R_; }
  size_t supp_Q_size() const { return this->supp_Q_.size(); }
  size_t supp_R_size() const { return this->supp_R_.size(); }
  ConfType conf() const { return this->conf_; }
  void CalConf() {
    if (!this->supp_Q_.empty()) {
      this->conf_ = static_cast<double>(this->supp_R_.size()) /
                    static_cast<double>(this->supp_Q_.size());
    }
  }
  int id() const { return this->GPAR_id; }
  void Setid(int id) { this->GPAR_id = id; }
};
template <class GPAR>
void WriteGPAR(const GPAR& gpar, uint32_t pattern_id,
               const std::string& output_dir) {
  std::string v_file =
      output_dir + "pattern_" + std::to_string(pattern_id) + "_v.csv";
  std::string e_file =
      output_dir + "pattern_" + std::to_string(pattern_id) + "_e.csv";
  GUNDAM::WriteCSVGraph(gpar.pattern, v_file, e_file);
}
}  // namespace gmine_new
#endif