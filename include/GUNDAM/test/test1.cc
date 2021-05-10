#include <cstdint>
#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/type_getter/vertex_handle.h"

#include "gundam/graph_type/graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/small_graph.h"

template <class GraphType>
void TestBuildGraph(GraphType& g) {
  bool res;

  typename GUNDAM::VertexHandle<GraphType>::type v1;
  std::tie(v1, res) = g.AddVertex(1, 1);
  ASSERT_TRUE(res);
  ASSERT_TRUE(v1);
  ASSERT_EQ(1, v1->id());
  ASSERT_EQ(1, v1->label());

  ASSERT_TRUE(g.AddVertex(2, 2).second);
  ASSERT_TRUE(g.AddVertex(3, 3).second);
  ASSERT_TRUE(g.AddVertex(4, 4).second);
  ASSERT_TRUE(g.AddVertex(5, 5).second);

  typename GUNDAM::VertexHandle<GraphType>::type v5;
  std::tie(v5, res) = g.AddVertex(5, 100);
  ASSERT_FALSE(res);
  ASSERT_TRUE(v5);
  ASSERT_EQ(5, v5->id());
  ASSERT_EQ(5, v5->label());

  ASSERT_TRUE(g.AddVertex(41, 1).second);
  ASSERT_TRUE(g.AddVertex(31, 1).second);
  ASSERT_TRUE(g.AddVertex(32, 2).second);
  ASSERT_TRUE(g.AddVertex(21, 1).second);
  ASSERT_TRUE(g.AddVertex(22, 2).second);
  ASSERT_TRUE(g.AddVertex(23, 3).second);
  ASSERT_TRUE(g.AddVertex(11, 1).second);
  ASSERT_TRUE(g.AddVertex(12, 2).second);
  ASSERT_TRUE(g.AddVertex(13, 3).second);
  ASSERT_TRUE(g.AddVertex(14, 4).second);

  ASSERT_EQ(15, g.CountVertex());

  ASSERT_TRUE(g.AddEdge(3, 13, 1, 1).second);
  ASSERT_TRUE(g.AddEdge(3, 23, 1, 2).second);
  ASSERT_TRUE(g.AddEdge(13, 3, 1, 3).second);
  ASSERT_TRUE(g.AddEdge(13, 23, 1, 4).second);
  ASSERT_TRUE(g.AddEdge(23, 3, 1, 5).second);
  ASSERT_TRUE(g.AddEdge(23, 13, 1, 6).second);

  ASSERT_TRUE(g.AddEdge(4, 14, 1, 7).second);
  ASSERT_TRUE(g.AddEdge(14, 4, 1, 8).second);

  ASSERT_TRUE(g.AddEdge(5, 5, 1, 9).second);

  ASSERT_TRUE(g.AddEdge(3, 4, 2, 10).second);
  ASSERT_TRUE(g.AddEdge(3, 14, 2, 11).second);
  ASSERT_TRUE(g.AddEdge(13, 4, 2, 12).second);
  ASSERT_TRUE(g.AddEdge(13, 14, 2, 13).second);
  ASSERT_TRUE(g.AddEdge(23, 4, 2, 14).second);
  ASSERT_TRUE(g.AddEdge(23, 14, 2, 15).second);

  // auto ret = g.AddEdge(2, 22, 1, 1);
  // ASSERT_FALSE(ret.second);
  // ASSERT_TRUE(ret.first);
  // ASSERT_EQ(3, ret.first->src());
  // ASSERT_EQ(13, ret.first->dst());

  ASSERT_EQ(15, g.CountEdge());
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
  std::cout << "Count: " << count << std::endl;
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
  std::cout << "Count: " << count << std::endl;
  ASSERT_EQ(count, g.CountEdge());
}

template <class GraphType>
void PrintGraph3(const GraphType& g) {
  int count_v = 0;
  int count_e = 0;

  for (auto it_v = g.VertexBegin(); !it_v.IsDone(); it_v++) {
    std::cout << "v - " << it_v->id() << " " << it_v->label() << std::endl;

    int count_out = 0;
    for (auto it_e = it_v->OutEdgeBegin(); !it_e.IsDone(); it_e++) {
      std::cout << "  e - " << it_e->id() << " " << it_e->label() << " "
                << it_e->src_id() << " " << it_e->dst_id() << std::endl;

      ASSERT_EQ(it_e->src_handle()->id(), it_e->src_id());
      ASSERT_EQ(it_e->dst_handle()->id(), it_e->dst_id());

      ++count_out;
    }
    ASSERT_EQ(it_v->CountOutEdge(), count_out);

    count_e += count_out;
    ++count_v;
  }

  std::cout << "Vertex: " << count_v << std::endl;
  std::cout << "  Edge: " << count_e << std::endl;

  ASSERT_EQ(count_v, g.CountVertex());
  ASSERT_EQ(count_e, g.CountEdge());
}

