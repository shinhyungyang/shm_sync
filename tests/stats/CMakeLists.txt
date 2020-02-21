set(TARGETBIN stats_test)
set(SRC_FILES stats_test.cpp)

add_executable(${TARGETBIN} ${SRC_FILES})
target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/stats)
target_link_libraries(${TARGETBIN} PRIVATE stats)

add_test(stats stats_test)
