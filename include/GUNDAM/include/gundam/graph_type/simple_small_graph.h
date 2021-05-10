#ifndef _GUNDAM_GRAPH_TYPE_SIMPLE_SMALL_GRAPH_H
#define _GUNDAM_GRAPH_TYPE_SIMPLE_SMALL_GRAPH_H

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include "gundam/component/container2.h"
#include "gundam/component/iterator2.h"
#include "gundam/component/pointer.h"

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/type_getter/edge_handle.h"
#include "gundam/type_getter/vertex_attribute_handle.h"
#include "gundam/type_getter/edge_attribute_handle.h"

#include "gundam/serialize/serialize.h"

namespace GUNDAM {

template <class VertexIDType, class VertexLabelType, class EdgeIDType,
          class EdgeLabelType>
class SimpleSmallGraph {
 public:
  static constexpr bool graph_has_vertex_label_index = false;

  static constexpr bool vertex_has_edge_label_index = false;
  
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

  using VertexContainer = PosKeyVector<VertexData>;
  using EdgeContainer = PosKeyVector<EdgeData>;

  template <bool is_const>
  class _Vertex;

  template <bool is_const>
  class _Edge;

  using Vertex = _Vertex<false>;

  using ConstVertex = _Vertex<true>;

  using Edge = _Edge<false>;

  using ConstEdge = _Edge<true>;

 private:
  friend class VertexHandle<SimpleSmallGraph>;
  friend class VertexHandle<const SimpleSmallGraph>;
  
  friend class EdgeHandle<SimpleSmallGraph>;
  friend class EdgeHandle<const SimpleSmallGraph>;

  using VertexPtr = GPointer<false, Vertex, ConstVertex>;

  using VertexConstPtr = GPointer<true, Vertex, ConstVertex>;

  using EdgePtr = GPointer<false, Edge, ConstEdge>;

  using EdgeConstPtr = GPointer<true, Edge, ConstEdge>;

 public:
  using VertexIterator =
      GIterator2<false, SimpleSmallGraph, typename VertexContainer::iterator,
                 Vertex, VertexPtr>;

  using VertexConstIterator =
      GIterator2<true, SimpleSmallGraph,
                 typename VertexContainer::const_iterator, ConstVertex,
                 VertexConstPtr>;

  using EdgeIterator =
      GIterator2<false, SimpleSmallGraph, typename EdgeContainer::iterator,
                 Edge, EdgePtr>;

  using EdgeConstIterator =
      GIterator2<true, SimpleSmallGraph, typename EdgeContainer::const_iterator,
                 ConstEdge, EdgeConstPtr>;

 private:
  template <bool is_const>
  class _Vertex {
   public:
    using GraphType = SimpleSmallGraph;

    using IDType = VertexIDType;

    using LabelType = VertexLabelType;

    using AttributeKeyType = std::string;

    using EdgeIterator =
        GIterator2<false, SimpleSmallGraph,
                   typename SortedVectorSet<EdgeIDType>::iterator, Edge,
                   EdgePtr>;

    using EdgeConstIterator =
        GIterator2<true, SimpleSmallGraph,
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

    _Vertex() : graph_(nullptr), data_(nullptr){};

    _Vertex(typename std::conditional<is_const, const GraphType *,
                                      GraphType *>::type graph,
            typename std::conditional<is_const, const VertexData &,
                                      VertexData &>::type data)
        : graph_(graph), data_(&data) {
      assert(graph_);
    }

    _Vertex(typename std::conditional<is_const, const GraphType *,
                                      GraphType *>::type graph,
            const VertexIDType &id)
        : graph_(graph) {
      assert(graph_);
      assert(id > 0 && id <= graph_->vertices_.Count());
      auto it = graph_->vertices_.Find(id - 1);
      data_ = &*it;
    }

   public:
    IDType id() const {
      assert(HasValue());
      return static_cast<IDType>(graph_->vertices_.GetPos(data_)) + 1;
    }

    const VertexLabelType &label() const {
      assert(HasValue());
      return data_->label_;
    }

    size_t CountOutEdge() const {
      assert(HasValue());
      return data_->out_edges_.Count();
    }

    size_t CountOutEdge(const EdgeLabelType &edge_label) const {
      size_t out_edge_count = 0;
      for (auto it = this->OutEdgeBegin(); !it.IsDone(); ++it) {
        if (it->label() == edge_label) out_edge_count++;
      }
      return out_edge_count;
    }

    size_t CountInEdge() const {
      assert(HasValue());
      return data_->in_edges_.Count();
    }
    
    size_t CountInEdge(const EdgeLabelType &edge_label) const {
      size_t in_edge_count = 0;
      for (auto it = this->InEdgeBegin(); !it.IsDone(); ++it) {
        if (it->label() == edge_label) in_edge_count++;
      }
      return in_edge_count;
    }

    size_t CountOutVertex() const {
      std::set<VertexIDType> out_vertex_id_set;
      for (auto it = this->OutEdgeBegin(); !it.IsDone(); ++it) {
        out_vertex_id_set.insert(it->const_dst_handle()->id());
      }
      return out_vertex_id_set.size();
    }

