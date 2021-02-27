//#include "gundam/boost_iso.h"
#include "gundam/csvgraph.h"
#include "gundam/dp_iso.h"
//#include "gundam/graph.h"
#include "gundam/large_graph.h"
#include "gundam/large_graph2.h"
#include "gundam/vf2_boost.h"
void test() {
  using namespace GUNDAM;
  using GraphType = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string,
                                        uint64_t, uint32_t, std::string>;

  using DataGraph = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string,
                                        uint64_t, uint32_t, std::string>;
  using TargetVertexPtr = typename DataGraph::VertexConstPtr;
  DataGraph data_graph;
  using VertexConstPtr = typename DataGraph::VertexConstPtr;
  using VertexIDType = typename DataGraph::VertexType::IDType;
  using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
  ReadCSVGraph(data_graph,
               "/Users/apple/Desktop/buaa/data/cu4999_1/liantong_v.csv",
               "/Users/apple/Desktop/buaa/data/cu4999_1/liantong_e.csv");
  for (int head = 1000000020; head <= 1000000020; head++) {
    GraphType pattern;
    pattern.AddVertex(1, 1001);
    pattern.AddVertex(2, 1000000002);
    pattern.AddVertex(3, 1002);
    pattern.AddVertex(4, 1001);
    pattern.AddVertex(5, head);
    pattern.AddVertex(6, 1002);
    pattern.AddVertex(7, 1000000049);
    pattern.AddEdge(3, 1, 2001, 1);
    pattern.AddEdge(4, 3, 2001, 2);
    pattern.AddEdge(3, 5, 1, 3);
    pattern.AddEdge(4, 2, 1, 4);
    pattern.AddEdge(6, 1, 2001, 5);
    pattern.AddEdge(4, 6, 2001, 6);
    pattern.AddEdge(6, 7, 1, 7);
    auto t_begin = clock();
    auto sz = DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, -1);
    auto t_end = clock();
    std::cout << "head = " << head << std::endl;
    std::cout << "time2 = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
              << std::endl;
    std::cout << "size = " << sz << std::endl;
  }
}
void testsinglegpar() {
  using namespace GUNDAM;
  using GraphType = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string,
                                        uint64_t, uint32_t, std::string>;

  using DataGraph = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string,
                                        uint64_t, uint32_t, std::string>;
  using TargetVertexPtr = typename DataGraph::VertexConstPtr;
  DataGraph data_graph;
  using VertexConstPtr = typename DataGraph::VertexConstPtr;
  using VertexIDType = typename DataGraph::VertexType::IDType;
  using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
  ReadCSVGraph(data_graph,
               "/Users/apple/Desktop/buaa/data/cu4999_1/liantong_v.csv",
               "/Users/apple/Desktop/buaa/data/cu4999_1/liantong_e.csv");
  constexpr int pattern_num = 1000;
  for (int i = 0; i < pattern_num; i++) {
    std::string v_file = "pattern_" + std::to_string(i) + "v.csv";
    std::string e_file = "pattern_" + std::to_string(i) + "e.csv";
    GraphType pattern;
    GUNDAM::ReadCSVGraph(pattern, v_file.c_str(), e_file.c_str());
    DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, -1);
  }
  std::set<int> res;
  clock_t t_begin, t_end;
  t_begin = clock();
  int a = 1;
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
      a++;
    }
  }
  // std::cout << "a=" << a << std::endl;
  t_end = clock();
  std::cout << "transpose a graph time is"
            << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC << std::endl;
  for (int head = 1000000003; head <= 1000000003; head++) {
    for (int length = 3003; length <= 3003; length++) {
      GraphType pattern;
      /*
      pattern.AddVertex(1, 1001);
      pattern.AddVertex(2, 1000000002);
      pattern.AddVertex(3, 1002);
      pattern.AddVertex(4, 3003);
      pattern.AddVertex(5, 1000012);
      pattern.AddVertex(6, 1001);
      pattern.AddVertex(7, 1002);
      // pattern.AddVertex(8, 3003);
      pattern.AddEdge(1, 3, 2001, 1);
      pattern.AddEdge(1, 7, 2002, 2);
      pattern.AddEdge(3, 4, 1, 3);
      pattern.AddEdge(4, 5, 2, 4);
      pattern.AddEdge(6, 3, 2001, 5);
      pattern.AddEdge(6, 7, 2002, 6);

      /*
      pattern.AddVertex(1, 1001);
      pattern.AddVertex(2, 1000000002);
      pattern.AddVertex(3, 1002);
      pattern.AddVertex(4, 1000000025);
      pattern.AddVertex(5, 1002);
      pattern.AddVertex(6, 1002);
      pattern.AddVertex(7, 3003);
      pattern.AddVertex(8, 3003);
      pattern.AddEdge(1, 5, 2001, 1);
      pattern.AddEdge(1, 6, 2001, 2);
      pattern.AddEdge(3, 1, 2001, 3);
      pattern.AddEdge(3, 4, 1, 4);
      pattern.AddEdge(5, 7, 1, 5);
      pattern.AddEdge(6, 8, 1, 6);
      */
      pattern.AddVertex(1, 1001);
      pattern.AddVertex(2, 1000000002);
      pattern.AddVertex(3, 1002);
      pattern.AddVertex(4, 3003);
      pattern.AddVertex(5, 1000012);
      pattern.AddVertex(6, 1002);
      pattern.AddVertex(7, 1002);
      pattern.AddVertex(8, 1002);
      pattern.AddVertex(9, 3003);
      pattern.AddEdge(1, 3, 2001, 1);
      pattern.AddEdge(1, 6, 2002, 2);
      pattern.AddEdge(1, 7, 2001, 3);
      pattern.AddEdge(1, 8, 2001, 4);
      pattern.AddEdge(3, 4, 1, 5);
      pattern.AddEdge(4, 5, 2, 6);
      pattern.AddEdge(7, 4, 1, 7);
      pattern.AddEdge(8, 9, 1, 8);

      // pattern.AddEdge(4, 3, 2001, 3);
      // pattern.AddEdge(3, 5, 1, 4);
      // pattern.AddEdge(4, 6, 1, 5);
      // pattern.AddEdge(5, 6, 1, 6);
      // pattern.AddEdge(4, 7, 1, 7);
      // pattern.AddEdge(5, 7, 1, 8);
      /*
      std::map<typename GraphType::VertexConstPtr, std::vector<VertexConstPtr>>
          candidate_set;
      GUNDAM::_dp_iso::InitCandidateSet<GUNDAM::MatchSemantics::kIsomorphism>(
          pattern, data_graph, candidate_set);
      GUNDAM::_dp_iso::RefineCandidateSet(pattern, data_graph, candidate_set);
      DataGraph candidate_space;
      t_begin = clock();
      for (auto vertex_it = pattern.VertexCBegin(); !vertex_it.IsDone();
           vertex_it++) {
        auto query_vertex_ptr = vertex_it;
        for (const auto &candidate_it :
             candidate_set.find(query_vertex_ptr)->second) {
          candidate_space.AddVertex(candidate_it->id(), candidate_it->label());
        }
      }
      // std::cout << "end!" << std::endl;
      for (auto vertex_it = pattern.VertexCBegin(); !vertex_it.IsDone();
           vertex_it++) {
        // std::cout << "id = " << vertex_it->id() << std::endl;
        auto query_vertex_ptr = vertex_it;
        for (const auto &src_candidate_it :
             candidate_set.find(query_vertex_ptr)->second) {
          for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
               edge_it++) {
            auto query_dst_ptr = edge_it->const_dst_ptr();
            for (const auto &dst_candidate_it :
                 candidate_set.find(query_dst_ptr)->second) {
              for (auto edge_it1 = src_candidate_it->OutEdgeCBegin(
                       edge_it->label(), dst_candidate_it);
                   !edge_it1.IsDone(); edge_it1++) {
                candidate_space.AddEdge(src_candidate_it->id(),
                                        dst_candidate_it->id(),
                                        edge_it1->label(), edge_it1->id());
              }
            }
          }
        }
      }
      */
      t_end = clock();
      std::cout << "build cs time = "
                << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC << std::endl;

      std::vector<VertexConstPtr> possible_supp, supp;
      for (int i = 0; i < 5000; i++) {
        possible_supp.push_back(data_graph.FindConstVertex(i));
      }
      VertexConstPtr query_vertex_ptr = pattern.FindConstVertex(1);
      using MatchMap =
          std::map<typename GraphType::VertexConstPtr, VertexConstPtr>;
      using MatchResult = std::vector<MatchMap>;
      MatchResult match_result;
      std::vector<typename GraphType::VertexConstPtr> supp_list;
      supp_list.push_back(query_vertex_ptr);
      auto user_callback = [&match_result](auto &match_state) {
        match_result.push_back(match_state);
        return true;
      };
      // DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, supp_list,
      //                                    user_callback, (double)100);
      // DPISO<MatchSemantics::kIsomorphism>(pattern, data_graph, 100000,
      //                                    match_result);
      std::cout << "size = " << match_result.size() << std::endl;
      // return;
      sort(possible_supp.begin(), possible_supp.end());
      auto t_begin = clock();
      auto sz = DPISO<MatchSemantics::kIsomorphism>(
          pattern, data_graph, query_vertex_ptr, possible_supp, supp);
      // auto sz = VF2_Label_Equal<MatchSemantics::kIsomorphism>(
      //    pattern, data_graph, 100000);
      auto t_end = clock();
      std::cout << "time2 = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
                << std::endl;

      int last = 0;
      std::set<int> s;
      for (const auto &it : supp) s.insert(it->id());
      for (const auto &it : s) {
        if (it >= 4100) last++;
        res.insert(it);
        // std::cout << it << " ";
      }
      if (supp.size() == 0) continue;
      std::cout << "head=" << head << " length=" << length << std::endl;
      std::cout << "sz=" << supp.size() << std::endl;
      std::cout << "supp_r = " << last << std::endl;
      for (const auto &it : s) {
        std::cout << it << " ";
        // std::cout << it << " ";
      }
      std::cout << std::endl;
      std::cout << "conf=" << (1.0 * last) / supp.size() << std::endl;
      std::cout << std::endl;
    }
  }
  std::cout << "res size=" << res.size() << std::endl;
  int sz = 0;
  for (const auto &it : res) {
    if (it >= 4100) sz++;
  }
  std::cout << "supp r=" << sz << std::endl;
}
int main() {
  test();
  return 0;
  // testsinglegpar();
  // return 0;
  using namespace GUNDAM;
  using GraphType = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  using TargetVertexPtr = typename DataGraph::VertexConstPtr;
  DataGraph data_graph;
  using VertexConstPtr = typename DataGraph::VertexConstPtr;
  using VertexIDType = typename DataGraph::VertexType::IDType;
  using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
  using EdgeConstPtr = typename GraphType::EdgeConstPtr;
  ReadCSVGraph(data_graph,
               "/Users/apple/Desktop/buaa/data/cu4999_1/liantong_v.csv",
               "/Users/apple/Desktop/buaa/data/cu4999_1/liantong_e.csv");
  clock_t t_begin, t_end;
  t_begin = clock();
  for (auto vertex_it = data_graph.VertexCBegin(); !vertex_it.IsDone();
       vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
         edge_it++) {
    }
  }
  t_end = clock();
  std::cout << "transpose a graph time is"
            << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC << std::endl;
  // int flag;
  GraphType pattern;
  // pattern.AddVertex(3, 5);
  pattern.AddVertex(1, 1001);
  pattern.AddVertex(2, 1000000002);
  pattern.AddVertex(3, 1002);
  pattern.AddVertex(4, 3003);
  // pattern.AddVertex(5, 1000000018);

  pattern.AddEdge(3, 1, 2001, 1);
  pattern.AddEdge(3, 4, 1, 2);
  // pattern.AddEdge(4, 3, 2002, 3);
  /*
  pattern.AddEdge(5, 4, 0, 4);
  pattern.AddEdge(5, 2, 3, 5);
  int cnt = 5;
  for (int i = 6; i <= 13; i++) {
    pattern.AddEdge(i, 1, 0, ++cnt);
    pattern.AddEdge(5, i, 0, ++cnt);
  }
  // pattern.AddEdge(3, 1, 0, 1);
  // pattern.AddEdge(3, 1, 1, 2);
  */
  /*
   pattern.AddEdge(3, 1, 1, 1);
   pattern.AddEdge(3, 4, 0, 2);
   pattern.AddEdge(4, 2, 3, 3);
   */
  // pattern.AddEdge(5, 6, 0, 4);
  // pattern.AddEdge(5, 7, 0, 5);
  // VertexConstPtr query_vertex_ptr = pattern.FindConstVertex(1);
  /*
  time_t t_begin = clock();
  std::vector<std::map<VertexConstPtr, VertexConstPtr>> match_result;
  int flag = VF2_Label_Equal<MatchSemantics::kIsomorphism>(pattern,
  data_graph, -1, match_result); time_t t_end = clock();

  std::cout << "time000 = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
            << std::endl;
  */
  /*
   std::vector<VertexConstPtr> init_candidate;
   for (int i = 0; i < 100; i++) {
     init_candidate.push_back(data_graph.FindConstVertex(i));
   }
   sort(init_candidate.begin(), init_candidate.end());
   std::set<VertexConstPtr> res_supp;
   auto match_callback = std::bind(
       _vf2_label_equal::MatchCallbackSaveSupp<VertexConstPtr,
   VertexConstPtr, std::set<VertexConstPtr>>, std::placeholders::_1,
   &query_vertex_ptr, &res_supp); auto supp_callback = std::bind(
       _vf2_label_equal::PruneSuppCallback<VertexConstPtr, VertexConstPtr,
                                           std::set<VertexConstPtr>>,
       std::placeholders::_1, &query_vertex_ptr, &res_supp);
   auto update_callback = std::bind(
       _vf2_label_equal::UpdateCandidateCallback<VertexConstPtr,
   VertexConstPtr>, std::placeholders::_1, &query_vertex_ptr,
   init_candidate); t_begin = clock(); flag =
   VF2_Label_Equal<MatchSemantics::kIsomorphism>( pattern, data_graph,
   match_callback, supp_callback, update_callback); t_end = clock();
   std::cout << "time001 = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
             << std::endl;
   std::cout << "size = " << res_supp.size() << std::endl;
   for (auto it : res_supp) {
     std::cout << "supp = " << it->id() << std::endl;
   }
  */
  VertexConstPtr query_vertex_ptr = pattern.FindConstVertex(1);
  std::set<VertexConstPtr> res_supp;
  auto match_callback =
      std::bind(_dp_iso::MatchCallbackSaveSupp<VertexConstPtr, VertexConstPtr,
                                               std::set<VertexConstPtr>>,
                std::placeholders::_1, &query_vertex_ptr, &res_supp);
  auto supp_callback =
      std::bind(_dp_iso::PruneSuppCallback<VertexConstPtr, VertexConstPtr,
                                           std::set<VertexConstPtr>>,
                std::placeholders::_1, &query_vertex_ptr, &res_supp);
  auto update_callback = std::bind(
      _dp_iso::UpdateCandidateCallbackEmpty<VertexConstPtr, VertexConstPtr>,
      std::placeholders::_1);
  int tot_size = 0;
  double tot_time = 0;
  t_begin = clock();
  int sz;
  /*
  std::vector<std::map<typename GraphType::VertexConstPtr, VertexConstPtr>> res;
  sz = VF2_Label_Equal<MatchSemantics::kIsomorphism>(pattern, data_graph, -1,
                                                     res);
  std::cout << "sz =" << sz << std::endl;
  std::set<int> phone_set, user_set;
  for (auto &it : res) {
    phone_set.insert(it[pattern.FindConstVertex(3)]->id());
    user_set.insert(it[pattern.FindConstVertex(4)]->id());
  }
  std::cout << "phone set = " << phone_set.size() << std::endl;
  for (auto &it : phone_set) {
    std::cout << it << " ";
  }
  std::cout << std::endl;
  std::cout << "user set = " << user_set.size() << std::endl;
  for (auto &it : user_set) {
    std::cout << it << " ";
  }
  std::cout << std::endl;
  */
  // sz = VF2_Label_Equal<MatchSemantics::kIsomorphism>(
  //     pattern, data_graph, match_callback, supp_callback, update_callback);
  // int sz =
  //    VF2_Label_Equal<MatchSemantics::kIsomorphism>(pattern, data_graph,
  //    -1);
  t_end = clock();
  std::cout << "time1 = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
            << std::endl;
  std::cout << "sz=" << sz << std::endl;

  std::vector<VertexConstPtr> possible_supp, supp;
  for (int i = 0; i < 5000; i++) {
    possible_supp.push_back(data_graph.FindConstVertex(i));
  }
  sort(possible_supp.begin(), possible_supp.end());
  t_begin = clock();
  sz = DPISO<MatchSemantics::kIsomorphism>(
      pattern, data_graph, query_vertex_ptr, possible_supp, supp);
  t_end = clock();
  std::cout << "time2 = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
            << std::endl;
  std::cout << "sz=" << supp.size() << std::endl;
  int last = 0;
  std::set<int> s;
  for (const auto &it : supp) s.insert(it->id());
  for (const auto &it : s) {
    if (it >= 4100) last++;
    std::cout << it << " ";
  }
  std::cout << std::endl;
  std::cout << "supp_r = " << last << std::endl;
  return 0;
  /*
  for (int i = 0; i < 0; i++) {
    // if (i != 26) continue;
    std::cout << "i=" << i << std::endl;
    time_t t_begin = clock();
    std::vector<std::map<typename Graph<>::VertexConstPtr, VertexConstPtr>>
        match_result;
    int flag = VF2_Label_Equal<MatchSemantics::kIsomorphism>(
        pattern, data_graph, init_candidate_set, 1, i, 1, match_result);
    std::cout << "size = " << match_result.size() << std::endl;
    tot_size += match_result.size() >= 1;
    time_t t_end = clock();
    std::cout << "time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
              << std::endl;
    tot_time += (1.0 * t_end - t_begin) / CLOCKS_PER_SEC;
  }
  std::cout << "total size = " << tot_size << std::endl;
  std::cout << "total time = " << tot_time << std::endl;
  return 0;
  */
}

