#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "gtest/gtest.h"
#include "gundam/graph_type/graph.h"
#include "gundam/data_type/label.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/simple_small_graph.h"
#include "gundam/graph_type/small_graph.h"
#include "gundam/algorithm/vf2.h"

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/type_getter/edge_handle.h"

inline uint64_t GetTime() { return clock() * 1000 / CLOCKS_PER_SEC; }

template <typename Handle1, 
          typename Handle2>
bool LabelEqual2(const Handle1& a, 
                 const Handle2& b) {
  return a->label() == b->label();
}

template <class QueryGraph, class TargetGraph>
void TestVF2_1() {
  using VertexLabelType = typename  QueryGraph::VertexType::LabelType;
  using   EdgeLabelType = typename TargetGraph::  EdgeType::LabelType;

  using  QueryVertexHandle = typename GUNDAM::VertexHandle< QueryGraph>::type;
  using TargetVertexHandle = typename GUNDAM::VertexHandle<TargetGraph>::type;

  using  QueryEdgeHandle = typename GUNDAM::EdgeHandle< QueryGraph>::type;
  using TargetEdgeHandle = typename GUNDAM::EdgeHandle<TargetGraph>::type;

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

  std::vector<std::map<QueryVertexHandle, 
                      TargetVertexHandle>> match_result;
  int count = GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query, target, -1, match_result);

  for (size_t i = 0; i < match_result.size(); i++) {
    std::cout << "match " << i << std::endl;
    for (const auto& mapping : match_result[i]) {
      std::cout << " " << mapping.first ->id() 
                << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result.size() << std::endl;
  ASSERT_EQ(count, 2);

  count = GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query, target, LabelEqual2<QueryVertexHandle, 
                                TargetVertexHandle>,
                     LabelEqual2<  QueryEdgeHandle, 
                                  TargetEdgeHandle>, -1, match_result);

  for (size_t i = 0; i < match_result.size(); i++) {
    std::cout << "match " << i << std::endl;
    for (const auto& mapping : match_result[i]) {
      std::cout << " " << mapping.first->id() << " " << mapping.second->id()
                << std::endl;
    }
  }
  std::cout << "count: " << match_result.size() << std::endl;
  ASSERT_EQ(count, 2);
}

TEST(TestGUNDAM, VF2_1) {
  using namespace GUNDAM;

  using G1 =
    Graph<SetVertexIDType<uint32_t>, 
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

  TestVF2_1<G1, G1>();
  TestVF2_1<G1, G2>();
  TestVF2_1<G1, G3>();
  TestVF2_1<G2, G1>();
  TestVF2_1<G2, G2>();
  TestVF2_1<G2, G3>();
  TestVF2_1<G3, G1>();
  TestVF2_1<G3, G2>();
  TestVF2_1<G3, G3>();
  TestVF2_1<LG, G1>();
  TestVF2_1<LG, G2>();
  TestVF2_1<LG, G3>();
  TestVF2_1<LG, LG>();
  TestVF2_1<SG, G1>();
  TestVF2_1<SG, G2>();
  TestVF2_1<SG, G3>();
  TestVF2_1<SG, LG>();
  TestVF2_1<SSG, G1>();
  TestVF2_1<SSG, G2>();
  TestVF2_1<SSG, G3>();
  TestVF2_1<SSG, LG>();
}

template <typename Handle1, typename Handle2>
bool LabelEqualCustom(const Handle1& a, 
                      const Handle2& b) {
  return *(uint32_t*)&a->label() == static_cast<uint32_t>(stoi(b->label()));
}

