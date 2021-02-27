#ifndef CONFIG_H
#define CONFIG_H

#include "global.h"
#include "yaml-cpp/yaml.h"

namespace toy {
/* Class Config
 * @desc: set and get configurations
*/
class Config {
 public:
  Config() {}
  ~Config() {}

  /* Load Discovery Config By Yaml
   * @param: discovery yaml path
   * @desc: load tgr discovery settings by yaml file
  */
  void LoadDiscoveryConfigByYaml(std::string yaml_path) {
    YAML::Node config;
    try {
      config = YAML::LoadFile(yaml_path);
      // graph, pattern
      app_ = APP_TYPE::DISCOVERY;
      const std::string path = config["Graph"]["Dir"].as<std::string>();
      const std::string vfile = config["Graph"]["VertexFile"].as<std::string>();
      const std::string efile = config["Graph"]["EdgeFile"].as<std::string>();
      graph_path_.first = path + vfile;
      graph_path_.second = path + efile;
      output_name_ = config["OutputGraph"]["Name"].as<std::string>();
      output_path_ = config["OutputGraph"]["Dir"].as<std::string>();
      // match settings
      use_order_ = true;
      use_match_filter_ = false;
      // discovery settings
      x_label_ = config["Settings"]["NodeXLabel"].as<uint32_t>();
      y_label_ = config["Settings"]["NodeYLabel"].as<uint32_t>();
      q_label_ = config["Settings"]["EdgeQLabel"].as<uint32_t>();
      q_has_order_ = config["Settings"]["EdgeQHasOrder"].as<bool>();
      supp_threshold_ = config["Settings"]["SuppRLimit"].as<uint32_t>();
      select_k_ = config["Settings"]["TopK"].as<uint32_t>();
      max_d_ = config["Settings"]["Radius"].as<uint32_t>();
      frequency_limit_ = config["Settings"]["EdgeFrequency"].as<float>();
      thread_num_ = config["Settings"]["ThreadNum"].as<uint32_t>();
    } catch (YAML::BadFile &e) {
      LOG_E("Load Discovery Configurations Error: ", e.what());
      throw(TException::ConfigError);
    }
  }

  /* Load Apply Config By Yaml
   * @param: apply yaml path
   * @desc: load tgr apply settings by yaml file
  */
  void LoadApplyConfigByYaml(std::string yaml_path) {
    YAML::Node config;
    try {
      config = YAML::LoadFile(yaml_path);
      // graph, pattern
      app_ = APP_TYPE::APPLY;
      const std::string path = config["Graph"]["Dir"].as<std::string>();
      const std::string vfile = config["Graph"]["VertexFile"].as<std::string>();
      const std::string efile = config["Graph"]["EdgeFile"].as<std::string>();
      graph_path_.first = path + vfile;
      graph_path_.second = path + efile;
      output_name_ = config["OutputGraph"]["Name"].as<std::string>();
      output_path_ = config["OutputGraph"]["Dir"].as<std::string>();
      // match settings
      use_order_ = true;
      use_match_filter_ = false;
      // TGR settings
      for (auto it = config["TGR"].begin(); it != config["TGR"].end(); it++) {
        const std::string dir = (*it)["Dir"].as<std::string>();
        const std::string vfile = (*it)["VertexFile"].as<std::string>();
        const std::string efile = (*it)["EdgeFile"].as<std::string>();
        const std::string x_literal = (*it)["LiteralX"].as<std::string>();
        const std::string y_literal = (*it)["LiteralY"].as<std::string>();
        pattern_path_vec_.push_back({dir + vfile, dir + efile});
        x_literal_.push_back(dir + x_literal);
        y_literal_.push_back(dir + y_literal);
        const std::string::size_type pos = vfile.rfind(V_FILE_SUFFIX);
        const std::string match_path =
            pos != vfile.npos ? vfile.substr(0, pos) + MATCH_FILE_SUFFIX
                              : vfile + MATCH_FILE_SUFFIX;
        match_path_vec_.push_back(output_path_ + match_path);
      }
    } catch (YAML::BadFile &e) {
      LOG_E("Load Apply Configurations Error: ", e.what());
      throw(TException::ConfigError);
    }
  }

  /* Load Time Window Config By Yaml
   * @param: time window yaml path
   * @desc: load time window settings by yaml file
  */
  void LoadTimeWindowConfigByYaml(std::string time_window_yaml) {
    YAML::Node config;
    try {
      config = YAML::LoadFile(time_window_yaml);
      time_window_number_ = config["TimeWindowNumber"].as<uint32_t>();
      time_window_size_ = config["TimeWindowSize"].as<uint32_t>();
      bool use_partition = config["UsePartition"].as<bool>();
      if (use_partition) {
        partition_method_ = time_window_number_ > 0
                                ? PARTITION_BY_TIME_WINDOW_NUMBER
                                : PARTITION_BY_TIME_WINDOW_SIZE;
      }
    } catch (YAML::BadFile &e) {
      LOG_E("Load Time Window Configurations Error: ", e.what());
      throw(TException::ConfigError);
    }
  }

