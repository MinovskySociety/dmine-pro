#ifndef TGRAPH_RULE_H
#define TGRAPH_RULE_H

#include "config.h"
#include "global.h"

namespace toy {
class TGraphRule {
 public:
  using iterator = std::vector<Q>::iterator;
  using const_iterator = std::vector<Q>::const_iterator;

  TGraphRule() {}
  ~TGraphRule() {}

  void LoadLink(Config &config) {
    LOG_S("Load TGR Strat");
    auto t_begin = std::chrono::steady_clock::now();

    // default link id like belows
    const VID_T from = 1;
    const VID_T to = 2;
    const EID_T eid = 1;
    // init link
    link_.from_ = from;
    link_.to_ = to;
    link_.id_ = eid;
    link_.from_label_ = config.XLabel();
    link_.to_label_ = config.YLabel();
    link_.label_ = config.QLabel();
    link_.has_order_ = config.WhetherQHasOrder();
    // init query
    tgr_set_.resize(1);
    auto &query = tgr_set_[0];
    long order = link_.has_order_ ? 1 : 0;
    query.AddVertex(from, link_.from_label_);
    query.AddVertex(to, link_.to_label_);
    auto edge_pair = query.AddEdge(from, to, link_.label_, eid);
    edge_pair.first->AddAttribute(TIME_KEY, static_cast<TIME_T>(order));

    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Load TGR Success, Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
  }

  void LoadTGRs(Config &config) {
    LOG_S("Load TGR Strat");
    auto t_begin = std::chrono::steady_clock::now();
    auto pattern_path_vec = config.PatternFilePathVec();
    auto x_literal_path_vec = config.XLiteralFilePathVec();
    auto y_literal_path_vec = config.YLiteralFilePathVec();
    tgr_set_.resize(pattern_path_vec.size());
    for (int i = 0; i < pattern_path_vec.size(); i++) {
      loadXLiteralFromLocal(x_literal_path_vec[i]);
      loadYLiteralFromLocal(y_literal_path_vec[i]);
      loadVEFileFromLocal(tgr_set_[i], pattern_path_vec[i].first,
                          pattern_path_vec[i].second);
    }
    auto t_end = std::chrono::steady_clock::now();
    LOG_S("Load TGR Success, Time: ",
          std::chrono::duration<double>(t_end - t_begin).count());
  }

  inline std::vector<Q> &GetTGRSet() { return tgr_set_; }
  inline iterator Begin() { return tgr_set_.begin(); }
  inline iterator End() { return tgr_set_.end(); }

  inline const_iterator CBegin() const { return tgr_set_.cbegin(); }
  inline const_iterator CEnd() const { return tgr_set_.cend(); }
  inline size_t Size() const { return tgr_set_.size(); }
  inline const Q &GetTGR(uint32_t n) const { return tgr_set_[n]; }
  inline const LinkBase &GetLink() const { return link_; }

 private:
  void loadXLiteralFromLocal(const std::string &xfile) {}
  void loadYLiteralFromLocal(const std::string &yfile) {
    std::ifstream infile;
    std::string line;
    try {
      infile.open(yfile.data());
      uint32_t cnt = 0;
      while (getline(infile, line)) {
        if (cnt++ == 0) continue;
        std::stringstream ss(line);
        std::string str;
        std::vector<std::string> str_list;
        while (getline(ss, str, ',')) str_list.emplace_back(str);
        link_.id_ = 1;
        link_.from_ = stol(str_list[1]);
        link_.to_ = stol(str_list[3]);
        link_.label_ = stol(str_list[5]);
        if (str_list.size() > 6 && str_list[6] != "") {
          link_.order_ = stol(str_list[6]);
          if (link_.order_ < 0) link_.order_ = 0;
        }
        link_.has_order_ = link_.order_ > 0;
      }
      infile.close();
    } catch (std::exception &e) {
      LOG_E("Load TGR YLiteralFile Error: ", e.what());
      throw(TException::LoadPatternError);
    }
  }
  void loadVEFileFromLocal(Q &query, const std::string &vfile,
                           const std::string &efile) {
    std::ifstream infile;
    std::string line;
    // load vfile
    try {
      infile.open(vfile.data());
      uint32_t cnt = 0;
      while (getline(infile, line)) {
        if (cnt++ == 0) continue;
        std::stringstream ss(line);
        std::string str;
        std::vector<std::string> str_list;
        while (getline(ss, str, ',')) str_list.emplace_back(str);
        VID_T vid = stol(str_list[0]);
        VLABEL_T vlabel = stol(str_list[1]);
        query.AddVertex(vid, vlabel);
        // set labels of x and y
        if (vid == link_.from_) {
          link_.from_label_ = vlabel;
        }
        if (vid == link_.to_) {
          link_.to_label_ = vlabel;
        }
      }
      infile.close();
    } catch (std::exception &e) {
      LOG_E("Load TGR VFile Error: ", e.what());
      throw(TException::LoadPatternError);
    }
    // set link q(x,y)
    auto edge_pair =
        query.AddEdge(link_.from_, link_.to_, link_.label_, link_.id_);
    edge_pair.first->AddAttribute(TIME_KEY, static_cast<TIME_T>(link_.order_));
    // load efile
    try {
      infile.open(efile.data());
      uint32_t cnt = 0;
      while (getline(infile, line)) {
        if (cnt++ == 0) continue;
        std::stringstream ss(line);
        std::string str;
        std::vector<std::string> str_list;
        while (getline(ss, str, ',')) str_list.emplace_back(str);
        EID_T eid = stol(str_list[0]);
        VID_T from = stol(str_list[1]);
        VID_T to = stol(str_list[2]);
        ELABEL_T label = stol(str_list[3]);
        TIME_T order = stol(str_list[4]);
        auto edge_pair = query.AddEdge(from, to, label, eid);
        edge_pair.first->AddAttribute(TIME_KEY, static_cast<TIME_T>(order));
      }
      infile.close();
    } catch (std::exception &e) {
      LOG_E("Load TGR EFile Error: ", e.what());
      throw(TException::LoadPatternError);
    }
  }

  std::vector<Q> tgr_set_;
  LinkBase link_;
};
}  // namespace toy

#endif  // TGRAPH_RULE_H
