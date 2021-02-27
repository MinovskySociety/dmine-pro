#ifndef _MLMODEL_H
#define _MLMODEL_H
#include "gpar_template/gpar_new.h"
namespace gmine_new {
template <class Pattern, class DataGraph>
class MLModelBase {
 public:
  // ml model base interface
  using DataGraphVertexPtr = typename DataGraph::VertexConstPtr;
  using DataGraphLabel = typename DataGraph::EdgeType::LabelType;
  virtual bool Satisfy(const DataGraphVertexPtr predict_vertex_x,
                       const DataGraphVertexPtr predict_vertex_y) const = 0;
  virtual void Training(
      std::vector<gmine_new::DiscoverdGPAR<Pattern, DataGraph>> &discover_gmar,
      DataGraph &data_graph) = 0;
  virtual void PrintModel() const = 0;
  virtual bool IsTerminal() const = 0;
  virtual void BuildMLGraph(DataGraph &ml_graph) const = 0;
  virtual DataGraphLabel ModelLabel() const = 0;
};

}  // namespace gmine_new

#endif
