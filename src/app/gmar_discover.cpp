#include "gmar/gmar_discover.h"

#include <glog/logging.h>

#include "gar/literalset.h"
#include "gmar/message_frquent_model.h"
#include "gmar/ml_model.h"
#include "gmar/user_phone_frquent_model.h"
#include "gpar_template/gpar_discover.h"
#include "gpar_template/gpar_new.h"
#include "gundam/graph_type/large_graph.h"
#include "gundam/graph_type/large_graph2.h"
#include "gundam/graph_type/small_graph.h"
#include "gundam/io/csvgraph.h"
int main(int argc, char* argv[]) {
  using Pattern = GUNDAM::SmallGraph<uint32_t, uint32_t, uint32_t, uint32_t>;
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO, "./");
  // build ML data graph
  DataGraph data_graph;
  std::string call_file =
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/liantong_e_call.csv";
  std::string send_file =
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/"
      "liantong_e_send_message.csv";
  std::string has_file =
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/liantong_e_has.csv";
  std::string is_file =
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/liantong_e_is.csv";
  std::string v_file =
      "/Users/apple/Desktop/buaa/data/cu4999_1_remove_web/liantong_v.csv";
  std::vector<std::string> v_list{v_file};
  std::vector<std::string> e_list{call_file, send_file, has_file, is_file};
  GUNDAM::ReadCSVGraph(data_graph, v_list, e_list);
  std::cout << "vertex num = " << data_graph.CountVertex() << " "
            << " edge num = " << data_graph.CountEdge() << std::endl;

  // gmine_new::MessageFrquentModel<Pattern, DataGraph> ml_model(10);

  gmine_new::UserPhoneFrquentModel<Pattern, DataGraph> ml_model(0);
  constexpr int max_iter_num = 3;
  gmine_new::GMARDiscover<Pattern, DataGraph>(ml_model, data_graph,
                                              max_iter_num);
  gmine_new::BaseContainer<gmine_new::MLModelBase<Pattern, DataGraph>>
      model_container;
  model_container.Add<gmine_new::UserPhoneFrquentModel<Pattern, DataGraph>>(0,
                                                                            -1);
  // gmine_new::GMARDiscover<Pattern, DataGraph>(model_container, data_graph,
  //                                            max_iter_num);
  return 0;
}