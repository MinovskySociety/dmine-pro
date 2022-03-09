#ifndef _EDGE_ATTRIBUTE_HANDLE_H
#define _EDGE_ATTRIBUTE_HANDLE_H

namespace GUNDAM {

template <typename GraphType>
class EdgeAttributeHandle;

// non-constant graph type
template <typename GraphType>
class EdgeAttributeHandle{
 public:
  using type = typename GraphType::EdgeAttributePtr;
};

// constant graph type
template <typename GraphType>
class EdgeAttributeHandle<const GraphType>{
 public:
  using type = typename GraphType::EdgeAttributeConstPtr;
};

// remove reference
template <typename GraphType>
class EdgeAttributeHandle<GraphType&>{
 public:
  using type = typename EdgeAttributeHandle<GraphType>::type;
};

// template <typename GraphType>
// class VertexConstHandle{
//  public:
//   using type = typename GraphType::VertexConstPtr;
// };

// // remove reference
// template <typename GraphType>
// class VertexConstHandle<GraphType&>{
//  public:
//   using type = typename VertexConstHandle<GraphType>::type;
// };

}; // namespace GUNDAM

#endif // _EDGE_ATTRIBUTE_HANDLE_H