add_library(graph_package "gpdef.h" "label_dict.h" "attribute_dict.h" "type_dict.h" "graph_package.h" "csvgp.h" "csvgp.cc")

add_executable(test_graph_package "test_graph_package.cc")
target_link_libraries(test_graph_package PRIVATE graph_package)
set_target_properties(test_graph_package PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/test)