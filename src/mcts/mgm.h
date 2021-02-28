#ifndef _DMINE_ALGO_H
#define _DMINE_ALGO_H

#include <omp.h>

#include <algorithm>
#include <chrono>
#include <queue>

#include "math.h"
#include "stdio.h"
//#include <torch/torch.h>
//#include <torch/script.h>
#include <unistd.h>

#include "get_datagraph_information.h"
#include "gpar.h"
#include "gundam/algorithm/dp_iso.h"
#include "gundam/algorithm/vf2.h"
#include "gundam/io/csvgraph.h"
#include "search_tree_node.h"
namespace gmine_new {
omp_lock_t filter_lock;
int MAX_THREAD_NUM = 16;
int gen_maxid = 0;
int gen_maxnodeid = 0;
const double EPS = 1e-6;

/**
 * @input:  Begin and end time stamp
 * @output: Time interval
 * @func:   Cal the time
 */
template <typename T>
inline constexpr double CalTimeOrigin(T& begin, T& end) {
  return double(
             std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                 .count()) *
         std::chrono::microseconds::period::num /
         std::chrono::microseconds::period::den;
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeInPatternOrigin(const GPAR& gpar,
                                      const VertexLabelSet& vertex_label_set,
                                      const EdgeLabelSet& edge_label_set,
                                      const EdgeTypeSet& edge_type_set,
                                      GPARList& expand_gpar_list);
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeOutPatternOrigin(const GPAR& gpar,
                                       const VertexLabelSet& vertex_label_set,
                                       const EdgeLabelSet& edge_label_set,
                                       const EdgeTypeSet& edge_type_set,
                                       GPARList& expand_gpar_list);
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void GPARExpandOrigin(const GPAR& gpar,
                             const VertexLabelSet& vertex_label_set,
                             const EdgeLabelSet& edge_label_set,
                             const EdgeTypeSet& edge_type_set,
                             GPARList& expand_gpar_list);
template <class GPARList>
inline void UniqueGPAROrigin(GPARList& gpar_list);
template <class GPAR, class DataGraph>
inline void CalSuppQ(GPAR& gpar, DataGraph& data_graph);
template <class GPAR, class DataGraph>
inline void CalSuppR(GPAR& gpar, DataGraph& data_graph);
template <class GPARList, typename SuppType, class DataGraph>
inline void FilterGPARUsingSuppRLimit(GPARList& gpar_list,
                                      const DataGraph& data_graph,
                                      const SuppType& sigma,
                                      const double& conf_limit,
                                      const double& PB);
template <class GPAR, class DataGraph>
inline double diff(GPAR& query, GPAR& target, const DataGraph& data_graph);
template <class GPAR, class DataGraph>
inline double conf(const GPAR& gpar, const DataGraph& data_graph);
template <class GPAR, class DataGraph>
inline double F(GPAR& query, GPAR& target, const DataGraph& data_graph,
                size_t k);
template <class GPAR, class TopKContainerType>
inline bool GPARInTopKContainer(const GPAR& gpar,
                                const TopKContainerType& top_k_container);
template <class GPAR>
inline bool CheckIsIsoOrigin(const GPAR& query, const GPAR& target);
template <class TopKContainerType, class GPARList, class GPAR, class DataGraph>
inline void IncDiv(TopKContainerType& top_k_container, size_t k, GPAR& gpar,
                   GPARList& total_gpar, const DataGraph& data_graph);
template <class GPAR, class DataGraph>
inline bool IsExtendable(const GPAR& gpar, const DataGraph& data_graph);

template <class GPARList>
inline void FilterGPARUsingSubjectiveAssessment(GPARList& gpar_list);
template <class Pattern, class DataGraph>
inline double get_sim_expand_score(
    DiscoverdGPAR<Pattern, DataGraph>& gpar,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const size_t& d);
template <class Pattern, class DataGraph>
inline void OutputDiscoverResult(SearchTreeNodeT<Pattern, DataGraph>* root,
                                 const size_t d, const char* output_dir,
                                 const double& conf_limit, const size_t& k);
template <class Pattern, class DataGraph>
inline void ShowSearchTree(SearchTreeNodeT<Pattern, DataGraph>* root);
template <class Pattern, class DataGraph>
inline SearchTreeNodeT<Pattern, DataGraph>* Select(
    SearchTreeNodeT<Pattern, DataGraph>* search_tree_root);
template <class Pattern, class DataGraph>
inline SearchTreeNodeT<Pattern, DataGraph>* SelectTreeRoad(
    SearchTreeNodeT<Pattern, DataGraph>* search_tree_root);
template <class Pattern, class DataGraph>
inline void Expand(
    SearchTreeNodeT<Pattern, DataGraph>* best_node,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const double& PB, const size_t& k);
template <class Pattern, class DataGraph>
inline void Sim(
    SearchTreeNodeT<Pattern, DataGraph>* best_node,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const size_t& d, const double& PB);
template <class Pattern, class DataGraph>
inline void Update(SearchTreeNodeT<Pattern, DataGraph>* best_node);
template <class Pattern, class DataGraph>
inline void Sim_Add_Tree_Node(
    SearchTreeNodeT<Pattern, DataGraph>* best_node,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const size_t& d, const double& PB);
template <class Pattern, class DataGraph, class GPARList>
inline void UniqueSearchTreeNode(
    SearchTreeNodeT<Pattern, DataGraph>* search_tree_root);
template <class Pattern, class DataGraph>
inline bool IsSearchOver(SearchTreeNodeT<Pattern, DataGraph>* search_tree_root,
                         const size_t& sigma, const double& conf_limit,
                         const size_t& d);

/**
 * @input:  G,q(x,y),support_limit,conf_limit,edgesize_limit,topk,output_fir
 * @output: Set of Graph Pattern Association Rule
 * @func:   Use mcts algo to quickly mine good gpar
 */
template <class Pattern, class DataGraph>
inline void MGM(
    int argc, char* argv[], const char* v_file,
    const char* e_file,  // data graph
    const typename Pattern::VertexType::LabelType x_node_label,
    const typename Pattern::VertexType::LabelType y_node_label,
    const typename Pattern::EdgeType::LabelType q_edge_label,  // q(x,y)
    const size_t sigma,                                        // supp_r limit
    const double conf_limit,                                   // conf limit
    const size_t k,                                            // top-k
    const size_t d,                                            // radius of GPAR
    const char* output_dir) {
  omp_set_num_threads(MAX_THREAD_NUM);
  using DataGraphVertexLabelType = typename DataGraph::VertexType::LabelType;
  using DataGraphEdgeLabelType = typename DataGraph::EdgeType::LabelType;
  gen_maxid = 0;
  gen_maxnodeid = 0;
  // data graph
  DataGraph data_graph;
  GUNDAM::ReadCSVGraph(data_graph, v_file, e_file);
  // DataGraph Information
  std::set<DataGraphVertexLabelType> vertex_label_set;
  std::set<DataGraphEdgeLabelType> edge_label_set;
  std::set<std::tuple<DataGraphVertexLabelType, DataGraphVertexLabelType,
                      DataGraphEdgeLabelType>>
      edge_type_set;
  GetDataGraphInformation(data_graph, vertex_label_set, edge_label_set,
                          edge_type_set);
  using SuppContainer = std::set<typename DataGraph::VertexConstPtr>;
  using SuppType = size_t;
  using ConfType = double;
  using GPARList = std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
  using TopKContainerType =
      std::vector<std::pair<DiscoverdGPAR<Pattern, DataGraph>,
                            DiscoverdGPAR<Pattern, DataGraph>>>;
  TopKContainerType top_k_container;
  DiscoverdGPAR<Pattern, DataGraph> root_gpar(x_node_label, y_node_label,
                                              q_edge_label);
  root_gpar.Setid(gen_maxid++);
  CalSuppQ(root_gpar, data_graph);
  CalSuppR(root_gpar, data_graph);
  size_t supp_r_size = root_gpar.supp_R_size();
  if (supp_r_size < sigma) {
    return;
  }
  omp_init_lock(&filter_lock);
  const double PB =
      (root_gpar.supp_R_size() * 1.0) / (data_graph.CountVertex() * 1.0);
  // std::cout << "q = " << root_gpar.supp_R_size() << " ;node = " <<
  // data_graph.CountVertex() << " ;PB = " << PB << std::endl;
  SearchTreeNodeT<Pattern, DataGraph>* search_tree_root =
      new SearchTreeNodeT<Pattern, DataGraph>();
  search_tree_root->discovergpar = root_gpar;
  search_tree_root->node_id = gen_maxnodeid++;
  search_tree_root->is_expand = true;
  SearchTreeNodeT<Pattern, DataGraph>* best_node;
  int round_num = 0;
  auto begin = std::chrono::system_clock::now();
  while (!IsSearchOver(search_tree_root, sigma, conf_limit, d)) {
    auto r_begin = std::chrono::system_clock::now();
    // mgm
    std::cout << "start select" << std::endl;
    auto mcts_begin = std::chrono::system_clock::now();
    best_node = Select(search_tree_root);
    // best_node = SelectTreeRoad(search_tree_root);
    auto mcts_end = std::chrono::system_clock::now();
    std::cout << "mcts : select time = " << CalTimeOrigin(mcts_begin, mcts_end)
              << std::endl;
    mcts_begin = std::chrono::system_clock::now();
    Expand(best_node, vertex_label_set, edge_label_set, edge_type_set,
           data_graph, sigma, conf_limit, PB, k);
    mcts_end = std::chrono::system_clock::now();
    std::cout << "mcts : expand time = " << CalTimeOrigin(mcts_begin, mcts_end)
              << std::endl;
    mcts_begin = std::chrono::system_clock::now();
    Sim(best_node, vertex_label_set, edge_label_set, edge_type_set, data_graph,
        sigma, conf_limit, d, PB);
    mcts_end = std::chrono::system_clock::now();
    std::cout << "mcts : sim time = " << CalTimeOrigin(mcts_begin, mcts_end)
              << std::endl;
    mcts_begin = std::chrono::system_clock::now();
    Update(best_node);
    mcts_end = std::chrono::system_clock::now();
    std::cout << "mcts : update time = " << CalTimeOrigin(mcts_begin, mcts_end)
              << std::endl;
    ShowSearchTree(search_tree_root);
    auto r_end = std::chrono::system_clock::now();
    round_num++;
    std::cout << "MCTS round " << round_num << " cal time is "
              << CalTimeOrigin(r_begin, r_end) << std::endl;
  }
  auto end = std::chrono::system_clock::now();
  std::cout << "MCTS total time = " << CalTimeOrigin(begin, end) << std::endl;
  OutputDiscoverResult(search_tree_root, d, output_dir, conf_limit, k);
  return;
}

/**
 * @input:  The root node of search tree,support_limit,conf_limit,edgesize_limit
 * @output: Termination flag(bool)
 * @func:   Determine whether the algorithm is terminated
 */
template <class Pattern, class DataGraph>
inline bool IsSearchOver(SearchTreeNodeT<Pattern, DataGraph>* search_tree_root,
                         const size_t& sigma, const double& conf_limit,
                         const size_t& d) {
  bool flag = true;
  if (search_tree_root->children.size() == 0) return false;
  std::stack<SearchTreeNodeT<Pattern, DataGraph>*> node_stack;
  SearchTreeNodeT<Pattern, DataGraph>* tmp;
  for (auto child : search_tree_root->children) {
    node_stack.push(child);
  }
  int num = 0;
  int all_node = 0, is_expand_node = 0;
  while (!node_stack.empty()) {
    tmp = node_stack.top();
    all_node++;
    node_stack.pop();
    if (tmp->discovergpar.conf() < conf_limit + EPS &&
        tmp->discovergpar.supp_R_size() >= sigma && !tmp->is_expand &&
        tmp->discovergpar.pattern.CountEdge() < d) {
      flag = false;
      is_expand_node++;
    } else {
      tmp->is_expand = true;
    }
    if (tmp->children.size() != 0) {
      for (auto child : tmp->children) {
        node_stack.push(child);
      }
    }
  }
  std::cout << "Can expand node size is " << is_expand_node
            << "; all node size is " << all_node << std::endl;
  return flag;
}

/**
 * @input:  The root node of search tree
 * @output: The most potential node
 * @func:   Use ucb algo to select the most potential node
 */
template <class Pattern, class DataGraph>
inline SearchTreeNodeT<Pattern, DataGraph>* Select(
    SearchTreeNodeT<Pattern, DataGraph>* search_tree_root) {
  // Select
  SearchTreeNodeT<Pattern, DataGraph>* best_node;
  double max_score = -1;
  if (search_tree_root->children.size() == 0) {
    best_node = search_tree_root;
  } else {
    std::stack<SearchTreeNodeT<Pattern, DataGraph>*> node_stack;
    SearchTreeNodeT<Pattern, DataGraph>* tmp;
    for (auto child : search_tree_root->children) {
      node_stack.push(child);
    }
    while (!node_stack.empty()) {
      tmp = node_stack.top();
      double tmp_score = tmp->get_score();
      if ((tmp_score + EPS) > max_score && !tmp->is_expand) {
        max_score = tmp_score;
        best_node = tmp;
      }
      node_stack.pop();
      if (tmp->children.size() != 0) {
        for (auto child : tmp->children) {
          node_stack.push(child);
        }
      }
    }
  }
  best_node->is_expand = true;
  std::cout << "The best node of search tree: depth = "
            << best_node->get_depth() << " ;id = " << best_node->node_id
            << " ;score = " << max_score
            << " ;support = " << best_node->discovergpar.supp_R_size()
            << " ;conf = " << best_node->discovergpar.conf() << std::endl;
  return best_node;
}
template <class Pattern, class DataGraph>
inline SearchTreeNodeT<Pattern, DataGraph>* getMaxScoreNode(
    SearchTreeNodeT<Pattern, DataGraph>* node) {
  int maxValue = -1;
  SearchTreeNodeT<Pattern, DataGraph>* tmp;
  for (auto child : node->children) {
    if (child->get_score() > maxValue) {
      maxValue = child->get_score();
      tmp = child;
    }
  }
  return tmp;
}

/**
 * @input:  The root node of search tree
 * @output: The most potential node
 * @func:   Use level traversal and ucb algo to select the most potential node
 */
template <class Pattern, class DataGraph>
inline SearchTreeNodeT<Pattern, DataGraph>* SelectTreeRoad(
    SearchTreeNodeT<Pattern, DataGraph>* search_tree_root) {
  // Select
  SearchTreeNodeT<Pattern, DataGraph>* best_node = search_tree_root;
  double max_score = -1;
  if (search_tree_root->children.size() == 0) {
    best_node = search_tree_root;
  } else {
    while (best_node->children.size() != 0) {
      best_node = getMaxScoreNode(best_node);
    }
  }
  best_node->is_expand = true;
  std::cout << "The best node of search tree: depth = "
            << best_node->get_depth() << " ;id = " << best_node->node_id
            << " ;score = " << max_score
            << " ;support = " << best_node->discovergpar.supp_R_size()
            << " ;conf = " << best_node->discovergpar.conf() << std::endl;
  return best_node;
}

/**
 * @input:  The most potential node
 * @output: Child node of the most potential node
 * @func:   Expand the gpar of the most potential node as child node
 */
template <class Pattern, class DataGraph>
inline void Expand(
    SearchTreeNodeT<Pattern, DataGraph>* best_node,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const double& PB, const size_t& k) {
  // Expand
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> this_round_gpar;
  auto singlegpar = best_node->discovergpar;
  GPARExpandOrigin(singlegpar, vertex_label_set, edge_label_set, edge_type_set,
                   this_round_gpar);
  UniqueGPAROrigin(this_round_gpar);
  std::cout << "gen " << this_round_gpar.size() << " gpars." << std::endl;
  FilterGPARUsingSubjectiveAssessment(this_round_gpar);
  FilterGPARUsingSuppRLimit(this_round_gpar, data_graph, sigma, conf_limit, PB);
  std::cout << "over filter " << this_round_gpar.size() << " gpars."
            << std::endl;
  for (auto gpar : this_round_gpar) {
    SearchTreeNodeT<Pattern, DataGraph>* treenode =
        new SearchTreeNodeT<Pattern, DataGraph>();
    treenode->discovergpar = gpar;
    treenode->node_id = gen_maxnodeid++;
    treenode->parent = best_node;
    treenode->is_expand = false;
    treenode->depth = best_node->depth + 1;
    best_node->children.push_back(treenode);
  }
}

/**
 * @input:  Child node of the most potential node
 * @output: Cal maximum potential value of child node
 * @func:   Simulation potential of child node
 */
template <class Pattern, class DataGraph>
inline void Sim(
    SearchTreeNodeT<Pattern, DataGraph>* best_node,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const size_t& d, const double& PB) {
  // Sim
#pragma omp parallel for schedule(static, MAX_THREAD_NUM)
  for (int i = 0; i < best_node->children.size(); i++) {
    // sim score
    best_node->children[i]->value = get_sim_expand_score(
        best_node->children[i]->discovergpar, vertex_label_set, edge_label_set,
        edge_type_set, data_graph, sigma, conf_limit, d, PB);
    // conf
    best_node->children[i]->value +=
        best_node->children[i]->discovergpar.conf();
    // supp
    best_node->children[i]->value +=
        best_node->children[i]->discovergpar.supp_R().size();
  }
}

/**
 * @input:  The most potential node and Child node
 * @output: NULL
 * @func:   Update search tree
 */
template <class Pattern, class DataGraph>
inline void Update(SearchTreeNodeT<Pattern, DataGraph>* best_node) {
  // Update
  double sum = 0;
  for (auto child : best_node->children) {
    sum += child->value;
  }
  best_node->num_visits++;
  if (best_node->children.size() != 0) {
    best_node->value += sum / best_node->children.size();
    while (best_node->parent != NULL) {
      best_node->parent->num_visits++;
      best_node->parent->value += best_node->value;
      best_node = best_node->parent;
    }
  }
}

/**
 * @input:  The root node of search tree
 * @output: NULL
 * @func:   Get the node information of search tree
 */
template <class Pattern, class DataGraph>
inline void ShowSearchTree(SearchTreeNodeT<Pattern, DataGraph>* root) {
  // print search tree
  int* log = new int[20];
  int num = 0;
  double score = 0;
  for (int i = 0; i < 20; i++) log[i] = 0;
  std::stack<SearchTreeNodeT<Pattern, DataGraph>*> node_stack;
  SearchTreeNodeT<Pattern, DataGraph>* tmp;
  for (auto child : root->children) {
    node_stack.push(child);
  }
  while (!node_stack.empty()) {
    tmp = node_stack.top();
    double tmp_score = tmp->discovergpar.conf();
    score += tmp_score;
    num++;
    int index, mod;
    index = tmp_score / 0.05;
    mod = fmod(tmp_score, 0.05) > EPS ? 1 : 0;
    log[index + mod - 1]++;
    node_stack.pop();
    if (tmp->children.size() != 0) {
      for (auto child : tmp->children) {
        node_stack.push(child);
      }
    }
  }
  std::cout << "-----------------result-----------------" << std::endl;
  for (int i = 0; i < 20; i++) {
    std::cout << "conf = " << i * 0.05 << "-" << (i + 1) * 0.05 << " : "
              << log[i] << std::endl;
  }
  if (num != 0) std::cout << "avg conf = " << score / num << std::endl;
  std::cout << "------------------over------------------" << std::endl;
  return;
}
template <class Pattern, class DataGraph>
inline double get_sim_expand_score(
    DiscoverdGPAR<Pattern, DataGraph>& gpar,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const size_t& d, const double& PB) {
  using GPARList = std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
  GPARList this_round_gpar, tmp_gpar_list;
  DiscoverdGPAR<Pattern, DataGraph> singlegpar = gpar;
  while (singlegpar.supp_R_size() >= sigma &&
         singlegpar.conf() < conf_limit + EPS &&
         singlegpar.pattern.CountEdge() < d) {
    GPARExpandOrigin(singlegpar, vertex_label_set, edge_label_set,
                     edge_type_set, this_round_gpar);
    if (tmp_gpar_list.size() == 0) {
      break;
    }
    UniqueGPAROrigin(this_round_gpar);
    FilterGPARUsingSubjectiveAssessment(this_round_gpar);
    FilterGPARUsingSuppRLimit(this_round_gpar, data_graph, sigma, conf_limit,
                              PB);
    if (this_round_gpar.size() == 0) {
      break;
    }
    // get conf max
    std::sort(this_round_gpar.begin(), this_round_gpar.end(),
              [](DiscoverdGPAR<Pattern, DataGraph>& a,
                 DiscoverdGPAR<Pattern, DataGraph>& b) {
                return a.conf() > b.conf();
              });
    singlegpar = this_round_gpar[0];
    this_round_gpar.clear();
    tmp_gpar_list.clear();
  }
  return singlegpar.conf();
}
template <class Pattern, class DataGraph>
inline void Sim_Add_Tree_Node(
    SearchTreeNodeT<Pattern, DataGraph>* best_node,
    std::set<typename DataGraph::VertexType::LabelType>& vertex_label_set,
    std::set<typename DataGraph::EdgeType::LabelType>& edge_label_set,
    std::set<std::tuple<typename DataGraph::VertexType::LabelType,
                        typename DataGraph::VertexType::LabelType,
                        typename DataGraph::EdgeType::LabelType>>&
        edge_type_set,
    DataGraph& data_graph, const size_t& sigma, const double& conf_limit,
    const size_t& d, const double& PB) {
  // Sim
  for (int i = 0; i < best_node->children.size(); i++) {
    std::vector<DiscoverdGPAR<Pattern, DataGraph>> this_round_gpar,
        tmp_gpar_list;
    DiscoverdGPAR<Pattern, DataGraph> singlegpar =
        best_node->children[i]->discovergpar;
    SearchTreeNodeT<Pattern, DataGraph>* tmp = best_node->children[i];
    while (singlegpar.supp_R_size() >= sigma &&
           singlegpar.conf() < conf_limit + EPS &&
           singlegpar.pattern.CountEdge() < d) {
      GPARExpandOrigin(singlegpar, vertex_label_set, edge_label_set,
                       edge_type_set, tmp_gpar_list);
      for (int index = 0; index < 1 && index < tmp_gpar_list.size(); index++) {
        this_round_gpar.push_back(tmp_gpar_list[index]);
      }
      UniqueGPAROrigin(this_round_gpar);
      FilterGPARUsingSubjectiveAssessment(this_round_gpar);
      FilterGPARUsingSuppRLimit(this_round_gpar, data_graph, sigma, conf_limit,
                                PB);

      SearchTreeNodeT<Pattern, DataGraph>* treenode =
          new SearchTreeNodeT<Pattern, DataGraph>();
      treenode->discovergpar = this_round_gpar[0];
      treenode->parent = tmp;
      treenode->node_id = gen_maxnodeid++;
      treenode->depth = tmp->depth + 1;
      tmp->children.push_back(treenode);

      singlegpar = this_round_gpar[0];
      tmp = treenode;

      this_round_gpar.clear();
      tmp_gpar_list.clear();
    }
    int values = 0;
    values = singlegpar.conf();
    std::queue<SearchTreeNodeT<Pattern, DataGraph>*> nodes;
    nodes.push(best_node->children[i]);
    while (!nodes.empty()) {
      SearchTreeNodeT<Pattern, DataGraph>* child_tmp = nodes.front();
      nodes.pop();
      child_tmp->value = values;
      for (auto child : child_tmp->children) nodes.push(child);
    }
  }
}

/**
 * @input:  The root node of search tree
 * @output: NULL
 * @func:   Delete redundant nodes
 */
template <class Pattern, class DataGraph, class GPARList>
inline void UniqueSearchTreeNode(
    SearchTreeNodeT<Pattern, DataGraph>* search_tree_root) {
  using GPAR = typename GPARList::value_type;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  std::vector<SearchTreeNodeT<Pattern, DataGraph>*> all_search_tree_node;
  all_search_tree_node.push_back(search_tree_root);
  int index = 0;
  while (index < all_search_tree_node.size()) {
    for (auto child : all_search_tree_node[index]->children) {
      all_search_tree_node.push_back(child);
    }
    index++;
  }
  std::vector<SearchTreeNodeT<Pattern, DataGraph>*> unique_search_tree_node;
  for (const auto& node : all_search_tree_node) {
    bool vf2_flag = false;
    for (const auto& target : unique_search_tree_node) {
      MatchResultList match_result;
      GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
          node->discovergpar.pattern, target->discovergpar.pattern,
          node->discovergpar.x_node_ptr()->id(),
          target->discovergpar.x_node_ptr()->id(),
          GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
          GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1,
          match_result);
      if (match_result.size() >= 1) {
        vf2_flag = true;
        break;
      }
    }
    if (!vf2_flag) {
      unique_search_tree_node.push_back(node);
    } else {
      SearchTreeNodeT<Pattern, DataGraph>* tmp = node->parent;
      node->parent = NULL;
      auto iter = tmp->children.begin();
      while (*iter != node) iter++;
      tmp->children.erase(iter);
    }
  }
}

/**
 * @input:  The expanded gpar lists
 * @output: Interested gpar
 * @func:   Use DL model to select interested gpar
 */
template <class GPARList>
inline void FilterGPARUsingSubjectiveAssessment(GPARList& gpar_list) {
  // Subjective Assessment to filter gpar lists
  // std::cout << "Subjective Assessment to filter gpar lists" << std::endl;
  /*
  GPARList filter_gpar_list;
  std::shared_ptr<torch::jit::script::Module> module =
  std::make_shared<torch::jit::script::Module>(torch::jit::load("../config/model.pt"));
  for (int index = 0; index < gpar_list.size(); index++) {
    auto gpar = gpar_list[index];
    std::vector<torch::jit::IValue> inputs;
    std::vector<std::vector<float>> ss(2708, std::vector<float>(1433, 0));
    for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      ss[vertex_it->id()][0] = vertex_it->label();
    }
    torch::Tensor feature = torch::from_blob(ss.data(), {2708, 1433},
  torch::kF32); std::vector<std::vector<float>> sss(2708,
  std::vector<float>(2708, 0)); for (auto vertex_it =
  gpar.pattern.VertexCBegin(); !vertex_it.IsDone(); vertex_it++) { for (auto
  edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone(); edge_it++) {
        sss[edge_it->src_id()][edge_it->dst_id()] = 1;
      }
    }
    torch::Tensor adj = torch::from_blob(sss.data(), {2708, 2708}, torch::kF32);
    feature = feature.to(torch::kF32);
    adj = adj.to(torch::kF32);
    inputs.push_back(feature);
    inputs.push_back(adj);
    torch::Tensor output = module->forward(inputs).toTensor();
    auto max_result = output.max(0, true);
    auto max_index = std::get<1>(max_result).item<float>();
    if(max_index == 0)
      filter_gpar_list.push_back(gpar_list[index]);
  }
  std::swap(filter_gpar_list, gpar_list);
  */
  return;
}

/**
 * @input:  Gpar
 * @output: Expaned gpar
 * @func:   add an edge
 */
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void GPARExpandOrigin(const GPAR& gpar,
                             const VertexLabelSet& vertex_label_set,
                             const EdgeLabelSet& edge_label_set,
                             const EdgeTypeSet& edge_type_set,
                             GPARList& expand_gpar_list) {
  AddNewEdgeInPatternOrigin(gpar, vertex_label_set, edge_label_set,
                            edge_type_set, expand_gpar_list);
  AddNewEdgeOutPatternOrigin(gpar, vertex_label_set, edge_label_set,
                             edge_type_set, expand_gpar_list);
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeInPatternOrigin(const GPAR& gpar,
                                      const VertexLabelSet& vertex_label_set,
                                      const EdgeLabelSet& edge_label_set,
                                      const EdgeTypeSet& edge_type_set,
                                      GPARList& expand_gpar_list) {
  using VertexSizeType = typename GPAR::VertexSizeType;
  using VertexIDType = typename GPAR::VertexIDType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  VertexSizeType pattern_size = gpar.pattern.CountVertex();
  VertexIDType x_node_id = gpar.x_node_ptr()->id();
  VertexIDType y_node_id = gpar.y_node_ptr()->id();
  EdgeLabelType q_edge_label = gpar.q_edge_label();
  for (auto src_node_iter = gpar.pattern.VertexCBegin();
       !src_node_iter.IsDone(); src_node_iter++) {
    for (auto dst_node_iter = gpar.pattern.VertexCBegin();
         !dst_node_iter.IsDone(); dst_node_iter++) {
      // not add self loop
      if (src_node_iter->id() == dst_node_iter->id()) continue;
      if (src_node_iter->id() == x_node_id && dst_node_iter->id() == y_node_id)
        continue;
      for (const auto& possible_edge_label : edge_label_set) {
        // not add edge which does not exist in data graph
        if (!edge_type_set.count(typename EdgeTypeSet::value_type(
                src_node_iter->label(), dst_node_iter->label(),
                possible_edge_label))) {
          continue;
        }
        GPAR expand_gpar(gpar);
        expand_gpar.pattern.AddEdge(src_node_iter->id(), dst_node_iter->id(),
                                    possible_edge_label,
                                    expand_gpar.pattern.CountEdge() + 1);
        expand_gpar.Setid(gen_maxid++);
        expand_gpar_list.emplace_back(std::move(expand_gpar));
      }
    }
  }
}
template <class GPAR, class VertexLabelSet, class EdgeLabelSet,
          class EdgeTypeSet, class GPARList>
inline void AddNewEdgeOutPatternOrigin(const GPAR& gpar,
                                       const VertexLabelSet& vertex_label_set,
                                       const EdgeLabelSet& edge_label_set,
                                       const EdgeTypeSet& edge_type_set,
                                       GPARList& expand_gpar_list) {
  using VertexSizeType = typename GPAR::VertexSizeType;
  using VertexIDType = typename GPAR::VertexIDType;
  using EdgeLabelType = typename GPAR::EdgeLabelType;
  VertexSizeType pattern_size = gpar.pattern.CountVertex();
  VertexIDType x_node_id = gpar.x_node_ptr()->id();
  VertexIDType y_node_id = gpar.y_node_ptr()->id();
  EdgeLabelType q_edge_label = gpar.q_edge_label();
  VertexIDType new_vertex_id = static_cast<VertexIDType>(pattern_size + 1);

  for (auto node_iter = gpar.pattern.VertexCBegin(); !node_iter.IsDone();
       node_iter++) {
    if (node_iter->id() == y_node_id) continue;
    for (const auto& possible_vertex_label : vertex_label_set) {
      for (const auto& possible_edge_label : edge_label_set) {
        // not add edge which does not exist in data graph
        if (edge_type_set.count(typename EdgeTypeSet::value_type(
                node_iter->label(), possible_vertex_label,
                possible_edge_label))) {
          GPAR expand_gpar(gpar);
          expand_gpar.pattern.AddVertex(new_vertex_id, possible_vertex_label);
          expand_gpar.pattern.AddEdge(node_iter->id(), new_vertex_id,
                                      possible_edge_label,
                                      expand_gpar.pattern.CountEdge() + 1);
          expand_gpar.Setid(gen_maxid++);
          expand_gpar_list.emplace_back(std::move(expand_gpar));
        }
        if (edge_type_set.count(typename EdgeTypeSet::value_type(
                possible_vertex_label, node_iter->label(),
                possible_edge_label))) {
          GPAR expand_gpar(gpar);
          expand_gpar.pattern.AddVertex(new_vertex_id, possible_vertex_label);
          expand_gpar.pattern.AddEdge(new_vertex_id, node_iter->id(),
                                      possible_edge_label,
                                      expand_gpar.pattern.CountEdge() + 1);
          expand_gpar.Setid(gen_maxid++);
          expand_gpar_list.emplace_back(std::move(expand_gpar));
        }
      }
    }
  }
}
template <class GPARList>
inline void UniqueGPAROrigin(GPARList& gpar_list) {
  using GPAR = typename GPARList::value_type;
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  GPARList unique_gpar_list;
  for (const auto& query : gpar_list) {
    bool find_iso_flag = false;
    for (const auto& target : unique_gpar_list) {
      MatchResultList match_result;
      GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
          query.pattern, target.pattern, query.x_node_ptr()->id(),
          target.x_node_ptr()->id(),
          GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
          GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1,
          match_result);
      if (match_result.size() >= 1) {
        find_iso_flag = true;
        break;
      }
    }
    if (!find_iso_flag) {
      unique_gpar_list.push_back(query);
    }
  }
  std::swap(gpar_list, unique_gpar_list);
}
template <class GPAR, class DataGraph>
inline void CalSuppQ(GPAR& gpar, DataGraph& data_graph) {
  using SuppType = std::vector<typename DataGraph::VertexConstPtr>;
  using SuppSet = std::set<typename DataGraph::VertexConstPtr>;
  using MatchMap = std::map<typename GPAR::VertexConstPtr,
                            typename DataGraph::VertexConstPtr>;
  using CandidateSetType =
      std::map<typename GPAR::VertexConstPtr,
               std::vector<typename DataGraph::VertexConstPtr>>;
  using MatchResult = std::vector<MatchMap>;
  SuppType& supp_Q = gpar.supp_Q();
  SuppType temp_supp_Q;
  if (supp_Q.empty()) {
    SuppSet temp_supp_set;
    GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
        gpar.pattern, data_graph, [&gpar, &temp_supp_set](auto& match_state) {
          temp_supp_set.insert(match_state.find(gpar.x_node_ptr())->second);
          return true;
        });
    for (const auto& it : temp_supp_set) {
      supp_Q.push_back(it);
    }
  } else {
    GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
        gpar.pattern, data_graph, gpar.x_node_ptr(), supp_Q, temp_supp_Q);
    supp_Q = temp_supp_Q;
    return;

