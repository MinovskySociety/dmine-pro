#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/tool/same_pattern.h"

#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/graph.h"

template <class GraphType>
void ConstructGraph0(GraphType& g){
  // 1 -> 2
  // /\   |
  // |    |
  // 3 <---

  // clear up
  g = GraphType();
  // AddVertex
  auto res1 = g.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(2, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(3, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  auto res2 = g.AddEdge(1, 2, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(2, 3, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 1, 42, 3);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
}

template <class GraphType>
void ConstructGraph1(GraphType& g){
  // 1 <- 2
  // |    /\
  // V    |
  // 3 ----

  // clear up
  g = GraphType();
  // AddVertex
  auto res1 = g.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(2, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(3, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  auto res2 = g.AddEdge(1, 3, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 2, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(2, 1, 42, 3);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
}

template <class GraphType>
void ConstructGraph2(GraphType& g){
  // 1 <- 2
  // |    
  // V     
  // 3     

  // clear up
  g = GraphType();
  // AddVertex
  auto res1 = g.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(2, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(3, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  auto res2 = g.AddEdge(1, 3, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(2, 1, 42, 3);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
}

template <class GraphType>
void ConstructGraph3(GraphType& g){
  // 1 -> 2 -> 4
  // |  / |  / |
  // V /  V /  V
  // 3 -> 5 -> 7
  // |  / |  / |
  // V /  V /  V
  // 6 -> 8 -> 9

  // clear up
  g = GraphType();
  // AddVertex
  auto res1 = g.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(2, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(3, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(4, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(5, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(6, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(7, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(8, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(9, 4);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  auto res2 = g.AddEdge(1, 2, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(1, 3, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(2, 3, 42, 3);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 2, 42, 4);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(2, 4, 42, 5);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(2, 5, 42, 6);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 5, 42, 7);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 6, 42, 8);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(4, 5, 42, 9);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(5, 4, 42, 10);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(5, 6, 42, 11);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(6, 5, 42, 12);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(4, 7, 42, 13);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(5, 7, 42, 14);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(5, 8, 42, 15);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(6, 8, 42, 16);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(7, 8, 42, 17);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(8, 7, 42, 18);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(7, 9, 42, 19);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(8, 9, 42, 20);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
}

template <class GraphType>
void ConstructGraph4(GraphType& g){
  // 1 -> 2 -> 4
  // |    |    |
  // V    V    V
  // 3 -> 5 -> 7
  // |    |    |
  // V    V    V
  // 6 -> 8 -> 9

  // clear up
  g = GraphType();
  // AddVertex
  auto res1 = g.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(2, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(3, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(4, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(5, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(6, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(7, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(8, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g.AddVertex(9, 4);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  auto res2 = g.AddEdge(1, 2, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(1, 3, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(2, 4, 42, 5);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(2, 5, 42, 6);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 5, 42, 7);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(3, 6, 42, 8);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(4, 7, 42, 13);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(5, 7, 42, 14);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(5, 8, 42, 15);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(6, 8, 42, 16);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g.AddEdge(7, 9, 42, 19);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g.AddEdge(8, 9, 42, 20);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
}

template <typename GraphType0,
          typename GraphType1>
void TestSamePattern() {

  GraphType0 g0;
  GraphType1 g1;

  ConstructGraph0(g0);
  ConstructGraph0(g1);
  ASSERT_TRUE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph0(g0);
  ConstructGraph1(g1);
  ASSERT_TRUE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph1(g0);
  ConstructGraph0(g1);
  ASSERT_TRUE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph1(g0);
  ConstructGraph1(g1);
  ASSERT_TRUE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph0(g0);
  ConstructGraph2(g1);
  ASSERT_FALSE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph2(g0);
  ConstructGraph0(g1);
  ASSERT_FALSE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph1(g0);
  ConstructGraph2(g1);
  ASSERT_FALSE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph2(g0);
  ConstructGraph1(g1);
  ASSERT_FALSE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph3(g0);
  ConstructGraph3(g1);
  ASSERT_TRUE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph3(g0);
  ConstructGraph4(g1);
  ASSERT_FALSE(GUNDAM::SamePattern(g0, g1));

  ConstructGraph4(g0);
  ConstructGraph3(g1);
  ASSERT_FALSE(GUNDAM::SamePattern(g0, g1));
}

TEST(TestGUNDAM, TestSamePattern) {
  using namespace GUNDAM;

  using G1 = LargeGraph<uint32_t, uint32_t, std::string, 
                        uint64_t, uint32_t, std::string>;

  using G2 = LargeGraph2<uint32_t, uint32_t, std::string, 
                         uint64_t, uint32_t, std::string>;  

  // using G3 = SmallGraph<uint32_t, uint32_t, 
  //                       uint64_t, uint32_t>;

  using G4 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<uint32_t>,
                   SetVertexAttributeKeyType<std::string>, 
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<uint32_t>, 
                   SetEdgeAttributeKeyType<std::string>>;

  using G5 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<uint32_t>,
                   SetVertexAttributeKeyType<std::string>,
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<uint32_t>, 
                   SetEdgeAttributeKeyType<std::string>, 
                   SetVertexLabelContainerType<GUNDAM::ContainerType::Map>,
                   SetVertexIDContainerType<GUNDAM::ContainerType::Map>>;

  using G6 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<uint32_t>,
                   SetVertexAttributeKeyType<std::string>,
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<uint32_t>, 
                   SetEdgeAttributeKeyType<std::string>, 
                   SetVertexPtrContainerType<GUNDAM::ContainerType::Map>,
                   SetEdgeLabelContainerType<GUNDAM::ContainerType::Map>>;

  using G7 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<uint32_t>,
                   SetVertexAttributeKeyType<std::string>,
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<uint32_t>, 
                   SetEdgeAttributeKeyType<std::string>, 
                   SetVertexLabelContainerType<GUNDAM::ContainerType::Map>,
                   SetVertexIDContainerType<GUNDAM::ContainerType::Map>, 
                   SetVertexPtrContainerType<GUNDAM::ContainerType::Map>,
                   SetEdgeLabelContainerType<GUNDAM::ContainerType::Map>>;

  TestSamePattern<G1, G1>();
  TestSamePattern<G1, G2>();
  // TestSamePattern<G1, G3>();
  TestSamePattern<G1, G4>();
  TestSamePattern<G1, G5>();
  TestSamePattern<G1, G6>();
  TestSamePattern<G1, G7>();

  TestSamePattern<G2, G1>();
  TestSamePattern<G2, G2>();
  // TestSamePattern<G2, G3>();
  TestSamePattern<G2, G4>();
  TestSamePattern<G2, G5>();
  TestSamePattern<G2, G6>();
  TestSamePattern<G2, G7>();

  // TestSamePattern<G3, G1>();
  // TestSamePattern<G3, G2>();
  // TestSamePattern<G3, G3>();
  // TestSamePattern<G3, G4>();
  // TestSamePattern<G3, G5>();
  // TestSamePattern<G3, G6>();
  // TestSamePattern<G3, G7>();

  TestSamePattern<G4, G1>();
  TestSamePattern<G4, G2>();
  // TestSamePattern<G4, G3>();
  TestSamePattern<G4, G4>();
  TestSamePattern<G4, G5>();
  TestSamePattern<G4, G6>();
  TestSamePattern<G4, G7>();

  TestSamePattern<G5, G1>();
  TestSamePattern<G5, G2>();
  // TestSamePattern<G5, G3>();
  TestSamePattern<G5, G4>();
  TestSamePattern<G5, G5>();
  TestSamePattern<G5, G6>();
  TestSamePattern<G5, G7>();

  TestSamePattern<G6, G1>();
  TestSamePattern<G6, G2>();
  // TestSamePattern<G6, G3>();
  TestSamePattern<G6, G4>();
  TestSamePattern<G6, G5>();
  TestSamePattern<G6, G6>();
  TestSamePattern<G6, G7>();

  TestSamePattern<G7, G1>();
  TestSamePattern<G7, G2>();
  // TestSamePattern<G7, G3>();
  TestSamePattern<G7, G4>();
  TestSamePattern<G7, G5>();
  TestSamePattern<G7, G6>();
  TestSamePattern<G7, G7>();
}