    size_t CountOutVertex(const EdgeLabelType &edge_label) const {
      std::set<VertexIDType> out_vertex_id_set;
      for (auto it = this->OutEdgeBegin(); !it.IsDone(); ++it) {
        if (it->label() == edge_label)
          out_vertex_id_set.insert(it->const_dst_handle()->id());
      }
      return out_vertex_id_set.size();
    }

    size_t CountInVertex() const {
      std::set<VertexIDType> in_vertex_id_set;
      for (auto it = this->InEdgeBegin(); !it.IsDone(); ++it) {
        in_vertex_id_set.insert(it->const_src_handle()->id());
      }
      return in_vertex_id_set.size();
    }

    size_t CountInVertex(const EdgeLabelType &edge_label) const {
      std::set<VertexIDType> in_vertex_id_set;
      for (auto it = this->InEdgeBegin(); !it.IsDone(); ++it) {
        if (it->label() == edge_label)
          in_vertex_id_set.insert(it->const_src_handle()->id());
      }
      return in_vertex_id_set.size();
    }

    EdgeIterator OutEdgeBegin() {
      assert(HasValue());
      return EdgeIterator(graph_, data_->out_edges_.begin(),
                          data_->out_edges_.end());
    }

    EdgeConstIterator OutEdgeBegin() const {
      assert(HasValue());
      return EdgeConstIterator(graph_, data_->out_edges_.begin(),
                               data_->out_edges_.end());
    }

    EdgeIterator InEdgeBegin() {
      assert(HasValue());
      return EdgeIterator(graph_, data_->in_edges_.begin(),
                          data_->in_edges_.end());
    }

    EdgeConstIterator InEdgeBegin() const {
      assert(HasValue());
      return EdgeConstIterator(graph_, data_->in_edges_.begin(),
                               data_->in_edges_.end());
    }

    bool operator==(const _Vertex &b) const {
      if (!graph_) {
        return !b.graph_;
      } else if (!b.graph_) {
        return false;
      } else {
        assert(data_);
        assert(b.data_);
        return data_ == b.data_;
      }
    }

    bool operator<(const _Vertex &b) const {
      if (!graph_ || !b.graph_) {
        return false;
      } else {
        assert(data_);
        assert(b.data_);
        return data_ < b.data_;
      }
    }

   private:
    void Reset() {
      graph_ = nullptr;
      data_ = nullptr;
    }

    bool HasValue() const { return (graph_ != nullptr); }

    typename std::conditional<is_const, const GraphType *, GraphType *>::type
        graph_;
    typename std::conditional<is_const, const VertexData *, VertexData *>::type
        data_;
  };

  template <bool is_const>
  class _Edge {
   public:
    using GraphType = SimpleSmallGraph;

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

    _Edge() : graph_(nullptr), data_(nullptr){};

    _Edge(typename std::conditional<is_const, const GraphType *,
                                    GraphType *>::type graph,
          typename std::conditional<is_const, const EdgeData &,
                                    EdgeData &>::type &data)
        : graph_(graph), data_(&data) {
      assert(graph_);
    }

    _Edge(typename std::conditional<is_const, const GraphType *,
                                    GraphType *>::type graph,
          const EdgeIDType &id)
        : graph_(graph) {
      assert(graph_);
      assert(id > 0 && id <= graph_->edges_.Count());
      auto it = graph_->edges_.Find(id - 1);
      data_ = &*it;
    }

   public:
    EdgeIDType id() const {
      assert(HasValue());
      return static_cast<EdgeIDType>(graph_->edges_.GetPos(data_)) + 1;
    }

    const EdgeLabelType &label() const {
      assert(HasValue());
      return data_->label_;
    }

    VertexIDType src_id() const {
      assert(HasValue());
      return ConstVertex(graph_, data_->src_).id();
    }

    VertexIDType dst_id() const {
      assert(HasValue());
      return ConstVertex(graph_, data_->dst_).id();
    }

    VertexPtr src_handle() {
      assert(HasValue());
      return VertexPtr(Vertex(graph_, data_->src_));
    }

