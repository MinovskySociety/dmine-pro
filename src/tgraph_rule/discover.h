#ifndef DISCOVER_H
#define DISCOVER_H

#include <omp.h>

#include "../tgraph_rule/k_queue.h"
#include "../tgraph_rule/match.h"
#include "../tgraph_rule/tgraph.h"
#include "../tgraph_rule/tgraph_rule.h"
#include "config.h"
#include "global.h"

namespace toy {

static omp_lock_t OMP_LOCK;
/* Class Discover
 * @desc: Discover TGR in loops
 */
class Discover {
 public:
  Discover(TGraph &tg, TGraphRule &tgr, Config &config,
           std::shared_ptr<Match> m_ptr,
           std::shared_ptr<Match> m_assistant_ptr = nullptr)
      : tgraph_(tg),
        tgraph_rule_(tgr),
        m_ptr_(m_ptr),
        m_assistant_ptr_(m_assistant_ptr),
        link_(tgr.GetLink()) {
    supp_threshold_ = config.SupportThreshold();
    select_k_ = config.SelectK();
    max_d_ = config.MaxD();
    use_order_ = config.WhetherUseOrder();
    output_name_ = config.OutputName();
    output_path_ = config.OutputPath();
    // init
    const auto &rule = tgr.GetTGR(0);
    candidates_.emplace_back(rule);
    top_k_queue_.init(select_k_);
    // multi-threads number
    size_t thread_num = config.ThreadNum();
    omp_set_num_threads(thread_num);
    next_candidates_.resize(thread_num);
    // once candidates max number
    /*const uint32_t candidates_max = 5000;
    for (int i = 0; i < thread_num; i++) {
      next_candidates_[i].reserve(candidates_max);
    }*/
  }
  ~Discover() {}

  /* Do Discover */
  void DoDiscover() {
    LOG_S("Discover Start...");
    auto t_begin = std::chrono::steady_clock::now();
    // cal |x|
    Q x_query;
    x_query.AddVertex(link_.from_, link_.from_label_);
    x_supp_ = calSupp(x_query);
    LOG_T("x_supp: ", x_supp_);
    if (x_supp_ == 0) {
      LOG_S("No Any Match For X!");
      return;
    }
    // cal |q(x,y)|
    x_query.AddVertex(link_.to_, link_.to_label_);
    auto edge_iter =
        x_query.AddEdge(link_.from_, link_.to_, link_.label_, 1).first;
    edge_iter->AddAttribute(TIME_KEY, static_cast<TIME_T>(0));
    xyq_supp_ = calSupp(x_query);
    LOG_T("xyq_supp: ", xyq_supp_);
    if (xyq_supp_ == 0) {
      LOG_S("No Any Match For q(x,y)!");
      return;
    }
    omp_init_lock(&OMP_LOCK);
    for (int i = 0; i <= max_d_; i++) {
      LOG_S("Expand Round ", i);
      auto t_begin_2 = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic)
      for (int pos = 0; pos < candidates_.size(); pos++) {
        const auto &query = candidates_[pos];  // tgr rule
        GRADE_T r_supp = 0, conf = 0;
        calConf(query, r_supp, conf);
        if (r_supp != 0) {
          // update top-k queue
          // LOG_T("update topk queue: ", pos);
          omp_set_lock(&OMP_LOCK);
          if (!top_k_queue_.full() || top_k_queue_.top().second < conf) {
            if (!existInTopKQueue(top_k_queue_, query, conf)) {
              auto query_conf = std::make_pair(query, conf);
              top_k_queue_.emplace(query_conf);
            }
          }
          omp_unset_lock(&OMP_LOCK);
          // LOG_T("update topk queue successfully, size = ",
          // top_k_queue_.size());
          // expand
          // LOG_T("expand edge: ", pos);
          if (i != max_d_ && r_supp >= supp_threshold_) {
            int thread_id = omp_get_thread_num();
            expandEdge(next_candidates_[thread_id], query, r_supp);
          }
          // LOG_T("expand edge end: ", pos);
        }
      }
      size_t cnt = 0;
      for (auto &can : next_candidates_) {
        cnt += can.size();
      }
      candidates_.clear();

      // solid support threshold
      /*candidates_.reserve(cnt);
      for (auto& can : next_candidates_) {
        for (auto& query : can) {
          if (!existInCandidates(candidates_, query.first)) {
            candidates_.emplace_back(query.first);
          }
        }
        can.clear();
      }*/
      // dynamic support threshold
      candidates_.reserve(2 * select_k_);
      std::vector<std::pair<Q, GRADE_T>> sort_cans;
      sort_cans.reserve(cnt);
      for (auto &can : next_candidates_) {
        for (auto &query : can) {
          if (!existInCandidates(sort_cans, query.first)) {
            sort_cans.emplace_back(query);
          }
        }
        can.clear();
      }
      sort(sort_cans.begin(), sort_cans.end(), conf_cmp);
      int snum = 2 * select_k_ < cnt
                     ? 2 * select_k_ + (cnt - 2 * select_k_) * 0.1
                     : 2 * select_k_;
      candidates_.reserve(snum);
      for (auto iter = sort_cans.begin(); iter != sort_cans.end(); iter++) {
        if (snum-- == 0) break;
        candidates_.emplace_back(iter->first);
      }

      printCandidates(candidates_, false);
      printTopKQueue(false);
    }
    omp_destroy_lock(&OMP_LOCK);
    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Discover End, Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
    printTopKQueue(true);
    writeTopKQueue(top_k_queue_);
  }

