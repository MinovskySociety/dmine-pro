#include <ctime>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "gundam/component/generator.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/component/util.h"

TEST(TestGUNDAM, IDGEN)
{
  using G = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                               uint32_t, std::string>;
  G g;
  GUNDAM::SimpleArithmeticIDGenerator<typename G::EdgeType::IDType> edge_id_gen;
  for (int i = 1; i <= 10; i++)
    g.AddVertex(i, 0);
  for (int i = 1; i <= 10; i++)
  {
    for (int j = i + 1; j <= 10; j++)
    {
      int id = rand() % 10000000;
      g.AddEdge(i, j, 0, id);
      edge_id_gen.UseID(id);
    }
  }
  bool res = g.AddEdge(1, 1, 1, edge_id_gen.GetID()).second;
  ASSERT_EQ(res, 1);
}