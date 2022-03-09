#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//#define _ENABLE_GRAPH

#include "gundam/io/csvgraph.h"
#ifdef _ENABLE_GRAPH
#include "gundam/graph_type/graph.h"
#endif

#include "gundam/graph_type/large_graph.h"
//#include "gundam/large_graph1.h"
#include "gundam/graph_type/large_graph2.h"
//#include "gundam/graph_type/large_graph3.h"
#include "gundam/graph_type/simple_small_graph.h"
#include "gundam/graph_type/small_graph.h"
#include "gundam/algorithm/vf2.h"
#include "gundam/algorithm/dp_iso.h"

inline uint64_t GetTime() { return clock() * 1000 / CLOCKS_PER_SEC; }

template <class QueryGraph, class TargetGraph>
int VF2_Run(const QueryGraph &query_graph, const TargetGraph &target_graph) {
  using namespace GUNDAM;
  using MatchMap = std::map<typename VertexHandle<QueryGraph>::type,
                            typename VertexHandle<TargetGraph>::type>;
  using MatchResult = std::vector<MatchMap>;

  MatchResult match_result;
  int result = VF2(query_graph, target_graph, 1000000, match_result);

  return result;
}

template <class QueryGraph, class TargetGraph>
int DPISO_Run(QueryGraph &query_graph, TargetGraph &target_graph) {
  using namespace GUNDAM;
  using  QueryVertexHandle = typename VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename VertexHandle<TargetGraph>::type;
  using MatchMap = std::map<QueryVertexHandle, TargetVertexHandle>;
  using MatchResult = std::vector<MatchMap>;

  MatchResult match_result;

  // all result
  //int max_result = -1;  
  //int result = DPISO(query_graph, target_graph, max_result, match_result);
 
  // group by supp
  std::vector<QueryVertexHandle> supp_list { query_graph.FindVertex(1) };
  auto match_callback = [&match_result](const MatchMap &match_map) {
    match_result.emplace_back(match_map);
  };
  int result = DPISO<MatchSemantics::kIsomorphism>(
      query_graph, target_graph, supp_list, match_callback, -1.0);
  
  //std::cout << match_result.size() << std::endl;

  return result;
}

struct CSVGraphInfo {
  std::vector<std::string> vertex_files;
  std::vector<std::string> edge_files;
};

struct MatchBenchmarkConfigure1 {
  std::string work_dir;
  CSVGraphInfo target_graph_info;  
  std::vector<CSVGraphInfo> query_graph_info_list;
  int match_times;
};

template <class QueryGraph, class TargetGraph>
int MatchBenchmark1() {
  uint64_t match_time = 0;
  int result;
  MatchBenchmarkConfigure1 config;
  std::vector<QueryGraph> query_graph_list;
  TargetGraph target_graph;

  std::cout << "Match Benchmark1" << std::endl;

  // Init config;
  config.match_times = 1;
  config.work_dir = "/share/work/";

  for (int i = 0; i < 1; i++) {
    //config.query_graph_info_list.emplace_back();
    //config.query_graph_info_list.back().vertex_files.emplace_back(
    //    "match_benchmark/q1_v.csv");
    //config.query_graph_info_list.back().edge_files.emplace_back(
    //    "match_benchmark/q1_e.csv");

    //config.query_graph_info_list.emplace_back();
    //config.query_graph_info_list.back().vertex_files.emplace_back(
    //    "match_benchmark/q2_v.csv");
    //config.query_graph_info_list.back().edge_files.emplace_back(
    //    "match_benchmark/q2_e.csv");

    //config.query_graph_info_list.emplace_back();
    //config.query_graph_info_list.back().vertex_files.emplace_back(
    //    "match_benchmark/q3_v.csv");
    //config.query_graph_info_list.back().edge_files.emplace_back(
    //    "match_benchmark/q3_e.csv");

    config.query_graph_info_list.emplace_back();
    config.query_graph_info_list.back().vertex_files.emplace_back(
        config.work_dir + 
        "match_benchmark/pattern_1_n.csv");
    config.query_graph_info_list.back().edge_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_1_e.csv");

    config.query_graph_info_list.emplace_back();
    config.query_graph_info_list.back().vertex_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_11_n.csv");
    config.query_graph_info_list.back().edge_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_11_e.csv");

    config.query_graph_info_list.emplace_back();
    config.query_graph_info_list.back().vertex_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_12_n.csv");
    config.query_graph_info_list.back().edge_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_12_e.csv");

    config.query_graph_info_list.emplace_back();
    config.query_graph_info_list.back().vertex_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_43_n.csv");
    config.query_graph_info_list.back().edge_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_43_e.csv");

    config.query_graph_info_list.emplace_back();
    config.query_graph_info_list.back().vertex_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_44_n.csv");
    config.query_graph_info_list.back().edge_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_44_e.csv");

    config.query_graph_info_list.emplace_back();
    config.query_graph_info_list.back().vertex_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_45_n.csv");
    config.query_graph_info_list.back().edge_files.emplace_back(
        config.work_dir + "match_benchmark/pattern_45_e.csv");

  }

  config.target_graph_info.vertex_files.emplace_back(config.work_dir +
                                                     "cu4999_1/liantong_v.csv");
  config.target_graph_info.edge_files.emplace_back(config.work_dir +
                                                   "cu4999_1/liantong_e.csv");

  // Loading graphs
  auto begin_time = GetTime();
  std::cout << std::endl << "Loading query graph..." << std::endl;
  for (size_t i = 0; i < config.query_graph_info_list.size(); ++i) {
    const auto &info = config.query_graph_info_list[i];
    query_graph_list.emplace_back();
    std::cout << "Query #" << (i + 1) << std::endl;
    result = GUNDAM::ReadCSVGraph(query_graph_list.back(), info.vertex_files,
                                  info.edge_files);
    if (result < 0) {
      return result;
    }
  }

  std::cout << std::endl << "Loading target graph..." << std::endl;
  result =
      GUNDAM::ReadCSVGraph(target_graph, config.target_graph_info.vertex_files,
                           config.target_graph_info.edge_files);
  if (result < 0) {
    return result;
  }

  std::cout << "Load time: " << GetTime() - begin_time << " ms" << std::endl;

  // Match
  std::cout << std::endl << "Matcing..." << std::endl;
  for (int j = 0; j < config.match_times; ++j) {
    for (int i = 0; i < query_graph_list.size(); ++i) {
      std::cout << "Query #" << (i + 1) << std::endl;
      const auto &query_graph = query_graph_list[i];
      begin_time = GetTime();
      //result = VF2_Run(query_graph, target_graph);
      result = DPISO_Run(query_graph, target_graph);
      auto end_time = GetTime();
      std::cout << "Result: " << result << std::endl
                << "Time: " << end_time - begin_time << " ms" << std::endl
                << std::endl;
      if (result < 0) {
        return result;
      }
      match_time += end_time - begin_time;
    }
  }
  std::cout << "Total match time: " << match_time << " ms" << std::endl;

  // Release graphs
  std::cout << std::endl << "Releasing graphs..." << std::endl;
  begin_time = GetTime();

  target_graph.Clear();
  query_graph_list.clear();

  std::cout << "Release time: " << GetTime() - begin_time << " ms" << std::endl;
  std::cout << std::endl;

  return static_cast<int>(match_time);
}

