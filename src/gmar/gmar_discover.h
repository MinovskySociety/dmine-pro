#ifndef _GMARDISCOVER_H
#define _GMARDISCOVER_H
#include <string>
#include <vector>

#include "gar/literalset.h"
#include "gmar_discover_info.h"
#include "gpar_template/gpar_discover.h"
#include "gpar_template/gpar_new.h"
#include "ml_model.h"
namespace gmine_new {
template <class Pattern, class DataGraph, class MLModel, class ManualCheck,
          class SelectCallback>
void GMARDiscover(MLModel &ml_model, DataGraph &data_graph,
                  GMARDiscoverInfo<Pattern, DataGraph> &gmar_discover_info,
                  ManualCheck manual_check, SelectCallback select_callback) {
  for (int i = 1; i <= gmar_discover_info.max_iter_num; i++) {
    std::cout << "round " << i << std::endl;
    DataGraph ml_graph = data_graph;
    ml_model.BuildMLGraph(ml_graph);
    std::cout << "vertex num = " << ml_graph.CountVertex() << " "
              << " edge num = " << ml_graph.CountEdge() << std::endl;
    GUNDAM::WriteCSVGraph(ml_graph, gmar_discover_info.v_file,
                          gmar_discover_info.e_file);
    // run dmine
    using DiscoverResultContainer =
        std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
    DiscoverResultContainer discover_gpar;
    std::cout << "discover begin" << std::endl;
    constexpr int inf = 200000;
    DMineProWithCallback<Pattern, DataGraph>(
        gmar_discover_info.v_file.c_str(), gmar_discover_info.e_file.c_str(),
        gmar_discover_info.x_node_label, gmar_discover_info.y_node_label,
        gmar_discover_info.q_edge_label, gmar_discover_info.supp_limit,
        gmar_discover_info.max_edge, inf, gmar_discover_info.output_dir.c_str(),
        manual_check, select_callback, nullptr, &discover_gpar);
    std::cout << "discover result = " << discover_gpar.size() << std::endl;
    std::cout << "discover end" << std::endl;
    // learning
    std::cout << "train begin" << std::endl;
    ml_model.Training(discover_gpar, ml_graph);
    std::cout << "train end" << std::endl;
    ml_model.PrintModel();
    if (ml_model.IsTerminal()) {
      break;
    }
  }
  DataGraph ml_graph = data_graph;
  ml_model.BuildMLGraph(ml_graph);
  std::cout << "vertex num = " << ml_graph.CountVertex() << " "
            << " edge num = " << ml_graph.CountEdge() << std::endl;
  GUNDAM::WriteCSVGraph(ml_graph, gmar_discover_info.v_file,
                        gmar_discover_info.e_file);
  std::cout << "last discover" << std::endl;
  DMinePro<Pattern, DataGraph>(
      gmar_discover_info.v_file.c_str(), gmar_discover_info.e_file.c_str(),
      gmar_discover_info.x_node_label, gmar_discover_info.y_node_label,
      gmar_discover_info.q_edge_label, gmar_discover_info.supp_limit,
      gmar_discover_info.max_edge, gmar_discover_info.top_k,
      gmar_discover_info.output_dir.c_str());
}
template <class Pattern, class DataGraph, class MLModel>
void GMARDiscover(MLModel &ml_model, DataGraph &data_graph,
                  const int max_iter_num) {
  for (int i = 1; i <= max_iter_num; i++) {
    std::cout << "round " << i << std::endl;
    DataGraph ml_graph = data_graph;
    ml_model.BuildMLGraph(ml_graph);
    std::string ml_v_file = "/Users/apple/Desktop/liantong_v.csv";
    std::string ml_e_file = "/Users/apple/Desktop/liantong_e.csv";
    std::cout << "vertex num = " << ml_graph.CountVertex() << " "
              << " edge num = " << ml_graph.CountEdge() << std::endl;
    GUNDAM::WriteCSVGraph(ml_graph, ml_v_file, ml_e_file);
    // run dmine
    using DiscoverResultContainer =
        std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
    DiscoverResultContainer discover_gpar;
    std::cout << "discover begin" << std::endl;
    auto manual_check = [](auto &gpar) {
      // gmar:
      /*
      for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
           vertex_it++) {
        if (vertex_it->id() == gpar.x_node_ptr()->id()) continue;
        if (vertex_it->CountOutEdge(-1) > 0) return false;
      }
      */

      // liantongdata:
      int not_fake_num = 0;
      int fake_num = 0;
      // x node cannot has q edge
      if (gpar.x_node_ptr()->CountOutEdge(gpar.q_edge_label()) > 0)
        return false;
      for (auto it = gpar.pattern.VertexCBegin(); !it.IsDone(); it++) {
        if (it->label() == 1000000001) not_fake_num++;
        if (it->label() == 1000000002) fake_num++;
      }
      // not fake or fake node num is less than 1
      if (not_fake_num >= 2 || fake_num >= 2) return false;
      for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
           vertex_it++) {
        // if (vertex_it->label() == 1002 && vertex_it->CountOutEdge(3) >= 1)
        //  return false;

        int head_num = 0;
        int length_num = 0;
        for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
             edge_it++) {
          // 不允许出现由 attribute 点拓展一个新的 user/phone 点出来
          if ((vertex_it->label() == 1001 || vertex_it->label() == 1002) &&
              vertex_it->CountOutEdge() == 1 && vertex_it->CountInEdge() == 0 &&
              edge_it->label() == 3) {
            return false;
          }
          // 不允许出现由 value 点拓展一个新的 attribute 点出来
          if (vertex_it->label() == 3003 && vertex_it->CountOutEdge() == 1 &&
              vertex_it->CountInEdge() == 0 && edge_it->label() == 4) {
            return false;
          }
          // 不允许出现由 constant 点拓展一个新的 value 点出来
          if (vertex_it->label() == 4002 && vertex_it->CountOutEdge() == 1 &&
              vertex_it->CountInEdge() == 0 && edge_it->label() == 5) {
            return false;
          }
          //一个phone最多只能有一个 head 跟一个 length
          if (vertex_it->label() == 1002) {
            if (edge_it->const_dst_ptr()->label() == 3003) length_num++;
            if (edge_it->const_dst_ptr()->label() >= 1000000003 &&
                edge_it->const_dst_ptr()->label() <= 1000000373)
              head_num++;
          }
        }
        if (length_num >= 2 || head_num >= 2) return false;
      }
      return true;
    };
    DMineProWithCallback<Pattern, DataGraph>(
        ml_v_file.c_str(), ml_e_file.c_str(), 1001, 1000000002, 3, 50, 3, 20000,
        "/Users/apple/Desktop/buaa/code/dmine-pro/output/dmineresult/",
        manual_check, gmine_new::select_callback, nullptr, &discover_gpar);
    std::cout << "discover result = " << discover_gpar.size() << std::endl;
    std::cout << "discover end" << std::endl;
    // learning
    std::cout << "train begin" << std::endl;
    ml_model.Training(discover_gpar, ml_graph);
    std::cout << "train end" << std::endl;
    ml_model.PrintModel();
    if (ml_model.IsTerminal()) {
      break;
    }
  }
  DataGraph ml_graph = data_graph;
  ml_model.BuildMLGraph(ml_graph);
  std::string ml_v_file = "/Users/apple/Desktop/liantong_v.csv";
  std::string ml_e_file = "/Users/apple/Desktop/liantong_e.csv";
  std::cout << "vertex num = " << ml_graph.CountVertex() << " "
            << " edge num = " << ml_graph.CountEdge() << std::endl;
  GUNDAM::WriteCSVGraph(ml_graph, ml_v_file, ml_e_file);
  std::cout << "last discover" << std::endl;
  DMinePro<Pattern, DataGraph>(
      ml_v_file.c_str(), ml_e_file.c_str(), 1001, 1000000002, 3, 50, 6, 50,
      "/Users/apple/Desktop/buaa/code/dmine-pro/output/dmineresult/");
}
template <class Pattern, class DataGraph>
void GMARDiscover(
    gmine_new::BaseContainer<gmine_new::MLModelBase<Pattern, DataGraph>>
        &model_container,
    DataGraph &data_graph, const int max_iter_num) {
  for (int i = 1; i <= max_iter_num; i++) {
    std::cout << "round " << i << std::endl;
    DataGraph ml_graph = data_graph;
    for (const auto &ml_model : model_container) {
      ml_model->BuildMLGraph(ml_graph);
    }
    std::string ml_v_file = "/Users/apple/Desktop/liantong_v.csv";
    std::string ml_e_file = "/Users/apple/Desktop/liantong_e.csv";
    std::cout << "vertex num = " << ml_graph.CountVertex() << " "
              << " edge num = " << ml_graph.CountEdge() << std::endl;
    GUNDAM::WriteCSVGraph(ml_graph, ml_v_file, ml_e_file);
    // run dmine
    using DiscoverResultContainer =
        std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
    DiscoverResultContainer discover_gpar;
    std::cout << "discover begin" << std::endl;
    auto manual_check = [&model_container](auto &gpar) {
      // gmar:
      for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
           vertex_it++) {
        if (vertex_it->id() == gpar.x_node_ptr()->id()) continue;
        for (const auto &ml_model : model_container) {
          if (vertex_it->CountOutEdge(ml_model->ModelLabel()) > 0) return false;
        }
      }

      // liantongdata:
      int not_fake_num = 0;
      int fake_num = 0;
      // x node cannot has q edge
      if (gpar.x_node_ptr()->CountOutEdge(gpar.q_edge_label()) > 0)
        return false;
      for (auto it = gpar.pattern.VertexCBegin(); !it.IsDone(); it++) {
        if (it->label() == 1000000001) not_fake_num++;
        if (it->label() == 1000000002) fake_num++;
      }
      // not fake or fake node num is less than 1
      if (not_fake_num >= 2 || fake_num >= 2) return false;
      for (auto vertex_it = gpar.pattern.VertexCBegin(); !vertex_it.IsDone();
           vertex_it++) {
        // if (vertex_it->label() == 1002 && vertex_it->CountOutEdge(3) >= 1)
        //  return false;

        int head_num = 0;
        int length_num = 0;
        for (auto edge_it = vertex_it->OutEdgeCBegin(); !edge_it.IsDone();
             edge_it++) {
          // 不允许出现由 attribute 点拓展一个新的 user/phone 点出来
          if ((vertex_it->label() == 1001 || vertex_it->label() == 1002) &&
              vertex_it->CountOutEdge() == 1 && vertex_it->CountInEdge() == 0 &&
              edge_it->label() == 3) {
            return false;
          }
          // 不允许出现由 value 点拓展一个新的 attribute 点出来
          if (vertex_it->label() == 3003 && vertex_it->CountOutEdge() == 1 &&
              vertex_it->CountInEdge() == 0 && edge_it->label() == 4) {
            return false;
          }
          // 不允许出现由 constant 点拓展一个新的 value 点出来
          if (vertex_it->label() == 4002 && vertex_it->CountOutEdge() == 1 &&
              vertex_it->CountInEdge() == 0 && edge_it->label() == 5) {
            return false;
          }
          //一个phone最多只能有一个 head 跟一个 length
          if (vertex_it->label() == 1002) {
            if (edge_it->const_dst_ptr()->label() == 3003) length_num++;
            if (edge_it->const_dst_ptr()->label() >= 1000000003 &&
                edge_it->const_dst_ptr()->label() <= 1000000373)
              head_num++;
          }
        }
        if (length_num >= 2 || head_num >= 2) return false;
      }
      return true;
    };
    DMineProWithCallback<Pattern, DataGraph>(
        ml_v_file.c_str(), ml_e_file.c_str(), 1001, 1000000002, 3, 50, 4, 100,
        "/Users/apple/Desktop/buaa/code/dmine-pro/output/dmineresult/",
        manual_check, gmine_new::select_callback, nullptr, &discover_gpar);
    std::cout << "discover result = " << discover_gpar.size() << std::endl;
    std::cout << "discover end" << std::endl;
    // learning
    std::cout << "train begin" << std::endl;
    for (auto &ml_model : model_container) {
      ml_model->Training(discover_gpar, ml_graph);
    }
    std::cout << "train end" << std::endl;
    bool terminal_flag = true;
    for (auto &ml_model : model_container) {
      ml_model->PrintModel();
      terminal_flag &= ml_model->IsTerminal();
    }

    if (terminal_flag) {
      break;
    }
  }
  DataGraph ml_graph = data_graph;
  for (const auto &ml_model : model_container) {
    ml_model->BuildMLGraph(ml_graph);
  }
  std::string ml_v_file = "/Users/apple/Desktop/liantong_v.csv";
  std::string ml_e_file = "/Users/apple/Desktop/liantong_e.csv";
  std::cout << "vertex num = " << ml_graph.CountVertex() << " "
            << " edge num = " << ml_graph.CountEdge() << std::endl;
  GUNDAM::WriteCSVGraph(ml_graph, ml_v_file, ml_e_file);
  std::cout << "last discover" << std::endl;
  DMinePro<Pattern, DataGraph>(
      ml_v_file.c_str(), ml_e_file.c_str(), 1001, 1000000002, 3, 50, 4, 200,
      "/Users/apple/Desktop/buaa/code/dmine-pro/output/dmineresult/");
}
template <class Pattern, class DataGraph, class ManualCheck,
          class SelectCallback>