 private:
  /* calculate support of given query */
  SIZE_T calSupp(const Q &query) {
    MatchResult match_rlt;
    // PrintTGR(query);
    m_ptr_->DoMatchWithX(tgraph_, query, match_rlt);
    // m_ptr_->DoMatch(tgraph_, query, match_rlt);
    // PrintMatch(match_rlt);
    return MatchNum(match_rlt);
  }

  /* calculate support of given query, using assistant match method */
  SIZE_T calSuppByAssistantMatchPtr(const Q &query) {
    MatchResult match_rlt;
    // PrintTGR(query);
    m_assistant_ptr_->DoMatchWithX(tgraph_, query, match_rlt);
    // PrintMatch(match_rlt);
    return MatchNum(match_rlt);
  }

  /* calculate confidnece of given query */
  void calConf(const Q &query, GRADE_T &r_supp, GRADE_T &conf) {
    r_supp = calSupp(query);
    conf = (r_supp * 1.0) / x_supp_;
    std::cout << "r_supp = " << r_supp << ", conf = " << conf << "\n";
  }

  /* calculate PCA confidnece of given query */
  void calPCAConf(const Q &query, GRADE_T &r_supp, GRADE_T &conf) {
    r_supp = calSupp(query);
    Q q_query(query);
    removeLink(q_query);
    GRADE_T q_supp = calSupp(q_query);
    conf = q_supp <= r_supp ? 0 : (r_supp * 1.0) / (q_supp - r_supp);
    std::cout << "q_supp = " << q_supp << ", r_supp = " << r_supp
              << ", pca-conf = " << conf << "\n";
  }

  /* calculate BF confidnece of given query */
  void calBFConf(const Q &query, GRADE_T &r_supp, GRADE_T &conf) {
    r_supp = calSupp(query);
    Q q_query(query);
    removeLink(q_query);
    GRADE_T q_supp = calSupp(q_query);
    conf = q_supp <= r_supp ? 0
                            : (r_supp * (x_supp_ - xyq_supp_) * 1.0) /
                                  ((q_supp - r_supp) * x_supp_);
    std::cout << "q_supp = " << q_supp << ", r_supp = " << r_supp
              << ", bf-conf = " << conf << "\n";
  }