    CandidateSetType candidate_set;
    if (!GUNDAM::_vf2::InitCandidateSet<GUNDAM::MatchSemantics::kIsomorphism>(
            gpar.pattern, data_graph,
            GUNDAM::_vf2::LabelEqual<typename GPAR::VertexConstPtr,
                                     typename DataGraph::VertexConstPtr>(),
            candidate_set)) {
      return;
    }
    for (const auto& target_ptr : supp_Q) {
      MatchResult match_result;
      auto t_begin = clock();
      GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
          gpar.pattern, data_graph, candidate_set, gpar.x_node_ptr()->id(),
          target_ptr->id(),
          GUNDAM::_vf2::LabelEqual<typename GPAR::VertexConstPtr,
                                   typename DataGraph::VertexConstPtr>(),
          GUNDAM::_vf2::LabelEqual<typename GPAR::EdgeConstPtr,
                                   typename DataGraph::EdgeConstPtr>(),
          1, match_result);
      auto t_end = clock();
      if (match_result.size() >= 1) {
        temp_supp_Q.push_back(target_ptr);
      }
    }
    supp_Q = temp_supp_Q;
  }
}
template <class GPAR, class DataGraph>
inline void CalSuppR(GPAR& gpar, DataGraph& data_graph) {
  using SuppType = std::vector<typename DataGraph::VertexConstPtr>;
  using SuppSet = std::set<typename DataGraph::VertexConstPtr>;
  using MatchMap = std::map<typename GPAR::VertexConstPtr,
                            typename DataGraph::VertexConstPtr>;
  using MatchResult = std::vector<MatchMap>;
  using CandidateSetType =
      std::map<typename GPAR::VertexConstPtr,
               std::vector<typename DataGraph::VertexConstPtr>>;
  SuppType& supp_R = gpar.supp_R();
  SuppType temp_supp_R;
  size_t gpar_edge = gpar.pattern.CountEdge();
  gpar.pattern.AddEdge(gpar.x_node_ptr()->id(), gpar.y_node_ptr()->id(),
                       gpar.q_edge_label(), gpar_edge + 1);
  if (supp_R.empty()) {
    SuppSet temp_supp_set;
    GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
        gpar.pattern, data_graph, [&gpar, &temp_supp_set](auto& match_state) {
          temp_supp_set.insert(match_state.find(gpar.x_node_ptr())->second);
          return true;
        });
    for (const auto& it : temp_supp_set) {
      supp_R.push_back(it);
    }
  } else {
    GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
        gpar.pattern, data_graph, gpar.x_node_ptr(), supp_R, temp_supp_R);
    supp_R = temp_supp_R;
    gpar.pattern.EraseEdge(gpar_edge + 1);
    return;

    CandidateSetType candidate_set;
    if (!GUNDAM::_vf2::InitCandidateSet<GUNDAM::MatchSemantics::kIsomorphism>(
            gpar.pattern, data_graph,
            GUNDAM::_vf2::LabelEqual<typename GPAR::VertexConstPtr,
                                     typename DataGraph::VertexConstPtr>(),
            candidate_set)) {
      return;
    }
    for (const auto& target_ptr : supp_R) {
      MatchResult match_result;
      GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
          gpar.pattern, data_graph, candidate_set, gpar.x_node_ptr()->id(),
          target_ptr->id(),
          GUNDAM::_vf2::LabelEqual<typename GPAR::VertexConstPtr,
                                   typename DataGraph::VertexConstPtr>(),
          GUNDAM::_vf2::LabelEqual<typename GPAR::EdgeConstPtr,
                                   typename DataGraph::EdgeConstPtr>(),
          1, match_result);
      if (match_result.size() >= 1) {
        temp_supp_R.push_back(target_ptr);
      }
    }
    supp_R = temp_supp_R;
  }
  gpar.pattern.EraseEdge(gpar_edge + 1);
}

