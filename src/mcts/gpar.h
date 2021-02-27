#ifndef _GPARNEW_H
#define _GPARNEW_H

#include <string>
#include <vector>

#include "gundam/csvgraph.h"
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
  using VertexPtr = typename Pattern::VertexPtr;
  using VertexConstPtr = typename Pattern::VertexConstPtr;
  using EdgePtr = typename Pattern::EdgePtr;
  using EdgeConstPtr = typename Pattern::EdgeConstPtr;
  using VertexSizeType = size_t;

  GPAR() = default;
  // init x_node_id = 1 and y_node_id = 2
  GPAR(const VertexLabelType x, const VertexLabelType y,
       const EdgeLabelType q) {
    this->pattern.AddVertex(1, x);
    this->pattern.AddVertex(2, y);
    this->x_node_ptr_ = this->pattern.FindConstVertex(1);
    this->y_node_ptr_ = this->pattern.FindConstVertex(2);
    this->q_edge_label_ = q;
  }
  // another constructor
  GPAR(Pattern pattern, const VertexIDType x_node_id,
       const VertexIDType y_node_id, const EdgeLabelType q) {
    this->pattern = pattern;
    this->x_node_ptr_ = this->pattern.FindConstVertex(x_node_id);
    this->y_node_ptr_ = this->pattern.FindConstVertex(y_node_id);
    this->q_edge_label_ = q;
  }
  // copy onstructor
  GPAR(const GPAR<Pattern>& b) {
    this->pattern = b.pattern;
    this->x_node_ptr_ = this->pattern.FindConstVertex(b.x_node_ptr_->id());
    this->y_node_ptr_ = this->pattern.FindConstVertex(b.y_node_ptr_->id());
    this->q_edge_label_ = b.q_edge_label_;
  }
  VertexConstPtr x_node_ptr() const { return this->x_node_ptr_; }
  VertexConstPtr y_node_ptr() const { return this->y_node_ptr_; }
  EdgeLabelType q_edge_label() const { return this->q_edge_label_; }
  bool operator<(const GPAR<Pattern>& b) const { return 1; }
  GPAR& operator=(const GPAR<Pattern>& b) {
    this->pattern = b.pattern;
    this->x_node_ptr_ = this->pattern.FindConstVertex(b.x_node_ptr_->id());
    this->y_node_ptr_ = this->pattern.FindConstVertex(b.y_node_ptr_->id());
    this->q_edge_label_ = b.q_edge_label_;
    return *this;
  }

  /*
  int* Serialization(int m, int n, bool flag) {
    int* data = new int[m*2+n*4+3];
    int index = 0;
    for (auto vertex_it = this->pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      data[index++] = vertex_it->id();
      data[index++] = vertex_it->label();
      std::cout << "send pattern support V " << vertex_it->id() << " : " << vertex_it->label() << std::endl;
    }
    for (auto vertex_it = this->pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        data[index++] = edge_it->id();
        data[index++] = edge_it->src_id();
        data[index++] = edge_it->dst_id();
        data[index++] = edge_it->label();
        std::cout << "send pattern support E " << edge_it->id() << " : " << edge_it->src_id() << " : " << edge_it->dst_id() << " : " << edge_it->label() << std::endl;
      }
    }
    data[index++] = this->x_node_ptr_->id();
    data[index++] = this->y_node_ptr_->id();
    data[index++] = this->q_edge_label_;
    std::cout << "send pattern source_id " << this->x_node_ptr_->id() << " :target_id " << this->y_node_ptr_->id() << " ;edge_label " << this->q_edge_label_ << std::endl;
    if(flag){
      data[index++] = this->supp_Q_size_;
      data[index++] = this->supp_R_size_;
      std::cout << "send pattern support Q " << this->supp_Q_size_ << " ; support R " << this->supp_R_size_ << std::endl;
    }
    return data;
  }

  void Deserialization(int* data, int m, int n, bool flag) {
    int index = 0;
    for(;index<2*m;index+=2){
      this->pattern.AddVertex(data[index], data[index+1]);
      std::cout << "recv pattern support V " << data[index] << " : " << data[index+1] << std::endl;
    }
    for(;index<2*m+4*n;index+=4){
      this->pattern.AddEdge(data[index], data[index+1], data[index+2], data[index+3]);
      std::cout << "recv pattern support E " << data[index] << " : " << data[index+1] << " : " << data[index+2] << " : " << data[index+3] << std::endl;
    }
    
    //this->x_node_ptr_->id() = data[index++];
    //this->y_node_ptr_->id() = data[index++];
    //this->q_edge_label_ = data[index++];
    
    int source_id = data[index++]; 
    int target_id = data[index++]; 
    int edge_label = data[index++]; 
    std::cout << "recv pattern source_id " << source_id << " :target_id " << target_id << " ;edge_label " << edge_label << std::endl;
    if(flag){
      this->supp_Q_size_ = data[index++];
      this->supp_R_size_ = data[index++];
      if (!this->supp_Q_size_) {
        this->gpar_conf = static_cast<double>(this->supp_R_size_) /
                     static_cast<double>(this->supp_Q_size_);
      }
    }
    std::cout << "recv pattern support Q " << this->supp_Q_size_ << " ; support R " << this->supp_R_size_ << " ;conf = " << this->gpar_conf << std::endl;
  }  
  */


 public:
  Pattern pattern;
  size_t supp_Q_size_;
  size_t supp_R_size_;
  double gpar_conf;
  VertexConstPtr x_node_ptr_, y_node_ptr_;
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
  using VertexPtr = typename Pattern::VertexPtr;
  using VertexConstPtr = typename Pattern::VertexConstPtr;
  using EdgePtr = typename Pattern::EdgePtr;
  using EdgeConstPtr = typename Pattern::EdgeConstPtr;
  using VertexSizeType = size_t;
  using DataGraphVertexPtr = typename DataGraph::VertexConstPtr;
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
    this->conf_ = 0;
    this->supp_Q_.clear();
    this->supp_R_.clear();
    this->GPAR_id = -1;
  }
  DiscoverdGPAR(Pattern pattern, const VertexIDType x_node_id,
                const VertexIDType y_node_id, const EdgeLabelType q)
      : GPAR<Pattern>(pattern, x_node_id, y_node_id, q) {
    this->conf_ = 0;
    this->supp_Q_.clear();
    this->supp_R_.clear();
    this->GPAR_id = -1;
  }
  DiscoverdGPAR(const DiscoverdGPAR<Pattern, DataGraph>& b) : GPAR<Pattern>(b) {
    this->supp_R_ = b.supp_R_;
    this->supp_Q_ = b.supp_Q_;
    this->conf_ = b.conf_;
    this->GPAR_id = b.GPAR_id;
    this->supp_Q_size_ = b.supp_Q_size_;
    this->supp_R_size_ = b.supp_R_size_;
    this->gpar_conf = b.gpar_conf;
  }
  bool operator<(const DiscoverdGPAR<Pattern, DataGraph>& b) const { return 1; }
  DiscoverdGPAR& operator=(const DiscoverdGPAR<Pattern, DataGraph>& b) {
    this->pattern = b.pattern;
    this->x_node_ptr_ = this->pattern.FindConstVertex(b.x_node_ptr()->id());
    this->y_node_ptr_ = this->pattern.FindConstVertex(b.y_node_ptr()->id());
    this->q_edge_label_ = b.q_edge_label();
    this->supp_R_ = b.supp_R_;
    this->supp_Q_ = b.supp_Q_;
    this->conf_ = b.conf_;
    this->GPAR_id = b.GPAR_id;
    this->supp_Q_size_ = b.supp_Q_size_;
    this->supp_R_size_ = b.supp_R_size_;
    this->gpar_conf = b.gpar_conf;
    return *this;
  }
  SuppType& supp_Q() { return this->supp_Q_; }
  SuppType& supp_R() { return this->supp_R_; }
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

  int* Serialization(int m, int n, bool flag, std::vector<int>& data) {
    int N = m*2+n*4+3;
    if(flag)
      N+=2;
    //data = new int[N];
    data.resize(N);
    int index = 0;
    for (auto vertex_it = this->pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      data[index++] = vertex_it->id();
      data[index++] = vertex_it->label();
      //std::cout << "send pattern support V " << vertex_it->id() << " : " << vertex_it->label() << std::endl;
    }
    for (auto vertex_it = this->pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        data[index++] = edge_it->id();
        data[index++] = edge_it->src_id();
        data[index++] = edge_it->dst_id();
        data[index++] = edge_it->label();
        //std::cout << "send pattern support E " << edge_it->id() << " : " << edge_it->src_id() << " : " << edge_it->dst_id() << " : " << edge_it->label() << std::endl;
      }
    }
    data[index++] = this->x_node_ptr_->id();
    data[index++] = this->y_node_ptr_->id();
    data[index++] = this->q_edge_label_;
    //std::cout << "send pattern source_id " << this->x_node_ptr_->id() << " :target_id " << this->y_node_ptr_->id() << " ;edge_label " << this->q_edge_label_ << std::endl;
    if(flag){
      data[index++] = this->supp_Q_.size();
      data[index++] = this->supp_R_.size();
      //std::cout << "send pattern support Q " << this->supp_Q_.size() << " ; support R " << this->supp_R_.size() << std::endl;
    }
    //std::cout << "ndata ";
    //  for(int i=0;i<m*2+n*4;i++)
    //    std::cout << data[i] << "-";
    //  std::cout << std::endl;
    return data.data();
  }

  void Deserialization(int* data, int m, int n, bool flag) {
    //std::cout << "ndata ";
    //  for(int i=0;i<m*2+n*4;i++)
    //    std::cout << data[i] << "-";
    //  std::cout << std::endl;
    int index = 0;
    for(index=0; index<m; index++){
      this->pattern.AddVertex(data[2*index], data[2*index+1]);
    //  std::cout << "recv pattern support V " << data[2*index] << " : " << data[2*index+1] << std::endl;
    }
    for(index=0; index<n; index++){
      this->pattern.AddEdge(data[2*m+4*index+1], data[2*m+4*index+2], data[2*m+4*index+3], data[2*m+4*index]);
    // std::cout << "recv pattern support E " << data[2*m+4*index] << " : " << data[2*m+4*index+1] << " : " << data[2*m+4*index+2] << " : " << data[2*m+4*index+3] << std::endl;
    }
    
    int source_id = data[2*m+4*n]; 
    int target_id = data[2*m+4*n+1]; 
    int edge_label = data[2*m+4*n+2]; 
    //std::cout << "recv pattern source_id " << this->x_node_ptr_->id() << " :target_id " << this->y_node_ptr_->id() << " ;edge_label " << this->q_edge_label_ << std::endl;
    if(flag){
      this->supp_Q_size_ = data[2*m+4*n+3];
      this->supp_R_size_ = data[2*m+4*n+4];
      this->gpar_conf = static_cast<double>(this->supp_R_size_) /
                     static_cast<double>(this->supp_Q_size_);
    //  std::cout << "recv pattern support Q " << this->supp_Q_size_ << " ; support R " << this->supp_R_size_ << " ;conf = " << this->gpar_conf << std::endl;
    }
  }

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