  /* Expand Edge
   * @param: next candidates set, query, rule support
   * @desc: expand given query(rule), select one edge
   */
  void expandEdge(std::vector<std::pair<Q, GRADE_T>> &next_candidates,
                  const Q &query, const GRADE_T r_supp) {
    NewEdge expand_e;  // expand edge info
    size_t v_cnt = query.CountVertex();
    size_t e_cnt = query.CountEdge();
    expand_e.next_vid_ = v_cnt + 1;
    expand_e.next_eid_ = e_cnt + 1;
    expand_e.max_order_ = 0;
    // find max order in query
    if (use_order_) {
      for (auto edge_iter = query.EdgeBegin(); !edge_iter.IsDone();
           edge_iter++) {
        if (edge_iter->id() == link_.id_) continue;  // jump link
        auto attr_ptr = edge_iter->FindAttribute(TIME_KEY);
        ATTRIBUTE_PTR_CHECK(attr_ptr);
        TIME_T order = attr_ptr->template value<int>();
        if (order > expand_e.max_order_) {
          expand_e.max_order_ = order;
        }
      }
    }
    // get expand edge statistics hash table
    const auto &vlabel2vlabel_hash_table = tgraph_.VLabelVLabelHashTable();
    const auto &vlabel2vlabel_inner_hash_table =
        tgraph_.VLabelVLabelInnerHashTable();
    const auto &vlabel2elabel_hash_table = tgraph_.VLabelELabelHashTable();

    for (auto vertex_iter = query.VertexBegin(); !vertex_iter.IsDone();
         vertex_iter++) {
      expand_e.expand_vid_ = vertex_iter->id();
      if (expand_e.expand_vid_ == link_.to_) continue;  // jump link dst
      expand_e.expand_vlabel_ = vertex_iter->label();
      // expand outer edge
      expand_e.is_out_ = true;
      if (vlabel2vlabel_hash_table.find(expand_e.expand_vlabel_) !=
          vlabel2vlabel_hash_table.end()) {
        const auto &dst_vlabel_set =
            vlabel2vlabel_hash_table.at(expand_e.expand_vlabel_);
        for (auto dst_vlabel : dst_vlabel_set) {
          expand_e.next_vlabel_ = dst_vlabel;
          uint64_t v1v2hash = GetLabelHash(expand_e.expand_vlabel_, dst_vlabel);
          if (vlabel2elabel_hash_table.find(v1v2hash) !=
              vlabel2elabel_hash_table.end()) {
            const auto &elabel_set = vlabel2elabel_hash_table.at(v1v2hash);
            for (auto elabel : elabel_set) {
              expand_e.expand_elabel_ = elabel;
              expand_e.has_order_ = tgraph_.HasOrder(elabel);
              addOneEdge(next_candidates, query, expand_e, r_supp);
            }
          }
        }
      }
      // expand inner edge
      expand_e.is_out_ = false;
      if (vlabel2vlabel_inner_hash_table.find(expand_e.expand_vlabel_) !=
          vlabel2vlabel_inner_hash_table.end()) {
        const auto &src_vlabel_set =
            vlabel2vlabel_inner_hash_table.at(expand_e.expand_vlabel_);
        for (auto src_vlabel : src_vlabel_set) {
          expand_e.next_vlabel_ = src_vlabel;
          uint64_t v1v2hash = GetLabelHash(src_vlabel, expand_e.expand_vlabel_);
          if (vlabel2elabel_hash_table.find(v1v2hash) !=
              vlabel2elabel_hash_table.end()) {
            const auto &elabel_set = vlabel2elabel_hash_table.at(v1v2hash);
            for (auto elabel : elabel_set) {
              expand_e.expand_elabel_ = elabel;
              expand_e.has_order_ = tgraph_.HasOrder(elabel);
              addOneEdge(next_candidates, query, expand_e, r_supp);
            }
          }
        }
      }
    }
  }

