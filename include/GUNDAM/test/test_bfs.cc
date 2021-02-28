#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/algorithm/bfs.h"

#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/graph.h"

template <class GraphType>
void TestBfs() {
  GraphType g0;

  // 1 -> 2 -> 4
  // |  / |  / |
  // V /  V /  V
  // 3 -> 5 -> 7
  // |  / |  / |
  // V /  V /  V
  // 6 -> 8 -> 9

  // label denotes the distance 

  // AddVertex
  auto res1 = g0.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(2, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(3, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(4, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(5, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(6, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(7, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(8, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g0.AddVertex(9, 4);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  auto res2 = g0.AddEdge(1, 2, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(1, 3, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g0.AddEdge(2, 3, 42, 3);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(3, 2, 42, 4);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g0.AddEdge(2, 4, 42, 5);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(2, 5, 42, 6);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(3, 5, 42, 7);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(3, 6, 42, 8);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g0.AddEdge(4, 5, 42, 9);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(5, 4, 42, 10);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(5, 6, 42, 11);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(6, 5, 42, 12);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g0.AddEdge(4, 7, 42, 13);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(5, 7, 42, 14);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(5, 8, 42, 15);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(6, 8, 42, 16);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g0.AddEdge(7, 8, 42, 17);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(8, 7, 42, 18);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g0.AddEdge(7, 9, 42, 19);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g0.AddEdge(8, 9, 42, 20);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  using VertexPtrType      = typename GraphType::VertexPtr;
  using VertexConstPtrType = typename GraphType::VertexConstPtr;

  auto my_callback0 = [](VertexPtrType vertex_ptr){
    return true;
  };

  bool distance_tested = true;

  auto my_callback = [&distance_tested](
                        VertexPtrType vertex_ptr, 
                        typename GraphType::VertexCounterType bfs_idx,
                        typename GraphType::VertexCounterType current_distance){
    if (vertex_ptr->label() != current_distance) {
      distance_tested = false;
    }
    return true;
  };
  
  auto src_ptr = g0.FindVertex(1);

  auto ret = GUNDAM::Bfs(g0, src_ptr);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::Bfs(g0, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);
  ret = GUNDAM::Bfs(g0, src_ptr, my_callback0);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::Bfs<false>(g0, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);
  ret = GUNDAM::Bfs<false>(g0, src_ptr, my_callback0);
  ASSERT_EQ(ret, 9);

  auto my_callback2 = [&distance_tested](
                         VertexPtrType vertex_ptr, 
                         typename GraphType::VertexCounterType bfs_idx,
                         typename GraphType::VertexCounterType current_distance){
    if (vertex_ptr->label() != 4 - current_distance) {
      distance_tested = false;
    }
    return true;
  };

  distance_tested = true;
  auto src_ptr2 = g0.FindVertex(9);
  ret = GUNDAM::Bfs<true>(g0, src_ptr2, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::Bfs<false>(g0, src_ptr2, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 1);

  distance_tested = true;
  ret = GUNDAM::Bfs<false>(g0, src_ptr2, my_callback);
  ASSERT_TRUE(!distance_tested);
  ASSERT_EQ(ret, 1);

  const int kVertexLimit = 5;

  auto my_callback3 
    = [&distance_tested,
       &kVertexLimit](VertexPtrType vertex_ptr, 
                      typename GraphType::VertexCounterType bfs_idx){
    if (bfs_idx == kVertexLimit)
      return false;
    return true;
  };

  distance_tested = true;
  ret = GUNDAM::Bfs(g0, src_ptr, my_callback3);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, kVertexLimit);


  GraphType g1;

  // 1 -> 2 -> 4
  // |    |    /\
  // V    V    |
  // 3 -> 5 <- 7
  // |    /\   /\
  // V    |    |
  // 6 <- 8 <- 9

  // label denotes the distance 

  // AddVertex
  res1 = g1.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(2, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(3, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(4, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(5, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(6, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(7, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(8, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  res1 = g1.AddVertex(9, 4);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  res2 = g1.AddEdge(1, 2, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(1, 3, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g1.AddEdge(2, 4, 42, 5);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(2, 5, 42, 6);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(3, 5, 42, 7);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(3, 6, 42, 8);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g1.AddEdge(7, 4, 42, 13);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(7, 5, 42, 14);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(8, 5, 42, 15);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(8, 6, 42, 16);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g1.AddEdge(9, 7, 42, 19);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g1.AddEdge(9, 8, 42, 20);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  using VertexPtrType      = typename GraphType::VertexPtr;
  using VertexConstPtrType = typename GraphType::VertexConstPtr;
  
  src_ptr = g1.FindVertex(1);
  ret = GUNDAM::Bfs(g1, src_ptr);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::Bfs(g1, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::Bfs<false>(g1, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::Bfs<true>(g1, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);

  src_ptr2 = g1.FindVertex(9);
  distance_tested = true;
  ret = GUNDAM::Bfs<true>(g1, src_ptr2, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::Bfs<false>(g1, src_ptr2, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::Bfs<false>(g1, src_ptr2, my_callback);
  ASSERT_TRUE(!distance_tested);
  ASSERT_EQ(ret, 6);
  
  distance_tested = true;
  ret = GUNDAM::Bfs(g1, src_ptr, my_callback3);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, kVertexLimit);

  // 1 -> 2    5 -> 6
  // /\   |    /\   |
  // |    V    |    V
  // 4 <- 3    8 <- 7

  GraphType g2;

  // AddVertex
  res1 = g2.AddVertex(1, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(2, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(3, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(4, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(5, 0);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(6, 1);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(7, 2);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);
  res1 = g2.AddVertex(8, 3);
  ASSERT_TRUE(res1.first);
  ASSERT_TRUE(res1.second);

  // AddEdge
  res2 = g2.AddEdge(1, 2, 42, 1);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g2.AddEdge(2, 3, 42, 2);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g2.AddEdge(3, 4, 42, 3);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g2.AddEdge(4, 1, 42, 4);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  res2 = g2.AddEdge(5, 6, 42, 5);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g2.AddEdge(6, 7, 42, 6);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g2.AddEdge(7, 8, 42, 7);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);
  res2 = g2.AddEdge(8, 5, 42, 8);
  ASSERT_TRUE(res2.first);
  ASSERT_TRUE(res2.second);

  distance_tested = true;
  src_ptr = g2.FindVertex(1);
  ret = GUNDAM::Bfs<false>(g2, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 4);

  distance_tested = true;
  src_ptr = g2.FindVertex(5);
  ret = GUNDAM::Bfs<false>(g2, src_ptr, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 4);
  return;
}

TEST(TestGUNDAM, TestBfs) {
  using namespace GUNDAM;

  using G1 = LargeGraph<uint32_t, uint32_t, std::string, 
                        uint64_t, uint32_t, std::string>;

  using G2 = LargeGraph2<uint32_t, uint32_t, std::string, 
                         uint64_t, uint32_t, std::string>;  

  using G3 = SmallGraph<uint32_t, uint32_t, 
                        uint64_t, uint32_t>;

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

  TestBfs<G1>();
  // TestBfs<G2>();
  // TestBfs<G3>();
  // TestBfs<G4>();
  // TestBfs<G5>();
  // TestBfs<G6>();
  // TestBfs<G7>();
}