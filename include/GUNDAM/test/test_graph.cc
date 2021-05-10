#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "gundam/graph_type/small_graph.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/graph.h"

template<class GraphType>
void TestAddVertexAddEdge(GraphType& g){

  // AddVertex
  auto res1 = g.AddVertex(1, "AAA");
  ASSERT_TRUE(res1.second);
  ASSERT_TRUE(res1.first);
  ASSERT_EQ(1, res1.first->id());
  ASSERT_EQ("AAA", res1.first->label());

  res1 = g.AddVertex(2, std::string{"AAA"});
  ASSERT_TRUE(res1.second);
  ASSERT_TRUE(res1.first);
  ASSERT_EQ(2, res1.first->id());
  ASSERT_EQ("AAA", res1.first->label());

  res1 = g.AddVertex(3, std::string("BBB"));
  ASSERT_TRUE(res1.second);
  ASSERT_TRUE(res1.first);
  ASSERT_EQ(3, res1.first->id());
  ASSERT_EQ("BBB", res1.first->label());

  res1 = g.AddVertex(4, "BBB");
  ASSERT_TRUE(res1.second);
  ASSERT_TRUE(res1.first);
  ASSERT_EQ(4, res1.first->id());
  ASSERT_EQ("BBB", res1.first->label());

  res1 = g.AddVertex(1, "AAA");
  ASSERT_FALSE(res1.second);
  ASSERT_EQ(1, res1.first->id());
  ASSERT_EQ("AAA", res1.first->label());

  res1 = g.AddVertex(3, "AAA");
  ASSERT_FALSE(res1.second);
  ASSERT_EQ(3, res1.first->id());
  ASSERT_TRUE(res1.first->label() == "BBB");

  // AddEdge
  auto res2 = g.AddEdge(1, 2, "aaa", 1);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 1);
  ASSERT_EQ(res2.first->label(), "aaa");

  res2 = g.AddEdge(2, 3, "aaa", 2);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 2);
  ASSERT_EQ(res2.first->label(), "aaa");

  res2 = g.AddEdge(3, 4, "aaa", 3);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 3);
  ASSERT_EQ(res2.first->label(), "aaa");

  res2 = g.AddEdge(4, 1, "aaa", 4);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 4);
  ASSERT_EQ(res2.first->label(), "aaa");

  res2 = g.AddEdge(2, 1, "bbb", 5);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 5);
  ASSERT_EQ(res2.first->label(), "bbb");

  res2 = g.AddEdge(3, 2, "bbb", 6);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 6);
  ASSERT_EQ(res2.first->label(), "bbb");

  res2 = g.AddEdge(4, 3, "bbb", 7);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 7);
  ASSERT_EQ(res2.first->label(), "bbb");

  res2 = g.AddEdge(1, 4, "bbb", 8);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 8);
  ASSERT_EQ(res2.first->label(), "bbb");

  res2 = g.AddEdge(1, 2, "ccc", 9);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 9);
  ASSERT_EQ(res2.first->label(), "ccc");

  res2 = g.AddEdge(3, 4, "ccc", 10);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 10);
  ASSERT_EQ(res2.first->label(), "ccc");

  res2 = g.AddEdge(3, 2, "ccc", 11);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 11);
  ASSERT_EQ(res2.first->label(), "ccc");

  res2 = g.AddEdge(4, 1, "ccc", 12);
  ASSERT_TRUE(res2.second);
  ASSERT_TRUE(res2.first);
  ASSERT_EQ(res2.first->id(), 12);
  ASSERT_EQ(res2.first->label(), "ccc");
}