  /* Add new Edge
   * @param: next candidates set, query, new edge, rule support
   * @desc: add new edge in given query(rule)
   */
  void addOneEdge(std::vector<std::pair<Q, GRADE_T>> &next_candidates,
                  const Q &query, const NewEdge &expand_e,
                  const GRADE_T r_supp) {
    // add edge with new vertex
    {
      Q q_tmp(query);
      q_tmp.AddVertex(expand_e.next_vid_, expand_e.next_vlabel_);
      EdgePtr edge_iter = nullptr;
      if (expand_e.is_out_) {
        edge_iter = q_tmp
                        .AddEdge(expand_e.expand_vid_, expand_e.next_vid_,
                                 expand_e.expand_elabel_, expand_e.next_eid_)
                        .first;
      } else {
        edge_iter = q_tmp
                        .AddEdge(expand_e.next_vid_, expand_e.expand_vid_,
                                 expand_e.expand_elabel_, expand_e.next_eid_)
                        .first;
      }
      edge_iter->AddAttribute(TIME_KEY, static_cast<TIME_T>(0));
      // PrintTGR(q_tmp);
      if (use_order_ && expand_e.has_order_) {  // consider order
        if (calSuppByAssistantMatchPtr(q_tmp) >= supp_threshold_) {
          expandOrder(next_candidates, q_tmp, edge_iter, expand_e, r_supp);
        }
      } else {
        next_candidates.emplace_back(q_tmp, r_supp);
      }
    }

    // add edge in existed vertex
    {
      Q q_tmp(query);
      for (auto vertex_iter = q_tmp.VertexBegin(); !vertex_iter.IsDone();
           vertex_iter++) {
        auto dst_vid = vertex_iter->id();
        if (dst_vid != expand_e.expand_vid_ &&
            vertex_iter->label() == expand_e.next_vlabel_) {
          EdgePtr edge_iter = nullptr;
          if (expand_e.is_out_) {
            edge_iter =
                q_tmp
                    .AddEdge(expand_e.expand_vid_, dst_vid,
                             expand_e.expand_elabel_, expand_e.next_eid_)
                    .first;
          } else {
            edge_iter =
                q_tmp
                    .AddEdge(dst_vid, expand_e.expand_vid_,
                             expand_e.expand_elabel_, expand_e.next_eid_)
                    .first;
          }
          edge_iter->AddAttribute(TIME_KEY, static_cast<TIME_T>(0));
          // PrintTGR(q_tmp);
          if (use_order_ && expand_e.has_order_) {  // consider order
            if (calSuppByAssistantMatchPtr(q_tmp) >= supp_threshold_) {
              expandOrder(next_candidates, q_tmp, edge_iter, expand_e, r_supp);
            }
          } else {
            next_candidates.emplace_back(q_tmp, r_supp);
          }
        }
      }
    }
  }

  /* Expand Order
   * @param: next candidates set, query, edge ptr, new edge, rule support
   * @desc: add order in given query(rule)
   */
  void expandOrder(std::vector<std::pair<Q, GRADE_T>> &next_candidates,
                   Q &query, EdgePtr edge_ptr, const NewEdge &expand_e,
                   const GRADE_T r_supp) {
    // LOG_T("expand order TGR");
    for (int x = 1; x <= expand_e.max_order_; x++) {
      // new edge order equals with [1, max_order]
      edge_ptr->SetAttribute(TIME_KEY, static_cast<TIME_T>(x));
      const auto src_vertex = edge_ptr->const_src_handle();
      const auto dst_vertex = edge_ptr->const_dst_handle();
      bool not_exist = true;
      std::unordered_set<TIME_T> order_hash;
      for (auto query_edge_iter =
               src_vertex->OutEdgeBegin(edge_ptr->label(), dst_vertex);
           !query_edge_iter.IsDone(); query_edge_iter++) {
        auto query_attr_ptr = query_edge_iter->FindAttribute(TIME_KEY);
        ATTRIBUTE_PTR_CHECK(query_attr_ptr);
        TIME_T query_order = query_attr_ptr->template value<int>();
        if (order_hash.find(query_order) == order_hash.end()) {
          order_hash.emplace(query_order);
        } else {
          not_exist = false;
          break;
        }
      }
      // std::cout << "######### equal order\n";
      // PrintTGR(rule);
      if (not_exist) {
        next_candidates.emplace_back(query, r_supp);
      }
      // new edge order insert in [1, max_order]
      Q query_tmp(query);
      for (auto query_edge_iter = query_tmp.EdgeBegin();
           !query_edge_iter.IsDone(); query_edge_iter++) {
        auto query_attr_ptr = query_edge_iter->FindAttribute(TIME_KEY);
        ATTRIBUTE_PTR_CHECK(query_attr_ptr);
        TIME_T query_order = query_attr_ptr->template value<int>();
        if (query_order >= x) {
          query_edge_iter->SetAttribute(TIME_KEY,
                                        static_cast<TIME_T>(query_order + 1));
        }
      }
      auto query_edge_iter = query_tmp.FindEdge(expand_e.next_eid_);
      query_edge_iter->SetAttribute(TIME_KEY, static_cast<TIME_T>(x));
      // std::cout << "######### insert order\n";
      // PrintTGR(rule_tmp);
      next_candidates.emplace_back(query_tmp, r_supp);
    }
    // new edge order is max_order + 1
    edge_ptr->SetAttribute(TIME_KEY,
                           static_cast<TIME_T>(expand_e.max_order_ + 1));
    // if link has order, order++
    if (link_.has_order_) {
      auto link_ptr = query.FindEdge(link_.id_);
      auto link_attr_ptr = link_ptr->FindAttribute(TIME_KEY);
      ATTRIBUTE_PTR_CHECK(link_attr_ptr);
      link_ptr->SetAttribute(TIME_KEY,
                             static_cast<TIME_T>(expand_e.max_order_ + 2));
    }
    // std::cout << "######### last order\n";
    // PrintTGR(rule);
    next_candidates.emplace_back(query, r_supp);
  }

