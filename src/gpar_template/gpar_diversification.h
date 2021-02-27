#ifndef _GPAR_DIVERSIFICATION_H
#define _GPAR_DIVERSIFICATION_H
#include <fstream>
#include <set>
#include <vector>

#include "gpar_config.h"
#include "gpar_match.h"
#include "round_log.h"
namespace gmine_new {
template <class Supp>
double CalJascard(const Supp &supp_a, const Supp &supp_b) {
  Supp intersection_result;
  intersection_result.clear();
  std::set_intersection(
      supp_a.cbegin(), supp_a.cend(), supp_b.cbegin(), supp_b.cend(),
      inserter(intersection_result, intersection_result.begin()));
  size_t intersection_size = intersection_result.size();
  size_t union_size =
      supp_a.size() + supp_b.size() - intersection_result.size();
  return static_cast<double>(intersection_size) / union_size;
}
template <class GPAR, class SelectCallback>
void SelectKNotSimiliarGPAR(int select_k, std::vector<GPAR> &gpar_list,
                            SelectCallback select_callback) {
  std::vector<GPAR> select_result;
  std::stable_sort(gpar_list.begin(), gpar_list.end(),
                   [&select_callback](auto &a, auto &b) {
                     if (a.conf() != b.conf())
                       return a.conf() > b.conf();
                     else {
                       if (a.supp_R_size() != b.supp_R_size())
                         return a.supp_R_size() > b.supp_R_size();
                       else {
                         return select_callback(a, b);
                       }
                     }
                   });
  for (auto &gpar : gpar_list) {
    if (select_result.size() == select_k) {
      break;
    }
    int simi_flag = 0;
    for (auto &single_select_result : select_result) {
      /*
      if (static_cast<double>(gpar.supp_R_size()) >
              (1.0 / similiar_limit) * single_select_result.supp_R_size() ||
          static_cast<double>(gpar.supp_R_size()) <
              (similiar_limit)*single_select_result.supp_R_size()) {
        continue;
      }
      if (static_cast<double>(gpar.supp_Q_size()) >
              (1.0 / similiar_limit) * single_select_result.supp_Q_size() ||
          static_cast<double>(gpar.supp_Q_size()) <
              (similiar_limit)*single_select_result.supp_Q_size()) {
        continue;
      }
      */
      double similiar =
          CalJascard(gpar.supp_R(), single_select_result.supp_R());
      if (similiar < similiar_limit) continue;
      similiar = CalJascard(gpar.supp_Q(), single_select_result.supp_Q());
      if (similiar < similiar_limit) continue;
      simi_flag = 1;
      break;
    }
    if (!simi_flag) {
      select_result.push_back(gpar);
    }
  }
  std::swap(select_result, gpar_list);
  std::cout << "remain size = " << gpar_list.size() << std::endl;
  return;
}
template <class GPAR, class SelectCallback>
void GPARDiversification(int round, std::vector<GPAR> &gpar_list,
                         SelectCallback select_callback) {
  std::cout << "begin div!" << std::endl;
  SelectKNotSimiliarGPAR(select_k, gpar_list, select_callback);
  std::cout << "div end!" << std::endl;
  return;
}
}  // namespace gmine_new

#endif