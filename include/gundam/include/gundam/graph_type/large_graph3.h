#ifndef _GUNDAM_GRAPH_TYPE_LARGE_GRAPH3_H
#define _GUNDAM_GRAPH_TYPE_LARGE_GRAPH3_H

#include "gundam/component/attribute.h"
#include "gundam/component/container2.h"
#include "gundam/component/iterator2.h"

#include "gundam/type_getter/vertex_handle.h"
#include "gundam/type_getter/edge_handle.h"

namespace GUNDAM {
template <class VertexIDType, class VertexLabelType,
          class VertexAttributeKeyType, class EdgeIDType, class EdgeLabelType,
          class EdgeAttributeKeyType>
class LargeGraph3 {
 private:
  class VertexData;

  class EdgeData;
  
  struct GetVertexID {
    constexpr VertexIDType operator()(const VertexData &v) const {
      return v.id();
    }
  };

  struct GetVertexLabel {
    constexpr VertexLabelType operator()(const VertexData &v) const {
      return v.label();
    }
  };

  struct GetEdgeID {
    constexpr EdgeIDType operator()(const EdgeData &e) const {
      return e.id();
    }
  };

  struct GetEdgeLabel {
    constexpr EdgeLabelType operator()(const EdgeData &e) const {
      return e.label();
    }
  };

  struct GetEdgeSrc {
    constexpr VertexData *operator()(EdgeData &e) const { return e.src_handle(); }
  };

  struct GetEdgeDst {
    constexpr VertexData *operator()(EdgeData &e) const { return e.dst_handle(); }
  };

  using VertexDataContainer =
      SortedVectorMultiKey<VertexData, VertexLabelType, GetVertexLabel, VertexIDType, GetVertexID>;

  using VertexIDLabelDict = SortedVectorDict<VertexIDType, VertexLabelType>;

