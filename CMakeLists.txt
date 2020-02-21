cmake_minimum_required(VERSION 3.10.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

option(BUILD_ADA
  "Description: Add Ada compiler to include benchmarks of Ada" ON)

option(BUILD_CXX
  "Description: Add CXX compiler to include benchmarks of CXX" ON)

option(BUILD_AR
       "Description: Add available AR build configurations" ON)

option(BUILD_DWATOMIC
       "Description: Add 16-byte atomic operations" OFF)
    
set(LANGUAGE CXX C)
if (BUILD_ADA)
  list(APPEND LANGUAGE Ada)
endif()

project(shm_sync
        LANGUAGES ${LANGUAGE} 
        VERSION 0.1.0
        DESCRIPTION "Lock-less synchronization benchmark suite on shared-memory multicores")

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

include(EnableCompilerFlags REQUIRED)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

if (BUILD_AR)
  set(MEMORDERS
    "sc" # sequential consistency
    "ar" # acquire-release
  )
endif(BUILD_AR)
if (BUILD_DWATOMIC)
  message(STATUS "Use 16-byte gcc atomic intrinsic")
  add_definitions(-DUSE_DWATOMIC)
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1)
    message(FATAL_ERROR "Require at least gcc-7.1")
  else()
    # Enable "over-alignment", e.g., on cache-line granularity:
    enable_cxx_compiler_flag_if_supported("-faligned-new")
  endif()
endif()
enable_c_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wall")

include(BuildType)

set(BINDIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BINDIR})

set(DATADIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${DATADIR})

set(CXXOPTS ${CMAKE_CURRENT_BINARY_DIR}/ext/cxxopts)
set(ATOMICINTRINSIC ${CMAKE_CURRENT_SOURCE_DIR}/common/atomic_intrinsic)
set(HAZARD_POINTERS ${CMAKE_CURRENT_SOURCE_DIR}/common/hazard_pointers)

if(BUILD_ADA)
  set(TAGGEDPOINTERS ${CMAKE_CURRENT_SOURCE_DIR}/common/tagged_pointers)
  set(PAPIBINDING ${CMAKE_CURRENT_SOURCE_DIR}/common/ada_binding/papi)
  set(BARRIERBINDING ${CMAKE_CURRENT_SOURCE_DIR}/common/ada_binding/barrier)
  set(STATSBINDING ${CMAKE_CURRENT_SOURCE_DIR}/common/ada_binding/stats)
  set(TIMERBINDING ${CMAKE_CURRENT_SOURCE_DIR}/common/ada_binding/timer)
endif()

add_subdirectory(benchmark)
add_subdirectory(common)
add_subdirectory(ext)
add_subdirectory(scripts)

enable_testing()
add_subdirectory(tests)
