#include <cstdint>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
//#include "gundam/graph_type/graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/simple_small_graph.h"

template <class GraphType>
void TestBuildSimpleGraph1(GraphType& g) {
  bool res;

  typename GUNDAM::VertexHandle<GraphType>::type v;
  std::tie(v, res) = g.AddVertex(1, 10);
  ASSERT_TRUE(res);
  ASSERT_TRUE(v);
  ASSERT_EQ(1, v->id());
  ASSERT_EQ(10, v->label());

  ASSERT_TRUE(g.AddVertex(2, 20).second);
  ASSERT_TRUE(g.AddVertex(3, 30).second);
  ASSERT_TRUE(g.AddVertex(4, 40).second); 
  ASSERT_TRUE(g.AddVertex(5, 20).second);
  ASSERT_TRUE(g.AddVertex(6, 30).second);
  ASSERT_TRUE(g.AddVertex(7, 40).second);
  ASSERT_TRUE(g.AddVertex(8, 30).second);
  ASSERT_TRUE(g.AddVertex(9, 40).second);  
  ASSERT_TRUE(g.AddVertex(10, 40).second);

  std::tie(v, res) = g.AddVertex(4, 400);
  ASSERT_FALSE(res);
  ASSERT_TRUE(v);
  ASSERT_EQ(4, v->id());
  ASSERT_EQ(40, v->label());  

  ASSERT_EQ(10, g.CountVertex());

  typename GUNDAM::EdgeHandle<GraphType>::type e;
  std::tie(e, res) = g.AddEdge(1, 1, 100, 1);
  ASSERT_TRUE(res);
  ASSERT_TRUE(e);
  ASSERT_EQ(1, e->id());
  ASSERT_EQ(100, e->label());
  ASSERT_EQ(1, e->src_id());
  ASSERT_EQ(1, e->dst_id());
  ASSERT_EQ(1, e->src_handle()->id());
  ASSERT_EQ(1, e->dst_handle()->id());

  std::tie(e, res) = g.AddEdge(2, 5, 200, 2);
  ASSERT_TRUE(res);
  ASSERT_TRUE(e);
  ASSERT_EQ(2, e->id());
  ASSERT_EQ(200, e->label());
  ASSERT_EQ(2, e->src_id());
  ASSERT_EQ(5, e->dst_id());
  ASSERT_EQ(2, e->src_handle()->id());
  ASSERT_EQ(5, e->dst_handle()->id());  

  ASSERT_TRUE(g.AddEdge(5, 2, 200, 3).second);
  ASSERT_TRUE(g.AddEdge(3, 6, 300, 4).second);
  ASSERT_TRUE(g.AddEdge(6, 8, 300, 5).second);
  ASSERT_TRUE(g.AddEdge(8, 3, 300, 6).second);
  ASSERT_TRUE(g.AddEdge(4, 7, 400, 7).second);
  ASSERT_TRUE(g.AddEdge(7, 9, 400, 8).second);
  ASSERT_TRUE(g.AddEdge(9, 10, 400, 9).second);
  ASSERT_TRUE(g.AddEdge(10, 4, 400, 10).second);
 
  ASSERT_EQ(10, g.CountEdge());
}

template <class GraphType>
void TestBuildSimpleGraph2(GraphType& g) {
  bool res;

  typename GUNDAM::VertexHandle<GraphType>::type v;

  auto ret1 = g.AddVertex(12, 1);
  ASSERT_FALSE(ret1.second);
  ASSERT_FALSE(ret1.first);

  std::tie(v, res) = g.AddVertex(0, 1);
  ASSERT_FALSE(res);
  ASSERT_FALSE(v);

  ASSERT_EQ(10, g.CountVertex());

  typename GUNDAM::EdgeHandle<GraphType>::type e;
  std::tie(e, res) = g.AddEdge(1, 1, 1000, 0);
  ASSERT_FALSE(res);
  ASSERT_FALSE(e);

  auto ret2 = g.AddEdge(1, 2, 1000, 20);
  ASSERT_FALSE(ret2.second);
  ASSERT_FALSE(ret2.first);
  
  ASSERT_EQ(10, g.CountEdge());
}

template <class GraphType>
void PrintGraph1(const GraphType& g) {
  int count;

  typename GraphType::VertexConstIterator it_v;
  ASSERT_TRUE(it_v.IsDone());

  std::cout << "Vertex" << std::endl;
  count = 0;
  for (it_v = g.VertexBegin(); !it_v.IsDone(); it_v++) {
    std::cout << it_v->id() << " " << it_v->label() << std::endl;
    ++count;
  }
  std::cout << "Count: " << count << std::endl;
  ASSERT_EQ(count, g.CountVertex());

  typename GraphType::EdgeConstIterator it_e;
  ASSERT_TRUE(it_e.IsDone());

  std::cout << "Edge" << std::endl;
  count = 0;
  for (it_e = g.EdgeBegin(); !it_e.IsDone(); it_e++) {
    std::cout << it_e->id() << " " << it_e->label() << " " << it_e->src_id()
              << " " << it_e->dst_id() << std::endl;
    ++count;
  }
  std::cout << "Count: " << count << std::endl << std::endl;
  ASSERT_EQ(count, g.CountEdge());
}

