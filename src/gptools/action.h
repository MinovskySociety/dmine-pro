#ifndef _ACTION_H
#define _ACTION_H

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>

#include "fold_unfold.h"
#include "gptdef.h"
#include "select.h"

namespace gptools {

class GPAction {
 public:
  virtual ~GPAction(){/* std::cout << "disconstruct" << std::endl; */};

  virtual int Run(GraphPackage &gp) = 0;
};

// ------------------------------------------------------------
// Dict
// ------------------------------------------------------------
class GPActionRegisterStandardType : public GPAction {
 public:
  GPActionRegisterStandardType(const YAML::Node &node) {
    using namespace YAML;
    type_name_ = node["TypeName"].as<std::string>();
    if (type_name_ == "int") {
      min_ = node["Min"].as<int>();
      max_ = node["Max"].as<int>();
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ RegisterStandardType ]" << std::endl;

    int res;

    std::cout << "TypeName: " << type_name_ << std::endl;
    if (type_name_ == "bool") {
      res = gp.RegisterBoolType();
      return res;
    } else if (type_name_ == "int") {
      std::cout << "Min: " << min_ << std::endl;
      std::cout << "Max: " << max_ << std::endl;
      res = gp.RegisterIntType(min_, max_);
      return res;
    } else {
      std::cout << "Invalid parameters." << std::endl;
      return -1;
    }
  }

 private:
  std::string type_name_;
  int min_ = 0;
  int max_ = 0;
};

class GPActionRegisterAttribute : public GPAction {
 public:
  GPActionRegisterAttribute(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["Label"];
    if (v1.IsDefined()) {
      if (v1.IsSequence()) {
        for (const auto &t : v1) {
          label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        label_name_list_.emplace_back(v1.as<std::string>());
      }
    }

    auto v2 = node["AttributeKey"];
    if (v2.IsSequence()) {
      for (const auto &t : v2) {
        attr_key_list_.emplace_back(t.as<std::string>());
      }
    } else {
      attr_key_list_.emplace_back(v2.as<std::string>());
    }

    auto v3 = node["ValueType"];
    if (v3.IsDefined()) {
      type_name_ = v3.as<std::string>();
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ RegisterAttribute ]" << std::endl;

    int res;
     
    std::set<Label> label_list;
    if (!label_name_list_.empty()) {
      std::cout << "Label:" << std::endl;
      res = GetLabelList(gp.label_dict(), label_name_list_,
                         label_list);
      if (res < 0) return res;
    }

    int count = 0;

    std::cout << "Attribute key:" << std::endl;
    for (const auto &attr_key : attr_key_list_) {
      std::cout << "  " << attr_key;
      if (type_name_.empty()) {
        res = gp.RegisterGraphAttributeAndValues(label_list, attr_key);
      } else {
        res = gp.RegisterGraphAttribute(label_list, attr_key, type_name_);
      }
      if (res < 0) {
        std::cout << " - failed (err: " << res << ")" << std::endl;
        return res;
      }
      std::cout << "- ok" << std::endl;
      count += res;
    }

    return count;
  }

 private:
  std::vector<std::string> label_name_list_;
  std::vector<AttributeKey> attr_key_list_;
  std::string type_name_;
};

// ------------------------------------------------------------
// Sampling
// ------------------------------------------------------------

class GPActionSampling : public GPAction {
 public:
  GPActionSampling(const YAML::Node &node) {
    using namespace YAML;
    vertex_label_name_ = node["VertexLabel"].as<std::string>();

    auto v1 = node["ListFile"];
    if (v1.IsDefined()) {
      list_file_ = node["ListFile"].as<std::string>();
    }

    auto v2 = node["Number"];
    if (v2.IsDefined()) {
      num_ = node["Number"].as<size_t>();
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ Sampling ]" << std::endl;

    int res;    

    if (!vertex_label_name_.empty() && !list_file_.empty()) {
      std::cout << "Vertex label: " << vertex_label_name_;
      Label vertex_label = gp.label_dict().GetLabelID(vertex_label_name_);
      if (vertex_label == 0) {
        std::cout << " not found." << std::endl;
        return -1;
      }
      std::cout << " (" << vertex_label << ")" << std::endl;

      std::cout << "List file: " << std::endl;
      std::set<ID> sampling_list;
      res = ReadIDList(list_file_, sampling_list);
      if (res < 0) return res;

      std::cout << "Sampling vertex by list:" << std::endl;
      auto count =
          SamplingVertexByIDList(gp.graph(), vertex_label, sampling_list);

      return static_cast<int>(count);
    } else if (!vertex_label_name_.empty() && num_ > 0) {      
      std::cout << "Vertex label: " << vertex_label_name_;
      Label vertex_label = gp.label_dict().GetLabelID(vertex_label_name_);
      if (vertex_label == 0) {
        std::cout << " not found." << std::endl;
        return -1;
      }
      std::cout << " (" << vertex_label << ")" << std::endl;

      std::cout << "Number: " << num_ << std::endl;
      std::cout << "Samping vertex:" << std::endl;
      auto count =
          SamplingVertexRandom(gp.graph(), vertex_label, num_);
      return static_cast<int>(count);
    }

    std::cout << "Invalid parameters." << std::endl;
    return -1;
  }