/**
 * @input:  Expanded gpar
 * @output: high support and conf
 * @func:   use support_limit, conf_limit and lift to select gpar
 */
template <class GPARList, typename SuppType, class DataGraph>
inline void FilterGPARUsingSuppRLimit(GPARList& gpar_list,
                                      const DataGraph& data_graph,
                                      const SuppType& sigma,
                                      const double& conf_limit,
                                      const double& PB) {
  GPARList filter_gpar_list;
  using SuppContainerType = std::set<typename DataGraph::VertexConstPtr>;
  int index = 0;
#pragma omp parallel for schedule(static, MAX_THREAD_NUM)
  for (auto& gpar : gpar_list) {
    CalSuppQ(gpar, data_graph);
    CalSuppR(gpar, data_graph);
    omp_set_lock(&filter_lock);
    if (gpar.supp_R().size() >= sigma) {
      gpar.CalConf();
      /*
          //全置信度
          double all_conf = gpar.supp_R_size()/max(gpar.supp_Q_size(),
         PB*data_graph.CountVertex());
          //最大置信度
          double max_conf = max(gpar.conf(),
         gpar.supp_R_size()/(PB*data_graph.CountVertex()));
          //Kulczynski
          double kulc = (gpar.conf() +
         gpar.supp_R_size()/(PB*data_graph.CountVertex()))/2;
          //余弦度量
          double cos = gpar.supp_R_size()/sqrt(gpar.supp_Q_size() * PB);
      */
      // std::cout << "gpar conf = " << gpar.conf() << " ;q(x,y) conf = " << PB
      // << std::endl; 提升度
      double lift = gpar.conf() / PB;
      if (lift >= 1) {
        filter_gpar_list.push_back(gpar);
      }
    }
    omp_unset_lock(&filter_lock);
  }
  std::swap(filter_gpar_list, gpar_list);
}
template <class GPAR, class DataGraph>
inline double diff(GPAR& query, GPAR& target, const DataGraph& data_graph) {
  using SuppType = std::vector<typename DataGraph::VertexConstPtr>;
  SuppType& a_supp_r = query.supp_R();
  SuppType& b_supp_r = target.supp_R();
  SuppType intersection_result;
  auto t_begin = clock();
  std::set_intersection(
      std::cbegin(a_supp_r), std::cend(a_supp_r), std::cbegin(b_supp_r),
      std::cend(b_supp_r),
      inserter(intersection_result, intersection_result.begin()));
  auto t_end = clock();
  size_t intersection_size = intersection_result.size();
  size_t union_size = a_supp_r.size() + b_supp_r.size() - intersection_size;
  return 1 - static_cast<double>(intersection_size) /
                 static_cast<double>(union_size);
}
template <class GPAR, class DataGraph>
inline double conf(const GPAR& gpar, const DataGraph& data_graph) {
  return gpar.conf();
}
template <class GPAR, class DataGraph>
inline double F(GPAR& query, GPAR& target, const DataGraph& data_graph,
                size_t k) {
  double lamda = 0.5;
  double ret = (1 - lamda) / (k - 1) *
                   (conf(query, data_graph) + conf(target, data_graph)) +
               2 * lamda / (k - 1) * diff(query, target, data_graph);
  return ret;
}
template <class GPAR>
inline bool CheckIsIsoOrigin(const GPAR& query, const GPAR& target) {
  using VertexConstPtr = typename GPAR::VertexConstPtr;
  using EdgeConstPtr = typename GPAR::EdgeConstPtr;
  using VertexIDType = typename GPAR::VertexIDType;
  using MatchMap = std::map<VertexConstPtr, VertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;
  if (query.pattern.CountVertex() != target.pattern.CountVertex()) return false;
  if (query.pattern.CountEdge() != target.pattern.CountEdge()) return false;
  MatchResultList match_result;
  VertexIDType query_x_id = query.x_node_ptr()->id();
  VertexIDType target_x_id = query.x_node_ptr()->id();
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      query.pattern, target.pattern, query_x_id, target_x_id,
      GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
      GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1, match_result);
  if (match_result.size() < 1) return false;
  match_result.clear();
  MatchResultList().swap(match_result);
  GUNDAM::VF2<GUNDAM::MatchSemantics::kIsomorphism>(
      target.pattern, query.pattern, query_x_id, target_x_id,
      GUNDAM::_vf2::LabelEqual<VertexConstPtr, VertexConstPtr>(),
      GUNDAM::_vf2::LabelEqual<EdgeConstPtr, EdgeConstPtr>(), 1, match_result);
  if (match_result.size() < 1) return false;
  return true;
}
template <class GPAR, class TopKContainerType>
inline bool GPARInTopKContainer(const GPAR& gpar,
                                const TopKContainerType& top_k_container) {
  for (const auto& it : top_k_container) {
    if (gpar.id() == it.first.id() || gpar.id() == it.second.id()) return true;
  }
  return false;
}
template <class TopKContainerType, class GPARList, class GPAR, class DataGraph>
inline void IncDiv(TopKContainerType& top_k_container, size_t k, GPAR& gpar,
                   GPARList& total_gpar, const DataGraph& data_graph) {
  size_t up_limit = k % 2 ? k / 2 + 1 : k / 2;
  if (GPARInTopKContainer(gpar, top_k_container)) {
    return;
  }
  double max_F = 0;
  std::pair<GPAR, GPAR> add_item;
  auto t_begin = clock();
  for (auto& another_gpar : total_gpar) {
    if (top_k_container.size() < up_limit &&
        another_gpar.pattern.CountEdge() != gpar.pattern.CountEdge()) {
      continue;
    }
    if (gpar.id() == another_gpar.id()) {
      continue;
    }
    if (GPARInTopKContainer(another_gpar, top_k_container)) {
      continue;
    }
    auto t_begin = clock();
    double f = F(gpar, another_gpar, data_graph, k);
    auto t_end = clock();
    if (max_F < f) {
      add_item = std::make_pair(gpar, another_gpar);
      max_F = f;
    }
  }
  auto t_end = clock();
  t_begin = clock();
  if (top_k_container.size() < up_limit) {
    top_k_container.emplace_back(std::move(add_item));
  } else {
    double min_F = std::numeric_limits<double>::max();
    size_t min_pos = -1;
    for (size_t i = 0; i < up_limit; i++) {
      double f =
          F(top_k_container[i].first, top_k_container[i].second, data_graph, k);
      if (f < min_F) {
        min_pos = i;
        min_F = f;
      }
    }
    top_k_container[min_pos] = add_item;
  }
  t_end = clock();
}
template <class GPAR, class DataGraph>
inline bool IsExtendable(const GPAR& gpar, const DataGraph& data_graph) {
  double gpar_conf = conf(gpar, data_graph);
  constexpr double eps = 1e-7;
  if (fabs(gpar_conf - 1) == eps || fabs(gpar_conf) == eps) return false;
  return true;
}

