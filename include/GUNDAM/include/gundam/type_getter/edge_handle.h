#ifndef _EDGE_HANDLE_H
#define _EDGE_HANDLE_H

namespace GUNDAM {

template <typename GraphType>
class EdgeHandle;

// non-constant graph type
template <typename GraphType>
class EdgeHandle{
 public:
  using type = typename GraphType::EdgePtr;
};

// constant graph type
template <typename GraphType>
class EdgeHandle<const GraphType>{
 public:
  using type = typename GraphType::EdgeConstPtr;
};

// remove reference
template <typename GraphType>
class EdgeHandle<GraphType&>{
 public:
  using type = typename EdgeHandle<GraphType>::type;
};

// template <typename GraphType>
// class EdgeConstHandle{
//  public:
//   using type = typename GraphType::EdgeConstPtr;
// };

// // remove reference
// template <typename GraphType>
// class EdgeConstHandle<GraphType&>{
//  public:
//   using type = typename EdgeConstHandle<GraphType>::type;
// };

}; // namespace GUNDAM

#endif // _EDGE_HANDLE_H