# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/graph_convert/)

# 生成可执行程序
add_executable(csv_to_rdf csv_to_rdf.cpp)
target_link_libraries(csv_to_rdf PRIVATE yaml-cpp glog::glog)
# 生成可执行程序
add_executable(csv_to_gsi_graph csv_to_gsi_graph.cpp)
target_link_libraries(csv_to_gsi_graph PRIVATE yaml-cpp glog::glog)