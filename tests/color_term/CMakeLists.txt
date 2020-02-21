set(TARGETBIN color_driver)
set(SRC_FILES color_driver.cpp)

add_executable(${TARGETBIN} ${SRC_FILES})
target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/stats)
target_link_libraries(${TARGETBIN} PRIVATE stats)

add_test(color_term color_driver)
