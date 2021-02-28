#include <ctime>
#include <queue>

#include "gpar.h"
#include "gundam/algorithm/vf2.h"
ReturnType gmine_new::DiscoverGPAR::DminePro(IN const char *v_file,
                                             IN const char *e_file) {
  this->discovery_result_.clear();

  clock_t read_begin = clock();
  GUNDAM::ReadCSVGraph(this->target_graph_.target_graph_, v_file, e_file);
  this->target_graph_.BuildInformation();
  clock_t read_end = clock();
  read_time = (1.0 * read_end - read_begin) / CLOCKS_PER_SEC;
  std::cout << "read time=" << (1.0 * read_end - read_begin) / CLOCKS_PER_SEC
            << std::endl;

  GraphType pattern;
  pattern.AddVertex(1, VertexLabelType(x_node_label_));
  GraphPatternAssociationRules single_x;
  single_x.SetPattern(pattern);
  single_x.SetQEdge(x_node_label_, y_node_label_, q_label_);
  single_x.LoadGraph(target_graph_);
  single_x.FirstRoundMatch();
  std::vector<GraphPatternAssociationRules> last_round_patterns,
      now_round_patterns;
  last_round_patterns.push_back(single_x);
  for (int round = 1; round <= this->up_edge_num_; ++round) {
    std::cout << "round=" << round << std::endl;
    clock_t round_begin = clock();
    now_round_patterns.clear();
    this->ExpandGPARs(round, last_round_patterns, now_round_patterns);
    last_round_patterns.clear();
    this->UpdateState(now_round_patterns);
    std::cout << "change size=" << now_round_patterns.size() << std::endl;
    clock_t round_end = clock();
    std::cout << "round " << round
              << " time=" << (1.0 * round_end - round_begin) / CLOCKS_PER_SEC
              << std::endl;
    for (auto &it : now_round_patterns) {
      if (it.GetConf() < eps) continue;
      if (fabs(it.GetConf() - 1) < eps) continue;
      last_round_patterns.push_back(it);
    }
  }
  int cnt = 0;
  for (auto &it : discovery_result_) {
    std::cout << "Conf=" << it.GetConf() << std::endl;
    TransposePattern(it.pattern_);
    it.PrintPattern(++cnt, output_dir_.c_str());
  }
  WriteLog();
  return E_OK;
}

ReturnType gmine_new::DiscoverGPAR::ExpandGPARs(
    IN int round, IN std::vector<GraphPatternAssociationRules> &last_patterns,
    OUT std::vector<GraphPatternAssociationRules> &now_patterns) {
  std::vector<GraphPatternAssociationRules> temp_results;
  clock_t expand_begin = clock();
  for (int i = 0; i < last_patterns.size(); ++i) {
    last_patterns[i].Expand(round, temp_results);
  }
  clock_t expand_end = clock();
  expand_time.push_back((1.0 * expand_end - expand_begin) / CLOCKS_PER_SEC);
  std::cout << "expand time="
            << (1.0 * expand_end - expand_begin) / CLOCKS_PER_SEC << std::endl;
  std::vector<double> round_match_time;
  std::vector<int> round_supp_r;
  std::vector<GraphType> round_pattern;
  clock_t match_begin = clock();
  int cnt = 0;
  for (auto &it : temp_results) {
    TagType find_iso_tag = 0;
    for (auto &it1 : now_patterns) {
      VertexIDType query_id = 1, target_id = 1;
      std::vector<
          std::map<GraphType::VertexConstPtr, GraphType::VertexConstPtr>>
          temp_match_result;
      temp_match_result.clear();
      int flag = GUNDAM::VF2_Label_Equal<GUNDAM::MatchSemantics::kIsomorphism>(
          it.pattern_, it1.pattern_, query_id, target_id, 1, temp_match_result);
      if (flag >= 0) {
        if (temp_match_result.size() >= 1) {
          find_iso_tag = 1;
          break;
        }
      }
    }
    if (find_iso_tag == 1) {
      continue;
    }
    cnt++;
    std::cout << "cnt=" << cnt << std::endl;
    TransposePattern(it.pattern_);
    clock_t match_begin = clock();
    it.Match();
    clock_t match_end = clock();
    // update log
    // match_time
    round_match_time.push_back((1.0 * match_end - match_begin) /
                               CLOCKS_PER_SEC);
    // pattern
    round_pattern.push_back(it.pattern_);
    it.CalConf();
    round_supp_r.push_back(static_cast<int>(it.GetSuppR()));
    if (it.GetSuppxSize() == 0) continue;
    // if (it.GetSuppR() < 40) continue;
    now_patterns.push_back(it);
  }
  match_time.push_back(round_match_time);
  pattern.push_back(round_pattern);
  supp.push_back(round_supp_r);
  clock_t match_end = clock();
  std::cout << "match time=" << (1.0 * match_end - match_begin) / CLOCKS_PER_SEC
            << std::endl;
  total_match_time.push_back((1.0 * match_end - match_begin) / CLOCKS_PER_SEC);
  std::cout << "expand size=" << now_patterns.size() << std::endl;
  return E_OK;
}

ReturnType gmine_new::DiscoverGPAR::WriteLog() {
  std::string cc = output_dir_;
  cc += "log_";
  cc.push_back('0' + this->up_edge_num_);
  cc += ".txt";

  std::ofstream log_file(cc);
  log_file << "read time=" << read_time << std::endl;
  for (int i = 0; i < expand_time.size(); i++) {
    log_file << "round " << i + 1 << ":\n";
    log_file << "total time=" << expand_time[i] + total_match_time[i]
             << std::endl;
    log_file << "expand time=" << expand_time[i] << std::endl;
    log_file << "total match time=" << total_match_time[i] << std::endl;
    log_file << "expand size=" << match_time[i].size() << std::endl;
    log_file << "each match time and supp(R):\n";
    for (int j = 0; j < match_time[i].size(); j++) {
      size_t sz = pattern[i][j].CountVertex();
      log_file << "node label:\n";
      for (int q = 1; q <= sz; q++) {
        log_file << q << " " << pattern[i][j].FindVertex(q)->label() << "\n";
      }
      log_file << "edge:\n";
      for (int q = 1; q <= sz; q++) {
        for (auto it = pattern[i][j].FindVertex(q)->OutEdgeBegin();
             !it.IsDone(); it++) {
          log_file << it->src_ptr()->id() << " " << it->dst_ptr()->id() << " "
                   << it->label() << "\n";
        }
      }
      log_file << "match time=" << match_time[i][j] << std::endl;
      log_file << "supp(R)=" << supp[i][j] << std::endl;
    }
  }
  return E_OK;
}

ReturnType gmine_new::DiscoverGPAR::UpdateState(
    IN std::vector<GraphPatternAssociationRules> &now_patterns) {
  for (auto it : now_patterns) {
    if (this->discovery_result_.size() < top_k_) {
      this->discovery_result_.push_back(it);
      continue;
    }
    int Minpos = -1;
    double MinConf = 2;
    for (int i = 0; i < this->discovery_result_.size(); i++) {
      if (this->discovery_result_[i].GetConf() < MinConf) {
        Minpos = i;
        MinConf = this->discovery_result_[i].GetConf();
      }
    }
    if (it.GetConf() > this->discovery_result_[Minpos].GetConf()) {
      this->discovery_result_[Minpos] = it;
    }
  }
  return E_OK;
}
