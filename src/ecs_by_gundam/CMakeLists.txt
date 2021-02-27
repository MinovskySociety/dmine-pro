# 头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 可执行文件目录
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 可执行文件
add_executable(ecs ecs_by_gundam.cpp)