/*
int main() {
  using namespace GUNDAM;
  Graph<> data_graph;
  using VertexConstPtr = typename Graph<>::VertexConstPtr;
  ReadCSVGraph(
      data_graph,
      "/Users/apple/Desktop/buaa/code/dmine-pro/testdata/"
      "liantong_n1.csv",
      "/Users/apple/Desktop/buaa/code/dmine-pro/testdata/liantong_e1.csv");
  Graph<> pattern;
  pattern.AddVertex(1, 4);
  pattern.AddVertex(3, 5);
  pattern.AddVertex(4, 6);
  pattern.AddVertex(5, 4);
  pattern.AddVertex(6, 4);
  pattern.AddEdge(1, 3, 0, 1);
  pattern.AddEdge(1, 4, 2, 2);
  pattern.AddEdge(3, 6, 0, 3);
  pattern.AddEdge(5, 4, 2, 4);
  for (int i = 0; i < 100; i++) {
    // if (i != 26) continue;
    time_t t_begin = clock();
    std::vector<std::map<VertexConstPtr, VertexConstPtr>> match_result;
    int flag = VF2_Label_Equal<MatchSemantics::kIsomorphism>(
        pattern, data_graph, 1, i, 1, match_result);
    std::cout << "size = " << match_result.size() << std::endl;
    time_t t_end = clock();
    std::cout << "time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
              << std::endl;
  }
  return 0;
}
*/