  class VertexData
      : public Attribute_<AttributeType::kSeparated,
                          false, // not const
                          std::pair<VertexLabelType, bool>,
                          VertexAttributeKeyType,
                          ContainerType::Vector,
                          SortType::Default> {
      // : public WithAttribute_<std::string, false, true, VertexAttributeKeyType,
      //                         ContainerType::Vector, SortType::Default> {
   private:
    friend class LargeGraph3;

    using EdgeDataContainer =
        SortedVectorMultiKey<EdgeData, EdgeLabelType, GetEdgeLabel, EdgeIDType,
                             GetEdgeID>;

   public:
    using GraphType = LargeGraph3;

    using IDType = VertexIDType;

    using LabelType = VertexLabelType;

    using AttributeKeyType = VertexAttributeKeyType;

    using EdgeIterator =
        GIterator<typename EdgeDataContainer::iterator, EdgeData, DefaultCast>;

    using EdgeConstIterator =
        GIterator<typename EdgeDataContainer::const_iterator, const EdgeData,
                  DefaultCast>;

    const IDType &id() const { return id_; }

    const VertexLabelType &label() const { return label_; }

    size_t CountOutEdge() const { return out_edge_data_.Count(); }

    size_t CountOutEdge(const EdgeLabelType &edge_label) const {
      return out_edge_data_.Count(edge_label);
    }

    // size_t CountOutEdge(const EdgeLabelType &edge_label,
    //                    const VertexData *vertex_ptr) const {
    //  auto it =
    //      out_edge_build_on_vertex_.Find(const_cast<VertexData
    //      *>(vertex_ptr));
    //  if (it == out_edge_build_on_vertex_.cend()) {
    //    return 0;
    //  }
    //  auto it1 = it->second.Find(edge_label);
    //  if (it1 == it->second.cend()) {
    //    return 0;
    //  }
    //  return it1->second.size();
    //}

    size_t CountInEdge() const { return in_edge_data_.Count(); }

    size_t CountInEdge(const EdgeLabelType &edge_label) const {
      return in_edge_data_.Count(edge_label);
    }    

    // size_t CountInEdge(const EdgeLabelType &edge_label,
    //                   const VertexData *vertex_ptr) const {
    //  auto it =
    //      in_edge_build_on_vertex_.Find(const_cast<VertexData *>(vertex_ptr));
    //  if (it == in_edge_build_on_vertex_.cend()) {
    //    return 0;
    //  }
    //  auto it1 = it->second.Find(edge_label);
    //  if (it1 == it->second.cend()) {
    //    return 0;
    //  }
    //  return it1->second.size();
    //}

    // size_t CountInVertex() const {
    //  return this->in_edge_build_on_vertex_.size();
    //}

    // size_t CountInVertex(const EdgeLabelType &edge_label) const {
    //  auto it = this->in_vertices_.Find(edge_label);
    //  if (it == this->in_vertices_.cend()) return 0;
    //  return it->second.size();
    //}

    // size_t CountOutVertex() const {
    //  return this->out_edge_build_on_vertex_.size();
    //}

    // size_t CountOutVertex(const EdgeLabelType &edge_label) const {
    //  auto it = this->out_vertices_.Find(edge_label);
    //  if (it == this->out_vertices_.cend()) return 0;
    //  return it->second.size();
    //}

    EdgeIterator OutEdgeBegin() {
      return EdgeIterator(out_edge_data_.begin(), out_edge_data_.end());
    }

    EdgeConstIterator OutEdgeCBegin() const {
      return EdgeConstIterator(out_edge_data_.cbegin(), out_edge_data_.cend());
    }

    EdgeIterator InEdgeBegin() {
      return EdgeIterator(in_edge_data_.begin(), in_edge_data_.end());
    }

    EdgeConstIterator InEdgeCBegin() const {
      return EdgeConstIterator(in_edge_data_.cbegin(), in_edge_data_.cend());
    }

    EdgeIterator OutEdgeBegin(const EdgeLabelType &edge_label) {
      auto [first, last] = out_edge_data_.Range(edge_label); 
      return {std::move(first), std::move(last)};
    }

    EdgeConstIterator OutEdgeCBegin(const EdgeLabelType &edge_label) const {
      auto [first, last] = out_edge_data_.CRange(edge_label);
      return {std::move(first), std::move(last)};
    }

    EdgeIterator InEdgeBegin(const EdgeLabelType &edge_label) {
      auto [first, last] = in_edge_data_.Range(edge_label);
      return {std::move(first), std::move(last)};
    }

    EdgeConstIterator InEdgeCBegin(const EdgeLabelType &edge_label) const {
      auto [first, last] = in_edge_data_.CRange(edge_label);
      return {std::move(first), std::move(last)};
    }

    // EdgeIterator OutEdgeBegin(const EdgeLabelType &edge_label,
    //                          const VertexData *vertex_ptr) {}

    // EdgeConstIterator OutEdgeCBegin(const EdgeLabelType &edge_label,
    //                                const VertexData *vertex_ptr) const {}

    // EdgeIterator InEdgeBegin(const EdgeLabelType &edge_label,
    //                         const VertexData *vertex_ptr) {}

    // EdgeConstIterator InEdgeCBegin(const EdgeLabelType &edge_label,
    //                               const VertexData *vertex_ptr) const {}

    VertexData(const IDType &id, const LabelType &label)
        : id_(id), label_(label) {}

    ~VertexData() {}

    std::pair<EdgeData *, bool> AddOutEdge(const EdgeIDType &edge_id,
                                           const EdgeLabelType &edge_label,
                                           VertexData *dst) {
      assert(dst);
      auto [it, r] = out_edge_data_.Insert(edge_label, edge_id, edge_id,
                                           edge_label, this, dst);
      if (!r) return {nullptr, false};
      return {&*it, true};
    }

    std::pair<EdgeData *, bool> AddInEdge(const EdgeIDType &edge_id,
                                          const EdgeLabelType &edge_label,
                                          VertexData *src) {
      assert(src);
      auto [it, r] = in_edge_data_.Insert(edge_label, edge_id, edge_id,
                                          edge_label, src, this);
      if (!r) return {nullptr, false};
      return {&*it, true};
    }

   private:
    VertexIDType id_;
    VertexLabelType label_;
    EdgeDataContainer out_edge_data_;
    EdgeDataContainer in_edge_data_;
  };

