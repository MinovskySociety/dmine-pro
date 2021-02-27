#ifndef _SEARCHTREENODE_H
#define _SEARCHTREENODE_H

#include <algorithm>
#include <chrono>

#include "gpar.h"

namespace gmine_new {
template <class Pattern, class DataGraph>
class SearchTreeNodeT {
  public:
    DiscoverdGPAR<Pattern, DataGraph> discovergpar;
    SearchTreeNodeT():
      parent(NULL),
      num_visits(1),
      value(0),
      depth(0),
      node_id(0),
      is_expand(false) {
    }
    // number of times the TreeNode has been visited
    int get_num_visits() const { return num_visits; }
    // accumulated value 
    double get_value() const { return value; }
    // how deep the TreeNode is in the tree
    int get_depth() const { return depth; }
    // number of children the TreeNode has
    int get_num_children() const { return children.size(); }
    // get the i'th child
    SearchTreeNodeT* get_child(int i) const { return children[i].get(); }
    // get parent
    SearchTreeNodeT* get_parent() const { return parent; }
    // get ucb score (don`t include root node)
    double get_score() {  
      return double(value) / double(num_visits) + std::sqrt(2.0 * std::log(double(parent->num_visits)) / num_visits); 
      //return double(value) + discovergpar.conf();
    }
  public:
    SearchTreeNodeT* parent;		// parent of this TreeNode
    int num_visits;			// number of times TreeNode has been visited
    double value;			// value of this TreeNode
    int depth;
    int node_id;
    bool is_expand;
    std::vector< SearchTreeNodeT* > children;	// all current children
};

}  // namespace gmine_new

#endif