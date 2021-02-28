#include "../gpar/gpar.h"
#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar/gar_chase.h"
#include "gar/gar_match.h"
#include "gundam/graph_type/large_graph.h"
void TestGARMatch() {
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, uint32_t, uint64_t,
                                       uint32_t, uint32_t>;
  using namespace gmine_new;
  GraphType query;
  DataGraph target;
  // build target
  for (int i = 1; i <= 3; i++) target.AddVertex(i, 1);
  for (int i = 4; i <= 7; i++) target.AddVertex(i, 2);
  target.AddVertex(8, 3);
  int cnt = 0;
  for (int i = 4; i <= 7; i++) target.AddEdge(1, i, 1, ++cnt);
  for (int i = 4; i <= 7; i++) target.AddEdge(2, i, 1, ++cnt);
  for (int i = 5; i <= 7; i++) target.AddEdge(3, i, 1, ++cnt);
  target.AddEdge(2, 8, 2, ++cnt);
  target.FindVertex(1)->AddAttribute(1, (std::string) "u1");
  target.FindVertex(2)->AddAttribute(1, (std::string) "u2");
  target.FindVertex(3)->AddAttribute(1, (std::string) "u3");
  target.FindVertex(4)->AddAttribute(2, (std::string) "123");
  target.FindVertex(5)->AddAttribute(2, (std::string) "456");
  target.FindVertex(6)->AddAttribute(2, (std::string) "789");
  target.FindVertex(7)->AddAttribute(2, (std::string) "000");

  // build query
  for (int i = 1; i <= 2; i++) query.AddVertex(i, 1);
  for (int i = 3; i <= 4; i++) query.AddVertex(i, 2);
  query.AddVertex(5, 3);
  cnt = 0;
  for (int i = 1; i <= 2; i++) {
    for (int j = 3; j <= 4; j++) {
      query.AddEdge(i, j, 1, ++cnt);
    }
  }
  query.AddEdge(2, 5, 2, ++cnt);

  // add X
  GraphAssociationRule<GraphType, DataGraph> test1(query);
  test1.AddX<ConstantLiteral<GraphType, DataGraph, std::string>>(
      2, 1, std::string{"u2"});
  test1.AddX<ConstantLiteral<GraphType, DataGraph, std::string>>(
      3, 2, std::string{"456"});
  test1.AddY<EdgeLiteral<GraphType, DataGraph>>(1, 5, 2);
  std::vector<std::map<VertexConstPtr, typename DataGraph::VertexConstPtr>>
      match_result;
  GraphAssociationRule<GraphType, DataGraph> test2 = test1;
  GARMatch<true>(test1, target, match_result);
  std::cout << "total Match = " << match_result.size() << std::endl;
  for (auto &it : match_result) {
    for (auto &it1 : it) {
      std::cout << it1.first->id() << " " << it1.second->id() << std::endl;
    }
    std::cout << std::endl;
  }
  std::cout << "another GAR:" << std::endl;
  match_result.clear();
  GARMatch<true>(test2, target, match_result);
  std::cout << "total Match = " << match_result.size() << std::endl;
  for (auto &it : match_result) {
    for (auto &it1 : it) {
      std::cout << it1.first->id() << " " << it1.second->id() << std::endl;
    }
    std::cout << std::endl;
  }
}
int main() {
  TestGARMatch();
  return 0;
}