 private:
  std::string vertex_label_name_{};
  std::filesystem::path list_file_{};
  size_t num_ = 0;
};

// ------------------------------------------------------------
// RemoveNotConnected
// ------------------------------------------------------------

class GPActionRemoveNotConnected : public GPAction {
 public:
  GPActionRemoveNotConnected(const YAML::Node &node) {
    using namespace YAML;

    auto v = node["StartVertexLabel"];
    if (v.IsSequence()) {
      for (const auto &t : v) {
        start_vertex_label_name_list_.emplace_back(t.as<std::string>());
      }
    } else {
      start_vertex_label_name_list_.emplace_back(v.as<std::string>());
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ RemoveNotConnected ]" << std::endl;

    int res;

    if (!start_vertex_label_name_list_.empty()) {
      std::cout << "Start vertex label:" << std::endl;
      std::set<Label> start_vertex_label_list;
      res = GetLabelList(gp.label_dict(), start_vertex_label_name_list_, start_vertex_label_list);
      if (res < 0) return res;
      
      std::cout << "Start vertex: " << std::endl;
      std::set<ID> start_vertex_id_list;
      res = GetVertexList(gp.graph(), start_vertex_label_list, start_vertex_id_list);
      if (res < 0) return res;

      std::cout << "Remove disconnected Entity/Relation:" << std::endl;
      res = RemoveDisconnectedEntityRelation(
          gp.graph(), std::move(start_vertex_id_list), gp.label_dict());
      if (res < 0) return res;

      return res;
    }

    return -1;
  }