template <class GraphType>
void PrintGraph2(GraphType& g) {
  int count;

  typename GraphType::VertexIterator it_v;
  ASSERT_TRUE(it_v.IsDone());

  std::cout << "Vertex" << std::endl;
  count = 0;
  for (it_v = g.VertexBegin(); !it_v.IsDone(); ++it_v) {
    std::cout << it_v->id() << " " << it_v->label() << std::endl;
    ++count;
  }
  std::cout << "Count: " << count << std::endl;
  ASSERT_EQ(count, g.CountVertex());

  typename GraphType::EdgeIterator it_e;
  ASSERT_TRUE(it_e.IsDone());

  std::cout << "Edge" << std::endl;
  count = 0;
  for (it_e = g.EdgeBegin(); !it_e.IsDone(); ++it_e) {
    std::cout << it_e->id() << " " << it_e->label() << " " << it_e->src_id()
              << " " << it_e->dst_id() << std::endl;
    ++count;
  }
  std::cout << "Count: " << count << std::endl << std::endl;
  ASSERT_EQ(count, g.CountEdge());
}

template <class GraphType>
void PrintGraphOutEdges(const GraphType& g) {
  int count_v = 0;
  int count_e = 0;

  for (auto it_v = g.VertexBegin(); !it_v.IsDone(); it_v++) {
    int count_out = 0;
    for (auto it_e = it_v->OutEdgeBegin(); !it_e.IsDone(); it_e++) {
      std::cout << it_e->id() << " " << it_e->label() << " " << it_e->src_id() << " " << it_e->dst_id() << std::endl;

      ASSERT_EQ(it_e->src_handle()->id(), it_e->src_id());
      ASSERT_EQ(it_e->dst_handle()->id(), it_e->dst_id());

      ++count_out;
    }
    ASSERT_EQ(it_v->CountOutEdge(), count_out);

    count_e += count_out;
    ++count_v;
  }
  
  std::cout << "  Edge: " << count_e << std::endl << std::endl;

  ASSERT_EQ(count_v, g.CountVertex());
  ASSERT_EQ(count_e, g.CountEdge());
}

template <class GraphType>
void PrintGraphInEdges(GraphType& g) {
  int count_v = 0;
  int count_e = 0;

  for (auto it_v = g.VertexBegin(); !it_v.IsDone(); ++it_v) {
    int count_out = 0;
    for (auto it_e = it_v->InEdgeBegin(); !it_e.IsDone(); ++it_e) {
      std::cout << it_e->id() << " " << it_e->label() << " " << it_e->src_id()
                << " " << it_e->dst_id() << std::endl;

      ASSERT_EQ(it_e->src_handle()->id(), it_e->src_id());
      ASSERT_EQ(it_e->dst_handle()->id(), it_e->dst_id());

      ++count_out;
    }
    ASSERT_EQ(it_v->CountOutEdge(), count_out);

    count_e += count_out;
    ++count_v;
  }

  std::cout << "  Edge: " << count_e << std::endl << std::endl;

  ASSERT_EQ(count_v, g.CountVertex());
  ASSERT_EQ(count_e, g.CountEdge());
}

template <class GraphType>
void TestSimpleGraphPtrIterator(GraphType &g) {
  typename GUNDAM::VertexHandle<GraphType>::type v1 = g.FindVertex(5);
  // typename GraphType::VertexConstPtr cv1 = g.FindConstVertex(5);
  ////typename GUNDAM::VertexHandle<GraphType>::type v2 = g.FindConstVertex(5); // wrong
  //typename GraphType::VertexConstPtr cv2 = g.FindVertex(5); // ok
}

template <class GraphType>
void TestSimpleGraph1() {
  GraphType g;

  TestBuildSimpleGraph1<GraphType>(g);

  PrintGraph1(g);
  PrintGraph2(g);
  PrintGraphOutEdges(g);
  PrintGraphInEdges(g);

  std::cout << std::endl;

  TestSimpleGraphPtrIterator(g);
}

TEST(TestGUNDAM, TestSimpleGraph1) {
  using namespace GUNDAM;
  
  using LG = LargeGraph<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                        std::string>;

  using SG = SmallGraph<uint64_t, uint32_t, uint64_t, uint32_t>;

  using SSG = SimpleSmallGraph<uint64_t, uint32_t, uint64_t, uint32_t>;

  TestSimpleGraph1<LG>();
  TestSimpleGraph1<SG>();
  TestSimpleGraph1<SSG>();
}
