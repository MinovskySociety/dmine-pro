//#include "gpar.h"
#ifndef _TEST_CPP
#define _TEST_CPP
#include "../gpar_template/gpar_new.h"
/*
void TestRead(){
    gmine::DiscoverGPAR g;
    gmine::ReadGraph(g.target_graph_.target_graph_,
              "C:\\Users\\Administrator\\Desktop\\graph\\testdata\\graph-0.v",
              "C:\\Users\\Administrator\\Desktop\\graph\\testdata\\graph-0.e");
    int node_sz=g.target_graph_.target_graph_.getGraphSize();
    std::cout<<"node label:\n";
    for (int i=1;i<=node_sz;++i){
        std::cout<<i<<"
"<<g.target_graph_.target_graph_.getVertex(i)->getLabel()<<std::endl;
    }
    std::cout<<"edge:\n";
    for (int i=1;i<=node_sz;i++){
        std::vector<int>
label_list=g.target_graph_.target_graph_.getVertex(i)->GetAllEdgeLabel(1); for
(auto it : label_list)
        {
            Edge e(it);
            for (auto bit =
g.target_graph_.target_graph_.getVertex(i)->getIter(1, e); !bit->isDone();
bit->toNext())
            {
                std::cout << i << " " << bit->getVertex()->getID() << " " << it
<< std::endl;
            }
        }
    }
}
*/
namespace gmine_new {
template <class GraphType>
void TransposePattern(const GraphType &G) {
  size_t sz = G.CountVertex();
  std::cout << "node label:\n";
  for (int i = 1; i <= sz; i++) {
    std::cout << i << " " << G.FindVertex(i)->label() << "\n";
  }
  std::cout << "edge:\n";
  for (int i = 1; i <= sz; i++) {
    for (auto it = G.FindVertex(i)->OutEdgeBegin(); !it.IsDone(); it++) {
      std::cout << it->const_src_handle()->id() << " "
                << it->const_dst_handle()->id() << " " << it->label() << "\n";
    }
  }
}
}  // namespace gmine_new

#endif