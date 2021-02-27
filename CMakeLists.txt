cmake_minimum_required (VERSION 3.10)

IF(APPLE)
    SET(CMAKE_C_COMPILER "/usr/local/Cellar/gcc/9.1.0/bin/gcc-9")
    SET(CMAKE_CXX_COMPILER "/usr/local/Cellar/gcc/9.1.0/bin/g++-9")

    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
ENDIF()

project(dmine-pro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  message("MSVC")

  add_definitions(-D_CRT_SECURE_NO_WARNINGS)  
  #add_compile_options(/WX /wd4819)
  
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message("GNU")
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -m64 -fexpensive-optimizations -O3" )

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("Clang")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  message("Intel")

endif()

#openmp
find_package(OpenMP)
if(OPENMP_FOUND)
  message("OpenMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#glags
find_package(gflags CONFIG REQUIRED)
message("gflags FOUND")

#glog
find_package(glog CONFIG REQUIRED)
message("glog FOUND")

#yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)
message("yaml-cpp FOUND")

#mpi
find_package(MPI)
if(MPI_FOUND)
  message("mpi FOUND")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/gundam/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)