    VertexConstPtr src_handle() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, data_->src_));
    }

    VertexConstPtr const_src_handle() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, data_->src_));
    }

    VertexPtr dst_handle() {
      assert(HasValue());
      return VertexPtr(Vertex(graph_, data_->dst_));
    }

    VertexConstPtr dst_handle() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, data_->dst_));
    }

    VertexConstPtr const_dst_handle() const {
      assert(HasValue());
      return VertexConstPtr(ConstVertex(graph_, data_->dst_));
    }

    bool operator==(const _Edge &b) const {
      if (!graph_) {
        return !b.graph_;
      } else if (!b.graph_) {
        return false;
      } else {
        assert(data_);
        assert(b.data_);
        return data_ == b.data_;
      }
    }

    bool operator<(const _Edge &b) const {
      if (!graph_ || !b.graph_) {
        return false;
      } else {
        assert(data_);
        assert(b.data_);
        return data_ < b.data_;
      }
    }

   private:
    void Reset() {
      graph_ = nullptr;
      data_ = nullptr;
    }

    bool HasValue() const { return (graph_ != nullptr); }

    typename std::conditional<is_const, const GraphType *, GraphType *>::type
        graph_;
    typename std::conditional<is_const, const EdgeData *, EdgeData *>::type
        data_;
  };

 public:
  using VertexType = Vertex;

  using VertexCounterType = size_t;

  using EdgeType = Edge;

  static constexpr bool vertex_has_attribute = false;

  static constexpr bool edge_has_attribute = false;

  static constexpr bool is_mutable = false;

  SimpleSmallGraph() = default;

  explicit SimpleSmallGraph(const SimpleSmallGraph &other) = default;

  SimpleSmallGraph(SimpleSmallGraph &&) = default;

  SimpleSmallGraph &operator=(const SimpleSmallGraph &other) = default;

  SimpleSmallGraph &operator=(SimpleSmallGraph &&) = default;

  ~SimpleSmallGraph() = default;

  size_t CountVertex() const { return vertices_.Count(); }

  std::pair<VertexPtr, bool> AddVertex(const VertexIDType &id,
                                       const VertexLabelType &label) {
    if (id == vertices_.Count() + 1) {
      auto it = vertices_.Insert(label);
      return std::make_pair(VertexPtr(Vertex(this, *it)), true);
    } else if (id > 0 && id <= vertices_.Count()) {
      auto it = vertices_.Find(id - 1);
      return std::make_pair(VertexPtr(Vertex(this, *it)), false);
    } else {
      return std::make_pair(VertexPtr(), false);
    }
  }

  VertexPtr FindVertex(const VertexIDType &id) {
    if (id == 0 || id > vertices_.Count()) {
      return VertexPtr();
    }
    auto it = vertices_.Find(id - 1);
    return VertexPtr(Vertex(this, *it));
  }

  VertexConstPtr FindVertex(const VertexIDType &id) const {
    if (id == 0 || id > vertices_.Count()) {
      return VertexConstPtr();
    }
    auto it = vertices_.Find(id - 1);
    return VertexConstPtr(ConstVertex(this, *it));
  }

  VertexIterator VertexBegin() {
    return VertexIterator(this, vertices_.begin(), vertices_.end());
  }

  VertexConstIterator VertexBegin() const {
    return VertexConstIterator(this, vertices_.cbegin(), vertices_.cend());
  }

  // VertexIterator VertexBegin(const VertexLabelType &label);

  // VertexConstIterator VertexCBegin(const VertexLabelType &label) const;

  // bool EraseVertex(const VertexIDType &id);

  size_t CountEdge() const { return edges_.Count(); }

  std::pair<EdgePtr, bool> AddEdge(const VertexIDType &src,
                                   const VertexIDType &dst,
                                   const EdgeLabelType &label,
                                   const EdgeIDType &id) {
    if (id == edges_.Count() + 1) {
      auto it_src = vertices_.Find(src - 1);
      if (it_src == vertices_.end()) {
        return std::make_pair(EdgePtr(), false);
      }

      auto it_dst = vertices_.Find(dst - 1);
      if (it_dst == vertices_.end()) {
        return std::make_pair(EdgePtr(), false);
      }

      auto it_e = edges_.Insert(label, src, dst);

      it_src->out_edges_.Insert(id);

      it_dst->in_edges_.Insert(id);

      return std::make_pair(EdgePtr(Edge(this, *it_e)), true);

    } else if (id > 0 && id <= edges_.Count()) {
      auto it_e = edges_.Find(id - 1);
      return std::make_pair(EdgePtr(Edge(this, *it_e)), false);

    } else {
      return std::make_pair(EdgePtr(), false);
    }
  }

  EdgePtr FindEdge(const EdgeIDType &id) {
    if (id == 0 || id > edges_.size()) {
      return EdgePtr();
    }
    auto it = edges_.Find(id - 1);
    if (it == edges_.end()) return EdgePtr();
    return EdgePtr(this, *it);
  }

  EdgeConstPtr FindEdge(const EdgeIDType &id) const {
    if (id == 0 || id > edges_.size()) {
      return EdgeConstPtr();
    }
    auto it = edges_.Find(id - 1);
    if (it == edges_.end()) return EdgeConstPtr();
    return EdgeConstPtr(this, *it);
  }

  EdgeIterator EdgeBegin() {
    return EdgeIterator(this, edges_.begin(), edges_.end());
  }

  EdgeConstIterator EdgeBegin() const {
    return EdgeConstIterator(this, edges_.cbegin(), edges_.cend());
  }

  // bool EraseEdge(const EdgeIDType &id);

  void Clear() {
    vertices_.Clear();
    edges_.Clear();
  }

 private:
  VertexContainer vertices_;
  EdgeContainer edges_;
};

}  // namespace GUNDAM

#endif