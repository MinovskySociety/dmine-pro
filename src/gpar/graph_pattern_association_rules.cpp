#include "gpar.h"
#include "gundam/vf2.h"
#include "gundam/vf2_label_equal.h"
//#include "gundam/csvgraph.h"
ReturnType gmine_new::GraphPatternAssociationRules::CalConf() {
  // conf=supp(R)/supp(Q)
  size_t supp_q = this->supp_x_.size();
  this->supp_r = 0;
  if (supp_q == 0) {
    this->conf_ = 0x3f3f3f3f;
    return E_OK;
  }
  for (auto &it : this->supp_x_) {
    TagType find_q_edge = 0;
    for (auto bit = it->OutEdgeBegin(EdgeLabelType(q_label_)); !bit.IsDone();
         bit++) {
      // std::cout << it->id() << " " << bit->dst_ptr()->id() << " "
      //           << bit->dst_ptr()->label() << std::endl;
      if (bit->dst_ptr()->label() == y_node_label_) {
        find_q_edge = 1;
        break;
      }
      /*
      if (bit->label() == EdgeLabelType(this->q_label_)) {
        find_q_edge = 1;
        break;
      }
      */
    }
    if (find_q_edge == 1) {
      ++(this->supp_r);
    }
  }
  this->conf_ = (1.0 * this->supp_r) / supp_q;
  return E_OK;
}

ReturnType gmine_new::GraphPatternAssociationRules::Expand(
    IN int round,
    OUT std::vector<GraphPatternAssociationRules> &expand_result) {
  this->AddNewEdgeInPattern(round, expand_result);
  this->AddNewEdgeOutPattern(round, expand_result);
  return E_OK;
}

ReturnType gmine_new::GraphPatternAssociationRules::AddNewEdgeInPattern(
    IN int round,
    OUT std::vector<GraphPatternAssociationRules> &expand_result) {
  size_t pattern_size = this->pattern_.CountVertex();
  for (VertexIDType from_node = 1; from_node <= pattern_size; ++from_node) {
    VertexPtr from_node_ptr = pattern_.FindVertex(from_node);
    for (VertexIDType to_node = 1; to_node <= pattern_size; ++to_node) {
      VertexPtr to_node_ptr = pattern_.FindVertex(to_node);
      if (from_node == 1 &&
          to_node_ptr->label() == EdgeLabelType(y_node_label_))
        continue;
      if (from_node == to_node) continue;
      for (auto edge_it : target_graph_->all_edge_weight_) {
        if (target_graph_->all_possible_edge_.count(std::make_pair(
                std::make_pair(from_node_ptr->label(), to_node_ptr->label()),
                edge_it)) == 0)
          continue;
        TagType Find = 0;
        for (auto bit = from_node_ptr->OutEdgeBegin(edge_it); !bit.IsDone();
             bit++) {
          if (bit->dst_ptr()->id() == to_node) {
            Find = 1;
            break;
          }
        }
        if (Find) continue;
        GraphPatternAssociationRules copy;
        copy = *this;
        copy.pattern_.AddEdge(from_node, to_node, edge_it, round);
        expand_result.push_back(copy);
      }
    }
  }
  return E_OK;
}

