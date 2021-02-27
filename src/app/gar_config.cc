#include "gar_config.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace gar {

int ReadGARMatchInfoYAML(const std::string& yaml_file,
                         GARMatchInfo& gar_match_info) {
  std::ifstream config_file(yaml_file);
  if (!config_file.is_open()) {
    std::cout << "cannot open: " << yaml_file << std::endl;
    return -1;
  }
  YAML::Node config = YAML::LoadFile(yaml_file);

  // get graph
  if (!config["Graph"]) {
    std::cout << "cannot get graph!" << std::endl;
    return -2;
  }
  if (!config["Graph"]["Dir"]) {
    std::cout << " cannot get graph dir!" << std::endl;
    return -2;
  }
  gar_match_info.graph.dir = config["Graph"]["Dir"].as<std::string>();
  if (!config["Graph"]["VertexFile"]) {
    std::cout << "cannot get vertex file!" << std::endl;
  }
  gar_match_info.graph.vertex_file_list.clear();
  for (const auto& n : config["Graph"]["VertexFile"]) {
    std::string v_file = gar_match_info.graph.dir + n.as<std::string>();
    gar_match_info.graph.vertex_file_list.emplace_back(std::move(v_file));
  }
  if (!config["Graph"]["EdgeFile"]) {
    std::cout << "cannot get edge file!" << std::endl;
  }
  gar_match_info.graph.edge_file_list.clear();
  for (const auto& n : config["Graph"]["EdgeFile"]) {
    std::string e_file = gar_match_info.graph.dir + n.as<std::string>();
    gar_match_info.graph.edge_file_list.emplace_back(std::move(e_file));
  }

  // get gar
  if (!config["GAR"]) {
    std::cout << "cannot get GAR!" << std::endl;
    return -2;
  }
  auto gar_node = config["GAR"];
  if (!gar_node["Dir"]) {
    std::cout << "cannot get GAR dir!" << std::endl;
    return -2;
  }
  gar_match_info.gar_list.clear();
  gar_match_info.gar_list.emplace_back();
  gar_match_info.gar_list[0].dir = gar_node["Dir"].as<std::string>();

  if (!gar_node["Name"]) {
    std::cout << "cannot get GAR name!" << std::endl;
    return -2;
  }
  gar_match_info.gar_list[0].name = gar_node["Name"].as<std::string>();
  if (!gar_node["VertexFile"]) {
    std::cout << "cannot get gar vertex file!" << std::endl;
    return -2;
  }
  gar_match_info.gar_list[0].vertex_file =
      gar_match_info.gar_list[0].dir + gar_node["VertexFile"].as<std::string>();

  if (!gar_node["EdgeFile"]) {
    std::cout << "cannot get gar edge file!" << std::endl;
    return -2;
  }
  gar_match_info.gar_list[0].edge_file =
      gar_match_info.gar_list[0].dir + gar_node["EdgeFile"].as<std::string>();

  if (!gar_node["LiteralX"]) {
    std::cout << "cannot get gar literal x!" << std::endl;
    return -2;
  }
  gar_match_info.gar_list[0].literal_x_file =
      gar_match_info.gar_list[0].dir + gar_node["LiteralX"].as<std::string>();

  if (!gar_node["LiteralY"]) {
    std::cout << "cannot get gar literal y!" << std::endl;
    return -2;
  }
  gar_match_info.gar_list[0].literal_y_file =
      gar_match_info.gar_list[0].dir + gar_node["LiteralY"].as<std::string>();

  // get dir
  if (!config["ResultDir"]) {
    std::cout << "cannot get result dir!" << std::endl;
    return -2;
  }
  gar_match_info.result_dir = config["ResultDir"].as<std::string>();

  return 0;
}

