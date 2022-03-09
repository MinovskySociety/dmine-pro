#include "yaml_config.h"

namespace gptools {

int ReadGraphPackageInfoYAML(const YAML::Node &graph_node,
                             GraphPackageInfo &graph_info) {
  using namespace YAML;

  graph_info.Reset();

  int count = 0;

  for (const auto &p : graph_node) {
    const auto node_name = p.first.as<std::string>();
    if (node_name == "Name" || node_name == "GraphName") {
      graph_info.name = p.second.as<std::string>();
      ++count;
    } else if (node_name == "Dir") {
      if (graph_info.dict_dir.empty()) {
        graph_info.dict_dir = p.second.as<std::string>();
      }
      if (graph_info.graph_dir.empty()) {
        graph_info.graph_dir = p.second.as<std::string>();
      }
      ++count;
    } else if (node_name == "DictDir") {
      graph_info.dict_dir = p.second.as<std::string>();      
      ++count;
    } else if (node_name == "GraphDir") {
      graph_info.graph_dir = p.second.as<std::string>();
      ++count;
    } else if (node_name == "LabelFile") {
      graph_info.label_file = p.second.as<std::string>();
      ++count;
    } else if (node_name == "AttributeFile") {
      graph_info.attr_file = p.second.as<std::string>();
      ++count;
    } else if (node_name == "TypeFile") {
      graph_info.type_file = p.second.as<std::string>();
      ++count;
    } else if (node_name == "VertexFile") {
      if (p.second.Type() == NodeType::Sequence) {
        for (const auto &node_v_file : p.second) {
          graph_info.vertex_files.emplace_back(node_v_file.as<std::string>());
          ++count;
        }
      } else if (p.second.Type() == NodeType::Scalar) {
        graph_info.vertex_files.emplace_back(p.second.as<std::string>());
        ++count;
      } else {
        throw InvalidNode();
        // throw InvalidNode("VertexFile");
      }
    } else if (node_name == "EdgeFile") {
      if (p.second.Type() == NodeType::Sequence) {
        for (const auto &node_e_file : p.second) {
          graph_info.edge_files.emplace_back(node_e_file.as<std::string>());
          ++count;
        }
      } else if (p.second.Type() == NodeType::Scalar) {
        graph_info.edge_files.emplace_back(p.second.as<std::string>());
        ++count;
      } else {
        throw InvalidNode();
        // throw InvalidNode("EdgeFile");
      }
    } else if (node_name == "HasDict" || node_name == "ReadDict" ||
               node_name == "WriteDict") {
      graph_info.has_dict = p.second.as<bool>();
      ++count;
    } else if (node_name == "HasGraph" || node_name == "ReadGraph" ||
               node_name == "WriteGraph") {
      graph_info.has_graph = p.second.as<bool>();
      ++count;
    } else if (node_name == "HasAttribute" || node_name == "HasAttributes" ||
               node_name == "ReadAttribute" || node_name == "WriteAttribute") {
      graph_info.has_attr = p.second.as<bool>();
      ++count;
    } else if (node_name == "IsGrouping") {
      graph_info.is_grouping = p.second.as<bool>();
      ++count;
    } else if (node_name == "HasSkeleton" || node_name == "WriteSkeleton") {
      graph_info.has_skeleton = p.second.as<bool>();
      ++count;
    } else if (node_name == "CheckGraph") {
      graph_info.check_graph = p.second.as<bool>();
      ++count;
    }
  }

  return count;
}

int ReadGPActionsYAML(const YAML::Node &action_list_node,
                      ActionList &action_list) {
  using namespace YAML;

  action_list.clear();

  if (!action_list_node.IsDefined()) return 0;
  if (!action_list_node.IsSequence()) {
    throw InvalidNode();
    // throw InvalidNode("ActionList");
  }

  for (const auto &t : action_list_node) {
    if (!t.IsMap()) {
      throw InvalidNode();
      // throw InvalidNode("");
    } 

    auto type = t["Action"].as<std::string>();
    if (type == "RegisterEntity") {
    } else if (type == "RegisterRelation") {
    } else if (type == "RegisterStandardType") {
      action_list.emplace_back(
          std::make_unique<GPActionRegisterStandardType>(t));
    } else if (type == "RegisterAttribute") {
      action_list.emplace_back(std::make_unique<GPActionRegisterAttribute>(t));
    } else if (type == "SelectVertex") {
    } else if (type == "SelectEdge") {
    } else if (type == "SelectAttribute") {
    } else if (type == "RemoveVertex") {
      action_list.emplace_back(std::make_unique<GPActionRemoveVertex>(t));
    } else if (type == "RemoveEdge") {
      action_list.emplace_back(std::make_unique<GPActionRemoveEdge>(t));
    } else if (type == "RemoveAttribute") {
      action_list.emplace_back(std::make_unique<GPActionRemoveAttribute>(t));
    } else if (type == "Sampling") {
      action_list.emplace_back(std::make_unique<GPActionSampling>(t));
    } else if (type == "RemoveNotConnected") {
      action_list.emplace_back(std::make_unique<GPActionRemoveNotConnected>(t));
    } else if (type == "FoldAttribute") {
      action_list.emplace_back(std::make_unique<GPActionFoldAttribute>(t));
    } else if (type == "UnfoldAttribute") {
      action_list.emplace_back(std::make_unique<GPActionUnfoldAttribute>(t));
    } else if (type == "FoldRelation") {
      action_list.emplace_back(std::make_unique<GPActionFoldRelation>(t));
    } else if (type == "UnfoldRelation") {
      action_list.emplace_back(std::make_unique<GPActionUnfoldRelation>(t));
    } else {
      throw InvalidNode();
      // throw InvalidNode("Action");
    }
  }

  return static_cast<int>(action_list.size());
}

int ReadGPToolsInfoYAML(const std::filesystem::path &config_file_path,
                        GPToolsInfo &gptools_info) {
  try {
    int res;

    YAML::Node config_node = YAML::LoadFile(config_file_path.string());

    res =
        ReadGraphPackageInfoYAML(config_node["Graph"], gptools_info.graph_info);
    if (res < 0) return res;

    res =
        ReadGPActionsYAML(config_node["ActionList"], gptools_info.action_list);
    if (res < 0) return res;

    res = ReadGraphPackageInfoYAML(config_node["ResultGraph"],
                                   gptools_info.result_info);
    if (res < 0) return res;

    return 0;
  } catch (YAML::BadFile) {
    std::cout << "cannot open config_node file." << std::endl;
    return -1;
  } catch (YAML::InvalidNode) {
    std::cout << "invalid node." << std::endl;
    return -2;
  }
}

}  // namespace gptools