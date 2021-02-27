#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/algorithm/dfs.h"

#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/graph.h"

template <class GraphType>
void TestDfs() {
  GraphType g;
  // 1 -> 2 -> 4
  // |  / |  / |
  // V /  V /  V
  // 3 -> 5 -> 7
  // |  / |  / |
  // V /  V /  V
  // 6 -> 8 -> 9

  // label denotes the distance 

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

  using VertexPtrType      = typename GraphType::VertexPtr;
  using VertexConstPtrType = typename GraphType::VertexConstPtr;


  auto my_callback = [](const VertexPtrType& vertex_ptr, 
                               const size_t& vertex_idx) -> bool{
    // do nothing
    return true;
  };
  
  auto src_ptr = g.FindVertex(1);

  auto ret = GUNDAM::Dfs(g, src_ptr);
  ASSERT_EQ(ret, g.CountVertex());

  ret = GUNDAM::Dfs(g, src_ptr, my_callback);
  ASSERT_EQ(ret, g.CountVertex());

  // ret = GUNDAM::Dfs<false>(g, src_ptr, my_callback);
  // ASSERT_EQ(ret, g.CountVertex());

  // auto my_callback2 = [&distance_tested](
  //                        const VertexPtrType& vertex_ptr, 
  //                        const size_t& current_distance) -> bool{
  //   if (vertex_ptr->label() != 4 - current_distance) {
  //     distance_tested = false;
  //   }
  //   return true;
  // };

  // distance_tested = true;
  // auto src_ptr2 = g.FindVertex(9);
  // ret = GUNDAM::Dfs<true>(g, src_ptr2, my_callback2);
  // ASSERT_TRUE(distance_tested);
  // ASSERT_EQ(ret, g.CountVertex());

  // distance_tested = true;
  // ret = GUNDAM::Dfs<false>(g, src_ptr2, my_callback2);
  // ASSERT_TRUE(distance_tested);
  // ASSERT_EQ(ret, 1);

  // distance_tested = true;
  // ret = GUNDAM::Dfs<false>(g, src_ptr2, my_callback);
  // ASSERT_TRUE(!distance_tested);
  // ASSERT_EQ(ret, 1);

  // int counter = 0;
  // const int kVertexLimit = 5;

  // auto my_callback3 
  //   = [&distance_tested,
  //      &counter,
  //      &kVertexLimit](const VertexPtrType& vertex_ptr, 
  //                     const size_t& current_distance) -> bool{
  //   if (vertex_ptr->label() != current_distance) {
  //     distance_tested = false;
  //   }
  //   counter++;
  //   if (counter == kVertexLimit)
  //     return false;
  //   return true;
  // };
  
  // distance_tested = true;
  // ret = GUNDAM::Dfs(g, src_ptr, my_callback3);
  // ASSERT_TRUE(distance_tested);
  // ASSERT_EQ(ret, kVertexLimit);

  // now_furthest = 0;

  // auto my_const_ptr_callback = [&now_furthest](VertexConstPtrType& vertex_ptr){
  //   ASSERT_TRUE(vertex_ptr->label() == now_furthest
  //            || vertex_ptr->label() == now_furthest + 1);
  //   now_furthest = vertex_ptr->label();
  //   return;
  // };
  // const auto& const_ref_g = g;
  // auto const_src_ptr = const_ref_g.FindVertex(1);
  // ret = GUNDAM::Dfs(g, const_src_ptr, my_const_ptr_callback);
  // assert(ret == g.CountVertex());
  return;
}

TEST(TestGUNDAM, TestDfs) {
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

  TestDfs<G1>();
  TestDfs<G2>();
  TestDfs<G3>();
  TestDfs<G4>();
  TestDfs<G5>();
  TestDfs<G6>();
  TestDfs<G7>();
}