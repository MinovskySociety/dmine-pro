#ifndef _GUNDAM_COMPONENT_VERTEX_SET_H
#define _GUNDAM_COMPONENT_VERTEX_SET_H

#include "gundam/component/"gundam/component/container2.h""
#include "gundam/component/iterator2.h"

namespace GUNDAM {

template <class VertexIDType, class VertexLabelType, class VertexDataType, class VertexPtrType, class VertexConstPtrType>
class SortedVectorVertexSet {
 public:
  size_t CountVertex() const { return vertices_.Count(); }

  std::pair<VertexPtrType, bool> AddVertex(size_t id,
      const VertexLabelType& label) {

      

  }

  VertexPtrType FindVertex(const VertexLabelType &label);

  VertexConstPtrType FindConstVertex(const VertexLabelType &label) const;

  bool EraseVertex(size_t id);

  void Clear();

  VertexIterator VertexBegin() {

      vertices_.begin(), vertices_.end();
  }

  VertexConstIterator VertexCBegin() const;

private:

  SortedVectorDict<VertexIDType, VertexDataType> vertices_;
};
}

VertexPtr<Graph, VertexID>

VertexIterator<Graph, VertexSet::Iterator>

EdgePtr<Graph, EdgeID>

EdgeIterator<Graph, Edge





#endif
