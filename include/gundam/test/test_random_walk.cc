#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/tool/random_walk.h"

#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/graph.h"

#include "gundam/type_getter/vertex_handle.h"

template <class GraphType>
void TestRandomWalk() {
  GraphType g;
  // 1 -> 2 -> 4
  // |    |  / |
  // V    V /  V
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

  // res2 = g.AddEdge(2, 3, 42, 3);
  // ASSERT_TRUE(res2.first);
  // ASSERT_TRUE(res2.second);
  // res2 = g.AddEdge(3, 2, 42, 4);
  // ASSERT_TRUE(res2.first);
  // ASSERT_TRUE(res2.second);

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

  using VertexHandleType = typename GUNDAM::VertexHandle<GraphType>::type;

  std::set<VertexHandleType> visited_vertex_set;
  std::vector<VertexHandleType> visited_vertexes;

  auto add_visited_callback 
       = [&visited_vertex_set,
          &visited_vertexes](VertexHandleType vertex_handle) -> bool{
    // added into the visited_vertex_set
    auto [visited_vertex_it,
          visited_vertex_ret] 
        = visited_vertex_set.emplace(vertex_handle);
    if (visited_vertex_ret){
      // this vertex has not been visited
      visited_vertexes.emplace_back(vertex_handle);
    }
    return true;
  };
  
  auto src_handle = g.FindVertex(1);

  std::cout << "####  Random Walk 0  ####" << std::endl;
  auto ret = GUNDAM::RandomWalk(g, src_handle, 1, 1, add_visited_callback);
  ASSERT_EQ(1, ret);
  ASSERT_EQ(1, visited_vertex_set.size());
  ASSERT_TRUE(src_handle->id() == (*(visited_vertexes.begin()))->id());
  ASSERT_TRUE(src_handle       ==  *(visited_vertexes.begin()));

  visited_vertex_set.clear();
  std::cout << "####  Random Walk 1  ####" << std::endl;
  ret = GUNDAM::RandomWalk<false>(g, src_handle, 1, 10, add_visited_callback);
  ASSERT_EQ(2, ret);
  ASSERT_EQ(2, visited_vertex_set.size());
  ASSERT_TRUE(src_handle->id() == (*(visited_vertexes.begin()))->id());
  ASSERT_TRUE(src_handle       ==  *(visited_vertexes.begin()));

  visited_vertex_set.clear();
  std::cout << "####  Random Walk 2  ####" << std::endl;
  ret = GUNDAM::RandomWalk<true>(g, src_handle, 1, 3, add_visited_callback);
  ASSERT_LE(2, ret);
  ASSERT_EQ(3, visited_vertex_set.size());
  ASSERT_TRUE(src_handle->id() == (*(visited_vertexes.begin()))->id());
  ASSERT_TRUE(src_handle       ==  *(visited_vertexes.begin()));

  visited_vertex_set.clear();
  std::cout << "####  Random Walk 3  ####" << std::endl;
  ret = GUNDAM::RandomWalk<true>(g, src_handle, 1000, 9, add_visited_callback);
  ASSERT_LE(9, ret);
  ASSERT_EQ(9, visited_vertex_set.size());
  return;
}

TEST(TestGUNDAM, TestRandomWalk) {
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

  std::cout << "##############" << std::endl
            << "##    G1    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G1>();
  std::cout << "##############" << std::endl
            << "##    G2    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G2>();
  std::cout << "##############" << std::endl
            << "##    G3    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G3>();
  std::cout << "##############" << std::endl
            << "##    G4    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G4>();
  std::cout << "##############" << std::endl
            << "##    G5    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G5>();
  std::cout << "##############" << std::endl
            << "##    G6    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G6>();
  std::cout << "##############" << std::endl
            << "##    G7    ##" << std::endl
            << "##############" << std::endl;
  TestRandomWalk<G7>();
}