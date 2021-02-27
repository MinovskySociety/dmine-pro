#ifndef _SMALL_GRAPH_H
#define _SMALL_GRAPH_H

#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>

#include "gundam/component/container2.h"
#include "gundam/component/iterator2.h"
#include "gundam/component/pointer.h"
#include "gundam/data_type/datatype.h"

namespace GUNDAM {

template <class VertexIDType, class VertexLabelType, 
          class   EdgeIDType, class   EdgeLabelType>
class SmallGraph;
  
template <class VertexIDType, class VertexLabelType, 
          class   EdgeIDType, class   EdgeLabelType>
std::string& operator<<(std::string& out_string, 
      const SmallGraph<VertexIDType, 
                    VertexLabelType,
                         EdgeIDType,   
                      EdgeLabelType>& small_graph) {

  out_string = std::move(out_string) + " <Graph ";

  out_string = std::move(out_string) + "vertex";
  for (auto vertex_it = small_graph.VertexCBegin(); 
           !vertex_it.IsDone();
            vertex_it++) {
    out_string = std::move(out_string) 
               + " " + ToString(vertex_it->id()) 
               + " " + ToString(vertex_it->label());
  }
  out_string = std::move(out_string) + " edge";
  for (auto vertex_it = small_graph.VertexCBegin(); 
           !vertex_it.IsDone();
            vertex_it++) {
    for (auto edge_it = vertex_it->OutEdgeCBegin(); 
             !edge_it.IsDone();
              edge_it++) {
      out_string = std::move(out_string) 
                 + " " + ToString(edge_it->src_id()) 
                 + " " + ToString(edge_it->dst_id())
                 + " " + ToString(edge_it->label())
                 + " " + ToString(edge_it->id());
    }
  }
  out_string = std::move(out_string) + " >";
  return out_string;
}
  
template <class VertexIDType, class VertexLabelType, 
          class   EdgeIDType, class   EdgeLabelType>
std::string& operator>>(std::string& in_string, 
            SmallGraph<VertexIDType, 
                    VertexLabelType,
                         EdgeIDType,   
                      EdgeLabelType>& small_graph) {
  
  using namespace GUNDAM;

  std::stringstream ss;
  ss << in_string;

  std::string str;
  
  ss>>str;
  assert(str == "<Graph");

  ss>>str;
  assert(str == "vertex");

  small_graph.Clear();

  while (ss>>str){
    if (str == "edge")
      break;
    VertexIDType vertex_id
      = StringToDataType<VertexIDType>(str);
    ss>>str;
    VertexLabelType vertex_label 
      = StringToDataType<VertexLabelType>(str);
    small_graph.AddVertex(vertex_id,
                          vertex_label);
  }

  while (ss>>str){
    if (str == ">"){
      // end symbol
      break;
    }
    VertexIDType src_id
      = StringToDataType<VertexIDType>(str);
    ss>>str;
    VertexIDType dst_id
      = StringToDataType<VertexIDType>(str);
    ss>>str;
    EdgeLabelType edge_label
      = StringToDataType<EdgeLabelType>(str);
    ss>>str;
    EdgeIDType edge_id
      = StringToDataType<EdgeIDType>(str);
 
    small_graph.AddEdge(src_id,
                        dst_id,
                       edge_label,
                       edge_id);
  }
  getline(ss, in_string);
  if (ss.fail())
    in_string.clear();
  return in_string;
}

template <class VertexIDType, class VertexLabelType, class EdgeIDType,
          class EdgeLabelType>
class SmallGraph {
 private:
  class VertexData {
   public:
    VertexData(const VertexLabelType &label) : label_(label){};

    VertexLabelType label_;
    SortedVectorSet<EdgeIDType> out_edges_;
    SortedVectorSet<EdgeIDType> in_edges_;
  };

  class EdgeData {
   public:
    EdgeData(const EdgeLabelType &label, const VertexIDType &src,
             const VertexIDType &dst)
        : label_(label), src_(src), dst_(dst){};

    EdgeLabelType label_;
    VertexIDType src_;
    VertexIDType dst_;
  };

  using VertexContainer = SortedVectorDict<VertexIDType, VertexData>;
  using EdgeContainer = SortedVectorDict<EdgeIDType, EdgeData>;

