# CMakeList.txt: TestCMake1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.10)

# TODO: 如有需要，请添加测试并安装目标。

include(GoogleTest)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable (test1 "test1.cc")
target_link_libraries(test1 GTest::GTest GTest::Main)
gtest_add_tests(TARGET test1)

add_executable (test2 "test2.cc")
target_link_libraries(test2 GTest::GTest GTest::Main)
gtest_add_tests(TARGET test2)

add_executable (test_csvgraph "test_csvgraph.cc")
target_link_libraries(test_csvgraph GTest::GTest GTest::Main)
#gtest_add_tests(TARGET test_csvgraph)

add_executable (test_vf2 "test_vf2.cc")
target_link_libraries(test_vf2 GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_vf2)

add_executable (test_dp_iso "test_dp_iso.cc")
target_link_libraries(test_dp_iso GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_dp_iso)

add_executable (test_dp_iso_using_match "test_dp_iso_using_match.cc")
target_link_libraries(test_dp_iso_using_match GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_dp_iso_using_match)

add_executable (test_bfs "test_bfs.cc")
target_link_libraries(test_bfs GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_bfs)

add_executable (test_dfs "test_dfs.cc")
target_link_libraries(test_dfs GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_dfs)

add_executable (test_connected "test_connected.cc")
target_link_libraries(test_connected GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_connected)

add_executable (test_has_isolate_vertex "test_has_isolate_vertex.cc")
target_link_libraries(test_has_isolate_vertex GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_has_isolate_vertex)

add_executable (test_same_pattern "test_same_pattern.cc")
target_link_libraries(test_same_pattern GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_same_pattern)

add_executable (test_multi_vf2 "test_multi_vf2.cc")
target_link_libraries(test_multi_vf2 GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_multi_vf2)

add_executable (test_idgen "test_idgen.cc")
target_link_libraries(test_idgen GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_idgen)

add_executable (test_matchresult "test_writematchresult.cc")
target_link_libraries(test_matchresult GTest::GTest GTest::Main)
#gtest_add_tests(TARGET test_matchresult)

add_executable (test_datatype "test_datatype.cc")
target_link_libraries(test_datatype GTest::GTest GTest::Main)
#gtest_add_tests(TARGET test_datatype)

#add_executable (test_container "test_container.cc")
#target_link_libraries(test_container GTest::GTest GTest::Main)
#add_test(AllTests test_container)

#add_executable (test_iterator "test_iterator.cc")
#target_link_libraries(test_iterator GTest::GTest GTest::Main)
#add_test(AllTests test_iterator)

add_executable (test_attribute "test_attribute.cc")
target_link_libraries(test_attribute GTest::GTest GTest::Main)
add_test(AllTests test_attribute)

add_executable (test_graph "test_graph.cc")
target_link_libraries(test_graph GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_graph)

add_executable (test_simple_graph "test_simple_graph.cc")
target_link_libraries(test_simple_graph GTest::GTest GTest::Main)
#gtest_add_tests(TARGET test_simple_graph)

add_executable (test_iterator2 "test_iterator2.cc")
target_link_libraries(test_iterator2 GTest::GTest GTest::Main)
#gtest_add_tests(TARGET test_iterator2)

add_executable (test_container2 "test_container2.cc")
target_link_libraries(test_container2 GTest::GTest GTest::Main)
gtest_add_tests(TARGET test_container2)

add_executable (test_pointer "test_pointer.cc")
target_link_libraries(test_pointer GTest::GTest GTest::Main)
#gtest_add_tests(TARGET test_pointer)