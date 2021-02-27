#ifndef _USERPHONEFREQUENTMODEL_H
#define _USERPHONEFREQUENTMODEL_H
#include <numeric>

#include "gundam/dp_iso.h"
#include "ml_model.h"
namespace gmine_new {
template <class Pattern, class DataGraph>
class UserPhoneFrquentModel : public MLModelBase<Pattern, DataGraph> {
 public:
  using DataGraphVertexPtr = typename DataGraph::VertexConstPtr;
  using DataGraphLabel = typename DataGraph::EdgeType::LabelType;
  UserPhoneFrquentModel(int frquent_bound)
      : frquent_bound_(frquent_bound), last_frquent_bound_(-1), ml_label_(-1) {}
  UserPhoneFrquentModel(int frquent_bound, DataGraphLabel ml_label)
      : frquent_bound_(frquent_bound),
        last_frquent_bound_(-1),
        ml_label_(ml_label) {}
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
    for (auto edge_it = predict_vertex_x->OutEdgeCBegin(2002, predict_vertex_y);
         !edge_it.IsDone(); edge_it++) {
      auto attribute_ptr = edge_it->FindConstAttributePtr("start_time");
      if (attribute_ptr.IsNull()) continue;
      std::string begin_time =
          attribute_ptr->template const_value<std::string>();
      send_begin_time.push_back(GetTimeCount(begin_time));
    }
    std::sort(send_begin_time.begin(), send_begin_time.end());
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
  int check(int mid, const DataGraphVertexPtr vertex_ptr_x,
            const DataGraphVertexPtr vertex_ptr_y) {
    this->frquent_bound_ = mid;
    return this->Satisfy(vertex_ptr_x, vertex_ptr_y);
  }
  int CalBound(const DataGraphVertexPtr vertex_ptr_x,
               const DataGraphVertexPtr vertex_ptr_y) {
    //.....
    // binary search ans
    int l = 0, r = 1e9 + 7;
    while (abs(r - l) > 1) {
      int mid = (l + r) >> 1;
      if (check(mid, vertex_ptr_x, vertex_ptr_y)) {
        l = mid;
      } else {
        r = mid - 1;
      }
    }
    for (int i = std::max(l, r); i >= std::max(0, std::min(l, r)); i--) {
      if (check(i, vertex_ptr_x, vertex_ptr_y)) {
        return i;
      }
    }
    return 0;
  }
  double CalAccur(
      double candidate_bound,
      const std::vector<
          std::tuple<DataGraphVertexPtr, DataGraphVertexPtr, bool>> &train_data,
      const std::vector<int> &send_message_frquent_bound) {
    if (train_data.size() == 0) return 0.0;
    int correct_num = 0;
    int tot_num = 0;
    for (size_t i = 0; i < train_data.size(); i++) {
      if (send_message_frquent_bound[i] >= candidate_bound &&
          std::get<2>(train_data[i])) {
        correct_num++;
      }
      if (send_message_frquent_bound[i] >= candidate_bound) {
        tot_num++;
      }
    }
    std::cout << "num = " << correct_num << " " << tot_num << std::endl;
    return (1.0 * correct_num) / tot_num;
  }
  bool CheckHasMLLiteral(gmine_new::DiscoverdGPAR<Pattern, DataGraph> &gmar) {
    for (auto vertex_it = gmar.pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        if (edge_it->label() == this->ml_label_) {
          return true;
        }
      }
    }
    return false;
  }
  template <class DataGraphVertexPtr>
  void GetPivotResult(
      DataGraph &data_graph,
      std::vector<std::pair<DataGraphVertexPtr, DataGraphVertexPtr>>
          &pivot_result) {
    for (auto vertex_it = data_graph.VertexCBegin(1001); !vertex_it.IsDone();
         vertex_it++) {
      DataGraphVertexPtr src_ptr = vertex_it;
      for (auto adj_it = vertex_it->OutVertexCBegin(2002); !adj_it.IsDone();
           adj_it++) {
        DataGraphVertexPtr dst_ptr = adj_it;
        if (this->Satisfy(src_ptr, dst_ptr))
          pivot_result.emplace_back(src_ptr, dst_ptr);
      }
    }
  }
  template <class PatternVertexPtr, class DataGraphVertexPtr,
            class CandidateSet>
  std::pair<int, int> CalMLSupp(
      gmine_new::DiscoverdGPAR<Pattern, DataGraph> &gmar, DataGraph &data_graph,
      CandidateSet &candidate_set, PatternVertexPtr src_ptr,
      DataGraphVertexPtr match_src_ptr, PatternVertexPtr dst_ptr,
      DataGraphVertexPtr match_dst_ptr) {
    using MatchMap = std::map<PatternVertexPtr, DataGraphVertexPtr>;
    using MatchContainer = std::vector<MatchMap>;
    using CandidateContainer = std::vector<DataGraphVertexPtr>;
    auto candidate_src_it = candidate_set.find(src_ptr);
    auto candidate_dst_it = candidate_set.find(dst_ptr);
    /*
    std::cout << "size = " << candidate_src_it->second.size() << " "
              << candidate_dst_it->second.size() << std::endl;
    for (auto &it : candidate_dst_it->second) {
      std::cout << it->id() << " ";
    }
    std::cout << std::endl;
    */
    if (candidate_src_it == candidate_set.end() ||
        candidate_dst_it == candidate_set.end()) {
      return std::make_pair(-1, -1);
    }

    if (!std::binary_search(candidate_src_it->second.begin(),
                            candidate_src_it->second.end(), match_src_ptr) ||
        !std::binary_search(candidate_dst_it->second.begin(),
                            candidate_dst_it->second.end(), match_dst_ptr)) {
      // std::cout << "not found!" << std::endl;
      return std::make_pair(0, 0);
    }

    CandidateContainer &x_node_candidate =
        candidate_set.find(gmar.x_node_ptr())->second;
    std::set<DataGraphVertexPtr> supp_container;
    // CandidateContainer supp_container;
    auto user_callback = [&supp_container, &gmar](MatchMap &match_state) {
      supp_container.insert(match_state[gmar.x_node_ptr()]);
      /*
      std::cout << "match:" << std::endl;
      for (auto &it : match_state) {
        std::cout << it.first->id() << " " << it.second->id() << std::endl;
      }
      */

      return false;
    };
    auto prune_callback = [](MatchMap &match_state) { return false; };
    for (auto &match_x_ptr : x_node_candidate) {
      // std::cout << "match x ptr = " << match_x_ptr->id() << std::endl;
      MatchMap match_state;
      match_state.emplace(src_ptr, match_src_ptr);
      match_state.emplace(dst_ptr, match_dst_ptr);
      // std::cout << src_ptr->id() << " " << gmar.x_node_ptr()->id() <<
      // std::endl; std::cout << src_ptr << " " << gmar.x_node_ptr() <<
      // std::endl;
      if (!(src_ptr == gmar.x_node_ptr())) {
        match_state.emplace(gmar.x_node_ptr(), match_x_ptr);
      }
      /*
      std::cout << "match state:" << std::endl;
      for (auto &it : match_state) {
        std::cout << it.first->id() << " " << it.second->id() << std::endl;
      }
      */
      CandidateSet copy_candidate_set{candidate_set};
      GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
          gmar.pattern, data_graph, copy_candidate_set, match_state,
          user_callback, prune_callback, 10.0);
      if (src_ptr == gmar.x_node_ptr()) {
        break;
      }
    }
    int supp_q = supp_container.size();
    int supp_r = std::accumulate(supp_container.begin(), supp_container.end(),
                                 0, [](int res, auto &it) {
                                   if (it->id() >= 4100)
                                     return res + 1;
                                   else
                                     return res;
                                 });
    return std::make_pair(supp_q, supp_r);
  }
  template <class TrainDataType>
  void GetTrainData(
      std::vector<gmine_new::DiscoverdGPAR<Pattern, DataGraph>> &discover_gmar,
      DataGraph &data_graph, std::vector<TrainDataType> &train_data) {
    using DataGraphVertexPtr = typename DataGraph::VertexConstPtr;
    using DataGraphVertexPair =
        std::pair<DataGraphVertexPtr, DataGraphVertexPtr>;
    using PivotResultContainer = std::vector<DataGraphVertexPair>;
    using PatternEdgeIDType = typename Pattern::EdgeType::IDType;
    using MLEdgeIDContainer = std::vector<PatternEdgeIDType>;
    using PatternVertexPtr = typename Pattern::VertexConstPtr;
    using CandidateSet =
        std::map<PatternVertexPtr, std::vector<DataGraphVertexPtr>>;
    LOG(INFO) << "begin train data";
    // Get Pivot Result
    PivotResultContainer pivot_result;
    LOG(INFO) << "begin get pivot result";
    GetPivotResult(data_graph, pivot_result);
    LOG(INFO) << " pivot result size = " << pivot_result.size();
    LOG(INFO) << "begin gen train data";
    std::cout << " pivot result size = " << pivot_result.size();
    std::cout << "begin gen train data";
    for (auto &single_gmar : discover_gmar) {
      if (!CheckHasMLLiteral(single_gmar)) continue;
      CandidateSet single_gmar_candidate_set;
      if (!GUNDAM::_dp_iso::InitCandidateSet<
              GUNDAM::MatchSemantics::kIsomorphism>(
              single_gmar.pattern, data_graph, single_gmar_candidate_set)) {
        continue;
      }

      if (!GUNDAM::_dp_iso::RefineCandidateSet(single_gmar.pattern, data_graph,
                                               single_gmar_candidate_set)) {
        continue;
      }

      for (auto vertex_it = single_gmar.pattern.VertexCBegin();
           !vertex_it.IsDone(); vertex_it++) {
        for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
             edge_it++) {
          if (edge_it->label() != this->ml_label_) continue;
          PatternEdgeIDType ml_id = edge_it->id();
          gmine_new::DiscoverdGPAR<Pattern, DataGraph> erase_ml_gmar(
              single_gmar);
          erase_ml_gmar.pattern.EraseEdge(ml_id);
          CandidateSet erase_ml_gmar_candidate_set;
          if (!GUNDAM::_dp_iso::InitCandidateSet<
                  GUNDAM::MatchSemantics::kIsomorphism>(
                  erase_ml_gmar.pattern, data_graph,
                  erase_ml_gmar_candidate_set)) {
            continue;
          }
          if (!GUNDAM::_dp_iso::RefineCandidateSet(
                  erase_ml_gmar.pattern, data_graph,
                  erase_ml_gmar_candidate_set)) {
            continue;
          }
          TransposePattern(single_gmar.pattern);
          TransposePattern(erase_ml_gmar.pattern);
          auto shuffle_pivot_result{pivot_result};
          std::random_shuffle(shuffle_pivot_result.begin(),
                              shuffle_pivot_result.end());
          int cnt = 0;
          for (auto &[user_ptr, phone_ptr] : shuffle_pivot_result) {
            cnt++;
            if (cnt > 500) break;
            /*
            if (!(user_ptr->id() == 4180 && phone_ptr->id() == 9095)) {
              continue;
            }
            */
            auto [after_supp_q, after_supp_r] =
                CalMLSupp(single_gmar, data_graph, single_gmar_candidate_set,
                          edge_it->const_src_ptr(), user_ptr,
                          edge_it->const_dst_ptr(), phone_ptr);
            auto [before_supp_q, before_supp_r] = CalMLSupp(
                erase_ml_gmar, data_graph, erase_ml_gmar_candidate_set,
                erase_ml_gmar.pattern.FindConstVertex(edge_it->src_id()),
                user_ptr,
                erase_ml_gmar.pattern.FindConstVertex(edge_it->dst_id()),
                phone_ptr);
            int positive_num = after_supp_r + (before_supp_q - before_supp_r -
                                               (after_supp_q - after_supp_r));
            int negative_num =
                (after_supp_q - after_supp_r) + (before_supp_r - after_supp_r);
            if (!before_supp_q && !after_supp_q) {
              continue;
            }
            /*
            std::cout << "ml edge is " << user_ptr->id() << " "
                      << phone_ptr->id() << std::endl;
            std::cout << "before is " << before_supp_q << " " << before_supp_r
                      << std::endl;
            std::cout << "after is " << after_supp_q << " " << after_supp_r
                      << std::endl;
            std::cout << "positive num = " << positive_num << " "
                      << negative_num << std::endl;
            */
            if (positive_num > negative_num) {
              train_data.emplace_back(user_ptr, phone_ptr, true);
            } else if (negative_num > positive_num) {
              train_data.emplace_back(user_ptr, phone_ptr, false);
            }
          }
        }
      }
    }
    LOG(INFO) << "gen data size is " << train_data.size();
    LOG(INFO) << "begin erase conflict train data";
    std::vector<TrainDataType> erase_conflict_train_data;
    std::set<TrainDataType> unique_data, conflict_data;
    for (auto &single_data : train_data) {
      if (conflict_data.count(single_data)) {
        continue;
      }
      TrainDataType inv_data{std::get<0>(single_data), std::get<1>(single_data),
                             std::get<2>(single_data) ^ 1};
      if (unique_data.count(inv_data)) {
        conflict_data.insert(single_data);
        unique_data.erase(inv_data);
        continue;
      }
      unique_data.insert(single_data);
    }
    erase_conflict_train_data.insert(erase_conflict_train_data.end(),
                                     unique_data.begin(), unique_data.end());
    std::swap(erase_conflict_train_data, train_data);
    LOG(INFO) << "last train data size is " << train_data.size();
  }

  void Training(
      std::vector<gmine_new::DiscoverdGPAR<Pattern, DataGraph>> &discover_gmar,
      DataGraph &data_graph) override {
    using VertexIDType = typename DataGraph::VertexType::IDType;
    using VertexLabelType = typename DataGraph::VertexType::LabelType;
    using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
    using VertexConstPtr = typename DataGraph::VertexConstPtr;
    using EdgeIDType = typename DataGraph::EdgeType::IDType;
    EdgeLabelType ml_edge_label = this->ml_label_;
    this->last_frquent_bound_ = this->frquent_bound_;
    //....
    /*
    // test small conf
    std::reverse(discover_gmar.begin(), discover_gmar.end());
    std::vector<gmine_new::DiscoverdGPAR<Pattern, DataGraph>> temp_gmar_list;
    for (int i = 0; i < 20; i++) temp_gmar_list.push_back(discover_gmar[i]);
    std::swap(discover_gmar, temp_gmar_list);
    */
    using TrainDataType =
        std::tuple<DataGraphVertexPtr, DataGraphVertexPtr, bool>;
    std::vector<TrainDataType> train_data;
    GetTrainData(discover_gmar, data_graph, train_data);
    std::vector<int> send_message_frquent_bound;
    LOG(INFO) << "Cal bound begin";
    for (const auto &[vertex_ptr_x, vertex_ptr_y, train_value] : train_data) {
      auto bound = this->CalBound(vertex_ptr_x, vertex_ptr_y);
      send_message_frquent_bound.push_back(bound);
    }
    std::ofstream log_file("/Users/apple/Desktop/log.txt");

    for (int i = 0; i < send_message_frquent_bound.size(); i++) {
      log_file << std::get<0>(train_data[i])->id() << " "
               << std::get<1>(train_data[i])->id() << " "
               << std::get<2>(train_data[i]) << " "
               << send_message_frquent_bound[i] << std::endl;
    }

    LOG(INFO) << "Cal bound end" << std::endl;
    double max_accur = 0.0;
    LOG(INFO) << "Cal frequent bound begin" << std::endl;
    auto temp = send_message_frquent_bound;
    std::sort(temp.begin(), temp.end());
    std::reverse(temp.begin(), temp.end());
    temp.erase(unique(temp.begin(), temp.end()), temp.end());
    for (const auto &candidate_bound : temp) {
      double accur = this->CalAccur(candidate_bound, train_data,
                                    send_message_frquent_bound);
      std::cout << candidate_bound << " " << accur << std::endl;
      if (accur >= 0.9) {
        max_accur = accur;
        this->frquent_bound_ = candidate_bound;
      }
    }
    LOG(INFO) << "Cal bound end" << std::endl;
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
    using VertexConstPtr = typename DataGraph::VertexConstPtr;
    using EdgeIDType = typename DataGraph::EdgeType::IDType;
    EdgeLabelType ml_edge_label = this->ml_label_;
    VertexIDType max_vertex_id = 0;
    EdgeIDType next_edge_id = 0;
    for (auto vertex_it = ml_graph.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      max_vertex_id = std::max(max_vertex_id, vertex_it->id());
      for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
           edge_it++) {
        next_edge_id = std::max(next_edge_id, edge_it->id());
      }
    }
    ++next_edge_id;
    for (auto vertex_it = ml_graph.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      VertexConstPtr vertex_ptr = vertex_it;
      for (auto adj_it = vertex_it->OutVertexCBegin(2002); !adj_it.IsDone();
           adj_it++) {
        VertexConstPtr adj_ptr = adj_it;
        if (adj_it->label() == 1002 && this->Satisfy(vertex_ptr, adj_ptr)) {
          ml_graph.AddEdge(vertex_it->id(), adj_ptr->id(), ml_edge_label,
                           next_edge_id++);
        }
      }
    }
  }
  DataGraphLabel ModelLabel() const override { return this->ml_label_; }

 private:
  int frquent_bound_;
  int last_frquent_bound_;
  DataGraphLabel ml_label_;
};
}  // namespace gmine_new
#endif