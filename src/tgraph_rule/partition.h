#ifndef PARTITION_H
#define PARTITION_H

#include "config.h"
#include "global.h"

namespace toy {
class Partition {
 public:
  Partition() {}
  ~Partition() {}

  static void DoPartition(Config &config) {
    // use time window
    PARTITION_METHOD method = config.PartitionMethod();
    if (method == NO_METHOD) {
      LOG_S("No Partition");
      return;
    }
    SIZE_T number = config.TimeWindowNumber();
    TIME_T size = config.TimeWindowSize();
    if (method == PARTITION_BY_TIME_WINDOW_SIZE) {
      if (size <= 0) {
        LOG_E("wrong error! time window size must greater than 0.");
        throw(TException::PartitionError);
      }
      LOG_S("Do Partition By Time Window Size...");
      partition(config.GraphEFilePath(), method, number, size);
      config.SetTimeWindowNumber(number);
    } else if (method == PARTITION_BY_TIME_WINDOW_NUMBER) {
      if (number <= 0) {
        LOG_E("wrong error! time window number must greater than 0.");
        throw(TException::PartitionError);
      }
      LOG_S("Do Partition By Time Window Number...");
      partition(config.GraphEFilePath(), method, number, size);
      config.SetTimeWindowSize(size);
    }
    // print info
    {
      LOG("Time Window Size: ", config.TimeWindowSize());
      LOG("Time Window Number: ", config.TimeWindowNumber());
    }
  }

 private:
  static void partition(const std::string efile, PARTITION_METHOD method,
                        SIZE_T &window_num, TIME_T &window_size) {
    std::ifstream infile;
    //  0,   1,   2,   3,    4
    // id, from, to, label, time
    std::vector<std::vector<uint64_t>> edge_list;
    infile.open(efile.data());
    std::string line;
    TIME_T max_time = 0;
    TIME_T min_time = 0x7FFFFFFF;
    TIME_T time_span = 0;

    // get edge info
    LOG("Read Graph...");
    int cnt = 0;
    while (getline(infile, line)) {
      if (cnt++ == 0) {
        // jump desc header
        continue;
      }
      if (cnt % 100000 == 0) {
        LOG_T("Partitioned: ", cnt);
      }
      std::stringstream ss(line);
      std::string str;
      std::vector<uint64_t> edge;
      edge.reserve(5);
      while (getline(ss, str, ',')) edge.emplace_back(stol(str));
      edge_list.emplace_back(edge);
      if (max_time < edge[4]) max_time = edge[4];
      if (edge[4] > 0 && min_time > edge[4]) min_time = edge[4];
    }
    // bigger than each timestamp of edge
    max_time++;
    time_span = max_time - min_time;
    LOG_T("min timestamp: ", min_time);
    LOG_T("max timestamp: ", max_time);

    // cal time window or partition number
    if (method == PARTITION_BY_TIME_WINDOW_SIZE) {
      window_num = static_cast<SIZE_T>(ceil((time_span * 1.0) / window_size));
    } else if (method == PARTITION_BY_TIME_WINDOW_NUMBER) {
      window_size = static_cast<TIME_T>(ceil((time_span * 1.0) / window_num));
    }
    LOG_T("time window number: ", window_num);
    LOG_T("time window size: ", window_size);

    // set output file
    LOG("Write Partitioned Graph...");
    std::vector<std::ofstream> outfiles(window_num);
    std::vector<uint64_t> outfile_cnts(window_num, 0);
    for (int i = 0; i < window_num; i++) {
      std::string efile_p_csv = efile + std::to_string(i);
      outfiles[i].open(efile_p_csv.data());
      outfiles[i] << "edge_id:int64,source_id:int64,target_id:int64,label_id:"
                     "int,timestamp:int64\n";
    }
    // partition
    for (auto &edge : edge_list) {
      if (edge[4] == 0) {
        for (int i = 0; i < window_num; i++) {
          outfiles[i] << ++(outfile_cnts[i]) << "," << edge[1] << "," << edge[2]
                      << "," << edge[3] << "," << edge[4] << "\n";
        }
      } else {
        size_t pos = (edge[4] - min_time) / window_size;
        outfiles[pos] << ++(outfile_cnts[pos]) << "," << edge[1] << ","
                      << edge[2] << "," << edge[3] << "," << edge[4] << "\n";
      }
    }
    // close file
    for (int i = 0; i < window_num; i++) {
      outfiles[i].close();
    }
  }

};  // class Partition
}  // namespace toy

#endif  // PARTITION_H
