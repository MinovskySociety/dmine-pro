#include <iostream>

#include "../gpar/gpar.h"
#include "gar/csv_gar.h"
#include "gundam/graph_type/large_graph.h"
void TestReadGAR() {
  using DataGraph = GUNDAM::LargeGraph<uint64_t, uint32_t, std::string,
                                       uint64_t, uint32_t, std::string>;

  gmine_new::GraphAssociationRule<gmine_new::GraphType, DataGraph> test;

  gmine_new::ReadGAR(test,
                     "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                     "pro\\testdata\\GAR\\node.csv",
                     "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                     "pro\\testdata\\GAR\\edge.csv",
                     "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                     "pro\\testdata\\GAR\\X.csv",
                     "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                     "pro\\testdata\\GAR\\Y.csv");
  std::cout << "read end!" << std::endl;
  gmine_new::WriteGAR(test,
                      "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                      "pro\\testdata\\GAR\\node1.csv",
                      "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                      "pro\\testdata\\GAR\\edge1.csv",
                      "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                      "pro\\testdata\\GAR\\X1.csv",
                      "C:\\Users\\Administrator\\Desktop\\buaa\\dmine-"
                      "pro\\testdata\\GAR\\Y1.csv");
  std::cout << "write end!" << std::endl;
}

int main() {
  TestReadGAR();
  return 0;
}