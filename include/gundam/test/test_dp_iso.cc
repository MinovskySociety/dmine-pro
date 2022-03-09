#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "gtest/gtest.h"
#include "gundam/algorithm/dp_iso.h"
#include "gundam/data_type/label.h"
#include "gundam/graph_type/graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/simple_small_graph.h"
#include "gundam/graph_type/small_graph.h"
#include "gundam/type_getter/vertex_handle.h"

inline uint64_t GetTime() { return clock() * 1000 / CLOCKS_PER_SEC; }

template <class QueryGraph, class TargetGraph>
void TestDPISO_1() {
  using VertexLabelType = typename QueryGraph::VertexType::LabelType;
  using EdgeLabelType = typename TargetGraph::EdgeType::LabelType;

  using QueryVertexHandle = typename GUNDAM::VertexHandle<QueryGraph>::type;
  using TargetVertexHandle = typename GUNDAM::VertexHandle<TargetGraph>::type;

  QueryGraph query;
  TargetGraph target;

  // query
  query.AddVertex(1, VertexLabelType(0));
  query.AddVertex(2, VertexLabelType(1));
  query.AddVertex(3, VertexLabelType(0));
  query.AddEdge(1, 2, EdgeLabelType(1), 1);
  query.AddEdge(3, 2, EdgeLabelType(1), 2);
  // query.AddEdge(3, 1, EdgeLabelType(1), 3);

  // target
  target.AddVertex(1, VertexLabelType(0));
  target.AddVertex(2, VertexLabelType(1));
  target.AddVertex(3, VertexLabelType(0));
  target.AddEdge(1, 2, EdgeLabelType(1), 1);
  target.AddEdge(3, 2, EdgeLabelType(1), 2);
  target.AddEdge(3, 1, EdgeLabelType(1), 3);

  std::vector<std::map<QueryVertexHandle, TargetVertexHandle>> match_result;
  int count = GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      query, target, -1, match_result);

  for (size_t i = 0; i < match_result.size(); i++) {
    std::cout << "match " << i << std::endl;
    for (const auto& mapping : match_result[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result.size() << std::endl;
  ASSERT_EQ(count, 2);

  const QueryGraph& query2 = query;
  const TargetGraph& target2 = target;

  using QueryVertexHandle2 =
      typename GUNDAM::VertexHandle<decltype(query2)>::type;
  using TargetVertexHandle2 =
      typename GUNDAM::VertexHandle<decltype(target2)>::type;

  std::vector<std::map<QueryVertexHandle2, TargetVertexHandle2>> match_result2;
  count = GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      query2, target2, -1, match_result2);

  for (size_t i = 0; i < match_result2.size(); i++) {
    std::cout << "match " << i << std::endl;
    for (const auto& mapping : match_result2[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result2.size() << std::endl;
  ASSERT_EQ(count, 2);

  QueryGraph& query3 = query;
  const TargetGraph& target3 = target;

  using QueryVertexHandle3 =
      typename GUNDAM::VertexHandle<decltype(query3)>::type;
  using TargetVertexHandle3 =
      typename GUNDAM::VertexHandle<decltype(target3)>::type;

  std::vector<std::map<QueryVertexHandle3, TargetVertexHandle3>> match_result3;
  count = GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      query3, target3, -1, match_result3);

  for (size_t i = 0; i < match_result3.size(); i++) {
    std::cout << "match " << i << std::endl;
    for (const auto& mapping : match_result3[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result3.size() << std::endl;
  ASSERT_EQ(count, 2);

  const QueryGraph& query4 = query;
  TargetGraph& target4 = target;

  using QueryVertexHandle4 =
      typename GUNDAM::VertexHandle<decltype(query4)>::type;
  using TargetVertexHandle4 =
      typename GUNDAM::VertexHandle<decltype(target4)>::type;

  std::vector<std::map<QueryVertexHandle4, TargetVertexHandle4>> match_result4;
  count = GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
      query4, target4, -1, match_result4);

  for (size_t i = 0; i < match_result4.size(); i++) {
    std::cout << "match " << i << std::endl;
    for (const auto& mapping : match_result4[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result4.size() << std::endl;
  ASSERT_EQ(count, 2);

  // test increament dpiso
  std::vector<TargetVertexHandle> delta_target_graph;
  match_result.clear();
  auto user_callback = [&match_result](auto& match_state) {
    match_result.emplace_back(match_state);
    return true;
  };
  GUNDAM::IncreamentDPISO(query, target, delta_target_graph, user_callback);
}

TEST(TestGUNDAM, DPISO_1) {
  using namespace GUNDAM;

  using G1 = Graph<SetVertexIDType<uint32_t>, SetVertexLabelType<uint32_t>,
                   SetEdgeIDType<uint32_t>, SetEdgeLabelType<uint32_t>>;

  using G2 =
      Graph<SetVertexIDType<uint32_t>,
            SetVertexAttributeStoreType<AttributeType::kGrouped>,
            SetVertexLabelType<uint32_t>,
            SetVertexLabelContainerType<ContainerType::Vector>,
            SetVertexIDContainerType<ContainerType::Map>,
            SetVertexPtrContainerType<ContainerType::Vector>,
            SetEdgeLabelContainerType<ContainerType::Vector>,
            SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint32_t>,
            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
            SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using G3 =
      Graph<SetVertexIDType<uint32_t>,
            SetVertexAttributeStoreType<AttributeType::kGrouped>,
            SetVertexLabelType<uint32_t>,
            SetVertexLabelContainerType<ContainerType::Vector>,
            SetVertexIDContainerType<ContainerType::Vector>,
            SetVertexPtrContainerType<ContainerType::Vector>,
            SetEdgeLabelContainerType<ContainerType::Vector>,
            SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint32_t>,
            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
            SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using LG = LargeGraph<uint32_t, uint32_t, std::string, uint32_t, uint32_t,
                        std::string>;
  using SG = SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;
  using SSG = SimpleSmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;

  TestDPISO_1<G1, G1>();
  TestDPISO_1<G1, G2>();
  TestDPISO_1<G1, G3>();
  TestDPISO_1<G2, G1>();
  TestDPISO_1<G2, G2>();
  TestDPISO_1<G2, G3>();
  TestDPISO_1<G3, G1>();
  TestDPISO_1<G3, G2>();
  TestDPISO_1<G3, G3>();
  TestDPISO_1<LG, G1>();
  TestDPISO_1<LG, G2>();
  TestDPISO_1<LG, G3>();
  TestDPISO_1<LG, LG>();
  TestDPISO_1<SG, G1>();
  TestDPISO_1<SG, G2>();
  TestDPISO_1<SG, G3>();
  TestDPISO_1<SG, LG>();
  TestDPISO_1<SSG, G1>();
  TestDPISO_1<SSG, G2>();
  TestDPISO_1<SSG, G3>();
  TestDPISO_1<SSG, LG>();
}

template <class QueryGraph, class TargetGraph>
void TestDPISO_2() {
  using namespace GUNDAM;

  QueryGraph query;
  TargetGraph target;

  // query
  ASSERT_TRUE(query.AddVertex(1, 0).second);
  ASSERT_TRUE(query.AddVertex(2, 1).second);
  ASSERT_TRUE(query.AddVertex(3, 0).second);
  ASSERT_TRUE(query.AddEdge(1, 2, 1, 1).second);
  ASSERT_TRUE(query.AddEdge(3, 2, 1, 2).second);

  // target
  ASSERT_TRUE(target.AddVertex(1, "0").second);
  ASSERT_TRUE(target.AddVertex(2, "1").second);
  ASSERT_TRUE(target.AddVertex(3, "0").second);
  ASSERT_TRUE(target.AddEdge(1, 2, std::string("1"), 1).second);
  ASSERT_TRUE(target.AddEdge(3, 2, std::string("1"), 2).second);
  ASSERT_TRUE(target.AddEdge(3, 1, std::string("1"), 3).second);

  std::vector<std::map<typename GUNDAM::VertexHandle<QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>>
      match_result;
  int count =
      DPISO<MatchSemantics::kIsomorphism>(query, target, -1, match_result);

  ASSERT_EQ(count, 2);
  for (size_t i = 0; i < match_result.size(); i++) {
    std::cout << "match: " << i << std::endl;
    for (const auto& mapping : match_result[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result.size() << std::endl;
}

// TEST(TestGUNDAM, DPISO_2) {
//   using namespace GUNDAM;

//   using QG1
//    = Graph<SetVertexIDType<uint32_t>,
//            SetVertexLabelType<uint32_t>,
//            SetEdgeIDType<uint32_t>,
//            SetEdgeLabelType<uint32_t>>;

//   using TG1
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexLabelType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeLabelType<std::string>>;

//   using QG2
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<uint32_t>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Map>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<uint32_t>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using TG2
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<std::string>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Map>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<std::string>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using QG3
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<uint32_t>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Vector>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<uint32_t>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using TG3
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<std::string>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Vector>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<std::string>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using QueryGraph = LargeGraph<uint32_t, Label<uint32_t>, std::string,
//                                 uint32_t, Label<uint32_t>, std::string>;

//   using TargetGraph = LargeGraph<uint64_t, std::string, std::string,
//                                  uint64_t, std::string, std::string>;

//   TestDPISO_2<QG1, TG1>();
//   TestDPISO_2<QG2, TG2>();
//   TestDPISO_2<QG3, TG3>();
//   TestDPISO_2<QueryGraph, TargetGraph>();
// }

template <class QueryGraph, class TargetGraph>
void TestDPISO_3() {
  using namespace GUNDAM;

  QueryGraph query;
  TargetGraph target;

  // query
  ASSERT_TRUE(query.AddVertex(1, 0).second);
  ASSERT_TRUE(query.AddVertex(2, 1).second);
  ASSERT_TRUE(query.AddVertex(3, 1).second);
  ASSERT_TRUE(query.AddEdge(1, 2, 1, 1).second);
  ASSERT_TRUE(query.AddEdge(1, 3, 1, 2).second);

  // target
  ASSERT_TRUE(target.AddVertex(1, "0").second);
  ASSERT_TRUE(target.AddVertex(2, "1").second);
  ASSERT_TRUE(target.AddVertex(3, "1").second);
  ASSERT_TRUE(target.AddEdge(1, 2, std::string("1"), 1).second);
  ASSERT_TRUE(target.AddEdge(1, 3, std::string("1"), 2).second);
  // ASSERT_TRUE(target.AddEdge(1, 2, std::string("1"), 3).second);

  std::vector<std::map<typename GUNDAM::VertexHandle< QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>>
      match_result;
  int count =
      DPISO<MatchSemantics::kIsomorphism>(query, target, -1, match_result);

  ASSERT_EQ(count, 2);
  // ASSERT_EQ(count, 4);
  for (size_t i = 0; i < match_result.size(); i++) {
    std::cout << "match: " << i << std::endl;
    for (const auto& mapping : match_result[i]) {
      std::cout << " " << mapping.first ->id() 
                << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result.size() << std::endl;
}

// TEST(TestGUNDAM, DPISO_3) {
//   using namespace GUNDAM;

//   using QG1 =
//     Graph<SetVertexIDType<uint32_t>,
//           SetVertexLabelType<Label<uint32_t>>,
//           SetEdgeIDType<uint32_t>,
//           SetEdgeLabelType<Label<uint32_t>>>;

//   using TG1 =
//     Graph<SetVertexIDType<uint64_t>,
//           SetVertexLabelType<std::string>,
//           SetEdgeIDType<uint64_t>,
//           SetEdgeLabelType<std::string>>;

//   using QG2
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<Label<uint32_t>>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Map>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<Label<uint32_t>>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using TG2
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<std::string>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Map>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<std::string>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using QG3
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<Label<uint32_t>>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Vector>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<Label<uint32_t>>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using TG3
//    = Graph<SetVertexIDType<uint64_t>,
//            SetVertexAttributeStoreType<AttributeType::kGrouped>,
//            SetVertexLabelType<std::string>,
//            SetVertexLabelContainerType<ContainerType::Vector>,
//            SetVertexIDContainerType<ContainerType::Vector>,
//            SetVertexPtrContainerType<ContainerType::Vector>,
//            SetEdgeLabelContainerType<ContainerType::Vector>,
//            SetVertexAttributeKeyType<std::string>,
//            SetEdgeIDType<uint64_t>,
//            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
//            SetEdgeLabelType<std::string>,
//            SetEdgeAttributeKeyType<std::string>>;

//   using QueryGraph = LargeGraph<uint32_t, Label<uint32_t>, std::string,
//                                 uint32_t, Label<uint32_t>, std::string>;

//   using TargetGraph = LargeGraph<uint64_t, std::string, std::string,
//                                  uint64_t, std::string, std::string>;

//   // TestDPISO_3<QG1, TG1>();
//   // TestDPISO_3<QG2, TG2>();
//   // TestDPISO_3<QG3, TG3>();
//   TestDPISO_3<QueryGraph, TargetGraph>();
// }

template <class QueryGraph, class TargetGraph>
void TestDPISOSpeed1(int times_outer, int times_inner) {
  using namespace GUNDAM;

   QueryGraph  query;
  TargetGraph target;

  std::cout << " QueryGraphType: " << typeid( query).name() << std::endl
            << "TargetGraphType: " << typeid(target).name() << std::endl;

  // query
  ASSERT_TRUE(query.AddVertex(1, 0).second);
  ASSERT_TRUE(query.AddVertex(2, 2).second);
  ASSERT_TRUE(query.AddVertex(3, 3).second);
  ASSERT_TRUE(query.AddVertex(4, 2).second);
  ASSERT_TRUE(query.AddVertex(5, 3).second);
  ASSERT_TRUE(query.AddEdge(1, 2, 1, 1).second);
  ASSERT_TRUE(query.AddEdge(1, 4, 1, 2).second);
  ASSERT_TRUE(query.AddEdge(3, 2, 2, 3).second);
  ASSERT_TRUE(query.AddEdge(5, 4, 2, 4).second);

  // target
  ASSERT_TRUE(target.AddVertex(1, 0).second);
  ASSERT_TRUE(target.AddVertex(2, 2).second);
  ASSERT_TRUE(target.AddVertex(3, 3).second);
  ASSERT_TRUE(target.AddVertex(4, 2).second);
  ASSERT_TRUE(target.AddVertex(5, 3).second);
  ASSERT_TRUE(target.AddVertex(6, 0).second);
  ASSERT_TRUE(target.AddVertex(7, 2).second);
  ASSERT_TRUE(target.AddVertex(8, 3).second);
  ASSERT_TRUE(target.AddVertex(9, 2).second);
  ASSERT_TRUE(target.AddVertex(10, 3).second);
  uint64_t eid = 0;
  ASSERT_TRUE(target.AddEdge(1, 2, 1, ++eid).second);
  ASSERT_TRUE(target.AddEdge(1, 4, 1, ++eid).second);
  ASSERT_TRUE(target.AddEdge(1, 7, 1, ++eid).second);
  ASSERT_TRUE(target.AddEdge(1, 9, 1, ++eid).second);
  ASSERT_TRUE(target.AddEdge(3, 2, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(5, 4, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(3, 4, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(5, 2, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(8, 7, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(10, 9, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(8, 9, 2, ++eid).second);
  ASSERT_TRUE(target.AddEdge(10, 7, 2, ++eid).second);

  std::vector<std::map<typename GUNDAM::VertexHandle<QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>>
      match_result1;

  std::vector<
      std::vector<std::pair<typename GUNDAM::VertexHandle<QueryGraph>::type,
                            typename GUNDAM::VertexHandle<TargetGraph>::type>>>
      match_result2;

  uint64_t start, end;

  for (int j = 0; j < times_outer; j++) {
    std::cout << "         Default: ";
    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count = DPISO(query, target, -1, match_result1);

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << end - start << " ms" << std::endl;

    std::cout << "       Recursive: ";
    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int max_result = -1;
      match_result1.clear();

      int count = _dp_iso::DPISO_Recursive<MatchSemantics::kIsomorphism>(
          query, target,
          std::bind(_dp_iso::MatchCallbackSaveResult<
                        typename GUNDAM::VertexHandle<QueryGraph>::type,
                        typename GUNDAM::VertexHandle<TargetGraph>::type,
                        std::vector<std::map<
                            typename GUNDAM::VertexHandle<QueryGraph>::type,
                            typename GUNDAM::VertexHandle<TargetGraph>::type>>>,
                    std::placeholders::_1, &max_result, &match_result1));

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << end - start << " ms" << std::endl;

    // std::cout << "    NonRecursive: ";
    // start = GetTime();
    // for (int i = 0; i < times_inner; i++) {
    //   int max_result = -1;
    //   match_result1.clear();

    //   int count = _vf2::DPISO_NonRecursive<MatchSemantics::kIsomorphism>(
    //       query, target,
    //       _vf2::LabelEqual<typename GUNDAM::VertexHandle<QueryGraph>::type,
    //                        typename
    //                        GUNDAM::VertexHandle<TargetGraph>::type>(),
    //       _vf2::LabelEqual<typename QueryGraph::EdgeConstPtr,
    //                        typename TargetGraph::EdgeConstPtr>(),
    //       std::bind(
    //           _vf2::MatchCallbackSaveResult<
    //               typename GUNDAM::VertexHandle<QueryGraph>::type,
    //               typename GUNDAM::VertexHandle<TargetGraph>::type,
    //               std::vector<std::map<typename
    //               GUNDAM::VertexHandle<QueryGraph>::type,
    //                                    typename
    //                                    GUNDAM::VertexHandle<TargetGraph>::type>>>,
    //           std::placeholders::_1, &max_result, &match_result1));

    //   ASSERT_EQ(40, count);
    // }
    // end = GetTime();
    // std::cout << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count = DPISO<MatchSemantics::kIsomorphism>(query, target, 20);

      ASSERT_EQ(20, count);
    }
    end = GetTime();
    std::cout << "           Limit: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count =
          DPISO<MatchSemantics::kIsomorphism>(query, target, -1, match_result2);

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "   Vector Result: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int max_result = -1;
      match_result1.clear();

      int count = DPISO<MatchSemantics::kIsomorphism>(
          query, target,
          std::bind(_dp_iso::MatchCallbackSaveResult1<
                        typename GUNDAM::VertexHandle<QueryGraph>::type,
                        typename GUNDAM::VertexHandle<TargetGraph>::type,
                        std::vector<std::map<
                            typename GUNDAM::VertexHandle<QueryGraph>::type,
                            typename GUNDAM::VertexHandle<TargetGraph>::type>>>,
                    std::placeholders::_1, &max_result, &match_result1));

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "Custom Callback1: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int max_result = -1;
      match_result1.clear();

      int count = DPISO<MatchSemantics::kIsomorphism>(
          query, target,
          std::bind(
              _dp_iso::MatchCallbackSaveResult2<
                  typename GUNDAM::VertexHandle<QueryGraph>::type,
                  typename GUNDAM::VertexHandle<TargetGraph>::type,
                  std::vector<std::vector<std::pair<
                      typename GUNDAM::VertexHandle<QueryGraph>::type,
                      typename GUNDAM::VertexHandle<TargetGraph>::type>>>>,
              std::placeholders::_1, &max_result, &match_result2));

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "Custom Callback2: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count =
          DPISO<MatchSemantics::kIsomorphism>(query, target, -1, match_result1);

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "  Custom Compare: " << end - start << " ms" << std::endl;

    std::cout << std::endl;
  }
}

TEST(TestGUNDAM, DPISO_Speed_1) {
  using namespace GUNDAM;

  using QG1 = Graph<SetVertexIDType<uint32_t>, SetVertexLabelType<uint32_t>,
                    SetEdgeIDType<uint32_t>, SetEdgeLabelType<uint32_t>>;

  using TG1 = Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<uint32_t>,
                    SetEdgeIDType<uint64_t>, SetEdgeLabelType<uint32_t>>;

  using QG2 =
      Graph<SetVertexIDType<uint64_t>,
            SetVertexAttributeStoreType<AttributeType::kGrouped>,
            SetVertexLabelType<uint32_t>,
            SetVertexLabelContainerType<ContainerType::Vector>,
            SetVertexIDContainerType<ContainerType::Map>,
            SetVertexPtrContainerType<ContainerType::Vector>,
            SetEdgeLabelContainerType<ContainerType::Vector>,
            SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint64_t>,
            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
            SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using TG2 =
      Graph<SetVertexIDType<uint64_t>,
            SetVertexAttributeStoreType<AttributeType::kGrouped>,
            SetVertexLabelType<uint32_t>,
            SetVertexLabelContainerType<ContainerType::Vector>,
            SetVertexIDContainerType<ContainerType::Map>,
            SetVertexPtrContainerType<ContainerType::Vector>,
            SetEdgeLabelContainerType<ContainerType::Vector>,
            SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint64_t>,
            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
            SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using QG3 =
      Graph<SetVertexIDType<uint64_t>,
            SetVertexAttributeStoreType<AttributeType::kGrouped>,
            SetVertexLabelType<uint32_t>,
            SetVertexLabelContainerType<ContainerType::Vector>,
            SetVertexIDContainerType<ContainerType::Vector>,
            SetVertexPtrContainerType<ContainerType::Vector>,
            SetEdgeLabelContainerType<ContainerType::Vector>,
            SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint64_t>,
            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
            SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using TG3 =
      Graph<SetVertexIDType<uint64_t>,
            SetVertexAttributeStoreType<AttributeType::kGrouped>,
            SetVertexLabelType<uint32_t>,
            SetVertexLabelContainerType<ContainerType::Vector>,
            SetVertexIDContainerType<ContainerType::Vector>,
            SetVertexPtrContainerType<ContainerType::Vector>,
            SetEdgeLabelContainerType<ContainerType::Vector>,
            SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint64_t>,
            SetEdgeAttributeStoreType<AttributeType::kGrouped>,
            SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using QLG = LargeGraph<uint32_t, uint32_t, std::string, uint32_t, uint32_t,
                         std::string>;

  using QSG = SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;

  using QSSG = SimpleSmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;

  using TLG = LargeGraph<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                         std::string>;

  TestDPISOSpeed1<QG1, TG1>(1, 10000);
  TestDPISOSpeed1<QG2, TG2>(1, 10000);
  TestDPISOSpeed1<QG3, TG3>(1, 10000);
  TestDPISOSpeed1<QLG, TLG>(1, 10000);
  TestDPISOSpeed1<QSG, TLG>(1, 10000);
  TestDPISOSpeed1<QSSG, TLG>(1, 10000);
}