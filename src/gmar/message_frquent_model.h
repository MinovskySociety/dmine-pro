#ifndef _MESSAGEFRQUENTMODEL_H
#define _MESSAGEFRQUENTMODEL_H
#include "ml_model.h"

namespace gmine_new {
// message frquent model
template <class Pattern, class DataGraph>
class MessageFrquentModel : public MLModelBase<Pattern, DataGraph> {
 public:
  using DataGraphVertexPtr = typename GUNDAM::VertexHandle<DataGraph>::type;
  MessageFrquentModel(int frquent_bound) {
    this->frquent_bound_ = frquent_bound;
    this->last_frquent_bound_ = -1;
  }
  int GetTimeCount(std::string &begin_time) const {
    int begin_day = (begin_time[0] - '0') * 10 + (begin_time[1] - '0');
    int begin_hour = (begin_time[2] - '0') * 10 + (begin_time[3] - '0');
    int begin_min = (begin_time[4] - '0') * 10 + (begin_time[5] - '0');
    int begin_second = (begin_time[6] - '0') * 10 + (begin_time[7] - '0');
    return begin_second + begin_min * 60 + begin_hour * 60 * 60 +
           begin_day * 60 * 60 * 24;
  }
  bool SendTimeLessthen1Min(std::string &begin_time,
                            std::string end_time) const {
    int begin_time_count = GetTimeCount(begin_time);
    int end_time_count = GetTimeCount(end_time);
    return begin_time_count + 60 <= end_time_count;
  }
  bool Satisfy(const DataGraphVertexPtr predict_vertex_x,
               const DataGraphVertexPtr predict_vertex_y) const override {
    // 1min send meggage num >= frquent_bound
    std::vector<int> send_begin_time;
    for (auto edge_it = predict_vertex_x->OutEdgeCBegin(2002);
         !edge_it.IsDone(); edge_it++) {
      auto attribute_ptr = edge_it->FindConstAttributePtr("start_time");
      if (attribute_ptr.IsNull()) continue;
      std::string begin_time =
          attribute_ptr->template const_value<std::string>();
      send_begin_time.push_back(GetTimeCount(begin_time));
    }
    std::sort(send_begin_time.begin(), send_begin_time.end());
    //  std::cout << "size = " << send_begin_time.size() << std::endl;
    if (this->frquent_bound_ == 0) return true;
    for (int i = 0; i + this->frquent_bound_ - 1 < (int)send_begin_time.size();
         i++) {
      int lpos = i;
      int rpos = i + this->frquent_bound_ - 1;
      if (send_begin_time[rpos] - send_begin_time[lpos] <= 60) {
        return true;
      }
    }
    return false;
  }
  int check(int mid, const DataGraphVertexPtr vertex_ptr) {
    this->frquent_bound_ = mid;
    return this->Satisfy(vertex_ptr, vertex_ptr);
  }
  int CalBound(const DataGraphVertexPtr vertex_ptr) {
    //.....
    // binary search ans
    int l = 0, r = 1e9 + 7;
    while (abs(r - l) > 1) {
      int mid = (l + r) >> 1;
      if (check(mid, vertex_ptr)) {
        l = mid;
      } else {
        r = mid - 1;
      }
    }
    for (int i = std::max(l, r); i >= std::max(0, std::min(l, r)); i--) {
      if (check(i, vertex_ptr)) {
        return i;
      }
    }
    return 0;
  }
  double CalAccur(
      double candidate_bound,
      const std::vector<std::pair<DataGraphVertexPtr, bool>> &train_data,
      const std::vector<int> &send_message_frquent_bound) {
    //....
    if (train_data.size() == 0) return 0.0;
    int correct_num = 0;
    for (size_t i = 0; i < train_data.size(); i++) {
      if (send_message_frquent_bound[i] < candidate_bound &&
          !train_data[i].second) {
        correct_num++;
      }
      if (send_message_frquent_bound[i] >= candidate_bound &&
          train_data[i].second) {
        correct_num++;
      }
    }
    return (1.0 * correct_num) / train_data.size();
  }
  void Training(
      std::vector<gmine_new::DiscoverdGPAR<Pattern, DataGraph>> &discover_gmar,
      DataGraph &data_graph) override {
    this->last_frquent_bound_ = this->frquent_bound_;
    //....
    std::set<std::pair<DataGraphVertexPtr, bool>> unique_data;
    std::vector<std::pair<DataGraphVertexPtr, bool>> train_data;

    std::cout << "unique data begin" << std::endl;
    for (auto &single_gmar : discover_gmar) {
      single_gmar.supp_Q().clear();
      gmine_new::GPARMatch(single_gmar, data_graph);
      for (const auto &it : single_gmar.supp_Q()) {
        if (it->id() >= 4100) {
          unique_data.emplace(it, true);
        } else {
          unique_data.emplace(it, false);
        }
      }
    }
    std::cout << "unique data end" << std::endl;
    train_data.insert(train_data.end(), unique_data.begin(), unique_data.end());
    std::vector<int> send_message_frquent_bound;
    std::cout << "Cal bound begin" << std::endl;
    for (const auto &[vertex_ptr, train_value] : train_data) {
      auto bound = this->CalBound(vertex_ptr);
      send_message_frquent_bound.push_back(bound);
    }
    std::cout << "Cal bound end" << std::endl;
    double max_accur = 0.0;
    std::cout << "Cal frequent bound begin" << std::endl;
    auto temp = send_message_frquent_bound;
    std::sort(temp.begin(), temp.end());
    std::reverse(temp.begin(), temp.end());
    for (const auto &candidate_bound : temp) {
      double accur = this->CalAccur(candidate_bound, train_data,
                                    send_message_frquent_bound);
      if (max_accur < accur) {
        this->frquent_bound_ = candidate_bound;
        max_accur = accur;
      }
    }
    std::cout << "Cal bound end" << std::endl;
  }
  void PrintModel() const override {
    std::cout << "bound is " << this->frquent_bound_ << std::endl;
  }
  bool IsTerminal() const override {
    return this->last_frquent_bound_ == this->frquent_bound_;
  }
  void BuildMLGraph(DataGraph &ml_graph) const override {
    //...
    using VertexIDType = typename DataGraph::VertexType::IDType;
    using VertexLabelType = typename DataGraph::VertexType::LabelType;
    using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
    using VertexConstPtr = typename GUNDAM::VertexHandle<DataGraph>::type;
    using EdgeIDType = typename DataGraph::EdgeType::IDType;
    /*
    constexpr VertexLabelType ml_label = -1;
    constexpr EdgeLabelType ml_edge_label = -1;
    VertexIDType max_vertex_id = 0;
    EdgeIDType next_edge_id = 0;
    for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      ml_graph.AddVertex(vertex_it->id(), vertex_it->label());
      max_vertex_id = std::max(max_vertex_id, vertex_it->id());
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        next_edge_id = std::max(next_edge_id, edge_it->id());
      }
    }
    VertexIDType ml_id = max_vertex_id + 1;
    ++next_edge_id;
    ml_graph.AddVertex(ml_id, ml_label);
    for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      VertexConstPtr vertex_ptr = vertex_it;
      if (vertex_ptr->label() == 1001 &&
          this->Satisfy(vertex_ptr, vertex_ptr)) {
        ml_graph.AddEdge(vertex_it->id(), ml_id, ml_edge_label, next_edge_id++);
      }
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        ml_graph.AddEdge(edge_it->src_id(), edge_it->dst_id(), edge_it->label(),
                         edge_it->id());
      }
    }
    */
  }

 private:
  int frquent_bound_;
  int last_frquent_bound_;
};

}  // namespace gmine_new
#endif