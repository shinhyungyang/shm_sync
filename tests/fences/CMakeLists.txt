set(TARGETBIN fence_test)
set(SRC_FILES fence_test.cpp)

add_executable(${TARGETBIN} ${SRC_FILES})
target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/atomic_intrinsic/c++)

add_test(fences fence_test)