  /* Print Config Info */
  void PrintConfig() const {
    LOG_T("...... Config Info ......");
    LOG_T("App Name: ", AppTypeStr(app_));
    LOG_T("Graph VFile: ", graph_path_.first);
    LOG_T("Graph EFile: ", graph_path_.second);
    LOG_T("Output Name: ", output_name_);
    LOG_T("Output Path: ", output_path_);
    // time window settings
    LOG_T("Time Window Number: ", time_window_number_);
    LOG_T("Time Window Size: ", time_window_size_);
    LOG_T("Partition Method: ", PartitonMethodStr(partition_method_));
    // match settings
    LOG_T("Whether Use Order: ", use_order_);
    LOG_T("Whether Use Match Filter: ", use_match_filter_);
    if (app_ == APP_TYPE::DISCOVERY) {
      LOG_T("X Label: ", x_label_);
      LOG_T("Y Label: ", y_label_);
      LOG_T("Q Label: ", q_label_);
      LOG_T("Q Has Order: ", q_has_order_);
      LOG_T("Support R Threshold: ", supp_threshold_);
      LOG_T("Top K: ", select_k_);
      LOG_T("Max Radius: ", max_d_);
      LOG_T("Edge Frequency Threshold: ", frequency_limit_);
      LOG_T("Threads Number: ", thread_num_);
    } else if (app_ == APP_TYPE::APPLY) {
      LOG_T("Pattern Amount: ", pattern_path_vec_.size());
      for (int i = 0; i < pattern_path_vec_.size(); i++) {
        LOG_T("Pattern VFile: ", pattern_path_vec_[i].first);
        LOG_T("Pattern EFile: ", pattern_path_vec_[i].second);
        LOG_T("X_Literal File: ", x_literal_[i]);
        LOG_T("Y_Literal File: ", y_literal_[i]);
        LOG_T("Match File: ", match_path_vec_[i]);
      }
    }
    LOG_T("...... Config Info End ......");
  }

  inline APP_TYPE App() const { return app_; }
  inline std::string GraphVFilePath() const { return graph_path_.first; }
  inline std::string GraphEFilePath() const { return graph_path_.second; }
  inline std::vector<std::pair<std::string, std::string>> PatternFilePathVec()
      const {
    return pattern_path_vec_;
  }
  inline std::vector<std::string> XLiteralFilePathVec() const {
    return x_literal_;
  }
  inline std::vector<std::string> YLiteralFilePathVec() const {
    return y_literal_;
  }
  inline std::vector<std::string> MatchFilePathVec() const {
    return match_path_vec_;
  }
  inline std::string OutputName() const { return output_name_; }
  inline std::string OutputPath() const { return output_path_; }
  inline SIZE_T TimeWindowNumber() const { return time_window_number_; }
  inline TIME_T TimeWindowSize() const { return time_window_size_; }
  inline PARTITION_METHOD PartitionMethod() const { return partition_method_; }
  inline bool WhetherUseOrder() const { return use_order_; }
  inline bool WhetherUseMatchFilter() const { return use_match_filter_; }
  inline VLABEL_T XLabel() const { return x_label_; }
  inline VLABEL_T YLabel() const { return y_label_; }
  inline ELABEL_T QLabel() const { return q_label_; }
  inline bool WhetherQHasOrder() const { return q_has_order_; }
  inline SIZE_T SupportThreshold() const { return supp_threshold_; }
  inline SIZE_T SelectK() const { return select_k_; }
  inline SIZE_T MaxD() const { return max_d_; }
  inline GRADE_T FrequencyLimit() const { return frequency_limit_; }
  inline SIZE_T ThreadNum() const { return thread_num_; }

  inline void SetTimeWindowSize(TIME_T time) { time_window_size_ = time; }
  inline void SetTimeWindowNumber(SIZE_T n) { time_window_number_ = n; }

 private:
  // data
  APP_TYPE app_;
  std::pair<std::string, std::string> graph_path_;
  std::vector<std::pair<std::string, std::string>> pattern_path_vec_;
  std::vector<std::string> x_literal_;
  std::vector<std::string> y_literal_;
  std::vector<std::string> match_path_vec_;
  std::string output_name_;
  std::string output_path_;
  // time window
  SIZE_T time_window_number_ = 1;
  TIME_T time_window_size_ = 0;
  PARTITION_METHOD partition_method_ = NO_METHOD;
  // match settings
  bool use_order_ = false;
  bool use_match_filter_ = false;
  // discover settings
  VLABEL_T x_label_;
  VLABEL_T y_label_;
  ELABEL_T q_label_;
  bool q_has_order_;
  uint32_t supp_threshold_;
  uint32_t select_k_;
  uint32_t max_d_;
  GRADE_T frequency_limit_;
  uint32_t thread_num_;

};  // class Config
}  // namespace toy
#endif  // CONFIG_H
