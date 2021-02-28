#ifndef _GPAR_H
#define _GPAR_H

//#include "stardand/errcode.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "gundam/io/csvgraph.h"
//#include "stardand/paratype.h"
//#include "gundam/graph.h"
#include "gundam/algorithm/vf2.h"
#include "gundam/graph_type/large_graph.h"

const double eps = 1e-6;
using ReturnType = int;
using TagType = int;
namespace gmine_new {
// using GraphType = GUNDAM::Graph<>;
using GraphType = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                     uint32_t, std::string>;
using VertexType = typename GraphType::VertexType;
using EdgeType = typename GraphType::EdgeType;
using VertexIDType = typename VertexType::IDType;
using VertexLabelType = typename VertexType::LabelType;
using EdgeLabelType = typename EdgeType::LabelType;
using VertexPtr = typename GraphType::VertexPtr;
using VertexConstPtr = typename GraphType::VertexConstPtr;
using EdgeIDType = typename GraphType::EdgeType::IDType;
using EdgeConstPtr = typename GraphType::EdgeConstPtr;
// Read File
/*
ReturnType ReadGraph(INOUT GraphType &target_graph, IN const char *v_file,
                     IN const char *e_file);
void TransposePattern(const GraphType &G);

// cal information of datagraph to expand edge without use DataGraph
class DataGraphInformation {
 public:
  GUNDAM::LargeGraph<int, int, int, int, int, int> target_graph_;
  std::set<VertexLabelType> all_node_label_;
  std::set<EdgeLabelType> all_edge_weight_;
  std::set<
      std::pair<std::pair<VertexLabelType, VertexLabelType>, EdgeLabelType>>
      all_possible_edge_;
  std::set<std::pair<VertexLabelType, VertexLabelType>> all_ban_edge_;

 public:
  DataGraphInformation() {
    all_node_label_.clear();
    all_edge_weight_.clear();
    all_possible_edge_.clear();
    all_ban_edge_.clear();
  }
  ReturnType BuildInformation();
};

// GPARs
class GraphPatternAssociationRules {
 public:
  GraphType pattern_;  // pattern
  size_t supp_r;       // size_t
 private:
  VertexLabelType x_node_label_;   // q(x,y) of x
  VertexLabelType y_node_label_;   // q(x,y) of y
  VertexLabelType q_label_;        // q(x,y) of q
  std::vector<VertexPtr> supp_x_;  // supp(x)
  double conf_;                    // conf
  std::shared_ptr<DataGraphInformation> target_graph_;

 public:
  GraphPatternAssociationRules() {
    this->x_node_label_ = this->y_node_label_ = this->q_label_ = 0;
    this->conf_ = 0;
    this->supp_x_.clear();
    this->target_graph_ = nullptr;
  }
  ~GraphPatternAssociationRules() { this->supp_x_.clear(); }
  GraphPatternAssociationRules(const GraphPatternAssociationRules &b) {
    this->x_node_label_ = b.x_node_label_;
    this->y_node_label_ = b.y_node_label_;
    this->q_label_ = b.q_label_;
    this->pattern_ = b.pattern_;
    this->supp_x_ = b.supp_x_;
    this->conf_ = b.conf_;
    this->target_graph_ = b.target_graph_;
  }
  GraphPatternAssociationRules &operator=(
      const GraphPatternAssociationRules &b) {
    this->x_node_label_ = b.x_node_label_;
    this->y_node_label_ = b.y_node_label_;
    this->q_label_ = b.q_label_;
    this->pattern_ = b.pattern_;
    this->supp_x_ = b.supp_x_;
    this->conf_ = b.conf_;
    this->target_graph_ = b.target_graph_;
    return *this;
  }
  inline void SetQEdge(IN VertexLabelType x_node_label,
                       IN VertexLabelType y_node_label,
                       IN VertexLabelType q_label) {
    this->x_node_label_ = x_node_label;
    this->y_node_label_ = y_node_label;
    this->q_label_ = q_label;
  }
  inline void SetPattern(IN GraphType &pattern) { this->pattern_ = pattern; }
  inline void LoadGraph(DataGraphInformation &target_graph) {
    this->target_graph_ = std::make_shared<DataGraphInformation>(target_graph);
  }
  bool operator<(GraphPatternAssociationRules &b) const {
    return this->conf_ > b.conf_;
  }
  inline double GetConf() const { return this->conf_; }
  inline size_t GetSuppxSize() const { return this->supp_x_.size(); }
  inline size_t GetSuppR() { return this->supp_r; }
  ReturnType CalConf();
  // GPAR expand
  ReturnType Expand(
      IN int round,
      OUT std::vector<GraphPatternAssociationRules> &expand_result);
  ReturnType AddNewEdgeInPattern(
      IN int round,
      OUT std::vector<GraphPatternAssociationRules> &expand_result);
  ReturnType AddNewEdgeOutPattern(
      IN int round,
      OUT std::vector<GraphPatternAssociationRules> &expand_result);
  // GPAR match
  ReturnType FirstRoundMatch();
  ReturnType Match();
  // GPAR print
  ReturnType PrintPattern(IN VertexIDType pattern_id,
                          IN const char *print_file);
};
class DiscoverGPAR {
 public:
  DataGraphInformation target_graph_;

 private:
  std::vector<GraphPatternAssociationRules> discovery_result_;
  int top_k_;
  int up_edge_num_;
  VertexLabelType x_node_label_, y_node_label_, q_label_;
  std::string output_dir_;
  // log
  double read_time;
  std::vector<double> expand_time;
  std::vector<double> total_match_time;
  std::vector<std::vector<GraphType>> pattern;
  std::vector<std::vector<double>> match_time;
  std::vector<std::vector<int>> supp;

 public:
  DiscoverGPAR() {
    top_k_ = -1;
    up_edge_num_ = 0;
    x_node_label_ = y_node_label_ = q_label_ = -1;
    read_time = 0.0;
  }

  void SetQEdge(IN VertexLabelType x_node_label,
                IN VertexLabelType y_node_label, IN VertexLabelType q_label) {
    x_node_label_ = x_node_label;
    y_node_label_ = y_node_label;
    q_label_ = q_label;
  }

  void SetTopK(IN int top_k) { top_k_ = top_k; }

  void SetUpEdgeNum(IN int up_edge_num) { up_edge_num_ = up_edge_num; }

  void SetOutputDir(const char *output_dir) {
    output_dir_ = output_dir;
    if (!output_dir_.empty() && output_dir_.back() != '/') {
      output_dir_.push_back('/');
    }
  }

  ReturnType ExpandGPARs(
      IN int round, IN std::vector<GraphPatternAssociationRules> &last_patterns,
      OUT std::vector<GraphPatternAssociationRules> &now_patterns);

  ReturnType DminePro(IN const char *v_file, IN const char *e_file);

  ReturnType WriteLog();

  ReturnType UpdateState(
      IN std::vector<GraphPatternAssociationRules> &now_patterns);
};
*/
}  // namespace gmine_new
#endif