  class EdgeData
      : public Attribute_<AttributeType::kSeparated,
                          false, // not const
                          std::pair<EdgeLabelType, bool>,
                          EdgeAttributeKeyType,
                          ContainerType::Vector,
                          SortType::Default> {
      // : public WithAttribute_<std::string, false, true, EdgeAttributeKeyType,
      //                         ContainerType::Vector, SortType::Default> {
   private:
    friend class LargeGraph3;

   public:
    using GraphType = LargeGraph3;

    using IDType = EdgeIDType;

    using LabelType = EdgeLabelType;

    using AttributeKeyType = EdgeAttributeKeyType;

    EdgeData(const EdgeIDType &id, const EdgeLabelType &label, VertexData *src,
             VertexData *dst)
        : id_(id), label_(label), src_(src), dst_(dst) {}

    ~EdgeData() {}

    const EdgeIDType &id() const { return id_; }

    const EdgeLabelType &label() const { return label_; }

    const VertexIDType &src_id() const { return src_->id(); }

    const VertexIDType &dst_id() const { return dst_->id(); }

    VertexData *src_handle() { return src_; }

    VertexData *dst_handle() { return dst_; }

    const VertexData *const_src_handle() const { return src_; }

    const VertexData *const_dst_handle() const { return dst_; }

   private:
    EdgeIDType id_;
    EdgeLabelType label_;
    VertexData *src_;
    VertexData *dst_;
    // EdgeAttributeListType attributes_;
  };

 private:
  friend class VertexHandle<LargeGraph3>;
  friend class VertexHandle<const LargeGraph3>;
  
  friend class EdgeHandle<LargeGraph3>;
  friend class EdgeHandle<const LargeGraph3>;

  using VertexPtr = VertexData *;

  using VertexConstPtr = const VertexData *;

  using EdgePtr = EdgeData *;

  using EdgeConstPtr = const EdgeData *;

 public:
  using VertexType = VertexData;

  using VertexCounterType = size_t;

  using EdgeType = EdgeData;

  using VertexIterator = GIterator<typename VertexDataContainer::iterator,
                                   VertexData, DefaultCast>;

  using VertexConstIterator =
      GIterator<typename VertexDataContainer::const_iterator, const VertexData,
                DefaultCast>;

  static constexpr bool vertex_has_attribute = true;

  static constexpr bool edge_has_attribute = true;

  LargeGraph3() = default;

  LargeGraph3(const LargeGraph3 &other) {
    for (auto v_it = other.VertexCBegin(); !v_it.IsDone(); ++v_it) {
      AddVertex(v_it->id(), v_it->label());            
    }

    for (auto v_it = other.VertexCBegin(); !v_it.IsDone(); ++v_it) {
      for (auto e_it = v_it->OutEdgeCBegin(); !e_it.IsDone(); ++e_it) {
        AddEdge(e_it->src_id(), e_it->dst_id(), e_it->label(), e_it->id());
      }
    }
  }

  LargeGraph3(LargeGraph3 &&) = default;

  LargeGraph3 &operator=(const LargeGraph3 &other) {
    Clear();

    for (auto v_it = other.VertexCBegin(); !v_it.IsDone(); ++v_it) {
      AddVertex(v_it->id(), v_it->label());
    }    

    for (auto v_it = other.VertexCBegin(); !v_it.IsDone(); ++v_it) {
      for (auto e_it = v_it->OutEdgeCBegin(); !e_it.IsDone(); ++e_it) {
        AddEdge(e_it->src_id(), e_it->dst_id(), e_it->label(), e_it->id());
      }
    }

    return *this;
  };

