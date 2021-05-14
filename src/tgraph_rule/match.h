#ifndef MATCH_H
#define MATCH_H

#include "global.h"
#include "tgraph.h"

namespace toy {
/* Match order algorithm
 * @param: match vertex pair
 * @return: true/false
 * @desc: return true if edge orders match
 */
static bool OrderMatchAlgorithm(const MatchMap &match_map) {
  // caution: No consider for duplicated edge, like this: e1=(u, v, l, t),
  // e2=(u, v, l, t)
  // LOG_T("Do Order Check Filter...");
  EdgeMap edge_map;
  std::vector<TimeQueue> time_q_set;
  std::vector<ConstQueryVertex> query_vertex_list;
  std::vector<ConstTargetVertex> target_vertex_list;
  for (auto &iter : match_map) {
    query_vertex_list.emplace_back(iter.first);
    target_vertex_list.emplace_back(iter.second);
  }
  // get edge info
  long cnt = 0;
  for (int i = 0; i < query_vertex_list.size(); i++) {
    for (int j = 0; j < query_vertex_list.size(); j++) {
      auto &query_src_vertex_id = query_vertex_list[i]->id();
      auto &query_dst_vertex_id = query_vertex_list[j]->id();
      // std::cout << query_src_vertex_id << "," << query_dst_vertex_id << "\n";
      // get query edge
      for (auto query_edge_iter = query_vertex_list[i]->OutEdgeBegin();
           !query_edge_iter.IsDone(); query_edge_iter++) {
        auto &vertex_id = query_edge_iter->dst_id();
        if (query_dst_vertex_id == vertex_id) {
          auto &edge_label = query_edge_iter->label();
          auto attribute_ptr = query_edge_iter->FindAttribute(TIME_KEY);
          ATTRIBUTE_PTR_CHECK(attribute_ptr);
          TIME_T timestamp = attribute_ptr->template value<int>();
          // needn't check order if timestamp equals 0.
          if (timestamp == 0) continue;
          // store edge info
          std::vector<TIME_T> edge_info;
          edge_info.emplace_back(timestamp);  // order in query edge
          edge_info.emplace_back(cnt++);      // map id
          edge_map.emplace_back(edge_info);
          // get target edge matched with query edge
          TimeQueue time_queue;
          auto &target_src_vertex_id = target_vertex_list[i]->id();
          auto &target_dst_vertex_id = target_vertex_list[j]->id();
          for (auto target_edge_iter = target_vertex_list[i]->OutEdgeBegin(
                   edge_label, target_vertex_list[j]);
               !target_edge_iter.IsDone(); target_edge_iter++) {
            auto attribute_ptr = target_edge_iter->FindAttribute(TIME_KEY);
            ATTRIBUTE_PTR_CHECK(attribute_ptr);
            TIME_T timestamp = attribute_ptr->template value<int>();
            time_queue.push(timestamp);
          }
          time_q_set.emplace_back(time_queue);
        }
      }
    }
  }

  // std::cout << "[TEST] before sort\n";
  // TestPrintEdgeMap(edge_map, time_q_set, 0);
  // sort by timestamp
  std::sort(edge_map.begin(), edge_map.end(), time_cmp);
  // std::cout << "[TEST] after sort\n";
  // TestPrintEdgeMap(edge_map, time_q_set, 0);

  // check timestamps whether obey the order
  for (int i = 0, same_end = 0; i < edge_map.size(); i++, same_end = i) {
    // LOG("............. Check match pair " + std::to_string(i));
    TIME_T time_limit = 0;
    auto &time_queue = time_q_set[edge_map[i][1]];
    // if cannot find match order, return
    if (time_queue.empty()) return false;
    // find time_limit
    // if order is same, judge its timestamp in graph
    if (i + 1 < edge_map.size() && edge_map[i][0] == edge_map[i + 1][0]) {
      // find same timestamp in graph
      std::list<TIME_T> inter_list;
      {
        TIME_T order = edge_map[i][0];
        while (same_end + 1 < edge_map.size() &&
               edge_map[same_end + 1][0] == order)
          same_end++;
        GetIntersection(inter_list, time_q_set[edge_map[i][1]]);
        for (int begin = i + 1; begin <= same_end; begin++) {
          if (inter_list.empty()) break;
          GetIntersection(inter_list, time_q_set[edge_map[begin][1]]);
        }
      }
      // if do not have same timestamp, return
      if (inter_list.empty()) return false;
      // pick first satisfied timestamp
      time_limit = *inter_list.begin();
    }
    // if order is not same, pick the min timestamp as time limit
    else {
      time_limit = time_queue.top();
    }
    // LOG_T("time_limit = " + std::to_string(time_limit));
    // std::cout << "[TEST] before filter\n";
    // TestPrintEdgeMap(edge_map, time_q_set, i);
    // filter later time queue by time limit
    for (int j = i; j < edge_map.size(); j++) {
      auto &check_time_queue = time_q_set[edge_map[j][1]];
      while (!check_time_queue.empty() && check_time_queue.top() <= time_limit)
        time_q_set[edge_map[j][1]].pop();
      // later time queue have no time to match, return false
      if (j > same_end && check_time_queue.empty()) return false;
    }
    // std::cout << "[TEST] after filter\n";
    // TestPrintEdgeMap(edge_map, time_q_set, i);
    // jump to unchecked pair
    if (same_end > i) i = same_end;
  }
  return true;
}

/* Clear match result
 * @param: match result
 */
void MatchClear(MatchResult &match_result) {
  match_result.clear();
  match_result.shrink_to_fit();
}

/* Get match number
 * @param: match result
 * @return: match number
 */
SIZE_T MatchNum(const MatchResult &match_result) {
  return static_cast<SIZE_T>(match_result.size());
}

/* Class Match
 * @desc: match method
 */
class Match {
 public:
  Match() {}
  ~Match() {}

