# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/app)

# 生成可执行程序
add_executable(gar_match gar_match.cc gar_config.cc)
target_link_libraries(gar_match PRIVATE yaml-cpp glog::glog)

if(MPI_FOUND)
  add_executable(gar_match_mpi gar_match_mpi.cc gar_config.cc)
  target_link_libraries(gar_match_mpi PRIVATE yaml-cpp glog::glog MPI::MPI_CXX)
endif()

add_executable(gar_chase gar_chase.cc gar_config.cc)
target_link_libraries(gar_chase PRIVATE yaml-cpp glog::glog)

add_executable(multi_gar_match multi_gar_match.cc)
#add_executable(dmine dmine.cpp)

#add_executable(dmine_new test_dmine_new.cpp)
#target_link_libraries(dmine_new PRIVATE glog::glog)

add_executable(dmine_pro dmine_pro.cc)
target_link_libraries(dmine_pro PRIVATE yaml-cpp glog::glog)

add_executable(dmine_origin dmine_origin.cc)
target_link_libraries(dmine_origin PRIVATE yaml-cpp glog::glog)

add_executable(dmine_algo dmine_algo.cc)
target_link_libraries(dmine_algo PRIVATE yaml-cpp glog::glog)

if(MPI_FOUND)
  add_executable(mgm mgm.cc)
  target_link_libraries(mgm PRIVATE yaml-cpp glog::glog MPI::MPI_CXX)
endif()

add_executable(multimgm multimgm.cc)
target_link_libraries(multimgm PRIVATE yaml-cpp glog::glog MPI::MPI_CXX)

#set(CMAKE_PREFIX_PATH "/home/sunguangzhi/libtorch")
#find_package(Torch REQUIRED)
#include_directories(${TORCH_INCLUDES})
#target_link_libraries(multimgm PRIVATE  ${TORCH_LIBRARIES})
#target_link_libraries(mgm PRIVATE  ${TORCH_LIBRARIES})
#set_property(TARGET multimgm PROPERTY CXX_STANDARD 14)

add_executable(tgraph_discover tgraph_discover.cc)
target_link_libraries(tgraph_discover PRIVATE yaml-cpp glog::glog)

add_executable(tgraph_apply tgraph_apply.cc)
target_link_libraries(tgraph_apply PRIVATE yaml-cpp glog::glog MPI::MPI_CXX)

#add_executable(pattern_gen pattern_gen.cpp)

add_executable(datagraph_information datagraph_information.cpp)


add_executable(gmar_discover gmar_discover.cpp)
target_link_libraries(gmar_discover PRIVATE yaml-cpp glog::glog)