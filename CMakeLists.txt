# add by muyang
cmake_minimum_required(VERSION 2.8)
IF(APPLE)
    SET(CMAKE_C_COMPILER "/usr/local/Cellar/gcc/9.1.0/bin/gcc-9")
    SET(CMAKE_CXX_COMPILER "/usr/local/Cellar/gcc/9.1.0/bin/g++-9")
ENDIF()
project(dmine-pro)

# ------------------------------------------------------------------------------
# cmake options
# ------------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Whether to build dminepro as shared library" ON)


include_directories(${PROJECT_SOURCE_DIR}include/gundam/include/)
include_directories(${PROJECT_SOURCE_DIR}include/gundam/)
include_directories(${PROJECT_SOURCE_DIR}include/)
include_directories(${PROJECT_SOURCE_DIR}include/standard/)

# ------------------------------------------------------------------------------
# setting default cmake type to Release
# ------------------------------------------------------------------------------

SET(DEFAULT_BUILD_TYPE "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
            STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

message("dminepro will be built in type: ${CMAKE_BUILD_TYPE}")

message("cmakelist.txt dir: ${CMAKE_CURRENT_SOURCE_DIR}")
# ------------------------------------------------------------------------------
# cmake configs
# ------------------------------------------------------------------------------

include(CheckLibraryExists)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
    SET(CMAKE_MACOSX_RPATH ON)
else ()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Werror -Wl,-rpath,$ORIGIN")
endif ()
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fprofile-arcs -ftest-coverage")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")


# ------------------------------------------------------------------------------
# find_libraries
# ------------------------------------------------------------------------------
find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})

# find Threads------------------------------------------------------------------
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

# find glog---------------------------------------------------------------------
include("cmake/FindGlog.cmake")
if (NOT GLOG_FOUND)
    message(FATAL_ERROR "glog not found, please install the glog library")
else ()
    include_directories(SYSTEM ${GLOG_INCLUDE_DIRS})
endif ()

# find gflags-------------------------------------------------------------------
Include("cmake/FindGFlags.cmake")
#find_package(gflags CONFIG REQUIRED)
if (NOT GFLAGS_FOUND)
    message(STATUS "gflags not found, build without gflags")
else ()
    include_directories(SYSTEM ${GFLAGS_INCLUDE_DIRS})
endif ()

# find yaml-------------------------------------------------------------------
Include("cmake/FindYaml.cmake")
if (NOT YAML_FOUND)
    message(STATUS "yaml not found, build without yaml")
else ()
    include_directories(SYSTEM ${GFLAGS_INCLUDE_DIRS})
endif ()

#openmp
find_package(OpenMP)
if(OPENMP_FOUND)
  message("OpenMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/gundam/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)