template <class GraphType>
void TestGraph1() {
  GraphType g;
  bool res;

  typename GUNDAM::VertexHandle<GraphType>::type v1;
  std::tie(v1, res) = g.AddVertex(1, 1);
  ASSERT_TRUE(res);
  ASSERT_TRUE(v1);
  ASSERT_EQ(1, v1->id());
  ASSERT_EQ(1, v1->label());

  ASSERT_TRUE(g.AddVertex(2, 2).second);
  ASSERT_TRUE(g.AddVertex(3, 3).second);
  ASSERT_TRUE(g.AddVertex(4, 4).second);
  ASSERT_TRUE(g.AddVertex(5, 5).second);

  typename GUNDAM::VertexHandle<GraphType>::type v5;
  std::tie(v5, res) = g.AddVertex(5, 100);
  ASSERT_FALSE(res);
  ASSERT_TRUE(v5);
  ASSERT_EQ(5, v5->id());
  ASSERT_EQ(5, v5->label());

  ASSERT_TRUE(g.AddVertex(41, 1).second);
  ASSERT_TRUE(g.AddVertex(31, 1).second);
  ASSERT_TRUE(g.AddVertex(32, 2).second);
  ASSERT_TRUE(g.AddVertex(21, 1).second);
  ASSERT_TRUE(g.AddVertex(22, 2).second);
  ASSERT_TRUE(g.AddVertex(23, 3).second);
  ASSERT_TRUE(g.AddVertex(11, 1).second);
  ASSERT_TRUE(g.AddVertex(12, 2).second);
  ASSERT_TRUE(g.AddVertex(13, 3).second);
  ASSERT_TRUE(g.AddVertex(14, 4).second);

  ASSERT_EQ(15, g.CountVertex());

  auto v1a = g.FindVertex(1);
  ASSERT_EQ(v1, v1a);

  typename GUNDAM::VertexHandle<GraphType>::type v2 = g.FindVertex(2);
  ASSERT_TRUE(v2);
  // typename GraphType::VertexConstPtr v2const = g.FindConstVertex(2);
  // ASSERT_TRUE(v2const);
  // ASSERT_EQ(v2, v2const);
  // ASSERT_EQ(2, v2const->id());
  // ASSERT_EQ(2, v2const->label());

  {
    typename GraphType::VertexIterator it;
    ASSERT_TRUE(it.IsDone());

    int count = 0;
    for (it = g.VertexBegin(); !it.IsDone(); ++it) {
      std::cout << it->id() << " " << it->label() << std::endl;
      ++count;
    }
    std::cout << std::endl;
    ASSERT_EQ(count, g.CountVertex());
  }

  {
    typename GraphType::VertexIterator it;
    ASSERT_TRUE(it.IsDone());

    int count = 0;
    for (it = g.VertexBegin(); !it.IsDone(); it++) {
      std::cout << it->id() << " " << it->label() << std::endl;
      ++count;
    }
    std::cout << std::endl;
    ASSERT_EQ(count, g.CountVertex());
  }

  //{
  //  GraphType::VertexConstIterator it;
  //  ASSERT_TRUE(it.IsDone());

  //  int count = 0;
  //  for (it = g.VertexBegin(1); !it.IsDone(); it++) {
  //    std::cout << it->id() << " " << it->label() << std::endl;
  //    ++count;
  //  }
  //  std::cout << std::endl;
  //  ASSERT_EQ(count, g.CountVertex(1));
  //}

  //{
  //  GraphType::VertexConstIterator it;
  //  ASSERT_TRUE(it.IsDone());

  //  int count = 0;
  //  for (it = g.VertexBegin(2); !it.IsDone(); it++) {
  //    std::cout << it->id() << " " << it->label() << std::endl;
  //    ++count;
  //  }
  //  std::cout << std::endl;
  //  ASSERT_EQ(count, g.CountVertex(2));
  //}
  
  auto [e, r] = g.AddEdge(3, 13, 1, 1);
  ASSERT_TRUE(e);
  ASSERT_TRUE(r);
  ASSERT_TRUE(g.AddEdge(3, 23, 1, 2).second);
  ASSERT_TRUE(g.AddEdge(13, 3, 1, 3).second);
  ASSERT_TRUE(g.AddEdge(13, 23, 1, 4).second);
  ASSERT_TRUE(g.AddEdge(23, 3, 1, 5).second);
  ASSERT_TRUE(g.AddEdge(23, 13, 1, 6).second);

  ASSERT_TRUE(g.AddEdge(4, 14, 1, 7).second);
  ASSERT_TRUE(g.AddEdge(14, 4, 1, 8).second);

  ASSERT_TRUE(g.AddEdge(5, 5, 1, 9).second);

  ASSERT_TRUE(g.AddEdge(3, 4, 2, 10).second);
  ASSERT_TRUE(g.AddEdge(3, 14, 2, 11).second);
  ASSERT_TRUE(g.AddEdge(13, 4, 2, 12).second);
  ASSERT_TRUE(g.AddEdge(13, 14, 2, 13).second);
  ASSERT_TRUE(g.AddEdge(23, 4, 2, 14).second);
  ASSERT_TRUE(g.AddEdge(23, 14, 2, 15).second);

  //ASSERT_EQ(15, g.CountEdge());

  // {
  //   typename GraphType::EdgeIterator it;
  //   ASSERT_TRUE(it.IsDone());

  //   int count = 0;
  //   for (it = g.EdgeBegin(); !it.IsDone(); ++it) {
  //     std::cout << it->id() << " " << it->label() << " " << it->src_handle()->id()
  //               << " " << it->dst_handle()->id() << std::endl;
  //     ++count;
  //   }
  //   std::cout << std::endl;
  //   ASSERT_EQ(count, g.CountEdge());
  // }

  // {
  //   typename GraphType::EdgeConstIterator it;
  //   ASSERT_TRUE(it.IsDone());

  //   int count = 0;
  //   for (it = g.EdgeBegin(); !it.IsDone(); it++) {
  //     std::cout << it->id() << " " << it->label() << " " << it->src_handle()->id()
  //               << " " << it->dst_handle()->id() << std::endl;
  //     ++count;
  //   }
  //   std::cout << std::endl;
  //   ASSERT_EQ(count, g.CountEdge());
  // }

  // g.EraseEdge(5);
  // g.EraseEdge(9);
  // g.EraseEdge(7);
  // ASSERT_EQ(15, g.CountVertex());
  //ASSERT_EQ(12, g.CountEdge());

  // g.EraseVertex(3);
  // ASSERT_EQ(14, g.CountVertex());
  //ASSERT_EQ(7, g.CountEdge());

  // g.EraseVertex(1);
  // g.EraseVertex(2);
  // g.EraseVertex(4);
  // g.EraseVertex(5);
  // ASSERT_EQ(10, g.CountVertex());
  //ASSERT_EQ(4, g.CountEdge());

  // {
  //   typename GraphType::VertexConstIterator it;
  //   ASSERT_TRUE(it.IsDone());

  //   int count = 0;
  //   for (it = g.VertexBegin(); !it.IsDone(); ++it) {
  //     std::cout << it->id() << " " << it->label() << std::endl;
  //     ++count;
  //   }
  //   std::cout << std::endl;
  //   ASSERT_EQ(count, g.CountVertex());
  // }

  // {
  //   typename GraphType::EdgeConstIterator it;
  //   ASSERT_TRUE(it.IsDone());

  //   int count = 0;
  //   for (it = g.EdgeBegin(); !it.IsDone(); ++it) {
  //     std::cout << it->id() << " " << it->label() << " " << it->src_handle()->id()
  //               << " " << it->dst_handle()->id() << std::endl;
  //     ++count;
  //   }
  //   ASSERT_EQ(count, g.CountEdge());
  // }

  // GraphType t1{g};
  // g.Clear();

  // GraphType t2{std::move(t1)};

  // g = t2;
}

