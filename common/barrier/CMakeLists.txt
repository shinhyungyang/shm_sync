######################
# Library Properties

enable_language(CXX)

set(TARGETBIN barrier)

set(SRC_FILES
  barrier.cpp
)

add_library(${TARGETBIN} STATIC ${SRC_FILES})

target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
