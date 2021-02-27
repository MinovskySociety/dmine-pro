#ifndef TGRAPH_H
#define TGRAPH_H

#include "config.h"
#include "global.h"

namespace toy {
class TGraph {
 public:
  using iterator = std::vector<DataGraph>::iterator;
  using const_iterator = std::vector<DataGraph>::const_iterator;

  TGraph() {}
  TGraph(Config &config) {
    vfile_ = config.GraphVFilePath();
    efile_ = config.GraphEFilePath();
    time_window_number_ = config.TimeWindowNumber();
    time_window_size_ = config.TimeWindowSize();
    frequency_limit_ = config.FrequencyLimit();
    use_partition_ = NO_METHOD == config.PartitionMethod() ? false : true;
  }
  ~TGraph() {}

  void LoadTGraph() {
    LOG_S("Load Graph Strat");
    auto t_begin = std::chrono::steady_clock::now();
    graph_stream_.resize(time_window_number_);
    if (use_partition_) {
      if (time_window_number_ == 0) {
        LOG_E("Time Window Number cannot be 0!");
        throw(TException::LoadGraphError);
      }
      for (int i = 0; i < time_window_number_; i++) {
        ReadCSVGraph(graph_stream_[i], vfile_, efile_ + std::to_string(i));
      }
    } else {
      ReadCSVGraph(graph_stream_[0], vfile_, efile_);
    }
    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Load Graph Success, Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
  }

  void LoadTGraphByHash(int w_id, int w_num) {
    LOG_S("Load Graph Strat");
    auto t_begin = std::chrono::steady_clock::now();
    if (use_partition_) {
      if (time_window_number_ == 0) {
        LOG_E("Time Window Number cannot be 0!");
        throw(TException::LoadGraphError);
      }
      int g_size = time_window_number_ / w_num;
      if (w_id < (time_window_number_ % w_num)) {
        g_size++;
      }
      graph_stream_.resize(g_size);
      int cnt = 0;
      for (int i = 0; i < time_window_number_; i++) {
        if (w_id == (i % w_num)) {
          ReadCSVGraph(graph_stream_[cnt++], vfile_,
                       efile_ + std::to_string(i));
        }
      }
    } else {
      if (w_id == 0) {
        graph_stream_.resize(1);
        ReadCSVGraph(graph_stream_[0], vfile_, efile_);
      }
    }
    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Load Graph Success, Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
  }

  inline std::vector<DataGraph> &GraphStream() { return graph_stream_; }
  inline iterator Begin() { return graph_stream_.begin(); }
  inline iterator End() { return graph_stream_.end(); }

  inline const_iterator CBegin() const { return graph_stream_.cbegin(); }
  inline const_iterator CEnd() const { return graph_stream_.cend(); }
  inline size_t Size() const { return graph_stream_.size(); }
  inline SIZE_T TimeWindowNumber() const { return time_window_number_; }
  inline TIME_T TimeWindowSize() const { return time_window_size_; }

  inline const std::unordered_map<VLABEL_T, std::unordered_set<VLABEL_T>>
      &VLabelVLabelHashTable() const {
    return vlabel2vlabel_;
  }
  inline const std::unordered_map<VLABEL_T, std::unordered_set<VLABEL_T>>
      &VLabelVLabelInnerHashTable() const {
    return vlabel2vlabel_inner_;
  }
  inline const std::unordered_map<uint64_t, std::unordered_set<ELABEL_T>>
      &VLabelELabelHashTable() const {
    return vlabel2elabel_;
  }

  /* judge elabel whether has order */
  inline bool HasOrder(ELABEL_T elabel) const {
    if (elabel2order_.find(elabel) != elabel2order_.end()) {
      return elabel2order_.at(elabel);
    }
    return false;
  }

  void CalEdgeStatics() {
    for (const auto &graph : graph_stream_) {
      for (auto edge_iter = graph.EdgeCBegin(); !edge_iter.IsDone();
           edge_iter++) {
        const auto src_vertex_label = edge_iter->const_src_ptr()->label();
        const auto dst_vertex_label = edge_iter->const_dst_ptr()->label();
        const auto edge_label = edge_iter->label();
        // std::cout << "TEST: " << src_vertex_label << "-" << dst_vertex_label
        // << "-" << edge_label << "\n";
        // set hash between vlabel1 to vlabel2
        if (vlabel2vlabel_.find(src_vertex_label) == vlabel2vlabel_.end()) {
          std::unordered_set<VLABEL_T> vlabel_set;
          vlabel2vlabel_[src_vertex_label] = vlabel_set;
        }
        vlabel2vlabel_[src_vertex_label].insert(dst_vertex_label);
        // set hash between vlabel2 to vlabel1 (for inner vertex expand)
        if (vlabel2vlabel_inner_.find(dst_vertex_label) ==
            vlabel2vlabel_inner_.end()) {
          std::unordered_set<VLABEL_T> vlabel_set;
          vlabel2vlabel_inner_[dst_vertex_label] = vlabel_set;
        }
        vlabel2vlabel_inner_[dst_vertex_label].insert(src_vertex_label);
        // set hash between vlabel1-vlabel2 to elabel
        const uint64_t v1v2hash =
            GetLabelHash(src_vertex_label, dst_vertex_label);
        if (vlabel2elabel_.find(v1v2hash) == vlabel2elabel_.end()) {
          std::unordered_set<VLABEL_T> elabel_set;
          vlabel2elabel_[v1v2hash] = elabel_set;
        }
        vlabel2elabel_[v1v2hash].insert(edge_label);
        // set edge whether has order
        if (elabel2order_.find(edge_label) == elabel2order_.end()) {
          auto attribute_ptr = edge_iter->FindConstAttributePtr(TIME_KEY);
          ATTRIBUTE_PTR_CHECK(attribute_ptr);
          const TIME_T timestamp = attribute_ptr->template const_value<int>();
          elabel2order_[edge_label] = timestamp ? true : false;
        }
        // conut frequency
        const uint64_t v1v2ehash =
            GetLabelHash(src_vertex_label, dst_vertex_label, edge_label);
        if (label2cnt_.find(v1v2ehash) == label2cnt_.end()) {
          label2cnt_[v1v2ehash] = 0;
        }
        label2cnt_[v1v2ehash]++;
      }
    }
  }

