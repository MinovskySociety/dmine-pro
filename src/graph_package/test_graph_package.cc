#include <graph_package/graph_package.h>
#include <graph_package/csvgp.h>

#include <gundam/large_graph2.h>

int main() {
  using namespace GUNDAM;
  using namespace graphpackage;

  using Graph = GUNDAM::LargeGraph2<uint64_t, uint32_t, std::string, uint64_t,
                                    uint32_t, std::string>;
  using GraphPackage = GraphPackage<Graph>;

  int res;

  GraphPackageInfo info;
  info.dict_dir = info.graph_dir = "/share/work/cu/cu4999_0";
  info.name = "liantong";

  GraphPackage gp;
  res = ReadGraphPackage(info, gp);
  if (res < 0) return res;
  
  info.dict_dir = info.graph_dir = "/share/work/cu/cu4999_0a";
  info.name = "liantong";

  res = WriteGraphPackage(info, gp);
  if (res < 0) return res;

  return 0;
}