template<class GraphType>
void TestConstGraph(const GraphType& g){

  using VertexHandle = typename GUNDAM::VertexHandle<decltype(g)>::type;
  using   EdgeHandle = typename GUNDAM::  EdgeHandle<decltype(g)>::type;

  auto v = g.FindVertex(0);
  ASSERT_FALSE(v);

  auto v1 = g.FindVertex(1);
  ASSERT_TRUE(v1);
  ASSERT_EQ(v1->id(), 1);

  auto v2 = g.FindVertex(2);
  ASSERT_TRUE(v2);
  ASSERT_EQ(v2->id(), 2);
  
  //auto e1 = v1->FindOutEdge("aaa", v2, 1);
  //ASSERT_TRUE(e1);
  //ASSERT_EQ(e1->id(), 1);
  //ASSERT_EQ(e1->label(), "aaa");

  //auto e1b = v2->FindInEdge(1);
  //ASSERT_TRUE(e1b);
  //ASSERT_EQ(e1b->id(), 1);
  //ASSERT_EQ(e1b->label(), "aaa");

  //ASSERT_EQ(e1, e1b);

  int count = 0;
  for (auto v_it = g.VertexBegin(); 
           !v_it.IsDone(); 
          ++v_it) {
    VertexHandle vertex_const_ptr = v_it;
    ASSERT_EQ(vertex_const_ptr->id(), v_it->id());
    ++count;
  }
  ASSERT_EQ(count, 4);
  
  count = 0;
  for (auto v_it = g.VertexBegin(); 
           !v_it.IsDone(); 
          ++v_it) {
    VertexHandle vertex_const_ptr = v_it;
    ASSERT_EQ(vertex_const_ptr->id(), v_it->id());
    ++count;
  }
  ASSERT_EQ(count, 4);

  count = 0;
  for (auto e_out_it = v1->OutEdgeBegin(); 
           !e_out_it.IsDone(); 
          ++e_out_it) {
    EdgeHandle edge_const_ptr = e_out_it;
    ASSERT_EQ(edge_const_ptr->const_src_handle(), v1);
    ++count;
  }
  ASSERT_EQ(count, 3);

  count = 0;
  for (auto e_out_it = v1->OutEdgeBegin(); 
           !e_out_it.IsDone(); 
          ++e_out_it) {
    EdgeHandle edge_const_ptr = e_out_it;
    ASSERT_EQ(edge_const_ptr->const_src_handle(), v1);
    ++count;
  }
  ASSERT_EQ(count, 3);

  count = 0;
  for (auto e_in_it = v1->InEdgeBegin(); 
           !e_in_it.IsDone(); 
          ++e_in_it) {
    EdgeHandle edge_const_ptr = e_in_it;
    ASSERT_EQ(edge_const_ptr->const_dst_handle(), v1);
    ++count;
  }
  ASSERT_EQ(count, 3);

  count = 0;
  for (auto e_in_it = v1->InEdgeBegin(); 
           !e_in_it.IsDone(); 
          ++e_in_it) {
    EdgeHandle edge_const_ptr = e_in_it;
    ASSERT_EQ(edge_const_ptr->const_dst_handle(), v1);
    ++count;
  }
  ASSERT_EQ(count, 3);
  return;
}

template<typename GraphType>
bool TestGraphSame(const GraphType& g1,
                   const GraphType& g2){
  using VertexLabelType = typename GraphType
                                ::VertexType
                                 ::LabelType;
  using VertexIDType = typename GraphType
                             ::VertexType
                                 ::IDType;
  using EdgeLabelType = typename GraphType
                                ::EdgeType
                               ::LabelType;
  using EdgeIDType = typename GraphType
                             ::EdgeType
                               ::IDType;
  if (g1.CountVertex() != g2.CountVertex()){
    std::cout<<"in-equivalent vertex number not same"<<std::endl;
    return false;
  }
  // test vertex is same
  for (auto vertex_it = g1.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    auto vertex_ptr = g2.FindVertex(vertex_it->id());
    if (!vertex_ptr){
      std::cout<<"vertex: "<<vertex_ptr->id()
               <<" cannot be found in g2"<<std::endl;
      return false;
    }
    if (vertex_ptr->label() != vertex_it->label()) {
      // same id has different label
      return false;
    }
  }

  std::set<std::tuple<VertexIDType, //  src_id
                      VertexIDType, //  dst_id
                     EdgeLabelType, // edge_label
                        EdgeIDType>> g1_edge_set;

  // test edge is same 
  for (auto vertex_it = g1.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    for (auto edge_it = vertex_it->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      auto ret = g1_edge_set.emplace(edge_it->const_src_handle()->id(),
                                     edge_it->const_dst_handle()->id(),
                                     edge_it->label(),
                                     edge_it->id());
      // should have been added successfully
      // e.g. should not have duplicated edge
      assert(ret.second);
    }
  }

  for (auto vertex_it = g2.VertexBegin();
           !vertex_it.IsDone();
            vertex_it++){
    for (auto edge_it = vertex_it->OutEdgeBegin();
             !edge_it.IsDone();
              edge_it++) {
      auto temp_edge = std::make_tuple(
                            edge_it->const_src_handle()->id(),
                            edge_it->const_dst_handle()->id(),
                            edge_it->label(),
                            edge_it->id()); 
      auto it = g1_edge_set.find(temp_edge);
      if (it == g1_edge_set.cend()){
        // g1 does not have this edge
        std::cout<<"edge: "<<std::endl
                 <<"src_id: "<<edge_it->const_src_handle()->id()<<std::endl
                 <<"dst_id: "<<edge_it->const_dst_handle()->id()<<std::endl
                 <<"edge_label: "<<edge_it->label()<<std::endl
                 <<"edge_id: "<<edge_it->id()<<std::endl
                 <<" cannot be found in g1"<<std::endl;
        return false;
      }
      // g1 has this edge
      g1_edge_set.erase(it);
    }
  }
  if (!g1_edge_set.empty()){
    // g1 has edge that is not contained in g2
    for (const auto& remained_g1_edge : g1_edge_set){
      std::cout<<"edge: "<<std::endl
               <<"src_id: "<<std::get<0>(remained_g1_edge)<<std::endl
               <<"dst_id: "<<std::get<1>(remained_g1_edge)<<std::endl
               <<"edge_label: "<<std::get<2>(remained_g1_edge)<<std::endl
               <<"edge_id: "<<std::get<3>(remained_g1_edge)<<std::endl
               <<" cannot be found in g2"<<std::endl;
    }
    return false;
  }
  return true;
}

