#ifndef GLOBAL_H
#define GLOBAL_H

#include <chrono>
#include <iostream>
#include <string>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "gundam/algorithm/dp_iso.h"
#include "gundam/algorithm/vf2_boost.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/io/csvgraph.h"

namespace toy {
using namespace GUNDAM;

/* Vertex and Edge TYPE */
typedef uint64_t VID_T;
typedef uint64_t EID_T;
typedef uint32_t VLABEL_T;
typedef uint32_t ELABEL_T;
typedef uint32_t TIME_T;
typedef uint32_t SIZE_T;
typedef float GRADE_T;

const uint64_t MAX_NODE_ID = 26;    // node id number < 2^26 = 67,108,864
const uint64_t MAX_LABEL_ID = 12;   // label number < 2^12 = 4,096
const double MAX_MATCH_TIME = 800;  // match time threshold
const std::string V_FILE_SUFFIX = "_v.csv";  // suffix of vertex filename
const std::string E_FILE_SUFFIX = "_e.csv";  // suffix of edge filename
const std::string MATCH_FILE_SUFFIX = "_match.csv";  // suffix of match filename
const std::string TIME_KEY = "timestamp";            // time order key
const std::string PLABEL = "*********  ";            // test log symbol
const std::string DISCOVERY_PRINT_FILE_NAME = "pattern";
const std::string TEST_SYMBOL = "[TEST] ";
const std::string INFO_SYMBOL = "[INFO] ";
const std::string SYSTEM_SYMBOL = "[SYSM] ";
const std::string WARN_SYMBOL = "[WARN] ";
const std::string ERROR_SYMBOL = "[ERROR] ";

/* Prediction Link
 * @desc: store information of prediction link
 */
struct LinkBase {
  EID_T id_;
  VID_T from_;
  VLABEL_T from_label_;
  VID_T to_;
  VLABEL_T to_label_;
  ELABEL_T label_;
  bool has_order_;
  TIME_T order_;
};

/* Prediction Link
 * @desc: store information of prediction link
 */
struct NewEdge {
  VID_T expand_vid_;        // expand from this vid
  VLABEL_T expand_vlabel_;  // its label
  VID_T next_vid_;          // next id if add new vertex
  VLABEL_T next_vlabel_;    // next vertex label
  EID_T next_eid_;          // next edge id
  ELABEL_T expand_elabel_;  // next edge label
  bool has_order_;          // next edge has order
  bool is_out_;             // next edge is outer edge
  TIME_T max_order_;        // max order in query
};

/* Some simplify */
using DataGraph = GUNDAM::LargeGraph2<VID_T, VLABEL_T, std::string, EID_T,
                                      ELABEL_T, std::string>;
using TargetVertex = typename GUNDAM::VertexHandle<DataGraph>::type;
using VertexIDType = typename DataGraph::VertexType::IDType;
using EdgeLabelType = typename DataGraph::EdgeType::LabelType;
using EdgePtr = typename GUNDAM::EdgeHandle<DataGraph>::type;
using EdgeConstPtr = typename GUNDAM::EdgeHandle<DataGraph>::type;
using Pattern = GUNDAM::LargeGraph2<VID_T, VLABEL_T, std::string, EID_T,
                                    ELABEL_T, std::string>;
using Q = Pattern;
using QueryVertex = typename GUNDAM::VertexHandle<Pattern>::type;
using MatchMap = std::map<QueryVertex, TargetVertex>;
using MatchResult = std::vector<MatchMap>;
using CandidateSetContainer = std::map<QueryVertex, std::vector<TargetVertex>>;
using EdgeMap = std::vector<std::vector<TIME_T>>;  // e in query, format:
                                                   // (timestamp, map_id), ...
using TimeQueue =
    std::priority_queue<TIME_T, std::vector<TIME_T>, std::greater<TIME_T>>;

/* comparision function as time ascending */
bool time_cmp(std::vector<TIME_T> &a, std::vector<TIME_T> &b) {
  return a[0] < b[0];
}

/* comparision function as confidence descending */
bool conf_cmp(std::pair<Q, GRADE_T> &a, std::pair<Q, GRADE_T> &b) {
  return a.second > b.second;
}

/////////////// App type //////////////
enum APP_TYPE {
  DISCOVERY = 0,
  APPLY = 1,
};

std::string AppTypeStr(APP_TYPE app_type) {
  switch (app_type) {
    case DISCOVERY:
      return "discovery";
    case APPLY:
      return "apply";
    default:
      return "unknown";
  }
}

/////////////// Time window partition method //////////////
enum PARTITION_METHOD {
  PARTITION_BY_TIME_WINDOW_SIZE = 0,
  PARTITION_BY_TIME_WINDOW_NUMBER = 1,
  NO_METHOD = 99,
};

std::string PartitonMethodStr(PARTITION_METHOD partition_method) {
  switch (partition_method) {
    case PARTITION_BY_TIME_WINDOW_SIZE:
      return "partition_by_time_window_size";
    case PARTITION_BY_TIME_WINDOW_NUMBER:
      return "partition_by_time_window_number";
    case NO_METHOD:
      return "no_method";
    default:
      return "unknown";
  }
}

/////////////// ACCESS, MKDIR & TIMER //////////////
#ifdef WIN32
#define Access(fileName, accessMode) _access(fileName, accessMode)
#define Mkdir(path) _mkdir(path)
#else
#define Access(fileName, accessMode) access(fileName, accessMode)
#define Mkdir(path) mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

/////////////// Log min-system //////////////
enum LOG_GRADE {
  LOG_T_GRADE = 0,  // Test Level
  LOG_I_GRADE = 1,  // Info Level
  LOG_S_GRADE = 2,  // System Level
  LOG_W_GRADE = 3,  // Warning Level
  LOG_E_GRADE = 4,  // Error Level
};

LOG_GRADE LOG_GRADE_SET = LOG_GRADE::LOG_S_GRADE;
void set_log_grade(LOG_GRADE grade) { LOG_GRADE_SET = grade; }

void LOG_T(const std::string str) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_T_GRADE) return;
  std::cout << TEST_SYMBOL << str << "\n";
}

