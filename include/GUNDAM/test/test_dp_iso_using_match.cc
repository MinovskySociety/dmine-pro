#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "gtest/gtest.h"

#include "gundam/data_type/label.h"

#include "gundam/graph_type/graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/simple_small_graph.h"
#include "gundam/graph_type/small_graph.h"

#include "gundam/algorithm/dp_iso_using_match.h"

inline uint64_t GetTime() { return clock() * 1000 / CLOCKS_PER_SEC; }

template <class  QueryGraph, 
          class TargetGraph>
void TestDPISOUsingMatch_1() {
  using namespace GUNDAM;


  using VertexLabelType = typename  QueryGraph::VertexType::LabelType;
  using   EdgeLabelType = typename TargetGraph::  EdgeType::LabelType;

   QueryGraph  query;
  TargetGraph target;

  std::cout << " QueryGraphType: " << typeid( query).name() << std::endl
            << "TargetGraphType: " << typeid(target).name() << std::endl;

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

  GUNDAM::MatchSet<QueryGraph, 
                  TargetGraph> match_result1;
  int count = GUNDAM::DpisoUsingMatch<MatchSemantics::kIsomorphism>(
               query, target, match_result1);
                  
  ASSERT_EQ(count, 2);
  ASSERT_EQ(count, match_result1.size());

  int match_counter = 0;
  for (auto match_it = match_result1.MatchBegin();
           !match_it.IsDone();
            match_it++) {
    std::cout << "match: " << match_counter << std::endl;
    for (auto map_it = match_it->MapBegin();
             !map_it.IsDone();
              map_it++) {
      std::cout << " " << map_it->src_handle()->id()
                << " " << map_it->dst_handle()->id()
                << std::endl;
    }
    match_counter++;
  }

  std::cout << "count: " << match_result1.size() << std::endl;
  const auto&  query2 = query;
  const auto& target2 = target;
  GUNDAM::MatchSet match_result2(query2, target2);
  count = GUNDAM::DpisoUsingMatch<MatchSemantics::kIsomorphism>(
               query2, target2, match_result2);
  ASSERT_EQ(count, 2);
  ASSERT_EQ(count, match_result2.size());

  match_counter = 0;
  for (auto match_it = match_result2.MatchBegin();
           !match_it.IsDone();
            match_it++) {
    std::cout << "match: " << match_counter << std::endl;
    for (auto map_it = match_it->MapBegin();
             !map_it.IsDone();
              map_it++) {
      std::cout << " " << map_it->src_handle()->id()
                << " " << map_it->dst_handle()->id()
                << std::endl;
    }
    match_counter++;
  }

  std::cout << "count: " << match_result2.size() << std::endl;

  const auto& query3 = query;
  auto& target3 = target;
  GUNDAM::MatchSet match_result3(query3, target3);
  count = GUNDAM::DpisoUsingMatch<MatchSemantics::kIsomorphism>(
               query3, target3, match_result3);
                  
  ASSERT_EQ(count, 2);
  ASSERT_EQ(count, match_result3.size());

  match_counter = 0;
  for (auto match_it = match_result3.MatchBegin();
           !match_it.IsDone();
            match_it++) {
    std::cout << "match: " << match_counter << std::endl;
    for (auto map_it = match_it->MapBegin();
             !map_it.IsDone();
              map_it++) {
      std::cout << " " << map_it->src_handle()->id()
                << " " << map_it->dst_handle()->id()
                << std::endl;
    }
    match_counter++;
  }

  std::cout << "count: " << match_result3.size() << std::endl;

  auto& query4 = query;
  const auto& target4 = target;
  GUNDAM::MatchSet match_result4(query4, target4);
  count = GUNDAM::DpisoUsingMatch<MatchSemantics::kIsomorphism>(
               query4, target4, match_result4);
                  
  ASSERT_EQ(count, 2);
  ASSERT_EQ(count, match_result4.size());

  match_counter = 0;
  for (auto match_it = match_result4.MatchBegin();
           !match_it.IsDone();
            match_it++) {
    std::cout << "match: " << match_counter << std::endl;
    for (auto map_it = match_it->MapBegin();
             !map_it.IsDone();
              map_it++) {
      std::cout << " " << map_it->src_handle()->id()
                << " " << map_it->dst_handle()->id()
                << std::endl;
    }
    match_counter++;
  }

  std::cout << "count: " << match_result4.size() << std::endl;
}

