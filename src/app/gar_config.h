#ifndef GAR_CONFIG_H_
#define GAR_CONFIG_H_

#include <string>
#include <vector>

namespace gar {

struct GraphInfo {
  std::string name;
  std::string dir;
  std::vector<std::string> vertex_file_list;
  std::vector<std::string> edge_file_list;
};

struct GARInfo {
  std::string name;
  std::string dir;
  std::string vertex_file;
  std::string edge_file;
  std::string literal_x_file;
  std::string literal_y_file;
};

struct GARMatchInfo {
  GraphInfo graph;
  std::vector<GARInfo> gar_list;
  std::string result_dir;
};

struct GARChaseInfo {
  GraphInfo graph;
  std::vector<GARInfo> gar_list;
  GraphInfo result_graph;
};

int ReadGARMatchInfoYAML(const std::string& yaml_file,
                         GARMatchInfo& gar_match_info);

int ReadGARChaseInfoYAML(const std::string& yaml_file,
                         GARChaseInfo& gar_chase_info);

}  // namespace gar

#endif  // GAR_CONFIG_H_