template <typename T>
void LOG_T(const std::string str, T n) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_T_GRADE) return;
  std::cout << TEST_SYMBOL << str << n << "\n";
}

void LOG(const std::string str) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_I_GRADE) return;
  std::cout << INFO_SYMBOL << str << "\n";
}

template <typename T>
void LOG(const std::string str, T n) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_I_GRADE) return;
  std::cout << INFO_SYMBOL << str << n << "\n";
}

void LOG_S(const std::string str) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_S_GRADE) return;
  std::cout << SYSTEM_SYMBOL << str << "\n";
}

template <typename T>
void LOG_S(const std::string str, T n) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_S_GRADE) return;
  std::cout << SYSTEM_SYMBOL << str << n << "\n";
}

void LOG_W(const std::string str) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_W_GRADE) return;
  std::cout << WARN_SYMBOL << str << "\n";
}

template <typename T>
void LOG_W(const std::string str, T n) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_W_GRADE) return;
  std::cout << WARN_SYMBOL << str << n << "\n";
}

void LOG_E(const std::string str) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_E_GRADE) return;
  std::cout << ERROR_SYMBOL << str << "\n";
}

template <typename T>
void LOG_E(const std::string str, T n) {
  if (LOG_GRADE_SET > LOG_GRADE::LOG_E_GRADE) return;
  std::cout << ERROR_SYMBOL << str << n << "\n";
}

/////////////// Exception min-system //////////////
enum TException {
  BasicError = 0,
  LoadGraphError = 1,
  LoadPatternError = 2,
  PartitionError = 3,
  ConfigError = 4,
  ParameterError = 5,
};

/////////////// Numerical check //////////////
void VERTEX_CHECK(bool f) {
  if (f == false) {
    LOG_E("Vertex Error: can't find given vid");
    throw(TException::ParameterError);
  }
}

template <typename T>
void NATURE_CHECK(T num) {
  if (num < 0) {
    LOG_E("given number < 0 is forbidden");
    throw(TException::ParameterError);
  }
}

template <typename T>
void POSITIVE_CHECK(T num) {
  if (num <= 0) {
    LOG_E("given number <= 0 is forbidden");
    throw(TException::ParameterError);
  }
}

template <typename T>
void ATTRIBUTE_PTR_CHECK(T ptr) {
  if (ptr.IsNull()) {
    LOG_E("Time attribute pointer is null!");
    throw(TException::ParameterError);
  }
}

/////////////// util method //////////////
template <typename T>
void GetIntersection(std::list<T> &inter, TimeQueue pri_queue) {
  // if inter is empty, add pri_queue into inter.
  if (inter.empty()) {
    while (!pri_queue.empty()) {
      inter.emplace_back(pri_queue.top());
      pri_queue.pop();
    }
    return;
  }
  // else, cal intersection between inter and pri_queue.
  auto inter_iter = inter.begin();
  while (inter_iter != inter.end() && !pri_queue.empty()) {
    if (*inter_iter == pri_queue.top()) {
      inter_iter++;
      pri_queue.pop();
    } else if (*inter_iter < pri_queue.top()) {
      inter_iter = inter.erase(inter_iter);
    } else {
      pri_queue.pop();
    }
  }
  while (inter_iter != inter.end()) {
    inter_iter = inter.erase(inter_iter);
  }
}

void RemoveLink(Q &query, const LinkBase &link) {
  query.EraseEdge(link.id_);
  /*auto vertex_ptr = query.FindConstVertex(link.to_);
  if (vertex_ptr) {
    if (vertex_ptr->CountOutEdge() == 0 && vertex_ptr->CountInEdge() == 0) {
      query.EraseVertex(link.to_);
    }
  }*/
}