  /* Do match with x in pattern
   * @param: tgraph, pattern, match result
   * @desc: only calculate match of pattern with x once in t-graph
   */
  void DoMatchWithX(const TGraph &tg, const Pattern &pattern,
                    MatchResult &match_result) {  // NOTE: setX must be called
                                                  // once before this function
    // LOG_S("Match Start");
    CandidateSetContainer candidate_set, candidate_set_tmp;
    MatchMap match_state;
    const auto &x_ptr = pattern.FindVertex(x_id_);
    auto target_set = x_hash_set_;
    // do match in a graph of stream
    auto t_begin = std::chrono::steady_clock::now();
    for (TGraph::const_iterator iter = tg.CBegin(); iter != tg.CEnd(); iter++) {
      candidate_set.clear();
      _dp_iso::InitCandidateSet<MatchSemantics::kIsomorphism>(pattern, *iter,
                                                              candidate_set);
      // std::cout << "T1\n";
      //_dp_iso::RefineCandidateSet(pattern, *iter, candidate_set);
      // std::cout << "T2\n";
      if (candidate_set[x_ptr].empty()) continue;
      candidate_set[x_ptr].clear();
      candidate_set[x_ptr].shrink_to_fit();
      // do match for each x candidate
      for (auto id_iter = target_set.begin(); id_iter != target_set.end();) {
        match_state.clear();
        match_state[x_ptr] = iter->FindVertex(*id_iter);
        candidate_set_tmp = candidate_set;
        size_t size_bf = match_result.size();
        // std::cout << size_bf << "\n";
        matchX(*iter, pattern, match_result, candidate_set_tmp, match_state);
        // match(*iter, pattern, match_result);
        size_t size_af = match_result.size();
        // std::cout << size_af << "\n";
        if (size_af > size_bf) {
          target_set.erase(id_iter++);
        } else {
          id_iter++;
        }
        // time stop
        auto t_end = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(t_end - t_begin).count() >
            MAX_MATCH_TIME) {
          break;
        }
      }
      auto t_end = std::chrono::steady_clock::now();
      if (std::chrono::duration<double>(t_end - t_begin).count() >
          MAX_MATCH_TIME) {
        break;
      }
    }
  }

  /* Do match
   * @param: tgraph, pattern, match result
   * @desc: calculate all matches of pattern in t-graph
   */
  void DoMatch(const TGraph &tg, const Pattern &pattern,
               MatchResult &match_result) {
    // LOG_S("Match Start");
    auto t_begin = std::chrono::steady_clock::now();
    // do match in a graph of stream
    for (TGraph::const_iterator iter = tg.CBegin(); iter != tg.CEnd(); iter++) {
      match(*iter, pattern, match_result);
    }
    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Match End, Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
  }

  void DoMatch(const Pattern &p1, const Pattern &p2,
               MatchResult &match_result) {
    match(p1, p2, match_result);
  }

  /* Init x
   * @param: tgraph, link
   * @desc: init x with link and record x before match
   */
  void InitX(const TGraph &tg, const LinkBase &link) {
    x_id_ = link.from_;
    const auto xlabel = link.from_label_;
    LOG_T("XLabel = ", xlabel);
    // Note: here only init X with vertexes in first time window
    const auto &graph = *(tg.CBegin());
    for (auto vertex_it = graph.VertexBegin(); !vertex_it.IsDone();
         vertex_it++) {
      if (vertex_it->label() == xlabel) {
        VID_T vid = vertex_it->id();
        if (x_hash_set_.find(vid) == x_hash_set_.end()) {
          x_hash_set_.emplace(vid);
        }
      }
    }
  }

 private:
  /* match without x */
  virtual void match(const DataGraph &data_graph, const Pattern &pattern,
                     MatchResult &match_result) = 0;
  /* match with x */
  virtual void matchX(const DataGraph &data_graph, const Pattern &pattern,
                      MatchResult &match_result,
                      CandidateSetContainer &candidates,
                      MatchMap &match_state) = 0;

 protected:
  // vertex statistics
  VID_T x_id_;  // x id
  // runtime data
  std::unordered_set<VID_T> x_hash_set_;  // x to target id
};                                        // class Match

