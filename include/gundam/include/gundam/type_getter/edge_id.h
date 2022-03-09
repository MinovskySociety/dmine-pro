#ifndef _EDGE_ID_H
#define _EDGE_ID_H

namespace GUNDAM{

template <typename GraphType>
class EdgeID{
 public:
  using type = typename GraphType::EdgeType::IDType;
};

} // namespace GUNDAM 

#endif // _EDGE_ID_H