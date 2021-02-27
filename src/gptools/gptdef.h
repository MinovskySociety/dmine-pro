#ifndef _GPTDEF_H
#define _GPTDEF_H

#include <graph_package/graph_package.h>
#include <gundam/large_graph.h>
#include <gundam/graph.h>

// #define USE_GRAPH

namespace gptools {

using ID = graphpackage::ID;

using Label = graphpackage::Label;

using AttributeKey = graphpackage::AttributeKey;

#ifdef USE_GRAPH
using Graph = GUNDAM::Graph<GUNDAM::SetVertexIDType<ID>, 
                            GUNDAM::SetVertexLabelType<Label>,
                            GUNDAM::SetVertexAttributeStoreType<GUNDAM::AttributeType::kGrouped>, 
                            GUNDAM::SetVertexAttributeKeyType<AttributeKey>,
                            GUNDAM::SetEdgeIDType<ID>, 
                            GUNDAM::SetEdgeLabelType<Label>, 
                            GUNDAM::SetEdgeAttributeStoreType<GUNDAM::AttributeType::kGrouped>,
                            GUNDAM::SetEdgeAttributeKeyType<AttributeKey>>;
#else
using Graph =
    GUNDAM::LargeGraph<ID, Label, AttributeKey, ID, Label, AttributeKey>;
#endif //USE_GRAPH
using GraphPackage = graphpackage::GraphPackage<Graph>;

using graphpackage::GraphPackageInfo;

using graphpackage::LabelType;

using graphpackage::ReservedLabel;

}  // namespace gptools

#endif