template <class GraphType>
void TestGraphVertexEdge() {
  using namespace GUNDAM;                 

  using VertexHandle = typename GUNDAM::VertexHandle<GraphType>::type;
  using   EdgeHandle = typename GUNDAM::  EdgeHandle<GraphType>::type;

  GraphType g1;

  TestAddVertexAddEdge(g1);

  VertexHandle v = g1.FindVertex(0);
  ASSERT_FALSE(v);

  VertexHandle v1 = g1.FindVertex(1);
  ASSERT_TRUE(v1);
  ASSERT_EQ(v1->id(), 1);

  VertexHandle v2 = g1.FindVertex(2);
  ASSERT_TRUE(v2);
  ASSERT_EQ(v2->id(), 2);
  
  //auto e1 = v1->FindOutEdge("aaa", v2, 1);
  //ASSERT_TRUE(e1);
  //ASSERT_EQ(e1->id(), 1);
  //ASSERT_EQ(e1->label(), "aaa");

  //auto e1b = v2->FindInEdge(1);
  //ASSERT_TRUE(e1b);
  //ASSERT_EQ(e1b->id(), 1);
  //ASSERT_EQ(e1b->label(), "aaa");

  //ASSERT_EQ(e1, e1b);

  // int count = 0;
  // for (auto v_it = g1.VertexCBegin(); 
  //          !v_it.IsDone(); 
  //         ++v_it) {
  //   VertexHandle vertex_const_ptr = v_it;
  //   ASSERT_EQ(vertex_const_ptr->id(), v_it->id());
  //   ++count;
  // }
  // ASSERT_EQ(count, 4);
  
  int count = 0;
  for (auto v_it = g1.VertexBegin(); 
           !v_it.IsDone(); 
          ++v_it) {
    VertexHandle vertex_ptr = v_it;
    ASSERT_EQ(vertex_ptr->id(), v_it->id());
    // VertexHandle vertex_const_ptr = v_it;
    // ASSERT_EQ(vertex_const_ptr->id(), v_it->id());
    ++count;
  }
  ASSERT_EQ(count, 4);

  // count = 0;
  // for (auto e_out_it = v1->OutEdgeCBegin(); 
  //          !e_out_it.IsDone(); 
  //         ++e_out_it) {
  //   EdgeHandle edge_const_ptr = e_out_it;
  //   ASSERT_EQ(edge_const_ptr->const_src_handle(), v1);
  //   ++count;
  // }
  // ASSERT_EQ(count, 3);

  count = 0;
  for (auto e_out_it = v1->OutEdgeBegin(); 
           !e_out_it.IsDone(); 
          ++e_out_it) {
    EdgeHandle edge_ptr = e_out_it;
    ASSERT_EQ(edge_ptr->src_handle(), v1);
    // EdgeHandle edge_const_ptr = e_out_it;
    // ASSERT_EQ(edge_const_ptr->const_src_handle(), v1);
    ++count;
  }
  ASSERT_EQ(count, 3);

  // count = 0;
  // for (auto e_in_it = v1->InEdgeCBegin(); 
  //          !e_in_it.IsDone(); 
  //         ++e_in_it) {
  //   // EdgeHandle edge_const_ptr = e_in_it;
  //   // ASSERT_EQ(edge_const_ptr->const_dst_handle(), v1);
  //   ++count;
  // }
  // ASSERT_EQ(count, 3);

  count = 0;
  for (auto e_in_it = v1->InEdgeBegin(); 
           !e_in_it.IsDone(); 
          ++e_in_it) {
    EdgeHandle edge_ptr = e_in_it;
    ASSERT_EQ(edge_ptr->dst_handle(), v1);
    // EdgeHandle edge_const_ptr = e_in_it;
    // ASSERT_EQ(edge_const_ptr->const_dst_handle(), v1);
    ++count;
  }
  ASSERT_EQ(count, 3);

  TestConstGraph(g1);
}