TEST(TestGUNDAM, DPISOUsingMatch_1) {
  using namespace GUNDAM;

  using G1
   = Graph<SetVertexIDType<uint32_t>, 
           SetVertexLabelType<uint32_t>,
           SetEdgeIDType<uint32_t>, 
           SetEdgeLabelType<uint32_t>>;

  using G2
   = Graph<SetVertexIDType<uint32_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint32_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using G3
   = Graph<SetVertexIDType<uint32_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint32_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using LG = LargeGraph<uint32_t, uint32_t, std::string, 
                        uint32_t, uint32_t, std::string>;
  using SG  =       SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;
  using SSG = SimpleSmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;

  TestDPISOUsingMatch_1<G1, G1>();
  TestDPISOUsingMatch_1<G1, G2>();
  TestDPISOUsingMatch_1<G1, G3>();
  TestDPISOUsingMatch_1<G1, SG>();
  TestDPISOUsingMatch_1<G1, LG>();

  TestDPISOUsingMatch_1<G2, G1>();
  TestDPISOUsingMatch_1<G2, G2>();
  TestDPISOUsingMatch_1<G2, G3>();
  TestDPISOUsingMatch_1<G2, SG>();
  TestDPISOUsingMatch_1<G2, LG>();

  TestDPISOUsingMatch_1<G3, G1>();
  TestDPISOUsingMatch_1<G3, G2>();
  TestDPISOUsingMatch_1<G3, G3>();
  TestDPISOUsingMatch_1<G3, SG>();
  TestDPISOUsingMatch_1<G3, LG>();
  
  TestDPISOUsingMatch_1<LG, G1>();
  TestDPISOUsingMatch_1<LG, G2>();
  TestDPISOUsingMatch_1<LG, G3>();
  TestDPISOUsingMatch_1<LG, SG>();
  TestDPISOUsingMatch_1<LG, LG>();

  TestDPISOUsingMatch_1<SG, G1>();
  TestDPISOUsingMatch_1<SG, G2>();
  TestDPISOUsingMatch_1<SG, G3>();
  TestDPISOUsingMatch_1<SG, LG>();
  TestDPISOUsingMatch_1<SG, SG>();

  TestDPISOUsingMatch_1<SSG, G1>();
  TestDPISOUsingMatch_1<SSG, G2>();
  TestDPISOUsingMatch_1<SSG, G3>();
  TestDPISOUsingMatch_1<SSG, LG>();
  TestDPISOUsingMatch_1<SSG, SG>();
}

template <class QueryGraph, class TargetGraph>
void TestDPISOUsingMatch_2() {
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

  GUNDAM::MatchSet<const  QueryGraph, 
                   const TargetGraph> match_result;
  const auto&  query_const_ref = query;
  const auto& target_const_ref = target;
  int count = GUNDAM::DpisoUsingMatch<MatchSemantics::kIsomorphism>(
               query_const_ref, 
              target_const_ref, 
                  match_result);

  int match_counter = 0;
  for (auto match_it = match_result.MatchBegin();
           !match_it.IsDone();
            match_it++) {
    std::cout << "match: " << match_counter << std::endl;
    for (auto map_it = match_it->MapBegin();
             !map_it.IsDone();
              map_it++) {
      std::cout << " " << map_it.src_handle()->id()
                << " " << map_it.dst_handle()->id()
                << std::endl;
    }
    match_counter++;
  }

  std::cout << "count: " << match_result.size() << std::endl;
}

// TEST(TestGUNDAM, DPISOUsingMatch_2) {
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

//   TestDPISOUsingMatch_2<QG1, TG1>();
//   TestDPISOUsingMatch_2<QG2, TG2>();
//   TestDPISOUsingMatch_2<QG3, TG3>();
//   TestDPISOUsingMatch_2<QueryGraph, TargetGraph>();
// }

template <class QueryGraph, class TargetGraph>
void TestDPISOUsingMatch_3() {
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

  GUNDAM::MatchSet<const  QueryGraph, 
                   const TargetGraph> match_result;
  const auto&  query_const_ref = query;
  const auto& target_const_ref = target;
  int count = GUNDAM::DpisoUsingMatch<MatchSemantics::kIsomorphism>(
               query_const_ref, 
              target_const_ref, 
                  match_result);

  ASSERT_EQ(count, 2);
  // ASSERT_EQ(count, 4);
  for (size_t i = 0; i < match_result.size(); i++) {
    std::cout << "match: " << i << std::endl;
    for (const auto& mapping : match_result[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result.size() << std::endl;
}