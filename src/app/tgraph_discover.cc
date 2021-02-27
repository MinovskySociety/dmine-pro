#include "../tgraph_rule/tgraph.h"
#include "../tgraph_rule/config.h"
#include "../tgraph_rule/discover.h"
#include "../tgraph_rule/match.h"
#include "../tgraph_rule/partition.h"
#include "../tgraph_rule/tgraph_rule.h"

using namespace toy;

int main(int argc, char **argv) {
  if (argc < 2) {
    LOG_E("config yaml path must be given!");
    exit(-1);
  }
  // init log level
  set_log_grade(LOG_T_GRADE);

  // init config
  Config config;
  config.LoadDiscoveryConfigByYaml(argv[1]);
  if (argc == 3) {
    config.LoadTimeWindowConfigByYaml(argv[2]);
  }
  config.PrintConfig();

  // partition graph by time window
  Partition::DoPartition(config);

  // load temporal graph
  TGraph tg(config);
  tg.LoadTGraph();

  // filter label by edge frequency limit
  tg.CalEdgeStatics();
  tg.PrintEdgeStatics();
  tg.FilterEdgeStatics();
  tg.PrintEdgeStatics();

  // load prediction link
  TGraphRule tg_rule;
  tg_rule.LoadLink(config);

  // init match method
  auto m_ptr = MatchFactory::GetMatchMethod(config);
  m_ptr->InitX(tg, tg_rule.GetLink());

  auto m_no_ptr = std::make_shared<MatchNoOrder>();
  m_no_ptr->InitX(tg, tg_rule.GetLink());

  // init discovery settings
  Discover dis(tg, tg_rule, config, m_ptr, m_no_ptr);
  // do discover
  dis.DoDiscover();
}