template <class QueryGraph, class TargetGraph>
void TestVF2_2() {
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

  std::vector<std::map<typename GUNDAM::VertexHandle< QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>>
      match_result;
  int count = VF2<MatchSemantics::kIsomorphism>(
      query, target,
      LabelEqualCustom<typename GUNDAM::VertexHandle< QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>,
      LabelEqualCustom<typename GUNDAM::  EdgeHandle< QueryGraph>::type,
                       typename GUNDAM::  EdgeHandle<TargetGraph>::type>,
      -1, match_result);

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

TEST(TestGUNDAM, VF2_2) {
  using namespace GUNDAM;

  using QG1
   = Graph<SetVertexIDType<uint32_t>, 
           SetVertexLabelType<uint32_t>,
           SetEdgeIDType<uint32_t>, 
           SetEdgeLabelType<uint32_t>>;

  using TG1
   = Graph<SetVertexIDType<uint64_t>, 
           SetVertexLabelType<std::string>,
           SetEdgeIDType<uint64_t>, 
           SetEdgeLabelType<std::string>>;

  using QG2
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using TG2
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<std::string>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<std::string>,
           SetEdgeAttributeKeyType<std::string>>;

  using QG3
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using TG3
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<std::string>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<std::string>,
           SetEdgeAttributeKeyType<std::string>>;

  using QueryGraph = LargeGraph<uint32_t, Label<uint32_t>, std::string,
                                uint32_t, Label<uint32_t>, std::string>;

  using TargetGraph = LargeGraph<uint64_t, std::string, std::string, 
                                 uint64_t, std::string, std::string>;

  TestVF2_2<QG1, TG1>();
  TestVF2_2<QG2, TG2>();
  TestVF2_2<QG3, TG3>();
  TestVF2_2<QueryGraph, TargetGraph>();
}

template <class  QueryGraph, 
          class TargetGraph>
void TestVF2_3() {
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
  int count = VF2<MatchSemantics::kIsomorphism>(
      query, target,
      LabelEqualCustom<typename GUNDAM::VertexHandle< QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>,
      LabelEqualCustom<typename GUNDAM::  EdgeHandle< QueryGraph>::type,
                       typename GUNDAM::  EdgeHandle<TargetGraph>::type>,
      -1, match_result);

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

TEST(TestGUNDAM, VF2_3) {
  using namespace GUNDAM;

  using QG1 =
    Graph<SetVertexIDType<uint32_t>, 
          SetVertexLabelType<Label<uint32_t>>,
          SetEdgeIDType<uint32_t>, 
          SetEdgeLabelType<Label<uint32_t>>>;

  using TG1 =
    Graph<SetVertexIDType<uint64_t>, 
          SetVertexLabelType<std::string>,
          SetEdgeIDType<uint64_t>, 
          SetEdgeLabelType<std::string>>;

  using QG2
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<Label<uint32_t>>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<Label<uint32_t>>,
           SetEdgeAttributeKeyType<std::string>>;

  using TG2
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<std::string>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<std::string>,
           SetEdgeAttributeKeyType<std::string>>;

  using QG3
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<Label<uint32_t>>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<Label<uint32_t>>,
           SetEdgeAttributeKeyType<std::string>>;

  using TG3
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<std::string>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<std::string>,
           SetEdgeAttributeKeyType<std::string>>;

  using QueryGraph = LargeGraph<uint32_t, Label<uint32_t>, std::string,
                                uint32_t, Label<uint32_t>, std::string>;

  using TargetGraph = LargeGraph<uint64_t, std::string, std::string, 
                                 uint64_t, std::string, std::string>;

  TestVF2_3<QG1, TG1>();
  TestVF2_3<QG2, TG2>();
  TestVF2_3<QG3, TG3>();
  TestVF2_3<QueryGraph, TargetGraph>();
}

template <class QueryGraph, 
          class TargetGraph>
void TestVF2Speed1(int times_outer, int times_inner) {
  using namespace GUNDAM;

   QueryGraph  query;
  TargetGraph target;

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

  std::vector<std::map<typename GUNDAM::VertexHandle< QueryGraph>::type,
                       typename GUNDAM::VertexHandle<TargetGraph>::type>>
      match_result1;

  std::vector<std::vector<std::pair<typename GUNDAM::VertexHandle< QueryGraph>::type,
                                    typename GUNDAM::VertexHandle<TargetGraph>::type>>>
      match_result2;

  uint64_t start, end;

  for (int j = 0; j < times_outer; j++) {
    std::cout << "         Default: ";
    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count = VF2(query, target, -1, match_result1);

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << end - start << " ms" << std::endl;

    std::cout << "       Recursive: ";
    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int max_result = -1;
      match_result1.clear();

      int count = _vf2::VF2_Recursive<MatchSemantics::kIsomorphism>(
          query, target,
          _vf2::LabelEqual<typename GUNDAM::VertexHandle< QueryGraph>::type,
                           typename GUNDAM::VertexHandle<TargetGraph>::type>(),
          _vf2::LabelEqual<typename GUNDAM::  EdgeHandle< QueryGraph>::type,
                           typename GUNDAM::  EdgeHandle<TargetGraph>::type>(),
          std::bind(
              _vf2::MatchCallbackSaveResult<
                  typename GUNDAM::VertexHandle< QueryGraph>::type,
                  typename GUNDAM::VertexHandle<TargetGraph>::type,
                  std::vector<std::map<typename GUNDAM::VertexHandle< QueryGraph>::type,
                                       typename GUNDAM::VertexHandle<TargetGraph>::type>>>,
              std::placeholders::_1, &max_result, &match_result1));

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << end - start << " ms" << std::endl;

    std::cout << "    NonRecursive: ";
    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int max_result = -1;
      match_result1.clear();

      int count = _vf2::VF2_NonRecursive<MatchSemantics::kIsomorphism>(
          query, target,
          _vf2::LabelEqual<typename GUNDAM::VertexHandle< QueryGraph>::type,
                           typename GUNDAM::VertexHandle<TargetGraph>::type>(),
          _vf2::LabelEqual<typename GUNDAM::  EdgeHandle< QueryGraph>::type,
                           typename GUNDAM::  EdgeHandle<TargetGraph>::type>(),
          std::bind(
              _vf2::MatchCallbackSaveResult<
                  typename GUNDAM::VertexHandle< QueryGraph>::type,
                  typename GUNDAM::VertexHandle<TargetGraph>::type,
                  std::vector<std::map<typename GUNDAM::VertexHandle< QueryGraph>::type,
                                       typename GUNDAM::VertexHandle<TargetGraph>::type>>>,
              std::placeholders::_1, &max_result, &match_result1));

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count = VF2<MatchSemantics::kIsomorphism>(query, target, 20);

      ASSERT_EQ(20, count);
    }
    end = GetTime();
    std::cout << "           Limit: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count =
          VF2<MatchSemantics::kIsomorphism>(query, target, -1, match_result2);

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "   Vector Result: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int max_result = -1;
      match_result1.clear();

      int count = VF2<MatchSemantics::kIsomorphism>(
          query, target,
          std::bind(
              _vf2::MatchCallbackSaveResult1<
                  typename GUNDAM::VertexHandle< QueryGraph>::type,
                  typename GUNDAM::VertexHandle<TargetGraph>::type,
                  std::vector<std::map<typename GUNDAM::VertexHandle< QueryGraph>::type,
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

      int count = VF2<MatchSemantics::kIsomorphism>(
          query, target,
          std::bind(_vf2::MatchCallbackSaveResult2<
                        typename GUNDAM::VertexHandle< QueryGraph>::type,
                        typename GUNDAM::VertexHandle<TargetGraph>::type,
                        std::vector<std::vector<
                            std::pair<typename GUNDAM::VertexHandle< QueryGraph>::type,
                                      typename GUNDAM::VertexHandle<TargetGraph>::type>>>>,
                    std::placeholders::_1, &max_result, &match_result2));

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "Custom Callback2: " << end - start << " ms" << std::endl;

    start = GetTime();
    for (int i = 0; i < times_inner; i++) {
      int count = VF2<MatchSemantics::kIsomorphism>(
          query, target,
          LabelEqual2<typename GUNDAM::VertexHandle< QueryGraph>::type,
                      typename GUNDAM::VertexHandle<TargetGraph>::type>,
          LabelEqual2<typename GUNDAM::  EdgeHandle< QueryGraph>::type,
                      typename GUNDAM::  EdgeHandle<TargetGraph>::type>,
          -1, match_result1);

      ASSERT_EQ(40, count);
    }
    end = GetTime();
    std::cout << "  Custom Compare: " << end - start << " ms" << std::endl;

    std::cout << std::endl;
  }
}

TEST(TestGUNDAM, VF2_Speed_1) {
  using namespace GUNDAM;

  using QG1 =
    Graph<SetVertexIDType<uint32_t>, 
          SetVertexLabelType<uint32_t>,
          SetEdgeIDType<uint32_t>, 
          SetEdgeLabelType<uint32_t>>;

  using TG1 =
    Graph<SetVertexIDType<uint64_t>, 
          SetVertexLabelType<uint32_t>,
          SetEdgeIDType<uint64_t>, 
          SetEdgeLabelType<uint32_t>>;

  using QG2
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using TG2
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Map>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using QG3
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using TG3
   = Graph<SetVertexIDType<uint64_t>,
           SetVertexAttributeStoreType<AttributeType::kGrouped>,
           SetVertexLabelType<uint32_t>,
           SetVertexLabelContainerType<ContainerType::Vector>,
           SetVertexIDContainerType<ContainerType::Vector>,
           SetVertexPtrContainerType<ContainerType::Vector>,
           SetEdgeLabelContainerType<ContainerType::Vector>,
           SetVertexAttributeKeyType<std::string>,
           SetEdgeIDType<uint64_t>,
           SetEdgeAttributeStoreType<AttributeType::kGrouped>,
           SetEdgeLabelType<uint32_t>,
           SetEdgeAttributeKeyType<std::string>>;

  using QLG = LargeGraph<uint32_t, uint32_t, std::string, 
                         uint32_t, uint32_t, std::string>;

  using QSG = SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;

  using QSSG = SimpleSmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;

  using TLG = LargeGraph<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                         std::string>;

  TestVF2Speed1<QG1, TG1>(1, 10000);
  TestVF2Speed1<QG2, TG2>(1, 10000);
  TestVF2Speed1<QG3, TG3>(1, 10000);
  TestVF2Speed1<QLG, TLG>(1, 10000);
  TestVF2Speed1<QSG, TLG>(1, 10000);
  TestVF2Speed1<QSSG, TLG>(1, 10000);
}