#ifndef _EDGE_LABEL_H
#define _EDGE_LABEL_H

namespace GUNDAM{

template <typename GraphType>
class EdgeLabel{
 public:
  using type = typename GraphType::EdgeType::LabelType;
};

} // namespace GUNDAM 

#endif // _EDGE_LABEL_H