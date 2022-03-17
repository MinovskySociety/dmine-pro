#ifndef _FOLD_UNFOLD_H
#define _FOLD_UNFOLD_H

#include <set>

#include "gptdef.h"

namespace gptools {

int FoldAttribute(GraphPackage &gp, const std::set<Label> &vertex_label_list,
                  const std::set<AttributeKey> &fold_attr_list,
                  const std::set<Label> &fold_edge_label);
 

int UnfoldAttribute(GraphPackage &gp, const std::set<Label> &vertex_label_list,
                    const std::set<AttributeKey> &unfold_attr_list,
                    int unfold_type);

int FoldRelation(GraphPackage &gp, const std::set<Label> &fold_relation_list);

int UnfoldRelation(GraphPackage &gp,
                   const std::set<Label> &unfold_relation_list);

}  // namespace gptools

#endif
