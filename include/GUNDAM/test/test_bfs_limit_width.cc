#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/algorithm/bfs_limit_width.h"

#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/graph.h"
#include "gundam/type_getter/vertex_handle.h"

template <class GraphType>
void TestBfsLimitWidth() {
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

  auto my_callback0 
    = [](typename GUNDAM::VertexHandle<decltype(g0)>::type vertex_handle){
    return true;
  };

  bool distance_tested = true;

  auto my_callback = [&distance_tested](
      typename GUNDAM::VertexHandle<decltype(g0)>::type vertex_handle, 
      typename GraphType::VertexCounterType bfs_idx,
      typename GraphType::VertexCounterType current_distance){
    if (vertex_handle->label() != current_distance) {
      distance_tested = false;
    }
    return true;
  };
  
  typename GUNDAM::VertexHandle<decltype(g0)>::type 
    src_handle = g0.FindVertex(1);

  distance_tested = true;
  auto ret = GUNDAM::BfsLimitWidth(g0, src_handle, 1000);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth(g0, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);
  ret = GUNDAM::BfsLimitWidth(g0, src_handle, 1000, my_callback0);
  ASSERT_EQ(ret, 9);

  const auto& g0_const_ref = g0;

  auto my_callback0_const_ref
    = [](typename GUNDAM::VertexHandle<decltype(g0_const_ref)>::type vertex_handle){
    return true;
  };

  auto my_callback_const_ref = [&distance_tested](
      typename GUNDAM::VertexHandle<decltype(g0_const_ref)>::type vertex_handle, 
      typename GraphType::VertexCounterType bfs_idx,
      typename GraphType::VertexCounterType current_distance){
    if (vertex_handle->label() != current_distance) {
      distance_tested = false;
    }
    return true;
  };
  
  typename GUNDAM::VertexHandle<decltype(g0_const_ref)>::type 
       src_handle_g0_const_ref = g0_const_ref.FindVertex(1);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth(g0_const_ref,
                   src_handle_g0_const_ref, 1000);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth(g0_const_ref,
                   src_handle_g0_const_ref, 1000, 
                     my_callback_const_ref);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);
  ret = GUNDAM::BfsLimitWidth(g0_const_ref,
                   src_handle_g0_const_ref, 1000, 
                    my_callback0_const_ref);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<false>(g0, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);
  ret = GUNDAM::BfsLimitWidth<false>(g0, src_handle, 1000, my_callback0);
  ASSERT_EQ(ret, 9);

  auto my_callback2 = [&distance_tested](
      typename GUNDAM::VertexHandle<decltype(g0)>::type vertex_handle, 
      typename GraphType::VertexCounterType bfs_idx,
      typename GraphType::VertexCounterType current_distance){
    if (vertex_handle->label() != 4 - current_distance) {
      distance_tested = false;
    }
    return true;
  };

  distance_tested = true;
  typename GUNDAM::VertexHandle<decltype(g0)>::type 
    src_handle2 = g0.FindVertex(9);
  ret = GUNDAM::BfsLimitWidth<true>(g0, src_handle2, 1000, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<false>(g0, src_handle2, 1000, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 1);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<false>(g0, src_handle2, 1000, my_callback);
  ASSERT_TRUE(!distance_tested);
  ASSERT_EQ(ret, 1);

  const int kVertexLimit = 5;

  auto my_callback3 
    = [&distance_tested,
       &kVertexLimit](typename GUNDAM::VertexHandle<decltype(g0)>::type vertex_handle, 
                      typename GraphType::VertexCounterType bfs_idx){
    if (bfs_idx == kVertexLimit - 1)
      return false;
    return true;
  };

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth(g0, src_handle, 1000, my_callback3);
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
  
  src_handle = g1.FindVertex(1);
  ret = GUNDAM::BfsLimitWidth(g1, src_handle, 1000);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth(g1, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<false>(g1, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<true>(g1, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);

  src_handle2 = g1.FindVertex(9);
  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<true>(g1, src_handle2, 1000, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 9);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<false>(g1, src_handle2, 1000, my_callback2);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 6);

  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth<false>(g1, src_handle2, 1000, my_callback);
  ASSERT_TRUE(!distance_tested);
  ASSERT_EQ(ret, 6);
  
  distance_tested = true;
  ret = GUNDAM::BfsLimitWidth(g1, src_handle, 1000, my_callback3);
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
  src_handle = g2.FindVertex(1);
  ret = GUNDAM::BfsLimitWidth<false>(g2, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 4);

  distance_tested = true;
  src_handle = g2.FindVertex(5);
  ret = GUNDAM::BfsLimitWidth<false>(g2, src_handle, 1000, my_callback);
  ASSERT_TRUE(distance_tested);
  ASSERT_EQ(ret, 4);
  return;
}

TEST(TestGUNDAM, TestBfsLimitWidth) {
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

  TestBfsLimitWidth<G1>();
  TestBfsLimitWidth<G2>();
  TestBfsLimitWidth<G3>();
  TestBfsLimitWidth<G4>();
  TestBfsLimitWidth<G5>();
  TestBfsLimitWidth<G6>();
  TestBfsLimitWidth<G7>();
}