template <class GraphType>
void TestIndex() {
  using namespace GUNDAM;                 

  using VertexHandle = typename GUNDAM::VertexHandle<GraphType>::type;
  using   EdgeHandle = typename GUNDAM::  EdgeHandle<GraphType>::type;

  GraphType g1;

  TestAddVertexAddEdge(g1);

  // int count = 0;
  // for (auto v_it = g1.VertexCBegin("AAA"); 
  //          !v_it.IsDone(); 
  //         ++v_it) {
  //   VertexHandle vertex_const_ptr = v_it;
  //   ASSERT_EQ(vertex_const_ptr->id(),    v_it->id());
  //   ASSERT_EQ(vertex_const_ptr->label(), v_it->label());
  //   ASSERT_EQ(vertex_const_ptr->label(), "AAA");
  //   ++count;
  // }
  // ASSERT_EQ(count, 2);
  
  // count = 0;
  // for (auto v_it = g1.VertexCBegin("BBB"); 
  //          !v_it.IsDone(); 
  //         ++v_it) {
  //   VertexHandle vertex_const_ptr = v_it;
  //   ASSERT_EQ(vertex_const_ptr->id(),    v_it->id());
  //   ASSERT_EQ(vertex_const_ptr->label(), v_it->label());
  //   ASSERT_EQ(vertex_const_ptr->label(), "BBB");
  //   ++count;
  // }
  // ASSERT_EQ(count, 2);

  // count = 0;
  // for (auto v_it = g1.VertexCBegin("zzz"); 
  //          !v_it.IsDone(); 
  //         ++v_it) {
  //   VertexHandle vertex_const_ptr = v_it;
  //   ASSERT_EQ(vertex_const_ptr->id(),    v_it->id());
  //   ASSERT_EQ(vertex_const_ptr->label(), v_it->label());
  //   ASSERT_EQ(vertex_const_ptr->label(), "zzz");
  //   ++count;
  // }
  // ASSERT_EQ(count, 0);

  int count = 0;
  for (auto v_it = g1.VertexBegin("AAA"); 
           !v_it.IsDone(); 
          ++v_it) {
    VertexHandle vertex_ptr = v_it;
    ASSERT_EQ(vertex_ptr->id(), v_it->id());
    ASSERT_EQ(vertex_ptr->label(), v_it->label());
    ASSERT_EQ(vertex_ptr->label(), "AAA");
    ++count;
  }
  ASSERT_EQ(count, 2);
  
  count = 0;
  for (auto v_it = g1.VertexBegin("BBB"); 
           !v_it.IsDone(); 
          ++v_it) {
    VertexHandle vertex_ptr = v_it;
    ASSERT_EQ(vertex_ptr->id(), v_it->id());
    ASSERT_EQ(vertex_ptr->label(), v_it->label());
    ASSERT_EQ(vertex_ptr->label(), "BBB");
    ++count;
  }
  ASSERT_EQ(count, 2);
  
  count = 0;
  for (auto v_it = g1.VertexBegin("zzz"); 
           !v_it.IsDone(); 
          ++v_it) {
    VertexHandle vertex_ptr = v_it;
    ASSERT_EQ(vertex_ptr->id(), v_it->id());
    ASSERT_EQ(vertex_ptr->label(), v_it->label());
    ASSERT_EQ(vertex_ptr->label(), "zzz");
    ++count;
  }
  ASSERT_EQ(count, 0);

  auto v1 = g1.FindVertex(1);
  ASSERT_TRUE(v1);
  ASSERT_EQ(v1->id(), 1);

  count = 0;
  for (auto e_out_it = v1->OutEdgeBegin("aaa");
           !e_out_it.IsDone(); e_out_it++){
    EdgeHandle edge_ptr = e_out_it;
    ASSERT_EQ(edge_ptr->src_handle(), v1);
    ASSERT_EQ(edge_ptr->label(), "aaa");
    ++count;
  }
  ASSERT_EQ(count, 1);

  count = 0;
  for (auto e_in_it = v1->InEdgeBegin("aaa");
           !e_in_it.IsDone(); e_in_it++){
    EdgeHandle edge_ptr = e_in_it;
    ASSERT_EQ(edge_ptr->dst_handle(), v1);
    ASSERT_EQ(edge_ptr->label(), "aaa");
    ++count;
  }
  ASSERT_EQ(count, 1);

  count = 0;
  for (auto e_out_it = v1->OutEdgeBegin("ZZZ");
           !e_out_it.IsDone(); e_out_it++){
    EdgeHandle edge_ptr = e_out_it;
    ASSERT_EQ(edge_ptr->src_handle(), v1);
    ASSERT_EQ(edge_ptr->label(), "ZZZ");
    ++count;
  }
  ASSERT_EQ(count, 0);

  count = 0;
  for (auto e_in_it = v1->InEdgeBegin("ZZZ");
           !e_in_it.IsDone(); e_in_it++){
    EdgeHandle edge_ptr = e_in_it;
    ASSERT_EQ(edge_ptr->dst_handle(), v1);
    ASSERT_EQ(edge_ptr->label(), "ZZZ");
    ++count;
  }
  ASSERT_EQ(count, 0);

  return;
}

