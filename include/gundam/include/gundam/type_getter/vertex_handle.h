#ifndef _VERTEX_HANDLE_H
#define _VERTEX_HANDLE_H

namespace GUNDAM {

template <typename GraphType>
class VertexHandle;

// non-constant graph type
template <typename GraphType>
class VertexHandle{
 public:
  using type = typename GraphType::VertexPtr;
};

// constant graph type
template <typename GraphType>
class VertexHandle<const GraphType>{
 public:
  using type = typename GraphType::VertexConstPtr;
};

// remove reference
template <typename GraphType>
class VertexHandle<GraphType&>{
 public:
  using type = typename VertexHandle<GraphType>::type;
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

#endif // _VERTEX_HANDLE_H