ReturnType gmine_new::GraphPatternAssociationRules::AddNewEdgeOutPattern(
    IN int round,
    OUT std::vector<GraphPatternAssociationRules> &expand_result) {
  VertexIDType node_sz =
      static_cast<VertexIDType>(this->pattern_.CountVertex());
  // TransposePattern(this->pattern_);
  for (VertexIDType from_node = 1; from_node <= node_sz; ++from_node) {
    VertexIDType to_node = node_sz + 1;
    VertexPtr from_node_ptr = this->pattern_.FindVertex(from_node);
    for (auto node_it : target_graph_->all_node_label_) {
      for (auto edge_it : target_graph_->all_edge_weight_) {
        if (from_node == 1 && ((edge_it == EdgeLabelType(q_label_)) ||
                               node_it == VertexLabelType(y_node_label_)))
          continue;
        if (target_graph_->all_possible_edge_.count(std::make_pair(
                std::make_pair(from_node_ptr->label(), node_it), edge_it)) == 0)
          continue;
        GraphPatternAssociationRules copy;
        copy = *this;
        copy.pattern_.AddVertex(to_node, node_it);
        copy.pattern_.AddEdge(from_node, to_node, edge_it, round);
        expand_result.push_back(copy);
      }
    }
  }
  for (VertexIDType to_node = 1; to_node <= node_sz; ++to_node) {
    VertexIDType from_node = node_sz + 1;
    VertexPtr to_node_ptr = this->pattern_.FindVertex(to_node);
    for (auto node_it : target_graph_->all_node_label_) {
      for (auto edge_it : target_graph_->all_edge_weight_) {
        if (to_node == 1 && ((edge_it == EdgeLabelType(q_label_)) ||
                             node_it == VertexLabelType(y_node_label_)))
          continue;
        if (target_graph_->all_possible_edge_.count(std::make_pair(
                std::make_pair(node_it, to_node_ptr->label()), edge_it)) == 0)
          continue;
        GraphPatternAssociationRules copy;
        copy = *this;
        copy.pattern_.AddVertex(from_node, node_it);
        copy.pattern_.AddEdge(from_node, to_node, edge_it, round);
        expand_result.push_back(copy);
      }
    }
  }
  return E_OK;
}

ReturnType gmine_new::GraphPatternAssociationRules::FirstRoundMatch() {
  for (auto it = this->target_graph_->target_graph_.VertexBegin(); !it.IsDone();
       ++it) {
    if (it->label() == VertexLabelType(this->x_node_label_)) {
      this->supp_x_.push_back(
          this->target_graph_->target_graph_.FindVertex(it->id()));
    }
  }
  return E_OK;
}

ReturnType gmine_new::GraphPatternAssociationRules::Match() {
  std::vector<VertexPtr> match_result;
  // TransposePattern(this->pattern_);
  for (auto it : this->supp_x_) {
    std::vector<std::map<GraphType::VertexConstPtr, GraphType::VertexConstPtr>>
        temp_match_result;
    // std::cout<<"uid="<<it->id()<<std::endl;
    VertexIDType query_id = 1, target_id = it->id();
    std::cout << "target_id = " << target_id << std::endl;
    ReturnType flag = GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
        this->pattern_, this->target_graph_->target_graph_, query_id, target_id,
        GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
        GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1,
        temp_match_result);
    if (temp_match_result.size() >= 1) {
      if (temp_match_result.size() >= 1) {
        match_result.push_back(it);
      }
    }
  }
  this->supp_x_ = match_result;
  return E_OK;
}

ReturnType gmine_new::GraphPatternAssociationRules::PrintPattern(
    IN VertexIDType pattern_id, IN const char *output_dir) {
  std::string Path = output_dir;
  std::string PathV = Path + "pattern_";
  PathV += std::to_string(pattern_id);
  PathV += "_n.csv";
  std::string PathE = Path + "pattern_";
  PathE += std::to_string(pattern_id);
  PathE += "_e.csv";
  GUNDAM::WriteCSVGraph(this->pattern_, PathV.c_str(), PathE.c_str());
  return E_OK;
  /*
  std::ofstream outfile(PathE);
  int sz = pattern_.CountVertex();
  for (int i = 1; i <= sz; i++) {
    outfile << i << "," << pattern_.FindVertex(i)->label() << "\n";
  }

  std::ofstream outfile1(PathE);
  for (int i = 1; i <= sz; i++) {
    for (auto it = pattern_.FindVertex(i)->OutEdgeBegin(); !it.IsDone(); it++) {
      outfile1 << it->src_ptr()->id() << " " << it->dst_ptr()->id() << " "
               << it->label() << std::endl;
    }
  }
  return E_OK;
  */
}