template <class GraphType>
void TestSerialize() {

  GraphType g1;

  TestAddVertexAddEdge(g1);
  
  GraphType g12(g1);
  auto vertex_id = g12.VertexBegin()->id();
  auto ret = g12.EraseVertex(vertex_id);
  ASSERT_TRUE(ret);

  const GraphType& g12_const_ref = g12;

  std::string out_str;
  out_str<<g1<<g12_const_ref;

  std::cout<<"#Serialized graph#"<<std::endl;
  std::cout<<out_str<<std::endl;
  
  GraphType g2;
  out_str>>g2;

  ASSERT_TRUE(TestGraphSame(g1, g2));
  
  GraphType g22;
  std::cout<<"#Serialized graph#"<<std::endl;
  std::cout<<out_str<<std::endl;
  out_str>>g22;
  ASSERT_EQ(out_str, "");
  ASSERT_TRUE(TestGraphSame(g12, g22));
}

TEST(TestGUNDAM, TestGraphVertexEdge) {
  using namespace GUNDAM;

  using G1 = LargeGraph<uint32_t, std::string, std::string, 
                        uint64_t, std::string, std::string>;

  using G2 = LargeGraph2<uint32_t, std::string, std::string, 
                         uint64_t, std::string, std::string>;  

  using G3 = SmallGraph<uint32_t, std::string, 
                        uint64_t, std::string>;

  using G4 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<std::string>,
                   SetVertexAttributeKeyType<std::string>, 
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<std::string>, 
                   SetEdgeAttributeKeyType<std::string>>;

  using G5 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<std::string>,
                   SetVertexAttributeKeyType<std::string>,
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<std::string>, 
                   SetEdgeAttributeKeyType<std::string>, 
                   SetVertexLabelContainerType<GUNDAM::ContainerType::Map>,
                   SetVertexIDContainerType<GUNDAM::ContainerType::Map>>;

  using G6 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<std::string>,
                   SetVertexAttributeKeyType<std::string>,
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<std::string>, 
                   SetEdgeAttributeKeyType<std::string>, 
                   SetVertexPtrContainerType<GUNDAM::ContainerType::Map>,
                   SetEdgeLabelContainerType<GUNDAM::ContainerType::Map>>;

  using G7 = Graph<SetVertexIDType<uint32_t>, 
                   SetVertexLabelType<std::string>,
                   SetVertexAttributeKeyType<std::string>,
                   SetEdgeIDType<uint64_t>,
                   SetEdgeLabelType<std::string>, 
                   SetEdgeAttributeKeyType<std::string>, 
                   SetVertexLabelContainerType<GUNDAM::ContainerType::Map>,
                   SetVertexIDContainerType<GUNDAM::ContainerType::Map>, 
                   SetVertexPtrContainerType<GUNDAM::ContainerType::Map>,
                   SetEdgeLabelContainerType<GUNDAM::ContainerType::Map>>;

  TestGraphVertexEdge<G1>();
  TestGraphVertexEdge<G2>();
  TestGraphVertexEdge<G3>();
  TestGraphVertexEdge<G4>();
  TestGraphVertexEdge<G5>();
  TestGraphVertexEdge<G6>();
  TestGraphVertexEdge<G7>();

  TestSerialize<G1>();
  TestSerialize<G2>();
  TestSerialize<G3>();

  TestIndex<G1>();
  TestIndex<G2>();
  TestIndex<G4>();
  TestIndex<G5>();
  TestIndex<G6>();
  TestIndex<G7>();
}