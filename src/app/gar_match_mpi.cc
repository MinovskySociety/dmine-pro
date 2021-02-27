#include <gundam/csvgraph.h>
#include <gundam/large_graph.h>
#include <gundam/matchresult.h>
#include <gundam/small_graph.h>
#include <mpi.h>

#include <string>

#include "gar/csv_gar.h"
#include "gar/gar.h"
#include "gar/gar_match.h"
#include "gar_config.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cout << "parameter num is not correct!(only 0 or 1)" << std::endl;
    return 0;
  }
  std::string config_file_path = "gar_match_config.yaml";
  if (argc == 2) {
    config_file_path = argv[1];
  }

  int res;

  gar::GARMatchInfo gar_match_info;
  res = ReadGARMatchInfoYAML(config_file_path, gar_match_info);
  if (res < 0) return res;

  using Pattern = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string, uint64_t,
                                     uint32_t, std::string>;
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  using GAR = gmine_new::GraphAssociationRule<Pattern, DataGraph>;
  using PatternVertexID = typename Pattern::VertexType::IDType;
  using DataGraphVertexID = typename DataGraph::VertexType::IDType;
  using PatternVertexConstPtr = typename Pattern::VertexConstPtr;
  using DataGraphVertexConstPtr = typename DataGraph::VertexConstPtr;

  using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
  using MatchResultList = std::vector<MatchMap>;

  GAR gar;
  res = gmine_new::ReadGAR(gar, gar_match_info.gar_list[0].vertex_file,
                           gar_match_info.gar_list[0].edge_file,
                           gar_match_info.gar_list[0].literal_x_file,
                           gar_match_info.gar_list[0].literal_y_file);
  if (res < 0) return res;

  DataGraph data_graph;
  res = GUNDAM::ReadCSVGraph(data_graph, gar_match_info.graph.vertex_file_list,
                             gar_match_info.graph.edge_file_list);
  if (res < 0) return res;

  // MPI init
  int myrank, nprocs;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (nprocs == 1) {
    // single machine
    // same as gar_match.cc

    auto time_begin = clock();

    MatchResultList match_result;

    int count = gmine_new::GARMatch<true>(gar, data_graph, match_result);
    auto time_end = clock();
    std::cout << "cal time = " << (1.0 * time_end - time_begin) / CLOCKS_PER_SEC
              << std::endl;
    if (count < 0) {
      std::cout << "Match Error!" << std::endl;
      return count;
    }
    std::cout << "Match count = " << count << std::endl;
    std::string result = gar_match_info.result_dir +
                         gar_match_info.gar_list[0].name + "_match.csv";
    std::ofstream result_file(result);

    MatchResultToFile<PatternVertexID, DataGraphVertexID>(match_result,
                                                          result_file);
    std::cout << "out end!" << std::endl;

  } else {
    auto time_begin = clock();

    // multi machine
    // add process get XY node
    auto& y_literal_set = gar.y_literal_set();
    auto& x_literal_set = gar.x_literal_set();
    std::set<PatternVertexConstPtr> pivot;
    for (const auto& it : x_literal_set) {
      it->CalPivot(pivot);
    }
    for (const auto& it : y_literal_set) {
      it->CalPivot(pivot);
    }
    Pattern contain_x_y_pattern;
    for (auto vertex_it = gar.pattern().VertexCBegin(); !vertex_it.IsDone();
         vertex_it++) {
      PatternVertexConstPtr vertex_ptr = vertex_it;
      if (pivot.count(vertex_ptr)) {
        contain_x_y_pattern.AddVertex(vertex_ptr->id(), vertex_ptr->label());
      }
    }
    int match_size;
    std::vector<std::vector<typename DataGraph::VertexType::IDType>>
        recv_message;
    if (myrank == 0) {
      // cal XY Match
      using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
      using MatchResultList = std::vector<MatchMap>;
      MatchResultList xy_match_result;
      GUNDAM::DPISO<GUNDAM::MatchSemantics::kIsomorphism>(
          contain_x_y_pattern, data_graph, -1, xy_match_result);
      // cal each process get match number
      int each_process_get_match_number =
          static_cast<int>(xy_match_result.size()) / (nprocs - 1);
      int remain_not_alloc_other_process_match_number =
          static_cast<int>(xy_match_result.size()) % (nprocs - 1);
      std::vector<int> each_process_get_match_number_list;
      for (int i = 1; i < nprocs; i++)
        each_process_get_match_number_list.push_back(
            each_process_get_match_number);
      for (int i = 0; i < remain_not_alloc_other_process_match_number; i++) {
        each_process_get_match_number_list[i]++;
      }
      // send each process match number
      for (int i = 1; i < nprocs; i++) {
        // std::cout << "rank = " << myrank << std::endl;
        MPI_Send(&each_process_get_match_number_list[i - 1], 1, MPI_INT, i, 0,
                 MPI_COMM_WORLD);
      }
      int now_complete_send_match_number = 0;
      for (int i = 1; i < nprocs; i++) {
        std::vector<std::vector<typename DataGraph::VertexType::IDType>>
            send_message;
        send_message.resize(pivot.size());
        // build send message to other process
        for (int match_pos = now_complete_send_match_number;
             match_pos < now_complete_send_match_number +
                             each_process_get_match_number_list[i - 1];
             match_pos++) {
          // std::cout << "match pos = " << match_pos << std::endl;
          int pivot_cur = 0;
          for (const auto& it : pivot) {
            // std::cout << "cur = " << pivot_cur << std::endl;
            send_message[pivot_cur].push_back(
                xy_match_result[match_pos]
                               [contain_x_y_pattern.FindConstVertex(it->id())]
                                   ->id());
            pivot_cur++;
          }
        }
        now_complete_send_match_number +=
            each_process_get_match_number_list[i - 1];
        for (const auto& it : send_message) {
          MPI_Send(it.data(), each_process_get_match_number_list[i - 1],
                   MPI_UINT64_T, i, 0, MPI_COMM_WORLD);
        }
      }
    } else {
      // each process recv xy match
      MPI_Status status;
      MPI_Recv(&match_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      recv_message.resize(pivot.size());
      for (int i = 0; i < pivot.size(); i++) recv_message[i].resize(match_size);
      for (int i = 0; i < pivot.size(); i++) {
        MPI_Recv(recv_message[i].data(), match_size, MPI_UINT64_T, 0, 0,
                 MPI_COMM_WORLD, &status);
      }
      std::cout << "[" << myrank << "]:get xy match from process 0!"
                << std::endl;
      std::cout << "[" << myrank << "]:match is:" << std::endl;
      int pivot_cur = 0;
      for (const auto& it : pivot) {
        std::cout << "[" << myrank << "]:" << it->id() << ":";
        for (const auto& message_it : recv_message[pivot_cur])
          std::cout << " " << message_it;
        std::cout << std::endl;
        pivot_cur++;
      }
    }
    using MatchMap = std::map<PatternVertexConstPtr, DataGraphVertexConstPtr>;
    using MatchResultList = std::vector<MatchMap>;
    MatchResultList total_match_result;
    if (myrank != 0) {
      // cal complete match
      using DataGraphVertexIDType = typename DataGraph::VertexType::IDType;
      using PatternVertexIDType = typename Pattern::VertexType::IDType;
      for (int i = 0; i < match_size; i++) {
        MatchMap match_state;
        int pivot_cur = 0;
        for (const auto& it : pivot) {
          DataGraphVertexIDType pivot_match_data_graph_id =
              recv_message[pivot_cur][i];
          match_state.emplace(
              it, data_graph.FindConstVertex(pivot_match_data_graph_id));
          pivot_cur++;
        }
        MatchResultList temp_match_result_list;
        gmine_new::GARMatch<true>(gar, data_graph, match_state,
                                  temp_match_result_list);
        for (const auto& single_match : temp_match_result_list)
          total_match_result.push_back(single_match);
      }
      // build send message
      std::vector<std::vector<DataGraphVertexIDType>> send_message;
      send_message.resize(gar.pattern().CountVertex());
      for (const auto& single_match : total_match_result) {
        int pattern_vertex_cur = 0;
        for (auto vertex_it = gar.pattern().VertexCBegin(); !vertex_it.IsDone();
             vertex_it++) {
          PatternVertexConstPtr vertex_ptr = vertex_it;
          send_message[pattern_vertex_cur].push_back(
              single_match.find(vertex_ptr)->second->id());
          pattern_vertex_cur++;
        }
      }
      // send to process 0
      int send_match_size = total_match_result.size();
      MPI_Send(&send_match_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      for (int i = 0; i < gar.pattern().CountVertex(); i++) {
        MPI_Send(send_message[i].data(), send_match_size, MPI_UINT64_T, 0, 0,
                 MPI_COMM_WORLD);
      }
    } else {
      using DataGraphVertexIDType = typename DataGraph::VertexType::IDType;
      using PatternVertexIDType = typename Pattern::VertexType::IDType;
      for (int i = 1; i < nprocs; i++) {
        int total_match_size;
        std::vector<std::vector<DataGraphVertexIDType>> recv_message;
        MPI_Status status;
        MPI_Recv(&total_match_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        recv_message.resize(gar.pattern().CountVertex());
        for (int i = 0; i < gar.pattern().CountVertex(); i++)
          recv_message[i].resize(total_match_size);
        for (int pos = 0; pos < gar.pattern().CountVertex(); pos++) {
          MPI_Recv(recv_message[pos].data(), total_match_size, MPI_UINT64_T, i,
                   0, MPI_COMM_WORLD, &status);
        }
        // build match result
        for (int num = 0; num < total_match_size; num++) {
          int pattern_vertex_cur = 0;
          MatchMap match_result;
          for (auto vertex_it = gar.pattern().VertexCBegin();
               !vertex_it.IsDone(); vertex_it++) {
            PatternVertexConstPtr vertex_ptr = vertex_it;
            DataGraphVertexConstPtr data_vertex_ptr =
                data_graph.FindConstVertex(
                    recv_message[pattern_vertex_cur][num]);
            match_result.emplace(vertex_ptr, data_vertex_ptr);
            pattern_vertex_cur++;
          }
          total_match_result.push_back(match_result);
        }
      }
      std::cout << "process 0 get match result is " << total_match_result.size()
                << std::endl;
      std::string result = gar_match_info.result_dir +
                           gar_match_info.gar_list[0].name + "_match.csv";
      std::ofstream result_file(result);
      MatchResultToFile<PatternVertexID, DataGraphVertexID>(total_match_result,
                                                            result_file);
      std::cout << "out end!" << std::endl;
    }
    MPI_Finalize();
    auto time_end = clock();
    std::cout << "Match Time: "
              << (1.0 * time_end - time_begin) / CLOCKS_PER_SEC << std::endl;
  }

  return 0;
}