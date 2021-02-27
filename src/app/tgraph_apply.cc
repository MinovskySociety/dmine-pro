#include "../tgraph_rule/tgraph.h"
#include "../tgraph_rule/comm.h"
#include "../tgraph_rule/config.h"
#include "../tgraph_rule/discover.h"
#include "../tgraph_rule/match.h"
#include "../tgraph_rule/partition.h"
#include "../tgraph_rule/tgraph_rule.h"

using namespace toy;

int worker_id_, worker_num_;
const int COORDINATOR = 0;
const int TAG_NUM = 2;
const int TAG_SUPP_Q = 1001;
const int TAG_SUPP_R = 1002;

/* Write match results into files */
void write_match(std::string output_path, const MatchResult &match_rlt) {
  if (match_rlt.empty()) return;
  std::ofstream outfile;
  outfile.open(output_path.data());
  auto &match = match_rlt[0];
  outfile << "match_id";
  for (auto iter : match) {
    outfile << "," << iter.first->id();
  }
  outfile << "\n";
  uint64_t cnt = 1;
  for (auto &match : match_rlt) {
    outfile << cnt++;
    for (auto iter : match) {
      outfile << "," << iter.second->id();
    }
    outfile << "\n";
  }
}

void app_apply(Config &config, bool cal_percision = false) {
  // partition graph by time window
  if (worker_id_ == COORDINATOR) {
    std::vector<VID_T> send_flag;
    send_flag.emplace_back(1);
    Partition::DoPartition(config);
    for (int i = 0; i < COORDINATOR; i++) {
      SendVector(send_flag, i);
    }
    for (int i = COORDINATOR + 1; i < worker_num_; i++) {
      SendVector(send_flag, i);
    }
  } else {
    std::vector<VID_T> recv_flag(1);
    RecvVector(recv_flag, COORDINATOR);
  }

  // load temporal graph
  TGraph tg(config);
  tg.LoadTGraphByHash(worker_id_, worker_num_);
  // PrintTGR(tg.GraphStream()[0]);

  // load temporal graph rules
  TGraphRule tg_rule;
  tg_rule.LoadTGRs(config);
  const size_t tgr_size = tg_rule.Size();
  const auto &link = tg_rule.GetLink();
  // PrintLink(link);

  // init match method
  auto m_ptr = MatchFactory::GetMatchMethod(config);
  m_ptr->InitX(tg, link);

  LOG_S("Apply Start on worker ", worker_id_);
  auto t_begin = std::chrono::steady_clock::now();

  // assemble msg & result
  std::vector<std::vector<VID_T>> assemble_msg;
  assemble_msg.resize(worker_num_);
  std::vector<std::map<int, std::unordered_set<VID_T>>> assemble_rlt;
  assemble_rlt.resize(tgr_size);

  // message buffer
  // tgrSize, tagSize, tgrIndex1, tag1, size, x1,x2,..., tag2, size, x1,x2,...
  std::vector<VID_T> msg;
  if (worker_id_ != COORDINATOR) {
    msg.emplace_back(tgr_size);  // tgrSize
    if (cal_percision) {
      msg.emplace_back(TAG_NUM);  // tagSize
    } else {
      msg.emplace_back(1);
    }
  }

  // match
  auto &tg_rules = tg_rule.GetTGRSet();
  auto match_path = config.MatchFilePathVec();
  for (int i = 0; i < tgr_size; i++) {
    LOG_S("Apply in TGR ", i + 1);
    auto &rule = tg_rules[i];
    if (worker_id_ != COORDINATOR) {
      msg.emplace_back(i);  // tgrIndex
    }
    // cal R block
    if (cal_percision) {
      MatchResult match_rlt;
      const auto &x_ptr = rule.FindConstVertex(link.from_);
      // PrintTGR(rule);
      m_ptr->DoMatchWithX(tg, rule, match_rlt);
      // set msg
      if (worker_id_ == COORDINATOR) {
        std::unordered_set<VID_T> tmp_set;
        assemble_rlt[i][TAG_SUPP_R] = tmp_set;
        for (auto &match : match_rlt) {
          assemble_rlt[i][TAG_SUPP_R].emplace(match[x_ptr]->id());
        }
      } else {
        msg.emplace_back(TAG_SUPP_R);        // tag
        msg.emplace_back(match_rlt.size());  // size
        for (auto &match : match_rlt) {      // match x
          msg.emplace_back(match[x_ptr]->id());
        }
      }
    }
    // block end
    // cal Q block
    {
      Q query = rule;
      RemoveLink(query, link);
      MatchResult match_rlt;
      const auto &x_ptr = query.FindConstVertex(link.from_);
      // PrintTGR(query);
      m_ptr->DoMatchWithX(tg, query, match_rlt);
      // set msg
      if (worker_id_ == COORDINATOR) {
        std::unordered_set<VID_T> tmp_set;
        assemble_rlt[i][TAG_SUPP_Q] = tmp_set;
        for (auto &match : match_rlt) {
          assemble_rlt[i][TAG_SUPP_Q].emplace(match[x_ptr]->id());
        }
        // write matches
        write_match(match_path[i], match_rlt);
      } else {
        msg.emplace_back(TAG_SUPP_Q);        // tag
        msg.emplace_back(match_rlt.size());  // size
        for (auto &match : match_rlt) {      // match x
          msg.emplace_back(match[x_ptr]->id());
        }
      }
    }
    // block end
  }
  // send & recv msg
  if (worker_id_ == COORDINATOR) {
    for (int i = 0; i < COORDINATOR; i++) {
      RecvVector(assemble_msg[i], i);
    }
    for (int i = COORDINATOR + 1; i < worker_num_; i++) {
      RecvVector(assemble_msg[i], i);
    }
  } else {
    SendVector(msg, COORDINATOR);
  }
  // coordinator assemble
  if (worker_id_ == COORDINATOR) {
    for (int i = 0; i < worker_num_; i++) {
      if (i == COORDINATOR) continue;
      const auto &msg = assemble_msg[i];
      if (msg.empty()) continue;
      size_t pos = 0;
      const size_t tgr_size = msg.at(pos++);
      const size_t tag_size = msg.at(pos++);
      for (int tgr_i = 0; tgr_i < tgr_size; tgr_i++) {
        const size_t tgr_index = msg.at(pos++);
        auto &tag_map = assemble_rlt[tgr_index];
        for (int tag_i = 0; tag_i < tag_size; tag_i++) {
          auto tag = msg.at(pos++);
          auto &x_set = tag_map[tag];
          auto x_size = msg.at(pos++);
          for (int x_i = 0; x_i < x_size; x_i++) {
            x_set.emplace(msg.at(pos++));
          }
        }
      }
    }
    if (cal_percision) {
      std::vector<std::pair<GRADE_T, GRADE_T>> confs;
      confs.resize(tgr_size);
      for (int i = 0; i < tgr_size; i++) {
        auto &tag_map = assemble_rlt[i];
        confs[i].first = tag_map[TAG_SUPP_Q].size();
        confs[i].second = tag_map[TAG_SUPP_R].size();
      }
      PrintConf(confs);
    } else {
      for (int i = 0; i < tgr_size; i++) {
        auto &tag_map = assemble_rlt[i];
        LOG("TGR Q Match Number: ", tag_map[TAG_SUPP_Q].size());
      }
    }
    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Apply Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
  }
  LOG_S("Apply End on worker ", worker_id_);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    LOG_E("config yaml path must be given!");
    exit(-1);
  }

  InitMPIComm();
  GetMPIInfo(worker_id_, worker_num_);
  // init log level
  set_log_grade(LOG_T_GRADE);

  // init config
  Config config;
  config.LoadApplyConfigByYaml(argv[1]);
  if (argc == 3) {
    config.LoadTimeWindowConfigByYaml(argv[2]);
  }
  config.PrintConfig();

  // run app
  // app_apply(config, true);
  app_apply(config);

  FinalizeMPIComm();
}