  using VertexContent = std::pair<VertexIDType, VertexData>;
  using EdgeContent = std::pair<EdgeIDType, EdgeData>;

  template <bool is_const>
  class _Vertex;

  template <bool is_const>
  class _Edge;

  using Vertex = _Vertex<false>;

  using ConstVertex = _Vertex<true>;

  using Edge = _Edge<false>;

  using ConstEdge = _Edge<true>;

 public:
  using VertexPtr      = GPointer<false, Vertex, ConstVertex>;

  using VertexConstPtr = GPointer< true, Vertex, ConstVertex>;

  using EdgePtr = GPointer<false, Edge, ConstEdge>;

  using EdgeConstPtr = GPointer<true, Edge, ConstEdge>;

  using VertexIterator =
      GIterator2<false, SmallGraph, typename VertexContainer::iterator, 
                 Vertex,
                 VertexPtr>;

  using VertexConstIterator =
      GIterator2< true, SmallGraph, typename VertexContainer::const_iterator,
                 ConstVertex, 
                 VertexConstPtr>;

  using EdgeIterator =
      GIterator2<false, SmallGraph, typename EdgeContainer::iterator, Edge,
                 EdgePtr>;

  using EdgeConstIterator =
      GIterator2<true, SmallGraph, typename EdgeContainer::const_iterator,
                 ConstEdge, EdgeConstPtr>;

 private:
  template <bool is_const>
  class _Vertex {
   public:
    using GraphType = SmallGraph;

    using IDType = VertexIDType;

    using LabelType = VertexLabelType;

    using AttributeKeyType = std::string;

    using EdgeIterator =
        GIterator2<false, SmallGraph,
                   typename SortedVectorSet<EdgeIDType>::iterator, Edge,
                   EdgePtr>;

    using EdgeConstIterator =
        GIterator2<true, SmallGraph,
                   typename SortedVectorSet<EdgeIDType>::const_iterator,
                   ConstEdge, EdgeConstPtr>;

   private:
    friend GraphType;
    friend typename GraphType::VertexPtr;
    friend typename GraphType::VertexConstPtr;
    friend typename GraphType::VertexIterator;
    friend typename GraphType::VertexConstIterator;
    friend typename GraphType::Edge;
    friend typename GraphType::ConstEdge;
    friend typename GraphType::EdgePtr;
    friend typename GraphType::EdgeConstPtr;
    friend typename GraphType::EdgeIterator;
    friend typename GraphType::EdgeConstIterator;

    _Vertex() : graph_(nullptr){};

    _Vertex(typename std::conditional<is_const, const GraphType *,
                                      GraphType *>::type graph,
            const VertexContent &content)
        : graph_(graph), id_(content.first), label_(content.second.label_) {
      assert(graph_);
    }

    _Vertex(typename std::conditional<is_const, const GraphType *,
                                      GraphType *>::type graph,
            const VertexIDType &id)
        : graph_(graph), id_(id) {
      assert(graph_);
      const auto &data = graph_->vertices_.Find(id_)->second;
      label_ = data.label_;
    }

   public:
    const IDType &id() const {
      assert(HasValue());
      return id_;
    }

    const VertexLabelType &label() const {
      assert(HasValue());
      return label_;
    }

    size_t CountOutEdge() const {
      assert(HasValue());
      const auto &data = graph_->vertices_.Find(id_)->second;
      return data.out_edges_.Count();
    }

    size_t CountOutEdge(const EdgeLabelType &edge_label) const {
      size_t out_edge_count = 0;
      for (auto it = this->OutEdgeCBegin(); !it.IsDone(); it++) {
        if (it->label() == edge_label) out_edge_count++;
      }
      return out_edge_count;
    }

    size_t CountInEdge() const {
      assert(HasValue());
      const auto &data = graph_->vertices_.Find(id_)->second;
      return data.in_edges_.Count();
    }

    size_t CountInEdge(const EdgeLabelType &edge_label) const {
      size_t in_edge_count = 0;
      for (auto it = this->InEdgeCBegin(); !it.IsDone(); it++) {
        if (it->label() == edge_label) in_edge_count++;
      }
      return in_edge_count;
    }

