#ifndef _VERTEX_ATTRIBUTE_HANDLE_H
#define _VERTEX_ATTRIBUTE_HANDLE_H

namespace GUNDAM {

template <typename GraphType>
class VertexAttributeHandle;

// non-constant graph type
template <typename GraphType>
class VertexAttributeHandle{
 public:
  using type = typename GraphType::VertexAttributePtr;
};

// constant graph type
template <typename GraphType>
class VertexAttributeHandle<const GraphType>{
 public:
  using type = typename GraphType::VertexAttributeConstPtr;
};

// remove reference
template <typename GraphType>
class VertexAttributeHandle<GraphType&>{
 public:
  using type = typename VertexAttributeHandle<GraphType>::type;
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

#endif // _VERTEX_ATTRIBUTE_HANDLE_H