int main() {
  using namespace GUNDAM;

#ifdef _ENABLE_GRAPH

  using GQ = Graph<SetVertexIDType<uint32_t>, SetVertexLabelType<uint32_t>,
                   SetVertexAttributeKeyType<std::string>,
                   SetVertexIDContainerType<ContainerType::Map>,
                   SetEdgeIDType<uint32_t>, SetEdgeLabelType<uint32_t>,
                   SetEdgeAttributeKeyType<std::string>>;

  using GT = Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<uint32_t>,
                   SetVertexAttributeKeyType<std::string>,
                   SetVertexIDContainerType<ContainerType::Map>,
                   SetEdgeIDType<uint64_t>, SetEdgeLabelType<uint32_t>,
                   SetEdgeAttributeKeyType<std::string>>;

  using GT1 = Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<uint32_t>,
                    SetVertexAttributeKeyType<std::string>,
                    SetVertexLabelContainerType<ContainerType::Map>,
                    SetVertexIDContainerType<ContainerType::Map>,
                    SetEdgeIDType<uint64_t>, SetEdgeLabelType<uint32_t>,
                    SetEdgeAttributeKeyType<std::string>>;

  using GT2 = Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<uint32_t>,
                    SetVertexAttributeKeyType<std::string>,
                    SetVertexIDContainerType<ContainerType::Map>,
                    SetEdgeIDType<uint64_t>, SetEdgeLabelType<uint32_t>,
                    SetEdgeAttributeKeyType<std::string>>;

#endif

  using LGQ = LargeGraph<uint32_t, uint32_t, std::string, uint32_t, uint32_t,
                         std::string>;

  using LGT = LargeGraph<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                         std::string>;

  using LGQ2 = LargeGraph2<uint32_t, uint32_t, std::string, uint32_t, uint32_t,
                           std::string>;

  using LGT2 = LargeGraph2<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                           std::string>;

  //using LGQ3 = LargeGraph3<uint32_t, uint32_t, std::string, uint32_t, uint32_t,
  //                         std::string>;

  //using LGT3 = LargeGraph3<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
  //                         std::string>;  

  // LGT3 g;
  // g.AddVertex(1, 1);
  // LGT3::VertexConstIterator it{g.VertexCBegin()};
  // std::cout << it->id() << std::endl;
  // LGT3::VertexConstPtr ptr{it};

  // return 0;

  int match_time[4];

  for (int i = 0; i < 1; ++i) {
    std::cout << "Loop " << i + 1 << std::endl;

#ifdef _ENABLE_GRAPH
    std::cout << "Case 1: GQ, GT" << std::endl;
    match_time[0] = MatchBenchmark1<GQ, GT>();
#endif        

    std::cout << "Case 2: LGQ, LGT" << std::endl;
    match_time[1] = MatchBenchmark1<LGQ, LGT>();

    std::cout << "Case 3: LGQ2, LGT2" << std::endl;
    match_time[2] = MatchBenchmark1<LGQ, LGT2>();

    //std::cout << "Case 4: LGQ3, LGT3" << std::endl;
    //match_time[3] = MatchBenchmark1<LGQ, LGT3>();

    // std::cout << "Match time:" << std::endl;
    // for (int j = 0; j < 4; ++j) {
    //  std::cout << "Case " << j + 1 << " : " << match_time[j] << std::endl;
    //}
    // std::cout << std::endl;
  }

  return 0;
}