void GMARDiscover(
    gmine_new::BaseContainer<gmine_new::MLModelBase<Pattern, DataGraph>>
        &model_container,
    DataGraph &data_graph,
    GMARDiscoverInfo<Pattern, DataGraph> &gmar_discover_info,
    ManualCheck manual_check, SelectCallback select_callback) {
  for (int i = 1; i <= gmar_discover_info.max_iter_num; i++) {
    std::cout << "round " << i << std::endl;
    DataGraph ml_graph = data_graph;
    for (const auto &ml_model : model_container) {
      ml_model->BuildMLGraph(ml_graph);
    }
    std::cout << "vertex num = " << ml_graph.CountVertex() << " "
              << " edge num = " << ml_graph.CountEdge() << std::endl;
    GUNDAM::WriteCSVGraph(ml_graph, gmar_discover_info.v_file,
                          gmar_discover_info.e_file);
    // run dmine
    using DiscoverResultContainer =
        std::vector<DiscoverdGPAR<Pattern, DataGraph>>;
    DiscoverResultContainer discover_gpar;
    std::cout << "discover begin" << std::endl;
    constexpr int inf = 200000;
    DMineProWithCallback<Pattern, DataGraph>(
        gmar_discover_info.v_file.c_str(), gmar_discover_info.e_file.c_str(),
        gmar_discover_info.x_node_label, gmar_discover_info.y_node_label,
        gmar_discover_info.q_edge_label, gmar_discover_info.supp_limit,
        gmar_discover_info.max_edge, inf, gmar_discover_info.output_dir.c_str(),
        manual_check, select_callback, nullptr, &discover_gpar);
    std::cout << "discover result = " << discover_gpar.size() << std::endl;
    std::cout << "discover end" << std::endl;
    // learning
    std::cout << "train begin" << std::endl;
    for (auto &ml_model : model_container) {
      ml_model->Training(discover_gpar, ml_graph);
    }
    std::cout << "train end" << std::endl;
    bool terminal_flag = true;
    for (auto &ml_model : model_container) {
      ml_model->PrintModel();
      terminal_flag &= ml_model->IsTerminal();
    }

    if (terminal_flag) {
      break;
    }
  }
  DataGraph ml_graph = data_graph;
  for (const auto &ml_model : model_container) {
    ml_model->BuildMLGraph(ml_graph);
  }
  std::cout << "vertex num = " << ml_graph.CountVertex() << " "
            << " edge num = " << ml_graph.CountEdge() << std::endl;
  GUNDAM::WriteCSVGraph(ml_graph, gmar_discover_info.v_file,
                        gmar_discover_info.e_file);
  std::cout << "last discover" << std::endl;
  DMinePro<Pattern, DataGraph>(
      gmar_discover_info.v_file.c_str(), gmar_discover_info.e_file.c_str(),
      gmar_discover_info.x_node_label, gmar_discover_info.y_node_label,
      gmar_discover_info.q_edge_label, gmar_discover_info.supp_limit,
      gmar_discover_info.max_edge, gmar_discover_info.top_k,
      gmar_discover_info.output_dir.c_str());
}
}  // namespace gmine_new
#endif