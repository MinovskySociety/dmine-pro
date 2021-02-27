#include <ctime>
#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/graph_type/large_graph.h"
#include "gundam/io/csvgraph.h"
#include "gundam/component/generator.h"
#include "gundam/match/matchresult.h"
#include "gundam/component/util.h"
#include "gundam/algorithm/vf2.h"

TEST(TestGUNDAM, WRITEMATCHRESULT) {
  using namespace GUNDAM;

  using GraphType = LargeGraph<uint32_t, uint32_t, std::string, uint32_t,
                               uint32_t, std::string>;
  using VertexType = typename GraphType::VertexType;
  using EdgeType = typename GraphType::EdgeType;
  using VertexIDType = typename VertexType::IDType;
  using VertexLabelType = typename VertexType::LabelType;
  using EdgeLabelType = typename EdgeType::LabelType;
  using VertexConstPtr = typename GraphType::VertexConstPtr;
  using EdgeConstPtr = typename GraphType::EdgeConstPtr;

  GraphType query, target;
  VertexIDType query_id = 1, target_id = 1;

  // query
  query.AddVertex(1, VertexLabelType(0));
  query.AddVertex(2, VertexLabelType(1));
  query.AddVertex(3, VertexLabelType(0));
  query.AddEdge(1, 2, EdgeLabelType(1), 1);
  query.AddEdge(3, 2, EdgeLabelType(1), 2);
  // query.AddEdge(3,1,EdgeLabelType(1));

  // target
  target.AddVertex(1, VertexLabelType(0));
  target.AddVertex(2, VertexLabelType(1));
  target.AddVertex(3, VertexLabelType(0));
  target.AddEdge(1, 2, EdgeLabelType(1), 1);
  target.AddEdge(3, 2, EdgeLabelType(1), 2);
  target.AddEdge(3, 1, EdgeLabelType(1), 3);

  std::vector<std::map<VertexConstPtr, VertexConstPtr>> match_result;
  int count =
      VF2<MatchSemantics::kIsomorphism>(query, target, -1, match_result);
  MatchResultToFile<VertexIDType, VertexIDType>(match_result, std::cout);
  std::ofstream file_out("match_result.csv");
  MatchResultToFile<VertexIDType, VertexIDType>(match_result, file_out);
}