    size_t CountOutVertex() const {
      std::set<VertexIDType> out_vertex_id_set;
      for (auto it = this->OutEdgeCBegin(); !it.IsDone(); it++) {
        out_vertex_id_set.insert(it->const_dst_ptr()->id());
      }
      return out_vertex_id_set.size();
    }
    size_t CountOutVertex(const EdgeLabelType &edge_label) const {
      std::set<VertexIDType> out_vertex_id_set;
      for (auto it = this->OutEdgeCBegin(); !it.IsDone(); it++) {
        if (it->label() == edge_label)
          out_vertex_id_set.insert(it->const_dst_ptr()->id());
      }
      return out_vertex_id_set.size();
    }
    size_t CountInVertex() const {
      std::set<VertexIDType> in_vertex_id_set;
      for (auto it = this->InEdgeCBegin(); !it.IsDone(); it++) {
        in_vertex_id_set.insert(it->const_src_ptr()->id());
      }
      return in_vertex_id_set.size();
    }
    size_t CountInVertex(const EdgeLabelType &edge_label) const {
      std::set<VertexIDType> in_vertex_id_set;
      for (auto it = this->InEdgeCBegin(); !it.IsDone(); it++) {
        if (it->label() == edge_label)
          in_vertex_id_set.insert(it->const_src_ptr()->id());
      }
      return in_vertex_id_set.size();
    }
    size_t CountVertex() const {
      std::set<VertexIDType> vertex_id_set;
      for (auto it = this->OutEdgeCBegin(); !it.IsDone(); it++) {
        vertex_id_set.insert(it->dst_ptr()->id());
      }
      for (auto it = this->InEdgeCBegin(); !it.IsDone(); it++) {
        vertex_id_set.insert(it->src_ptr()->id());
      }
      return vertex_id_set.size();
    }

    EdgeIterator OutEdgeBegin() {
      assert(HasValue());
      auto &data = graph_->vertices_.Find(id_)->second;
      return EdgeIterator(graph_, 
                          data.out_edges_.begin(),
                          data.out_edges_.end());
    }

    EdgeConstIterator OutEdgeBegin() const {
      return this->OutEdgeCBegin();
    }

    EdgeConstIterator OutEdgeCBegin() const {
      assert(HasValue());
      const auto &data = graph_->vertices_.Find(id_)->second;
      return EdgeConstIterator(graph_, data.out_edges_.cbegin(),
                               data.out_edges_.cend());
    }

    EdgeIterator InEdgeBegin() {
      assert(HasValue());
      auto &data = graph_->vertices_.Find(id_)->second;
      return EdgeIterator(graph_, data.in_edges_.begin(), data.in_edges_.end());
    }

    EdgeConstIterator InEdgeBegin() const {
      return this->InEdgeCBegin();
    }

    EdgeConstIterator InEdgeCBegin() const {
      assert(HasValue());
      const auto &data = graph_->vertices_.Find(id_)->second;
      return EdgeConstIterator(graph_, data.in_edges_.cbegin(),
                               data.in_edges_.cend());
    }

    bool operator==(const _Vertex &b) const {
      if (!graph_) {
        return !b.graph_;
      } else if (!b.graph_) {
        return false;
      } else {
        return id_ == b.id_;
      }
    }

    bool operator<(const _Vertex &b) const {
      if (!graph_ || !b.graph_) {
        return false;
      } else {
        return id_ < b.id_;
      }
    }

   private:
    void Reset() { graph_ = nullptr; }

    bool HasValue() const { return (graph_ != nullptr); }

    typename std::conditional<is_const, const GraphType *, GraphType *>::type
        graph_;
    VertexIDType id_;
    VertexLabelType label_;
  };

  template <bool is_const>
  class _Edge {
   public:
    using GraphType = SmallGraph;

    using IDType = EdgeIDType;

    using LabelType = EdgeLabelType;

    using AttributeKeyType = std::string;

   private:
    friend GraphType;
    friend typename GraphType::VertexPtr;
    friend typename GraphType::VertexConstPtr;
    friend typename GraphType::VertexIterator;
    friend typename GraphType::VertexConstIterator;
    friend typename GraphType::EdgePtr;
    friend typename GraphType::EdgeConstPtr;
    friend typename GraphType::EdgeIterator;
    friend typename GraphType::EdgeConstIterator;
    friend typename GraphType::Vertex;
    friend typename GraphType::ConstVertex;
    friend typename GraphType::Vertex::EdgeIterator;
    friend typename GraphType::Vertex::EdgeConstIterator;