  void FilterEdgeStatics() {
    // cal all count
    long total_cnt = 0;
    for (const auto &iter : label2cnt_) {
      total_cnt += iter.second;
    }
    // do filter
    auto vlabel_iter = vlabel2vlabel_.begin();
    while (vlabel_iter != vlabel2vlabel_.end()) {
      const auto &vlabel1 = vlabel_iter->first;
      const auto &vlabel2_set = vlabel_iter->second;
      auto vlabel2_iter = vlabel2_set.begin();
      while (vlabel2_iter != vlabel2_set.end()) {
        const auto &vlabel2 = *vlabel2_iter;
        const uint64_t v1v2hash = GetLabelHash(vlabel1, vlabel2);
        if (vlabel2elabel_.find(v1v2hash) != vlabel2elabel_.end()) {
          const auto &elabel_set = vlabel2elabel_[v1v2hash];
          auto elabel_iter = elabel_set.begin();
          while (elabel_iter != elabel_set.end()) {
            const auto &elabel = *elabel_iter;
            const uint64_t v1v2ehash = GetLabelHash(vlabel1, vlabel2, elabel);
            if (label2cnt_.find(v1v2ehash) != label2cnt_.end()) {
              const auto cnt = label2cnt_[v1v2ehash];
              GRADE_T percent = static_cast<GRADE_T>(cnt) / total_cnt;
              if (percent < frequency_limit_) {
                std::cout << PLABEL << "Filter " << vlabel1 << "-" << vlabel2
                          << "-" << elabel << ", Percent: " << percent << "\n";
                // delete this vlabel1-vlabel2-elabel statics
                label2cnt_.erase(v1v2ehash);
                vlabel2elabel_[v1v2hash].erase(elabel_iter++);
              } else {
                elabel_iter++;
              }
            }
          }
          if (elabel_set.empty()) {
            vlabel2elabel_.erase(v1v2hash);
            vlabel2vlabel_[vlabel1].erase(vlabel2_iter++);
            vlabel2vlabel_inner_[vlabel2].erase(vlabel1);
          } else {
            vlabel2_iter++;
          }
          if (vlabel2vlabel_inner_[vlabel2].empty()) {
            vlabel2vlabel_inner_.erase(vlabel2);
          }
        }
      }
      if (vlabel2vlabel_[vlabel1].empty()) {
        vlabel2vlabel_.erase(vlabel_iter++);
      } else {
        vlabel_iter++;
      }
    }
    // remove elabel order
    std::unordered_map<ELABEL_T, bool> tmp;
    for (auto v2e_iter : vlabel2elabel_) {
      const auto &elabel_set = v2e_iter.second;
      for (const auto elabel : elabel_set) {
        if (tmp.find(elabel) == tmp.end()) {
          tmp[elabel] = elabel2order_[elabel];
        }
      }
    }
    elabel2order_ = tmp;
  }

  void PrintEdgeStatics() const {
    std::cout << PLABEL << "VLabel-VLabel-ELabel, Count\n";
    for (const auto &label : vlabel2vlabel_) {
      const auto &vlabel1 = label.first;
      const auto &vlabel2_set = label.second;
      for (const auto &vlabel2 : vlabel2_set) {
        const uint64_t v1v2hash = GetLabelHash(vlabel1, vlabel2);
        if (vlabel2elabel_.find(v1v2hash) != vlabel2elabel_.end()) {
          const auto &elabel_set = vlabel2elabel_.at(v1v2hash);
          for (const auto &elabel : elabel_set) {
            const uint64_t v1v2ehash = GetLabelHash(vlabel1, vlabel2, elabel);
            if (label2cnt_.find(v1v2ehash) != label2cnt_.end()) {
              const auto &cnt = label2cnt_.at(v1v2ehash);
              std::cout << PLABEL << vlabel1 << "-" << vlabel2 << "-" << elabel
                        << ": " << cnt << "\n";
            }
          }
        }
      }
    }
    std::cout << PLABEL << "ELabel Whether Has Order\n";
    for (const auto &iter : elabel2order_) {
      std::cout << PLABEL << iter.first << ": " << iter.second << "\n";
    }
  }

 protected:
  std::string vfile_, efile_;
  std::vector<DataGraph> graph_stream_;
  SIZE_T time_window_number_;
  TIME_T time_window_size_;
  GRADE_T frequency_limit_;
  bool use_partition_;

  // edge statistics
  std::unordered_map<uint64_t, SIZE_T>
      label2cnt_;  // <vlabel1-vlabel2-elabel, count>
  std::unordered_map<VLABEL_T, std::unordered_set<VLABEL_T>>
      vlabel2vlabel_;  // <vlabel1, set<vlabel2>>
  std::unordered_map<VLABEL_T, std::unordered_set<VLABEL_T>>
      vlabel2vlabel_inner_;  // <vlabel2, set<vlabel1>>
  std::unordered_map<uint64_t, std::unordered_set<ELABEL_T>>
      vlabel2elabel_;  // <vlabel1-vlabel2, set<elabel>>
  std::unordered_map<ELABEL_T, bool>
      elabel2order_;  // <elabel, whether_has_order>
};
}  // namespace toy

#endif  // TGRAPH_H