 private:
  std::vector<std::string> start_vertex_label_name_list_;
};

// ------------------------------------------------------------
// RemoveVertex
// ------------------------------------------------------------

class GPActionRemoveVertex : public GPAction {
 public:
  GPActionRemoveVertex(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["VertexLabel"];
    if (v1.IsDefined()) {
      if (v1.IsSequence()) {
        for (const auto &t : v1) {
          vertex_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        vertex_label_name_list_.emplace_back(v1.as<std::string>());
      }
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ RemoveVertex ]" << std::endl;

    int res;

    std::set<Label> vertex_label_list;
    if (!vertex_label_name_list_.empty()) {
      std::cout << "Vertex label: " << std::endl;
      res = GetLabelList(gp.label_dict(), vertex_label_name_list_,
                         vertex_label_list);
      if (res < 0) return res;
    }

    std::cout << "Remove vertex:" << std::endl;
    res = RemoveVertexByLabelList(gp.graph(), vertex_label_list);
    if (res < 0) return res;

    return res;
  }

 private:
  std::vector<std::string> vertex_label_name_list_;
};

// ------------------------------------------------------------
// RemoveEdge
// ------------------------------------------------------------

class GPActionRemoveEdge : public GPAction {
 public:
  GPActionRemoveEdge(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["EdgeLabel"];
    if (v1.IsDefined()) {
      edge_label_name_ = v1.as<std::string>();
    }

    auto v2 = node["SourceVertexLabel"];
    if (v2.IsDefined()) {
      if (v2.IsSequence()) {
        for (const auto &t : v2) {
          source_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        source_label_name_list_.emplace_back(v2.as<std::string>());
      }
    }

    auto v3 = node["TargetVertexLabel"];
    if (v3.IsDefined()) {
      if (v3.IsSequence()) {
        for (const auto &t : v3) {
          target_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        target_label_name_list_.emplace_back(v3.as<std::string>());
      }
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ RemoveEdge ]" << std::endl;

    int res;

    Label edge_label = 0;
    if (!edge_label_name_.empty()) {
      std::cout << "Edge label: " << edge_label_name_;
      edge_label = gp.label_dict().GetLabelID(edge_label_name_);
      if (edge_label == 0) {
        std::cout << " - not found." << std::endl;
        return -1;
      }
      std::cout << " (" << edge_label << ")" << std::endl;
    }

    std::set<Label> source_label_list;
    if (!source_label_name_list_.empty()) {
      std::cout << "Source vertex label: " << std::endl;
      res = GetLabelList(gp.label_dict(), source_label_name_list_,
                         source_label_list);
      if (res < 0) return res;
    }

    std::set<Label> target_label_list;
    if (!target_label_name_list_.empty()) {
      std::cout << "Target vertex label: " << std::endl;
      res = GetLabelList(gp.label_dict(), target_label_name_list_,
                         target_label_list);
      if (res < 0) return res;
    }

    std::cout << "Remove edge:" << std::endl;
    res = RemoveEdgeByLabel(gp.graph(), edge_label, source_label_list,
                            target_label_list);
    if (res < 0) return res;

    return res;
  }

 private:
  std::string edge_label_name_ = "";
  std::vector<std::string> source_label_name_list_;
  std::vector<std::string> target_label_name_list_;
};

// ------------------------------------------------------------
// RemoveAttribute
// ------------------------------------------------------------

class GPActionRemoveAttribute : public GPAction {
 public:
  GPActionRemoveAttribute(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["VertexLabel"];
    if (v1.IsSequence()) {
      for (const auto &t : v1) {
        vertex_label_name_list_.emplace_back(t.as<std::string>());
      }
    } else {
      vertex_label_name_list_.emplace_back(v1.as<std::string>());
    }

    auto v2 = node["AttributeKey"];
    if (v2.IsSequence()) {
      for (const auto &t : v2) {
        attr_key_list_.emplace_back(t.as<std::string>());
      }
    } else {
      attr_key_list_.emplace_back(v2.as<std::string>());
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ RemoveAttribute ]" << std::endl;

    int res;

    if (!vertex_label_name_list_.empty() && !attr_key_list_.empty()) {
      std::cout << "Vertex label:" << std::endl;
      std::set<Label> vertex_label_list;
      res = GetLabelList(gp.label_dict(), vertex_label_name_list_,
                         vertex_label_list);
      if (res < 0) return res;

      std::cout << "Attribute key:" << std::endl;
      for (const auto &attr_key : attr_key_list_) {
        std::cout << "  " << attr_key << std::endl;
      }

      std::cout << "Remove vetex attribute:" << std::endl;
      auto count =
          RemoveVertexAttribute(gp.graph(), vertex_label_list, attr_key_list_);

      return static_cast<int>(count);
    }

    return -1;
  }

 private:
  std::vector<std::string> vertex_label_name_list_;
  std::vector<std::string> attr_key_list_;
};

// ------------------------------------------------------------
// FoldAttribute
// ------------------------------------------------------------

class GPActionFoldAttribute : public GPAction {
 public:
  GPActionFoldAttribute(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["VertexLabel"];
    if (v1.IsDefined()) {
      if (v1.IsSequence()) {
        for (const auto &t : v1) {
          vertex_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        vertex_label_name_list_.emplace_back(v1.as<std::string>());
      }
    }

    auto v2 = node["AttributeKey"];
    if (v2.IsDefined()) {
      if (v2.IsSequence()) {
        for (const auto &t : v2) {
          attr_list_.emplace(t.as<std::string>());
        }
      } else {
        attr_list_.emplace(v2.as<std::string>());
      }
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ FoldAttribute ]" << std::endl;

    int res;

    std::cout << "Vertex label:" << std::endl;
    std::set<Label> vertex_label_list;
    res = GetLabelList(gp.label_dict(), vertex_label_name_list_,
                       vertex_label_list);
    if (res < 0) return res;

    std::cout << "Attribute key:" << std::endl;
    for (const auto &attr_key : attr_list_) {
      std::cout << "  " << attr_key << std::endl;
    }

    std::cout << "Fold:" << std::endl;
    res = FoldAttribute(gp, vertex_label_list, attr_list_);
    if (res < 0) return res;

    return res;
  }

 private:
  std::vector<std::string> vertex_label_name_list_;
  std::set<std::string> attr_list_;
};

// ------------------------------------------------------------
// UnfoldAttribute
// ------------------------------------------------------------

class GPActionUnfoldAttribute : public GPAction {
 public:
  GPActionUnfoldAttribute(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["VertexLabel"];
    if (v1.IsDefined()) {
      if (v1.IsSequence()) {
        for (const auto &t : v1) {
          vertex_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        vertex_label_name_list_.emplace_back(v1.as<std::string>());
      }
    }

    auto v2 = node["AttributeKey"];
    if (v2.IsSequence()) {
      for (const auto &t : v2) {
        attr_list_.emplace(t.as<std::string>());
      }
    } else {
      attr_list_.emplace(v2.as<std::string>());
    }

    auto v3 = node["Type"];
    unfold_type_ = v3.as<int>();
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ UnfoldAttribute ]" << std::endl;

    int res;

    if (!attr_list_.empty()) {
      std::cout << "Vertex label:" << std::endl;
      std::set<Label> vertex_label_list;
      res = GetLabelList(gp.label_dict(), vertex_label_name_list_,
                         vertex_label_list);
      if (res < 0) return res;

      std::cout << "Attribute key:" << std::endl;
      for (const auto &attr_key : attr_list_) {
        std::cout << "  " << attr_key << std::endl;
      }

      std::cout << "Unfold:" << std::endl;
      res = UnfoldAttribute(gp, vertex_label_list, attr_list_, unfold_type_);
      if (res < 0) return res;

      return res;
    }

    return -1;
  }

 private:
  std::vector<std::string> vertex_label_name_list_;
  std::set<std::string> attr_list_;
  int unfold_type_;
};

// ------------------------------------------------------------
// FoldRelation
// ------------------------------------------------------------

class GPActionFoldRelation : public GPAction {
 public:
  GPActionFoldRelation(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["VertexLabel"];
    if (v1.IsDefined()) {
      if (v1.IsSequence()) {
        for (const auto &t : v1) {
          vertex_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        vertex_label_name_list_.emplace_back(v1.as<std::string>());
      }
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ FoldRelation ]" << std::endl;

    int res;

    std::cout << "Vertex label:" << std::endl;
    std::set<Label> vertex_label_list;
    res = GetLabelList(gp.label_dict(), vertex_label_name_list_,
                       vertex_label_list);
    if (res < 0) return res;

    std::cout << "Fold:" << std::endl;
    res = FoldRelation(gp, vertex_label_list);
    if (res < 0) return res;

    return res;
  }

 private:
  std::vector<std::string> vertex_label_name_list_;
};

// ------------------------------------------------------------
// UnfoldRelation
// ------------------------------------------------------------

class GPActionUnfoldRelation : public GPAction {
 public:
  GPActionUnfoldRelation(const YAML::Node &node) {
    using namespace YAML;

    auto v1 = node["EdgeLabel"];
    if (v1.IsDefined()) {
      if (v1.IsSequence()) {
        for (const auto &t : v1) {
          edge_label_name_list_.emplace_back(t.as<std::string>());
        }
      } else {
        edge_label_name_list_.emplace_back(v1.as<std::string>());
      }
    }
  }

  int Run(GraphPackage &gp) override final {
    std::cout << "[ UnfoldRelation ]" << std::endl;

    int res;

    std::cout << "Edge label:" << std::endl;
    std::set<Label> edge_label_list;
    res = GetLabelList(gp.label_dict(), edge_label_name_list_,
                       edge_label_list);
    if (res < 0) return res;

    std::cout << "Unfold:" << std::endl;
    res = UnfoldRelation(gp, edge_label_list);
    if (res < 0) return res;

    return res;
  }

 private:
  std::vector<std::string> edge_label_name_list_;  
};

}  // namespace gptools

#endif