  /* Auto Morphism
   * @param: query1, query2
   * @return: true/false
   * @desc: check whether 2 querys are isomorphic
   */
  bool autoMorphism(const Q &q1, const Q &q2) {
    MatchResult match_rlt;
    m_ptr_->DoMatch(q1, q2, match_rlt);
    if (MatchNum(match_rlt) > 0) {
      MatchClear(match_rlt);
      m_ptr_->DoMatch(q2, q1, match_rlt);
      if (MatchNum(match_rlt) > 0) {
        return true;
      }
    }
    return false;
  }

  /* Exist In Candidates
   * @param: candidate list, query
   * @return: true/false
   * @desc: check whether query and items in candidate list are isomorphic
   */
  bool existInCandidates(const std::vector<Q> &candidate_list, const Q &query) {
    for (const auto &candidate : candidate_list) {
      if (autoMorphism(query, candidate)) {
        return true;
      }
    }
    return false;
  }

  /* Exist In Candidates
   * @param: candidate list, query
   * @return: true/false
   * @desc: check whether query and items in candidate list are isomorphic
   */
  bool existInCandidates(
      const std::vector<std::pair<Q, GRADE_T>> &candidate_list,
      const Q &query) {
    for (const auto &candidate : candidate_list) {
      if (autoMorphism(query, candidate.first)) {
        return true;
      }
    }
    return false;
  }

  /* Exist In Top-K Queue
   * @param: top-k queue, query, confidence
   * @return: true/false
   * @desc: check whether query is existed in top-k queue
   */
  bool existInTopKQueue(const KQueue<Q> &k_queue, const Q &query,
                        GRADE_T conf) {
    for (auto iter = k_queue.cbegin(); iter != k_queue.cend(); iter++) {
      GRADE_T r_conf = (*iter).second;
      if (conf == r_conf) {
        if (autoMorphism(query, (*iter).first)) {
          return true;
        }
      }
    }
    return false;
  }

  /* remove prediction link in query */
  void removeLink(Q &query) {
    query.EraseEdge(link_.id_);
    auto vertex_ptr = query.FindVertex(link_.to_);
    if (vertex_ptr) {
      if (vertex_ptr->CountOutEdge() == 0 && vertex_ptr->CountInEdge() == 0) {
        query.EraseVertex(link_.to_);
      }
    }
  }

  /* For Test, print candidates info */
  void printCandidates(const std::vector<Q> &candidate_list,
                       bool print_detail) const {
    size_t num = candidate_list.size();
    LOG_T("Candidates number: ", num);
    if (print_detail) {
      for (const auto &candidate : candidate_list) {
        PrintTGR(candidate);
      }
    }
  }

  /* For Test, print top-k queue info */
  void printTopKQueue(bool print_detail) const {
    size_t num = top_k_queue_.size();
    LOG_T("Top-K Queue number: ", num);
    if (print_detail) {
      size_t cnt = 0;
      for (auto iter = top_k_queue_.crbegin(); iter != top_k_queue_.crend();
           iter++) {
        std::cout << "No." << ++cnt << ", Conf = " << (*iter).second << "\n";
        PrintTGR((*iter).first);
      }
    }
  }

