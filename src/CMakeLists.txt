#include_directories(gpar)
#link_directories(gpar)
#add_subdirectory(gpar)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/app)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/app)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

#include_directories(GAR)
#link_directories(GAR)
#add_subdirectory(GAR)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/graph_convert)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/graph_convert)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/graph_convert)

#add_subdirectory(ecs_by_gundam)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/graph_package)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gptools)

# 源文件列表
aux_source_directory(. SRCS)
# 可执行文件目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# 可执行文件

#add_executable(main main.cpp)
#target_link_libraries(main GAR)
#target_link_libraries(main app)
#target_link_libraries(main test)
#target_link_libraries(main GPAR)