    _Edge() : graph_(nullptr){};

    _Edge(typename std::conditional<is_const, const GraphType *,
                                    GraphType *>::type graph,
          const EdgeContent &content)
        : graph_(graph),
          id_(content.first),
          label_(content.second.label_),
          src_(content.second.src_),
          dst_(content.second.dst_) {
      assert(graph_);
    }

    _Edge(typename std::conditional<is_const, const GraphType *,
                                    GraphType *>::type graph,
          const EdgeIDType &id)
        : graph_(graph), id_(id) {
      assert(graph_);
      const auto &data = graph_->edges_.Find(id_)->second;
      label_ = data.label_;
      src_ = data.src_;
      dst_ = data.dst_;
    }

   public:
    const EdgeIDType &id() const {
      assert(HasValue());
      return id_;
    }

    const EdgeLabelType &label() const {
      assert(HasValue());
      return label_;
    }

    const VertexIDType &src_id() const {
      assert(HasValue());
      return src_;
    }

    const VertexIDType &dst_id() const {
      assert(HasValue());
      return dst_;
    }

    template <bool judge = !is_const,
              class = typename std::enable_if<judge>::type>
    VertexPtr src_ptr() {
      assert(HasValue());
      return VertexPtr(Vertex(graph_, src_));
    }

    VertexConstPtr src_ptr() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, src_));
    }

    VertexConstPtr const_src_ptr() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, src_));
    }

    template <bool judge = !is_const,
              class = typename std::enable_if<judge>::type>
    VertexPtr dst_ptr() {
      assert(HasValue());
      return VertexPtr(Vertex(graph_, dst_));
    }

    VertexConstPtr dst_ptr() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, dst_));
    }

    VertexConstPtr const_dst_ptr() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, dst_));
    }

    bool operator==(const _Edge &b) const {
      if (!graph_) {
        return !b.graph_;
      } else if (!b.graph_) {
        return false;
      } else {
        return id_ == b.id_;
      }
    }

    bool operator<(const _Edge &b) const {
      if (!graph_ || !b.graph_) {
        return false;
      } else {
        return id_ < b.id_;
      }
    }

   private:
    void Reset() { graph_ = nullptr; }

    bool HasValue() const { return (graph_ != nullptr); }

    typename std::conditional<is_const, const GraphType *, GraphType *>::type
        graph_;
    EdgeIDType id_;
    EdgeLabelType label_;
    VertexIDType src_;
    VertexIDType dst_;
  };

 public:
  using VertexType = Vertex;

  using EdgeType = Edge;

  static constexpr bool vertex_has_attribute = false;

  static constexpr bool edge_has_attribute = false;

  static constexpr bool is_mutable = true;

  SmallGraph() = default;

  SmallGraph(const SmallGraph &other) = default;

  SmallGraph(SmallGraph &&) = default;

  SmallGraph &operator=(const SmallGraph &other) = default;

  SmallGraph &operator=(SmallGraph &&) = default;

  ~SmallGraph() = default;

  size_t CountVertex() const { return vertices_.Count(); }

  std::pair<VertexPtr, bool> AddVertex(const VertexIDType &id,
                                       const VertexLabelType &label) {
    typename VertexContainer::iterator it;
    bool res;
    std::tie(it, res) = vertices_.Insert(id, label);

    return std::make_pair(VertexPtr(Vertex(this, *it)), res);
  }

  VertexPtr FindVertex(const VertexIDType &id) {
    auto it = vertices_.Find(id);
    if (it == vertices_.end()) return VertexPtr();
    return VertexPtr(Vertex(this, *it));
  }
  
  VertexConstPtr FindVertex(const typename VertexType
                                             ::IDType& id) const{
    return this->FindConstVertex(id);
  }

  VertexConstPtr FindConstVertex(const VertexIDType &id) const {
    auto it = vertices_.Find(id);
    if (it == vertices_.end()) return VertexConstPtr();
    return VertexConstPtr(ConstVertex(this, *it));
  }

  VertexIterator VertexBegin() {
    return VertexIterator(this, vertices_.begin(), vertices_.end());
  }

  VertexConstIterator VertexBegin() const {
    return this->VertexCBegin();
  }

  VertexConstIterator VertexCBegin() const {
    return VertexConstIterator(this, vertices_.cbegin(), vertices_.cend());
  }

  // VertexIterator VertexBegin(const VertexLabelType &label);

  // VertexConstIterator VertexCBegin(const VertexLabelType &label) const;

  size_t EraseVertex(const VertexIDType &id) {
    auto it = vertices_.Find(id);
    if (it == vertices_.end()) return 0;

    size_t count = 0;

    auto &v_data = it->second;

    std::vector<VertexIDType> edges;
    std::merge(v_data.out_edges_.begin(), 
               v_data.out_edges_. end (),
               v_data. in_edges_.begin(), 
               v_data. in_edges_. end (),
               std::back_inserter(edges));

    for (auto it  = v_data.out_edges_.begin();
              it != v_data.out_edges_.end(); it++){
      auto ret = vertices_.Find(this->edges_.Find(*it)->second.dst_);
      ret->second.in_edges_.Erase(*it);
    }

    for (auto it  = v_data.in_edges_.begin();
              it != v_data.in_edges_.end(); it++){
      auto ret = vertices_.Find(this->edges_.Find(*it)->second.src_);
      ret->second.out_edges_.Erase(*it);
    }

    for (auto it_r  = edges.rbegin(); 
              it_r != edges.rend(); 
            ++it_r) {
      if (it_r != edges.rbegin() && *it_r == *(it_r - 1)) 
        continue;
      count += edges_.Erase(*it_r);
    }

    vertices_.Erase(it);
    ++count;

    return count;
  }

  size_t CountEdge() const { return edges_.Count(); }

  std::pair<EdgePtr, bool> AddEdge(const VertexIDType &src,
                                   const VertexIDType &dst,
                                   const EdgeLabelType &label,
                                   const EdgeIDType &id) {
    auto it_src = vertices_.Find(src);
    if (it_src == vertices_.end()) {
      return std::make_pair(EdgePtr(), false);
    }

    auto it_dst = vertices_.Find(dst);
    if (it_dst == vertices_.end()) {
      return std::make_pair(EdgePtr(), false);
    }

    auto ret1 = edges_.Insert(id, label, src, dst);
    if (!ret1.second) {
      return std::make_pair(EdgePtr(Edge(this, *ret1.first)), false);
    }

    it_src->second.out_edges_.Insert(id);

    it_dst->second.in_edges_.Insert(id);

    return std::make_pair(EdgePtr(Edge(this, *ret1.first)), true);
  }

  EdgePtr FindEdge(const EdgeIDType &id) {
    auto it = edges_.Find(id);
    if (it == edges_.end()) return EdgePtr();
    return EdgePtr(Edge(this, *it));
  }

  EdgeConstPtr FindConstEdge(const EdgeIDType &id) const {
    auto it = edges_.Find(id);
    if (it == edges_.end()) return EdgeConstPtr();
    return EdgeConstPtr(Edge(this, *it));
  }

  EdgeIterator EdgeBegin() {
    return EdgeIterator(this, edges_.begin(), edges_.end());
  }

  EdgeConstIterator EdgeCBegin() const {
    return EdgeConstIterator(this, edges_.cbegin(), edges_.cend());
  }

  size_t EraseEdge(const EdgeIDType &id) {
    auto it1 = edges_.Find(id);
    if (it1 == edges_.end()) return 0;

    auto &e_data = it1->second;

    auto it2 = vertices_.Find(e_data.src_);
    auto &src_data = it2->second;
    bool res = src_data.out_edges_.Erase(id);
    assert(res);

    auto it3 = vertices_.Find(e_data.dst_);
    auto &dst_data = it3->second;
    res = dst_data.in_edges_.Erase(id);
    assert(res);

    edges_.Erase(it1);
    return 1;
  }

  void Clear() {
    vertices_.Clear();
    edges_.Clear();
  }

 public:
  static constexpr bool vertex_has_edge_label_index = false;

 private:
  VertexContainer vertices_;
  EdgeContainer edges_;
};

}  // namespace GUNDAM

#endif