/* MatchNoOrder
 * @desc: match without order of edge
 */
class MatchNoOrder : public Match {
 public:
  MatchNoOrder() {}
  ~MatchNoOrder() {}

 private:
  virtual void match(const DataGraph &data_graph, const Pattern &pattern,
                     MatchResult &match_result) {
    /* empty match callback */
    auto match_callback = [&match_result](const MatchMap &match_map) {
      match_result.emplace_back(match_map);  // match
      return true;                           // continue match
    };

    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, match_callback,
                                        MAX_MATCH_TIME);
  }

  virtual void matchX(const DataGraph &data_graph, const Pattern &pattern,
                      MatchResult &match_result,
                      CandidateSetContainer &candidates,
                      MatchMap &match_state) {
    /* empty match callback */
    auto match_callback = [&match_result](const MatchMap &match_map) {
      match_result.emplace_back(match_map);  // match
      return false;                          // stop match
    };

    /* empty prune callback */
    auto prune_callback = [](const MatchMap &match_state) { return false; };

    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, candidates,
                                        match_state, match_callback,
                                        prune_callback, MAX_MATCH_TIME);
  }

};  // class MatchNoOrder

/* MatchWithOrder
 * @desc: match with order of edge
 */
class MatchWithOrder : public Match {
 public:
  MatchWithOrder() {}
  ~MatchWithOrder() {}

 private:
  virtual void match(const DataGraph &data_graph, const Pattern &pattern,
                     MatchResult &match_result) {
    /* order match callback */
    auto match_callback = [&match_result](const MatchMap &match_map) {
      if (OrderMatchAlgorithm(match_map)) {
        match_result.emplace_back(match_map);  // match
      }
      return true;  // continue match
    };

    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, match_callback,
                                        MAX_MATCH_TIME);
  }

  virtual void matchX(const DataGraph &data_graph, const Pattern &pattern,
                      MatchResult &match_result,
                      CandidateSetContainer &candidates,
                      MatchMap &match_state) {
    /* order match callback */
    auto match_callback = [&match_result](const MatchMap &match_map) {
      if (OrderMatchAlgorithm(match_map)) {
        match_result.emplace_back(match_map);  // match
        return false;                          // stop match
      }
      return true;
    };

    /* empty prune callback */
    auto prune_callback = [](const MatchMap &match_state) { return false; };

    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, candidates,
                                        match_state, match_callback,
                                        prune_callback, MAX_MATCH_TIME);
  }

};  // class MatchWithOrder

/* MatchWithOrder
 * @desc: match with order of edge and filter in every match node
 */
class MatchWithOrderFilter : public Match {
 public:
  MatchWithOrderFilter() {}
  ~MatchWithOrderFilter() {}

 private:
  virtual void match(const DataGraph &data_graph, const Pattern &pattern,
                     MatchResult &match_result) {
    /* empty match callback */
    auto match_callback = [&match_result](const MatchMap &match_map) {
      match_result.emplace_back(match_map);  // match
      return true;                           // continue match
    };

    /* prune callback */
    auto prune_callback = [](const MatchMap &match_state) {
      if (OrderMatchAlgorithm(match_state)) {
        return false;
      }
      return true;
    };

    /* empty candidates callback */
    auto update_callback = [](CandidateSetContainer &candidate_set) {
      return true;
    };

    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, match_callback,
                                        prune_callback, update_callback,
                                        MAX_MATCH_TIME);
  }

  virtual void matchX(const DataGraph &data_graph, const Pattern &pattern,
                      MatchResult &match_result,
                      CandidateSetContainer &candidates,
                      MatchMap &match_state) {
    /* empty match callback */
    auto match_callback = [&match_result](const MatchMap &match_map) {
      match_result.emplace_back(match_map);  // match
      return false;                          // stop match
    };

    /* prune callback */
    auto prune_callback = [](const MatchMap &match_state) {
      if (OrderMatchAlgorithm(match_state)) {
        return false;
      }
      return true;
    };

    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, candidates,
                                        match_state, match_callback,
                                        prune_callback, MAX_MATCH_TIME);
  }

};  // class MatchWithOrderFilter

/* Class MatchFactory
 * @desc: return one match method
 */
class MatchFactory {
 public:
  MatchFactory() {}
  ~MatchFactory() {}

  static std::shared_ptr<Match> GetMatchMethod(Config &config) {
    bool use_order = config.WhetherUseOrder();
    bool use_match_filter = config.WhetherUseMatchFilter();
    if (!use_order) {
      return std::make_shared<MatchNoOrder>();
    } else if (use_order && !use_match_filter) {
      return std::make_shared<MatchWithOrder>();
    } else if (use_order && use_match_filter) {
      return std::make_shared<MatchWithOrderFilter>();
    } else {
      throw("Not Support Match Method!");
    }
  }
};  // class MatchFactory

}  // namespace toy
#endif  // MATCH_H
