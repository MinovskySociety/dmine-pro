#ifdef _MSC_VER
#include <crtdbg.h>
#endif  // _MSC_VER

#include "ecs_api.h"
#include "gundam/graph_type/graph.h"
#include "gundam/io/csvgraph.h"
#include "gundam/io/rapidcsv.h"

using namespace std;
using namespace GUNDAM;
using namespace ECS;

int main() {
#ifdef _MSC_VER
  //_CrtSetBreakAlloc(636658);
#endif  // _MSC_VER
  Graph<SetVertexIDType<uint64_t>, SetVertexLabelType<string>,
        SetVertexHasAttribute<true>, SetVertexAttributeKeyType<string>,
        SetEdgeIDType<uint64_t>, SetEdgeLabelType<string>,
        SetEdgeAttributeKeyType<string>, SetAllowMultipleEdge<true>>
      graph;
  try {
    uint64_t ret = ECS_E_UNKNOWN;
    vector<std::string> v_list;
    vector<std::string> e_list;
    // Node file从左至右分别是Node ID、Node Label ID、Node Name、Node Attributes
    v_list.emplace_back(
        "E:\\test\\graphData\\test_repeat_edge_with_attrs\\liantong_n_attr_"
        "phone.csv");
    v_list.emplace_back(
        "E:\\test\\graphData\\test_repeat_edge_with_attrs\\liantong_n_attr_"
        "user.csv");
    e_list.emplace_back(
        "E:\\test\\graphData\\test_repeat_edge_with_attrs\\liantong_e_attr_"
        "call.csv");
    ret = ReadCSVGraph(graph, v_list, e_list);
    if (ret < 0) {
      return static_cast<int>(ret);
    }
    cout << "The total number of nodes and edges is: " << ret << endl;

    ECS::print_graph(graph);
    // count the number of calls per user
    cout << endl << endl;
    cout << "Begin count user_call number..." << endl;
    // ret = count_user_call_nums(graph, true);
    if (ret != ECS_OK) {
      return static_cast<int>(ret);
    }

    cout << endl << endl;
    cout << "Begin remove duplicate edge..." << endl;
    ret = remove_dup_calls(graph);
    if (ret != ECS_OK) {
      return static_cast<int>(ret);
    }
    ECS::print_graph(graph);
  } catch (const exception& ex) {
    cout << ex.what() << endl;
    return ECS_E_UNKNOWN;
  }
#ifdef _MSC_VER
  _CrtDumpMemoryLeaks();
#endif  // _MSC_VER
}