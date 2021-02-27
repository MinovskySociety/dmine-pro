#ifndef _YAML_CONFIG_H
#define _YAML_CONFIG_H

#include <yaml-cpp/yaml.h>

#include <memory>
#include <vector>

#include "action.h"
#include "gptdef.h"

namespace gptools {

using ActionList = std::vector<std::unique_ptr<GPAction>>;

struct GPToolsInfo {
  GraphPackageInfo graph_info;
  ActionList action_list;
  GraphPackageInfo result_info;
};

int ReadGraphPackageInfoYAML(const YAML::Node &graph_node,
                             GraphPackageInfo &graph_info);

int ReadGPActionsYAML(const YAML::Node &action_list_node,
                      ActionList &action_list);

int ReadGPToolsInfoYAML(const std::filesystem::path &config_file_path,
                        GPToolsInfo &gptools_info);

}  // namespace gptools

#endif