template <class GraphType>
void TestGraph2() {
  GraphType g;

  TestBuildGraph<GraphType>(g);

  PrintGraph1(g);    

  GraphType g1{g};
  PrintGraph2(g1);

  GraphType g2;
  g2 = g1;
  PrintGraph3(g2);
}

TEST(TestGUNDAM, TestGraph1) {
  using namespace GUNDAM;

  using G0 =
     Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<uint32_t>,
           SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint64_t>,
           SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using G1 = LargeGraph<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                        std::string>;

  using G2 = SmallGraph<uint64_t, uint32_t, uint64_t, uint32_t>;

  TestGraph1<G0>();
  TestGraph1<G1>();
  // TestGraph1<G2>();
}

TEST(TestGUNDAM, TestGraph2) {
  using namespace GUNDAM;

  using G0 =
     Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<uint32_t>,
           SetVertexAttributeKeyType<std::string>, SetEdgeIDType<uint64_t>,
           SetEdgeLabelType<uint32_t>, SetEdgeAttributeKeyType<std::string>>;

  using G1 = LargeGraph2<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                        std::string>;

  using G2 = LargeGraph2<uint64_t, uint32_t, std::string, uint64_t, uint32_t,
                         std::string>;

  using G3 = SmallGraph<uint64_t, uint32_t, uint64_t, uint32_t>;

  // TestGraph2<G0>();
  TestGraph2<G1>();
  TestGraph2<G2>();
  //TestGraph2<G2>();
}
