#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/graph_type/large_graph.h"

TEST(TestGUNDAM, TestGraphLabel) {
  using namespace GUNDAM;

  LargeGraph<uint64_t, std::string, std::string, 
             uint64_t, int32_t, std::string>
      g1;

  // AddVertex
  auto res1 = g1.AddVertex(1, "AAA");
  ASSERT_TRUE(res1.second);
  ASSERT_TRUE(res1.first);
  ASSERT_EQ(res1.first->id(), 1);
  ASSERT_EQ(res1.first->label(), "AAA");

  res1 = g1.AddVertex(2, "AAA");
  ASSERT_TRUE(res1.second);
  ASSERT_TRUE(res1.first);
  ASSERT_EQ(res1.first->id(), 2);
  ASSERT_EQ(res1.first->label(), "AAA");

  // AddEdge
  using EdgeLabelType = decltype(g1)::EdgeType::LabelType;
  auto res2 = g1.AddEdge(2, 1, EdgeLabelType(1), 1);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 1);

  // ASSERT_EQ(res2.first->label(), Label<std::string>("ccc"));
  std::cout << "Edge 1's label: " << res2.first->label()
            << std::endl;
}

TEST(TestGUNDAM, GraphEdge) {
  using namespace GUNDAM;

  using GraphType1 = LargeGraph<uint32_t, std::string, std::string, uint64_t,
                                std::string, std::string>;            

  GraphType1 g1;

  typename GUNDAM::VertexHandle<GraphType1>::type v1, v3;

  bool res_bool;
  std::tie(v1, res_bool) = g1.AddVertex(1, "AAA");
  ASSERT_TRUE(res_bool);

  ASSERT_TRUE(g1.AddVertex(2, std::string("AAA")).second);

  auto res1 = g1.AddVertex(3, "BBB");
  ASSERT_TRUE(res1.second);
  v3 = res1.first;
  ASSERT_TRUE(v3);

  typename GUNDAM::EdgeHandle<GraphType1>::type e1, e2;

  auto res2 = g1.AddEdge(1, 2, "a", 1);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(1, res2.first->id());
  ASSERT_EQ(1, res2.first->const_src_handle()->id());
  ASSERT_EQ(2, res2.first->const_dst_handle()->id());

  std::tie(e2, res_bool) = g1.AddEdge(2, 3, "b", 2);
  ASSERT_TRUE(res_bool);
  ASSERT_TRUE(e2);
  ASSERT_EQ(2, e2->id());
  ASSERT_EQ(2, e2->const_src_handle()->id());
  ASSERT_EQ(3, e2->const_dst_handle()->id());

  ASSERT_TRUE(g1.AddEdge(1, 3, "b", 3).second);

  e1 = g1.FindEdge(1);
  ASSERT_TRUE(e1);

  auto e_it1 = v1->OutEdgeBegin();  
  auto e_it_const1 = v1->OutEdgeBegin();

  typename GUNDAM::EdgeHandle<GraphType1>::type e1_1 = e_it1;
  // GraphType1::EdgeConstPtr e1_const1 = e_it_const1;

  // ASSERT_EQ(e1->id(), e1_const1->id());
  // ASSERT_EQ(e1->const_src_handle(), e1_const1->const_src_handle());
  // ASSERT_EQ(e1->const_dst_handle(), e1_const1->const_dst_handle());
  // ASSERT_EQ(e1->label(), e1_const1->label());
  // ASSERT_EQ(e1, e1_const1);

  // auto e_it2 = v1->OutEdgeBegin("a");
  // GraphType1::EdgeConstPtr e1_const2{e_it2};
  // ASSERT_EQ(e1->id(), e1_const2->id());
  // ASSERT_EQ(e1->const_src_handle(), e1_const2->const_src_handle());
  // ASSERT_EQ(e1->const_dst_handle(), e1_const2->const_dst_handle());
  // ASSERT_EQ("a", e1_const2->label());
  // ASSERT_EQ(e1, e1_const2);

  typename GUNDAM::EdgeHandle<GraphType1>::type e1a = e1;
  e1a = e1;
  // GraphType1::EdgeConstPtr e1b = e1_const1;
  // e1b = e1_const2;

  res2 = g1.AddEdge(1, 1, "sss", 4);
  ASSERT_TRUE(res2.second);
  ASSERT_EQ(res2.first->const_src_handle(), res2.first->const_dst_handle());
}