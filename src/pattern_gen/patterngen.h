#ifndef _PATTERNGEN_H
#define _PATTERNGEN_H
#include <cstdlib>
#include <ctime>

#include "../gpar/test.cpp"
#include "../gpar_template/get_datagraph_information.h"
#include "gundam/csvgraph.h"
//#include "gundam/graph.h"
#include "gundam/large_graph.h"

namespace gmine_new {
void PatternGen(const char* v_file, const char* e_file, unsigned int edge_num,
                unsigned int pattern_num) {
  srand((unsigned)time(NULL));
  using namespace gmine_new;
  
  const VertexLabelType fake = 7, not_fake = 8, user = 4, phone = 5, web = 6;
  const EdgeLabelType call = 0, send = 1, visit = 2;
  const int user_num = 3;
  for (unsigned int number = 1; number <= pattern_num; number++) {
    GraphType g;
    for (VertexIDType vertex_id = 1; vertex_id <= user_num; vertex_id++) {
      g.AddVertex(vertex_id, user);
    }
    for (VertexIDType vertex_id = user_num + 1; vertex_id <= edge_num - 1;
         vertex_id++) {
      int pos = rand() % 2;
      if (pos)
        g.AddVertex(vertex_id, phone);
      else
        g.AddVertex(vertex_id, web);
    }
    for (EdgeIDType edge_id = 1; edge_id <= edge_num; edge_id++) {
      VertexIDType user_pos = rand() % user_num + 1;
      VertexIDType another_pos =
          rand() % (edge_num - 1 - user_num) + (user_num + 1);
      if (g.FindConstVertex(another_pos)->label() == web) {
        g.AddEdge(user_pos, another_pos, visit, edge_id);
      } else {
        int direct = rand() % 2;
        EdgeLabelType edge_type = rand() % 2 ? call : send;
        if (direct)
          g.AddEdge(user_pos, another_pos, edge_type, edge_id);
        else
          g.AddEdge(another_pos, user_pos, edge_type, edge_id);
      }
    }
    std::cout << "i=" << number << std::endl;
    TransposePattern(g);
  }
}
}  // namespace gmine_new
#endif