  LargeGraph3 &operator=(LargeGraph3 &&) = default;

  ~LargeGraph3() {}

  size_t CountVertex() const { return vertex_data_.Count(); }

  size_t CountVertex(const typename VertexType::LabelType &label) const {
    return vertex_data_.Count(label);
  }

  std::pair<VertexPtr, bool> AddVertex(
      const typename VertexType::IDType &id,
      const typename VertexType::LabelType &label) {
        
    auto [it, r] = vertex_data_.Insert(label, id, id, label);
    if (!r) {
      return std::make_pair(&*it, false);
    }

    r = vertex_id_dict_.Insert(id, label).second;
    assert(r);

    return std::make_pair(&*it, true);
  }

  VertexPtr FindVertex(const typename VertexType::IDType &id) {
    auto it1 = vertex_id_dict_.Find(id);
    if (it1 == vertex_id_dict_.end()) return VertexPtr(nullptr);
    const auto &label = it1->second;
    auto it2 = vertex_data_.Find(label, id);
    assert(it2 != vertex_data_.end());
    return VertexPtr(&*it2);
  }

 private:
  VertexConstPtr FindConstVertex(const typename VertexType::IDType &id) const {
    auto it = vertex_id_dict_.Find(id);
    if (it == vertex_id_dict_.cend()) return VertexConstPtr(nullptr);
    const auto &label = it->second;
    auto it2 = vertex_data_.Find(label, id);
    assert(it2 != vertex_data_.cend());
    return VertexPtr(&*it2);
  }    
 
 public:
  VertexIterator VertexBegin() {
    return VertexIterator(vertex_data_.begin(), vertex_data_.end());
  }

  VertexConstIterator VertexCBegin() const {
    return VertexConstIterator(vertex_data_.cbegin(), vertex_data_.cend());
  }

  VertexIterator VertexBegin(const typename VertexType::LabelType &label) {
    auto [first, last] = vertex_data_.Range(label);
    return {std::move(first), std::move(last)};    
  };

  VertexConstIterator VertexCBegin(
      const typename VertexType::LabelType &label) const {
    auto [first, last] = vertex_data_.CRange(label);
    return {std::move(first), std::move(last)}; 
  }

  //bool EraseVertex(const typename VertexType::IDType &id) {}

  //size_t CountEdge() const { return edges_.Count(); }

  std::pair<EdgePtr, bool> AddEdge(const typename VertexType::IDType &src_id,
                                   const typename VertexType::IDType &dst_id,
                                   const typename EdgeType::LabelType &label,
                                   const typename EdgeType::IDType &id) {
    VertexPtr src_ptr = FindVertex(src_id);
    VertexPtr dst_ptr = FindVertex(dst_id);
    if (!src_ptr || !dst_ptr) return {nullptr, false};

    auto [e1, r1] = src_ptr->AddOutEdge(id, label, dst_ptr);
    if (!r1) return {nullptr, false};

    auto [e2, r2] = dst_ptr->AddInEdge(id, label, src_ptr);
    if (!r2) return {nullptr, false};

    return {e1, true};
  }

  //EdgePtr FindEdge(const typename EdgeType::IDType &id){}
  //
  //EdgeConstPtr FindConstEdge(const typename EdgeType::IDType &id) const {}    

  //EdgeIterator EdgeBegin() {}
  //  
  //EdgeConstIterator EdgeCBegin() const {}
  //  
  //bool EraseEdge(const typename EdgeType::IDType &id) {}

  void Clear() {    
    vertex_id_dict_.Clear();    
    vertex_data_.Clear();
  }

 private:    
  VertexDataContainer vertex_data_;
  VertexIDLabelDict vertex_id_dict_;
};

}  // namespace GUNDAM

#endif
