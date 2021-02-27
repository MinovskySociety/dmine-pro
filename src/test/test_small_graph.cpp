#include "../gpar_template/gpar_new.h"
#include "gundam/csvgraph.h"
#include "gundam/dp_iso.h"
#include "gundam/large_graph2.h"
#include "gundam/simple_small_graph.h"
#include "gundam/small_graph.h"
#include "gundam/vf2.h"
int main() {
  using Pattern = GUNDAM::SmallGraph<unsigned int, unsigned int, unsigned int,
                                     unsigned int>;
  using TargetGraph =
      GUNDAM::LargeGraph2<unsigned int, unsigned int, unsigned int,
                          unsigned int, unsigned int, unsigned int>;
  TargetGraph data_graph;
  int first = 0, second = 2923922, third = 3099037;
  int tot = 5199355;
  for (int i = first; i < second; i++) {
    data_graph.AddVertex(i, 0);
  }
  for (int i = second; i < third; i++) {
    data_graph.AddVertex(i, 1);
  }
  for (int i = third; i <= tot; i++) {
    data_graph.AddVertex(i, 2);
  }
  std::ifstream e_file("/Users/apple/Desktop/patent.e");
  int u, v, w;
  int cnt = 0;
  while (!e_file.eof()) {
    e_file >> u >> v >> w;
    data_graph.AddEdge(u, v, w, cnt);
    cnt++;
  }
  Pattern pattern;
  pattern.AddVertex(0, 0);
  pattern.AddVertex(1, 1);
  for (int i = 2; i <= 6; i++) pattern.AddVertex(i, 0);
  pattern.AddEdge(5, 1, 1, 0);
  pattern.AddEdge(2, 5, 0, 1);
  pattern.AddEdge(6, 5, 0, 2);
  pattern.AddEdge(3, 1, 1, 3);
  pattern.AddEdge(4, 1, 1, 4);
  pattern.AddEdge(2, 6, 0, 5);
  pattern.AddEdge(4, 0, 0, 6);
  using MatchMap = std::map<typename Pattern::VertexConstPtr,
                            typename TargetGraph::VertexConstPtr>;
  std::vector<MatchMap> match_result;
  auto user_callback = [&match_result](auto &match_state) {
    match_result.push_back(match_state);
    if (match_result.size() == 20416) return false;
    return true;
  };
  auto prune_callback = [](auto &match_state) { return false; };
  auto update_callback = [](auto &candidate_set) { return true; };
  MatchMap match_state;
  match_state.emplace(pattern.FindConstVertex(0),
                      data_graph.FindConstVertex(2599834));
  match_state.emplace(pattern.FindConstVertex(1),
                      data_graph.FindConstVertex(2994577));
  auto t_begin = clock();
  GUNDAM::DPISO(pattern, data_graph, match_state, user_callback, prune_callback,
                update_callback, 100.0);
  auto t_end = clock();
  std::cout << "time = " << (1.0 * t_end - t_begin) / CLOCKS_PER_SEC
            << std::endl;
  std::cout << "size = " << match_result.size() << std::endl;
  return 0;
}