/* Get Hash
 * @param: src id, dst id, elabel
 * @return: hash code
 * @desc: calculate hash code by given numbers
 */
uint64_t GetHash(uint64_t from, uint64_t to, uint64_t elabel) {
  return (from << (MAX_NODE_ID + MAX_LABEL_ID)) + (to << MAX_LABEL_ID) + elabel;
}

/* Get Label Hash
 * @param: vertex label1, vertex label2, elabel
 * @return: hash code
 * @desc: calculate hash code by given numbers
 */
uint64_t GetLabelHash(VLABEL_T vlabel1, VLABEL_T vlabel2, ELABEL_T elabel) {
  return (vlabel1 << (MAX_LABEL_ID * 2)) + (vlabel2 << MAX_LABEL_ID) + elabel;
}

/* Get Label Hash
 * @param: vertex label1, vertex label2
 * @return: hash code
 * @desc: calculate hash code by given numbers
 */
uint64_t GetLabelHash(VLABEL_T vlabel1, VLABEL_T vlabel2) {
  return (vlabel1 << MAX_LABEL_ID) + vlabel2;
}

/* For Test, Print Edge Map Info */
void TestPrintEdgeMap(EdgeMap edge_map, std::vector<TimeQueue> time_q_set,
                      int i = 0) {
  if (edge_map.size() != time_q_set.size()) {
    throw "error not matched";
  }
  std::cout << "edge map info:\n";
  for (; i < edge_map.size(); i++) {
    auto order = edge_map[i][0];
    auto map_id = edge_map[i][1];
    std::cout << "order: " << order << ", map_id: " << map_id
              << ", timestamp: ";
    auto &time_queue = time_q_set[map_id];
    while (!time_queue.empty()) {
      std::cout << time_queue.top() << ", ";
      time_queue.pop();
    }
    std::cout << "\n";
  }
}

/* For Test, Print Link Info */
void PrintLink(const LinkBase &link) {
  std::cout << PLABEL << "Link Detail  " << PLABEL << "\n";
  std::cout << PLABEL << "X\n";
  std::cout << PLABEL << link.from_ << "," << link.from_label_ << "\n";
  std::cout << PLABEL << "Y\n";
  std::cout << PLABEL << link.to_ << "," << link.to_label_ << "\n";
  std::cout << PLABEL << "L\n";
  std::cout << PLABEL << link.id_ << "," << link.from_ << "," << link.to_ << ","
            << link.label_ << "," << link.has_order_ << "\n";
}

/* For Test, Print TGR Info */
void PrintTGR(const Q &query) {
  std::cout << PLABEL << "TGR Detail  " << PLABEL << "\n";
  std::cout << PLABEL << "V\n";
  for (auto v_iter = query.VertexBegin(); !v_iter.IsDone(); v_iter++) {
    std::cout << PLABEL << v_iter->id() << "," << v_iter->label() << "\n";
  }
  std::cout << PLABEL << "E\n";
  for (auto e_iter = query.EdgeBegin(); !e_iter.IsDone(); e_iter++) {
    auto attribute_ptr = e_iter->FindAttribute(TIME_KEY);
    ATTRIBUTE_PTR_CHECK(attribute_ptr);
    TIME_T timestamp = attribute_ptr->template value<int>();
    std::cout << PLABEL << e_iter->id() << "," << e_iter->src_id() << ","
              << e_iter->dst_id() << "," << e_iter->label() << "," << timestamp
              << "\n";
  }
}

/* For Test, Print Match Info */
void PrintMatch(const std::vector<MatchResult> &match_stream) {
  uint64_t cnt = 0;
  for (const auto &match_res : match_stream) {
    cnt += match_res.size();
  }
  std::cout << PLABEL << "Match Detail  " << PLABEL << "\n";
  std::cout << PLABEL << "Total Match Number: " << cnt << "\n";
  cnt = 0;
  for (const auto &match_set : match_stream) {
    std::cout << PLABEL << "Time Window " << cnt++ << ", Match "
              << match_set.size() << "\n";
    for (const auto &match : match_set) {
      std::cout << PLABEL;
      for (const auto iter : match) {
        const auto query_vertex = iter.first;
        const auto target_vertex = iter.second;
        std::cout << query_vertex->id() << "-" << target_vertex->id() << ", ";
      }
      std::cout << "\n";
    }
  }
}

/* For Test, Print Conf Info */
void PrintConf(const std::vector<std::pair<GRADE_T, GRADE_T>> &confs) {
  for (const auto &co : confs) {
    GRADE_T supp_q = co.first;
    GRADE_T supp_r = co.second;
    LOG("Supp Q: ", supp_q);
    LOG("Supp R: ", supp_r);
    GRADE_T conf = (supp_r * 1.0) / supp_q;
    LOG("Percision: ", conf);
  }
}

}  // namespace toy
#endif  // GLOBAL_H
