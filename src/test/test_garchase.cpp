
#include "../gpar/gpar.h"
#include "OutputGraph.h"
#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar/gar_chase.h"
#include "gar/gar_match.h"
#include "gundam/component/generator.h"
#include "gundam/graph_type/large_graph.h"
void TestGARMatch() {
  using GraphType = GUNDAM::LargeGraph<uint64_t, uint32_t, uint32_t, uint64_t,
                                       uint32_t, uint32_t>;
  using namespace gmine_new;
  GraphType query, target;
  GUNDAM::SimpleArithmeticIDGenerator<EdgeIDType> edge_id_gen;
  // build target
  for (int i = 1; i <= 3; i++) target.AddVertex(i, 1);
  for (int i = 4; i <= 7; i++) target.AddVertex(i, 2);
  target.AddVertex(8, 3);
  int cnt = 0;
  for (int i = 4; i <= 7; i++) {
    target.AddEdge(1, i, 1, ++cnt);
  }
  for (int i = 4; i <= 7; i++) {
    target.AddEdge(2, i, 1, ++cnt);
  }
  for (int i = 5; i <= 7; i++) {
    target.AddEdge(3, i, 1, ++cnt);
  }
  target.AddEdge(2, 8, 2, ++cnt);
  edge_id_gen.UseID(cnt);
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
  /*
  GraphAssociationRule<GraphType, GraphType> test;

    ReadGAR(test,
            "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
            "pro\\testdata\\GAR\\node.csv",
            "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
            "pro\\testdata\\GAR\\edge.csv",
            "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
            "pro\\testdata\\GAR\\X.csv",
            "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
            "pro\\testdata\\GAR\\Y.csv");

    GraphAssociationRule<GraphType, GraphType> test1(query);
    test1.AddX<ConstantLiteral<GraphType, GraphType, std::string>>(
        2, 1, std::string{"u2"});
    test1.AddX<ConstantLiteral<GraphType, GraphType, std::string>>(
        3, 2, std::string{"456"});
    test1.AddY<EdgeLiteral<GraphType, GraphType>>(1, 5, 2);
    std::vector<GraphAssociationRule<GraphType, GraphType>> gar_list;
    gar_list.push_back(test);
    std::cout << "before chase:" << std::endl;
    OutputGraph(target);

    // Chase(gar_list, target, edge_id_gen);
    std::cout << "after chase:" << std::endl;
    OutputGraph(target);
    return;
  */
}
int main() {
  TestGARMatch();
  return 0;
}