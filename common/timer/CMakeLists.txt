######################
# Library Properties

enable_language(CXX)

set(TARGETBIN timer)

set(SRC_FILES
    timer_x86.cpp
)

add_library(${TARGETBIN} STATIC ${SRC_FILES})

target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