int ReadGARChaseInfoYAML(const std::string& yaml_file,
    GARChaseInfo& gar_chase_info) {

  std::ifstream config_file(yaml_file);
  if (!config_file.is_open()) {
    std::cout << "cannot open config file." << std::endl;
    return -2;
  }
  YAML::Node config = YAML::LoadFile(yaml_file);

  // get graph
  if (!config["Graph"]) {
    std::cout << "cannot get graph!" << std::endl;
    return -2;
  }
  if (!config["Graph"]["Dir"]) {
    std::cout << " cannot get graph dir!" << std::endl;
    return -2;
  }
  gar_chase_info.graph.dir = config["Graph"]["Dir"].as<std::string>();
  if (!config["Graph"]["VertexFile"]) {
    std::cout << "cannot get vertex file!" << std::endl;
  }
  for (const auto& it : config["Graph"]["VertexFile"]) {
    std::string v_file = gar_chase_info.graph.dir + it.as<std::string>();
    gar_chase_info.graph.vertex_file_list.emplace_back(std::move(v_file));
  }
  if (!config["Graph"]["EdgeFile"]) {
    std::cout << "cannot get edge file!" << std::endl;
  }
  std::vector<std::string> edge_file_list;
  for (const auto& it : config["Graph"]["EdgeFile"]) {
    std::string e_file = gar_chase_info.graph.dir + it.as<std::string>();
    gar_chase_info.graph.edge_file_list.emplace_back(std::move(e_file));
  }

  // get gar
  if (!config["GAR"]) {
    std::cout << "cannot get GAR!" << std::endl;
    return -2;
  }
  for (auto gar_node : config["GAR"]) {
    gar_chase_info.gar_list.emplace_back();
    auto& gar_info = gar_chase_info.gar_list.back();

    if (!gar_node["Dir"]) {
      std::cout << "cannot get GAR dir!" << std::endl;
      return -2;
    }
    gar_info.dir = gar_node["Dir"].as<std::string>();

    if (!gar_node["Name"]) {
      std::cout << "cannot get GAR name!" << std::endl;
      return -2;
    }
    gar_info.name = gar_node["Name"].as<std::string>();

    if (!gar_node["VertexFile"]) {
      std::cout << "cannot get gar vertex file!" << std::endl;
      return -2;
    }
    gar_info.vertex_file =
        gar_info.dir + gar_node["VertexFile"].as<std::string>();

    if (!gar_node["EdgeFile"]) {
      std::cout << "cannot get gar edge file!" << std::endl;
      return -2;
    }
    gar_info.edge_file = gar_info.dir + gar_node["EdgeFile"].as<std::string>();

    if (!gar_node["LiteralX"]) {
      std::cout << "cannot get gar literal x!" << std::endl;
      return -2;
    }
    gar_info.literal_x_file =
        gar_info.dir + gar_node["LiteralX"].as<std::string>();

    if (!gar_node["LiteralY"]) {
      std::cout << "cannot get gar literal y!" << std::endl;
      return -2;
    }
    gar_info.literal_y_file =
        gar_info.dir + gar_node["LiteralY"].as<std::string>();
  }

  auto output_graph_node = config["OutputGraph"];
  if (!output_graph_node) {
    std::cout << "cannot get Output Graph!" << std::endl;
    return -2;
  }

  if (!output_graph_node["Dir"]) {
    std::cout << "cannot get output graph dir!" << std::endl;
    return -2;
  }
  gar_chase_info.result_graph.dir = output_graph_node["Dir"].as<std::string>();

  gar_chase_info.result_graph.vertex_file_list.clear();
  if (!output_graph_node["VertexFile"]) {
    std::cout << "cannot get output graph vertex file!" << std::endl;
    return -2;
  }
  gar_chase_info.result_graph.vertex_file_list.emplace_back();
  gar_chase_info.result_graph.vertex_file_list.back() =
      gar_chase_info.result_graph.dir +
      output_graph_node["VertexFile"].as<std::string>();

  gar_chase_info.result_graph.edge_file_list.clear();
  if (!output_graph_node["EdgeFile"]) {
    std::cout << "cannot get output graph edge file!" << std::endl;
    return -2;
  }

  gar_chase_info.result_graph.edge_file_list.emplace_back();
  gar_chase_info.result_graph.edge_file_list.back() =
      gar_chase_info.result_graph.dir +
      output_graph_node["EdgeFile"].as<std::string>();

  return 0;
}

}  // namespace gar