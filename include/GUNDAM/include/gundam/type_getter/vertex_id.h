#ifndef _VERTEX_ID_H
#define _VERTEX_ID_H

namespace GUNDAM{

template <typename GraphType>
class VertexID{
 public:
  using type = typename GraphType::VertexType::IDType;
};

} // namespace GUNDAM 

#endif // _VERTEX_ID_H