template <class Pattern, class DataGraph>
bool compareConf(const DiscoverdGPAR<Pattern, DataGraph>& value1,
                 const DiscoverdGPAR<Pattern, DataGraph>& value2) {
  return value1.conf() < value2.conf();
}

/**
 * @input:  result and output dir
 * @output: NULL
 * @func:   Save the result to output dir
 */
template <class Pattern, class DataGraph>
inline void OutputDiscoverResult(SearchTreeNodeT<Pattern, DataGraph>* root,
                                 const size_t d, const char* output_dir,
                                 const double& conf_limit, const size_t& k) {
  int pattern_id = 0;
  std::stack<SearchTreeNodeT<Pattern, DataGraph>*> node_stack;
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> gpar_list;
  std::vector<DiscoverdGPAR<Pattern, DataGraph>> result_gpar_list;
  SearchTreeNodeT<Pattern, DataGraph>* tmp;
  double aver_conf = 0.0;
  for (auto child : root->children) {
    node_stack.push(child);
  }
  while (!node_stack.empty()) {
    tmp = node_stack.top();
    if (tmp->discovergpar.conf() >= conf_limit) {
      gpar_list.push_back(tmp->discovergpar);
    }
    node_stack.pop();
    if (tmp->children.size() != 0) {
      for (auto child : tmp->children) {
        node_stack.push(child);
      }
    }
  }
  std::sort(gpar_list.begin(), gpar_list.end(),
            compareConf<Pattern, DataGraph>);
  for (int i = 0; i < k && i < gpar_list.size(); i++)
    result_gpar_list.push_back(gpar_list[i]);
  for (auto gpar : result_gpar_list) {
    aver_conf += gpar.conf();
    pattern_id++;
    std::cout << " this gpar conf = " << gpar.conf()
              << "; edge size = " << gpar.pattern.CountEdge() << std::endl;
    WriteGPAR(gpar, pattern_id, output_dir);
  }
  double avg = 0;
  if (pattern_id != 0) avg = aver_conf / pattern_id;
  std::cout << "the averge conf = " << avg << std::endl;
  std::cout << "the total gpar size = " << pattern_id << std::endl;
}
}  // namespace gmine_new

#endif