  /* Write top-k TGR into files */
  void writeTopKQueue(const KQueue<Q> &q) const {
    std::ofstream outfile;
    uint64_t cnt = 1;
    for (auto iter = q.crbegin(); iter != q.crend(); iter++, cnt++) {
      std::string file_path =
          output_path_ + DISCOVERY_PRINT_FILE_NAME + "_" + std::to_string(cnt);
      if (Access(file_path.data(), 0) != 0) {
        if (Mkdir(file_path.data()) != 0) {
          LOG_W("Create Results Dir Failure, File Dir: ", file_path);
          return;
        }
      }
      const auto &query = (*iter).first;
      TIME_T prediction_link_order = 0;
      // write vfile
      {
        std::string vfile = file_path + "/" + DISCOVERY_PRINT_FILE_NAME + "_" +
                            std::to_string(cnt) + "_v.csv";
        outfile.open(vfile.data());
        outfile << "vertex_id:int64,label_id:int\n";
        for (auto v_iter = query.VertexBegin(); !v_iter.IsDone(); v_iter++) {
          outfile << v_iter->id() << "," << v_iter->label() << "\n";
        }
        outfile.close();
      }
      // write efile
      {
        EID_T eid = 2;  // 1 is for prediction link
        std::string efile = file_path + "/" + DISCOVERY_PRINT_FILE_NAME + "_" +
                            std::to_string(cnt) + "_e.csv";
        outfile.open(efile.data());
        outfile << "edge_id:int64,source_id:int64,target_id:int64,label_id:int,"
                   "timestamp:int64\n";
        for (auto e_iter = query.EdgeBegin(); !e_iter.IsDone(); e_iter++) {
          if (e_iter->id() == link_.id_) {
            auto attribute_ptr = e_iter->FindAttribute(TIME_KEY);
            ATTRIBUTE_PTR_CHECK(attribute_ptr);
            prediction_link_order = attribute_ptr->template value<int>();
            continue;
          }
          auto attribute_ptr = e_iter->FindAttribute(TIME_KEY);
          ATTRIBUTE_PTR_CHECK(attribute_ptr);
          TIME_T timestamp = attribute_ptr->template value<int>();
          outfile << eid++ << "," << e_iter->src_id() << "," << e_iter->dst_id()
                  << "," << e_iter->label() << "," << timestamp << "\n";
        }
        outfile.close();
      }
      // write X
      {
        std::string efile = file_path + "/" + DISCOVERY_PRINT_FILE_NAME + "_" +
                            std::to_string(cnt) + "_x.csv";
        outfile.open(efile.data());
        outfile << "type,x_id,x_attr,y_id,y_attr,edge_label,c\n";
        outfile.close();
      }
      // write Y
      {
        std::string efile = file_path + "/" + DISCOVERY_PRINT_FILE_NAME + "_" +
                            std::to_string(cnt) + "_y.csv";
        outfile.open(efile.data());
        outfile << "type,x_id,x_attr,y_id,y_attr,edge_label,c\n";
        outfile << "Edge," << link_.from_ << ",," << link_.to_ << ",,"
                << link_.label_ << ",";
        if (prediction_link_order > 0) outfile << prediction_link_order;
        outfile << "\n";
        outfile.close();
      }
    }
  }

  // graph & match method
  const TGraph &tgraph_;
  const TGraphRule &tgraph_rule_;
  const LinkBase &link_;
  std::shared_ptr<Match> m_ptr_;
  std::shared_ptr<Match> m_assistant_ptr_;
  // parameter
  uint32_t supp_threshold_;
  uint32_t select_k_;
  uint32_t max_d_;
  bool use_order_;
  // top-k queue & candidates list
  KQueue<Q> top_k_queue_;
  std::vector<Q> candidates_;
  std::vector<std::vector<std::pair<Q, GRADE_T>>> next_candidates_;
  // runtime data
  GRADE_T x_supp_;
  GRADE_T xyq_supp_;
  // output
  std::string output_name_;
  std::string output_path_;
};
}  // namespace toy

#endif  // DISCOVER_H
