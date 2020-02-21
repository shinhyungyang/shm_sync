######################
# Library Properties

enable_language(CXX)

set(TARGETBIN stats)

set(SRC_FILES
  stats.cpp
  smpl_collector.cpp
  color_term.cpp
  log.cpp
)

add_library(${TARGETBIN} STATIC ${SRC_FILES})

target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
