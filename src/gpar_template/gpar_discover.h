#ifndef _GPAR_DISCOVER_H
#define _GPAR_DISCOVER_H
#include <glog/logging.h>
#include <omp.h>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iterator>

#include "../gpar/test.cpp"
#include "filter_gpar_by_eval_func.h"
#include "get_datagraph_information.h"
#include "gpar_config.h"
#include "gpar_dfs_code.h"
#include "gpar_diversification.h"
#include "gpar_expand.h"
#include "gpar_match.h"
#include "gpar_new.h"
#include "gpar_unique.h"
#include "gundam/boost_iso.h"
#include "gundam/csvgraph.h"
#include "round_log.h"
#include "top_k_container.h"
namespace gmine_new {
// TopKContainer CMP
template <class GPAR>
class CMP {
 public:
  bool operator()(GPAR &a, GPAR &b) { return a.conf() > b.conf(); }
};
template <class DataGraph>
inline void ReadDataGraph(const char *v_file, const char *e_file,
                          DataGraph &data_graph) {
  auto read_data_graph_begin = std::chrono::system_clock::now();
  GUNDAM::ReadCSVGraph(data_graph, v_file, e_file);
  auto read_data_graph_end = std::chrono::system_clock::now();
  double read_data_graph_time =
      CalTime(read_data_graph_begin, read_data_graph_end);
  std::cout << "read end!" << std::endl;
  std::cout << "read data graph time = " << read_data_graph_time << std::endl;
  LOG(INFO) << "read data graph time = " << read_data_graph_time;
}
template <class GPAR, class GPARList>
inline void UpdateState(GPAR &gpar, GPARList &gpar_list) {
  gpar_list.push_back(gpar);
}

template <class GPAR, class GPARList>
inline void AddNoMatchPattern(int32_t round, GPAR &gpar,
                              GPARList &no_match_gpar_list) {
  using VertexLabelType = typename GPAR::VertexLabelType;
  using VertexIDType = typename GPAR::VertexIDType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  const VertexIDType x_node_id = gpar.x_node_ptr()->id();
  const VertexIDType y_node_id = gpar.y_node_ptr()->id();
  const EdgeLabelType q_label = gpar.q_edge_label();
  GPAR match_zero_pattern = gpar;
  if (fabs(gpar.conf()) <= eps)
    match_zero_pattern.pattern.AddEdge(x_node_id, y_node_id, q_label,
                                       round + 1);
  no_match_gpar_list.push_back(match_zero_pattern);
}
template <class GPAR, class DataGraph, class GPARList>
inline void CalEachGPAR(int32_t round, GPAR &gpar, DataGraph &data_graph,
                        int32_t supp_r_limit, GPARList &gpar_list,
                        GPARList &supp_zero_gpar_list,
                        GPARList &less_supp_limit_gpar_list) {
  if (gpar.pattern.CountEdge() > 0 && gpar.supp_R_size() < supp_r_limit &&
      gpar.supp_Q_size() > 0) {
    less_supp_limit_gpar_list.push_back(gpar);
    if (gpar.supp_R_size() == 0) {
      AddNoMatchPattern(round, gpar, supp_zero_gpar_list);
    }
    return;
  }
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;
  std::vector<DataGraphVertexConstPtr> result_list;
  auto zero_begin = std::chrono::system_clock::now();
  GPARMatch(gpar, data_graph);
  auto zero_end = std::chrono::system_clock::now();
  double match_time = CalTime(zero_begin, zero_end);
  statisticlog::max_match_time =
      std::max(statisticlog::max_match_time, match_time);
  if (gpar.supp_Q_size() == 0) {
    omp_set_lock(&gmine_new::statisticlog::lock);
    statisticlog::match_zero_num++;
    omp_unset_lock(&gmine_new::statisticlog::lock);
    statisticlog::WriteNoMatchGPAR(gpar, match_time);
    supp_zero_gpar_list.push_back(gpar);
    return;
  } else {
    omp_set_lock(&gmine_new::statisticlog::lock);
    statisticlog::match_not_zero_num++;
    omp_unset_lock(&gmine_new::statisticlog::lock);
  }
  double pattern_conf;
  int32_t pattern_suppr;
  pattern_suppr = CalSuppR(gpar, data_graph);
  gpar.CalConf();
  pattern_conf = gpar.conf();
  if (pattern_suppr < supp_r_limit) {
    less_supp_limit_gpar_list.push_back(gpar);
    if (gpar.supp_R_size() == 0) {
      AddNoMatchPattern(round, gpar, supp_zero_gpar_list);
    }
    return;
  }
  if (fabs(pattern_conf) <= eps) {
    statisticlog::WriteGPARWithConf(gpar, gpar.supp_Q_size(), pattern_suppr,
                                    pattern_conf, match_time,
                                    statisticlog::output_conf_0_GPAR);
    statisticlog::conf_0_num++;
  } else if (fabs(pattern_conf - 1) <= eps) {
    statisticlog::WriteGPARWithConf(gpar, gpar.supp_Q_size(), pattern_suppr,
                                    pattern_conf, match_time,
                                    statisticlog::output_conf_1_GPAR);
    statisticlog::conf_1_num++;
  } else {
    statisticlog::WriteGPARWithConf(gpar, gpar.supp_Q_size(), pattern_suppr,
                                    pattern_conf, match_time,
                                    statisticlog::output_other_GPAR);
  }
  UpdateState(gpar, gpar_list);
  if (fabs(pattern_conf - 1) <= eps) {
    less_supp_limit_gpar_list.push_back(gpar);
  }
}
template <class GPARList>
inline void EachThreadMessageClear(GPARList &gpar_list,
                                   GPARList &match_zero_gpar_list,
                                   GPARList &less_supp_gpar_list) {
  auto clear_callback = [](auto &it) { it.clear(); };
  std::for_each(gpar_list.begin(), gpar_list.end(), clear_callback);
  std::for_each(match_zero_gpar_list.begin(), match_zero_gpar_list.end(),
                clear_callback);
  std::for_each(less_supp_gpar_list.begin(), less_supp_gpar_list.end(),
                clear_callback);
}
template <class Pattern, class DataGraph, class ManualCheck,
          class SelectCallback>
inline void DMineProWithCallback(
    const char *v_file, const char *e_file,
    const typename Pattern::VertexType::LabelType x_node_label,
    const typename Pattern::VertexType::LabelType y_node_label,
    const typename Pattern::EdgeType::LabelType q_edge_label,
    const int32_t supp_limit, const int32_t max_edge, const int32_t top_k,
    const char *output_dir, ManualCheck manual_check,
    SelectCallback select_callback,
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> *root_gpars = nullptr,
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> *discover_gpars = nullptr) {
  std::cout << "x y q =" << x_node_label << " " << y_node_label << " "
            << q_edge_label << std::endl;
  std::cout << "supp = " << supp_limit << std::endl;
  std::cout << "max edge = " << max_edge << std::endl;
  std::cout << "topk=" << top_k << std::endl;
  using DataGraphVertexLabelType = typename DataGraph::VertexType::LabelType;
  using DataGraphEdgeLabelType = typename DataGraph::EdgeType::LabelType;
  statisticlog::Init();
  // L
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> supp_zero_gpar;
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> less_supp_limit_gpar;
  // data graph
  DataGraph data_graph;

  // read DataGraph
  ReadDataGraph(v_file, e_file, data_graph);
  LOG(INFO) << "read end!";

  // DataGraph Information
  std::set<DataGraphVertexLabelType> vertex_label_set;
  std::set<DataGraphEdgeLabelType> edge_label_set;
  std::set<std::tuple<DataGraphVertexLabelType, DataGraphVertexLabelType,
                      DataGraphEdgeLabelType>>
      edge_type_set;

  // build datagraph information
  auto build_data_information_begin = std::chrono::system_clock::now();
  GetDataGraphInformation(data_graph, vertex_label_set, edge_label_set,
                          edge_type_set);
  auto build_data_information_end = std::chrono::system_clock::now();
  double build_data_information_time =
      CalTime(build_data_information_begin, build_data_information_end);
  // log
  LOG(INFO) << "build data graph information time = "
            << build_data_information_time;
  LOG(INFO) << "get information end!" << std::endl;

  // set root gpar
  // run dmine using structure
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> last_round_gpars;
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> total_gpar_set;
  TopKContainer<DiscoverdGPAR<Pattern, DataGraph>,
                CMP<DiscoverdGPAR<Pattern, DataGraph>>>
      top_k_container(top_k);
  int32_t max = 0;
  if (root_gpars == nullptr) {
    DiscoverdGPAR<Pattern, DataGraph> root_gpar(x_node_label, y_node_label,
                                                q_edge_label);
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> root_supp_zero_gpar;
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> root_less_supp_gpar;
    // root gpar match and cal conf
    CalEachGPAR(0 + max, root_gpar, data_graph, supp_limit, last_round_gpars,
                root_supp_zero_gpar, root_less_supp_gpar);
  } else {
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> root_supp_zero_gpar;
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> root_less_supp_gpar;
    for (const auto &gpar : *root_gpars) {
      max = std::max(max, (int)gpar.pattern.CountEdge());
    }
    for (auto &gpar : *root_gpars) {
      CalEachGPAR(0 + max, gpar, data_graph, supp_limit, last_round_gpars,
                  root_supp_zero_gpar, root_less_supp_gpar);
    }
  }
  auto time_begin = std::chrono::system_clock::now();
  omp_init_lock(&cal_lock);
  for (int32_t round = 1; round <= max_edge; round++) {
    auto round_begin = std::chrono::system_clock::now();
    statisticlog::InitRound(round);
    LOG(INFO) << "round=" << round;

    std::vector<DiscoverdGPAR<Pattern, DataGraph>> now_round_gpars;
    // each thread collect message
    std::vector<std::vector<DiscoverdGPAR<Pattern, DataGraph>>>
        each_thread_gpars(omp_get_num_procs());
    std::vector<std::vector<DiscoverdGPAR<Pattern, DataGraph>>>
        each_thread_supp_zero_gpar(omp_get_num_procs());
    std::vector<std::vector<DiscoverdGPAR<Pattern, DataGraph>>>
        each_thread_less_supp_gpar(omp_get_num_procs());
    EachThreadMessageClear(each_thread_gpars, each_thread_supp_zero_gpar,
                           each_thread_less_supp_gpar);
    auto expand_begin = std::chrono::system_clock::now();
    std::cout << "supp zero gpar size = " << supp_zero_gpar.size() << std::endl;
    std::cout << "less supp gpar size = " << less_supp_limit_gpar.size()
              << std::endl;
    size_t expand_ten_percent_num = last_round_gpars.size() / 10;
    not_match_num = total_num = 0;
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(last_round_gpars.size()); i++) {
      GPARExpand(last_round_gpars[i], vertex_label_set, edge_label_set,
                 edge_type_set, supp_zero_gpar, less_supp_limit_gpar,
                 round + max, each_thread_gpars[omp_get_thread_num()],
                 manual_check);
      if (expand_ten_percent_num != 0 &&
          (i + 1) % expand_ten_percent_num == 0) {
        std::cout << "expand " << (i + 1) / expand_ten_percent_num
                  << "0% GPARs." << std::endl;
      }
    }
    std::cout << "expand end!" << std::endl;
    std::cout << "total num = " << total_num << std::endl;
    std::cout << "not match num = " << not_match_num << std::endl;
    for (size_t i = 0; i < omp_get_num_procs(); i++) {
      now_round_gpars.insert(now_round_gpars.end(),
                             each_thread_gpars[i].begin(),
                             each_thread_gpars[i].end());
    }
    auto expand_end = std::chrono::system_clock::now();
    double expand_time = CalTime(expand_begin, expand_end);
    std::cout << "expand time is " << expand_time << "s" << std::endl;
    statisticlog::expand_gpar_num = static_cast<int>(now_round_gpars.size());
    statisticlog::expand_time = expand_time;
    auto unique_begin = std::chrono::system_clock::now();
    if constexpr (using_dfs_code) {
      UniqueGPARListUsingDFSCode(now_round_gpars);
    } else {
      UniqueGPARList(now_round_gpars);
    }
    auto unique_end = std::chrono::system_clock::now();
    double unique_time = CalTime(unique_begin, unique_end);
    statisticlog::unique_gpar_num = static_cast<int>(now_round_gpars.size());
    statisticlog::unique_time = unique_time;
    std::cout << "round " << round << " unique time is " << unique_time
              << std::endl;
    LOG(INFO) << "round " << round << " unique time is " << unique_time;
    LOG(INFO) << "round " << round << " gen " << now_round_gpars.size()
              << " gpars.";
    std::cout << "round " << round << " gen " << now_round_gpars.size()
              << " gpars." << std::endl;

    last_round_gpars.clear();
    std::vector<DiscoverdGPAR<Pattern, DataGraph>>().swap(last_round_gpars);

    // cal supp and conf
    auto round_match_begin = std::chrono::system_clock::now();
    EachThreadMessageClear(each_thread_gpars, each_thread_supp_zero_gpar,
                           each_thread_less_supp_gpar);
    size_t ten_percent_num = static_cast<int>(now_round_gpars.size()) / 10;
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(now_round_gpars.size()); i++) {
      CalEachGPAR(round + max, now_round_gpars[i], data_graph, supp_limit,
                  each_thread_gpars[omp_get_thread_num()],
                  each_thread_supp_zero_gpar[omp_get_thread_num()],
                  each_thread_less_supp_gpar[omp_get_thread_num()]);
      if (ten_percent_num != 0 && (i + 1) % ten_percent_num == 0) {
        std::cout << "cal " << (i + 1) / ten_percent_num << "0% GPARs."
                  << std::endl;
      }
    }
    for (size_t i = 0; i < omp_get_num_procs(); i++) {
      total_gpar_set.insert(total_gpar_set.end(), each_thread_gpars[i].begin(),
                            each_thread_gpars[i].end());
      for (size_t j = 0; j < each_thread_gpars[i].size(); j++) {
        auto conf = each_thread_gpars[i][j].conf();
        if (fabs(conf) <= eps || fabs(1 - conf) <= eps) {
          continue;
        }
        last_round_gpars.push_back(each_thread_gpars[i][j]);
      }
      for (auto &it : each_thread_supp_zero_gpar[i]) {
        if (supp_zero_gpar.size() == L_size) break;
        supp_zero_gpar.push_back(it);
      }
      for (auto &it : each_thread_less_supp_gpar[i]) {
        if (less_supp_limit_gpar.size() == L_size) break;
        less_supp_limit_gpar.push_back(it);
      }
    }
    std::string round_output =
        (std::string)(output_dir + std::to_string(round) + "/");
    std::filesystem::path output_path(round_output);
    std::filesystem::create_directory(output_path);
    auto temp_list = last_round_gpars;
    SelectKNotSimiliarGPAR(temp_list.size(), temp_list, select_callback);
    int now_id = 0;
    std::ofstream supp_file(round_output + "supp.txt");
    for (auto &conf_pattern : temp_list) {
      now_id++;
      std::string round_output =
          (std::string)(output_dir + std::to_string(round) + "/pattern_" +
                        std::to_string(now_id) + "/");
      std::filesystem::path output_path(round_output);
      std::filesystem::create_directory(output_path);
      WriteGPAR(conf_pattern, now_id, round_output);
      supp_file << "id = " << now_id << std::endl;
      supp_file << "supp Q = " << conf_pattern.supp_Q_size() << std::endl;
      supp_file << "supp R = " << conf_pattern.supp_R_size() << std::endl;
      supp_file << "conf = " << conf_pattern.conf() << std::endl;
    }
    auto div_begin = std::chrono::system_clock::now();
    if constexpr (using_selectk) {
      GPARDiversification(round, last_round_gpars, select_callback);
    }
    auto div_end = std::chrono::system_clock::now();
    double div_time = CalTime(div_begin, div_end);
    LOG(INFO) << "div time = " << div_time;
    std::cout << "div time = " << div_time << std::endl;
    auto round_match_end = std::chrono::system_clock::now();
    auto round_end = std::chrono::system_clock::now();
    double round_match_time = CalTime(round_match_begin, round_match_end);
    double total_time = CalTime(round_begin, round_end);
    statisticlog::round_match_time = round_match_time;
    statisticlog::total_time = total_time;
    statisticlog::WriteRoundLog();
  }
  if constexpr (!remain_all_result)
    SelectKNotSimiliarGPAR(top_k, total_gpar_set, select_callback);
  auto time_end = std::chrono::system_clock::now();
  statisticlog::End();
  int32_t pattern_id = 0;
  std::ofstream supp_file((std::string)output_dir + "supp.txt");
  if (discover_gpars != nullptr) {
    for (auto &single_gpar : total_gpar_set) {
      discover_gpars->push_back(single_gpar);
    }
  }
  for (auto &conf_pattern : total_gpar_set) {
    pattern_id++;
    std::cout << "pattern:" << std::endl;
    TransposePattern(conf_pattern.pattern);
    std::string round_output =
        (std::string)((std::string)output_dir + "pattern_" +
                      std::to_string(pattern_id) + "/");
    std::filesystem::path output_path(round_output);
    std::filesystem::create_directory(output_path);
    WriteGPAR(conf_pattern, pattern_id, round_output);
    std::cout << "supp Q = " << conf_pattern.supp_Q_size() << std::endl;
    std::cout << "supp R = " << conf_pattern.supp_R_size() << std::endl;
    std::cout << "conf = " << conf_pattern.conf() << std::endl;
    supp_file << "id = " << pattern_id << std::endl;
    supp_file << "supp Q = " << conf_pattern.supp_Q_size() << std::endl;
    supp_file << "supp R = " << conf_pattern.supp_R_size() << std::endl;
    supp_file << "conf = " << conf_pattern.conf() << std::endl;
  }
}
template <class Pattern, class DataGraph>
inline void DMinePro(
    const char *v_file, const char *e_file,
    const typename Pattern::VertexType::LabelType x_node_label,
    const typename Pattern::VertexType::LabelType y_node_label,
    const typename Pattern::EdgeType::LabelType q_edge_label,
    const int32_t supp_limit, const int32_t max_edge, const int32_t top_k,
    const char *output_dir,
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> *root_gpars = nullptr,
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> *discover_gpars = nullptr) {
  DMineProWithCallback<Pattern, DataGraph>(
      v_file, e_file, x_node_label, y_node_label, q_edge_label, supp_limit,
      max_edge, top_k, output_dir, gmine_new::manual_check,
      gmine_new::select_callback, root_gpars, discover_gpars);
}
}  // namespace gmine_new

#endif
