#ifndef _GUNDAM_TOOL_UNIQUE_PATTERNS_H
#define _GUNDAM_TOOL_UNIQUE_PATTERNS_H

#include "gundam/tool/same_pattern.h"

template<typename GraphPatternType>
inline void UniquePatterns(std::vector<GraphPatternType>& graph_patterns) {

  for (auto graph_pattern_it  = graph_patterns.begin();
            graph_pattern_it != graph_patterns.end();){
    bool duplicate = false;
    for (auto uniqued_graph_pattern_it  = graph_patterns.begin();
              uniqued_graph_pattern_it != graph_pattern_it;
              uniqued_graph_pattern_it++) {
      if (SamePattern(*graph_pattern_it,
              *uniqued_graph_pattern_it)){
        duplicate = true;
        break;
      }
    }
    if (duplicate){
      graph_pattern_it = graph_patterns.erase(graph_pattern_it);
      continue;
    }
    graph_pattern_it++;
  }
  return;
}

#endif // _GUNDAM_TOOL_UNIQUE_PATTERNS_H