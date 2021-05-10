#ifndef _ROUNDLOG_H
#define _ROUNDLOG_H
#include <omp.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
namespace gmine_new {
template <typename T>
inline constexpr double CalTime(T &begin, T &end) {
  return double(
             std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                 .count()) *
         std::chrono::microseconds::period::num /
         std::chrono::microseconds::period::den;
}
namespace statisticlog {
constexpr bool write_log_flag = false;
// total statistic csv fstream
std::ofstream output_data_csv;
// total output GPAR fstream
static omp_lock_t lock;
std::ofstream output_conf_0_GPAR;
std::ofstream output_conf_1_GPAR;
std::ofstream output_learn_match_zero_GPAR;
std::ofstream output_learn_less_supp_limit_GPAR;
std::ofstream output_match_zero_GPAR;
std::ofstream output_other_GPAR;
// round statistic data
int round_id;
double total_time;
double expand_time;
int expand_gpar_num;
double unique_time;
int unique_gpar_num;
double round_match_time;
double max_match_time;
int learn_zero_num;
int match_zero_num;
int match_not_zero_num;
int conf_0_num;
int conf_1_num;
void InitDataCsvHead() {
  if constexpr (!write_log_flag) {
    return;
  }
  output_data_csv
      << "edge,total time,expand time,expand GPAR num,unique time,unique GPAR "
         "num,round match time,max match time,match zero num,learn zero "
         "num,not zero num,conf=0 num,conf=1 num"
      << std::endl;
}
void Init() {
  omp_init_lock(&lock);
  if constexpr (!write_log_flag) {
    return;
  }
  auto t =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::stringstream ss;
  ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
  std::string str_time = ss.str();
  output_data_csv.open(("./statistic_" + str_time + ".csv").c_str());
  output_conf_0_GPAR.open(("./conf_0." + str_time).c_str());
  output_conf_1_GPAR.open(("./conf_1." + str_time).c_str());
  output_match_zero_GPAR.open(("./match_zero." + str_time).c_str());
  output_other_GPAR.open(("./other." + str_time).c_str());
  output_learn_less_supp_limit_GPAR.open(
      ("./learn_less_supp_r." + str_time).c_str());
  output_learn_match_zero_GPAR.open(("./learn_match_zero." + str_time).c_str());
  InitDataCsvHead();
}
void End() {
  omp_destroy_lock(&lock);
  if constexpr (!write_log_flag) {
    return;
  }
  output_match_zero_GPAR.close();
  output_data_csv.close();
  output_other_GPAR.close();
  output_conf_0_GPAR.close();
  output_conf_1_GPAR.close();
  output_learn_match_zero_GPAR.close();
  output_learn_less_supp_limit_GPAR.close();
}
void WriteRoundLog() {
  if constexpr (!write_log_flag) {
    return;
  }
  LOG(INFO) << "round " << round_id << " total time is " << total_time;
  LOG(INFO) << "round " << round_id << " expand time is " << expand_time;
  LOG(INFO) << "round " << round_id << " expand GPAR num is "
            << expand_gpar_num;
  LOG(INFO) << "round " << round_id << " unique time is " << unique_time;
  LOG(INFO) << "round " << round_id << " unique GPAR num is "
            << unique_gpar_num;
  LOG(INFO) << "round " << round_id << " cal GPAR's supp and conf time is "
            << round_match_time;
  LOG(INFO) << "round " << round_id << " max cal supp time is "
            << max_match_time;
  LOG(INFO) << "round " << round_id << " using iso know supp_q=0 GPAR num is "
            << match_zero_num;
  LOG(INFO) << "round " << round_id << " using learn know supp_q=0 GPAR num is "
            << learn_zero_num;
  LOG(INFO) << "round " << round_id << " supp_q!=0 GPAR num is "
            << match_not_zero_num;
  LOG(INFO) << "round " << round_id << " conf=0 GPAR num is " << conf_0_num;
  LOG(INFO) << "round " << round_id << " conf=1 GPAR num is " << conf_1_num;
  output_data_csv << round_id << ",";
  output_data_csv << total_time << ",";
  output_data_csv << expand_time << ",";
  output_data_csv << expand_gpar_num << ",";
  output_data_csv << unique_time << ",";
  output_data_csv << unique_gpar_num << ",";
  output_data_csv << round_match_time << ",";
  output_data_csv << max_match_time << ",";
  output_data_csv << match_zero_num << ",";
  output_data_csv << learn_zero_num << ",";
  output_data_csv << match_not_zero_num << ",";
  output_data_csv << conf_0_num << ",";
  output_data_csv << conf_1_num << ",";
  output_data_csv << std::endl;
}
void InitRound(int round) {
  round_id = round;
  match_zero_num = 0;
  match_not_zero_num = 0;
  expand_gpar_num = unique_gpar_num = learn_zero_num = conf_0_num = conf_1_num =
      0;
  total_time = expand_time = unique_time = round_match_time = max_match_time =
      0.0;
}
template <class Pattern>
void WritePatternToFile(Pattern &pattern, std::ofstream &out) {
  if constexpr (!write_log_flag) {
    return;
  }
  out << "node label:" << std::endl;
  for (auto vertex_it = pattern.VertexBegin(); !vertex_it.IsDone();
       vertex_it++) {
    out << vertex_it->id() << " " << vertex_it->label() << std::endl;
  }
  out << "edge:" << std::endl;
  for (auto vertex_it = pattern.VertexBegin(); !vertex_it.IsDone();
       vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeBegin(); !edge_it.IsDone();
         edge_it++) {
      out << edge_it->const_src_handle()->id() << " "
          << edge_it->const_dst_handle()->id() << " " << edge_it->label()
          << std::endl;
    }
  }
}
template <class GPAR>
void WriteGPARWithConf(GPAR &gpar, int supp_q, int supp_r, double conf,
                       double match_time, std::ofstream &out) {
  if constexpr (!write_log_flag) {
    return;
  }
  omp_set_lock(&lock);
  WritePatternToFile(gpar.pattern, out);
  out << "supp_q = " << supp_q << std::endl;
  out << "supp_r = " << supp_r << std::endl;
  out << "conf = " << conf << std::endl;
  out << "time = " << match_time << std::endl;
  omp_unset_lock(&lock);
}
template <class GPAR>
void WriteNoMatchGPAR(GPAR &gpar, double match_time) {
  if constexpr (!write_log_flag) {
    return;
  }
  omp_set_lock(&lock);
  WritePatternToFile(gpar.pattern, output_match_zero_GPAR);
  output_match_zero_GPAR << "supp_q = 0" << std::endl;
  output_match_zero_GPAR << "time = " << match_time << std::endl;
  omp_unset_lock(&lock);
}
}  // namespace statisticlog
}  // namespace gmine_new

#endif