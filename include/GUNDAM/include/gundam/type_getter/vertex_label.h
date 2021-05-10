#ifndef _VERTEX_LABEL_H
#define _VERTEX_LABEL_H

namespace GUNDAM{

template <typename GraphType>
class VertexLabel{
 public:
  using type = typename GraphType::VertexType::LabelType;
};

} // namespace GUNDAM 

#endif // _VERTEX_LABEL_H