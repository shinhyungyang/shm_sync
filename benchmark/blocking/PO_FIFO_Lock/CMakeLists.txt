######################
# Benchmark Properties
set(BENCHMARK PO_FIFO_Lock)
set(TARGETBIN driver_po_fifo_lock)

file(MAKE_DIRECTORY ${BINDIR}/${BENCHMARK})
set(BINDIR ${BINDIR}/${BENCHMARK})

set(SRC_FILES po_fifo_lock.adb driver_po_fifo_lock.adb)

add_executable(${TARGETBIN} ${SRC_FILES})
target_include_directories(${TARGETBIN} PRIVATE ${PAPIBINDING})
target_include_directories(${TARGETBIN} PRIVATE ${BARRIERBINDING})
target_include_directories(${TARGETBIN} PRIVATE ${STATSBINDING})
target_include_directories(${TARGETBIN} PRIVATE ${ATOMICINTRINSIC}/ada)
target_include_directories(${TARGETBIN} PRIVATE ${TIMERBINDING})

target_link_libraries(${TARGETBIN} PRIVATE papi_binding)
target_link_libraries(${TARGETBIN} PRIVATE atomic_intrinsic)
target_link_libraries(${TARGETBIN} PRIVATE barrier_binding)
target_link_libraries(${TARGETBIN} PRIVATE stats_binding)
target_link_libraries(${TARGETBIN} PRIVATE timer_binding)

set_target_properties(
    ${TARGETBIN}
    PROPERTIES
    LINK_FLAGS
    "\"-aI${CMAKE_CURRENT_SOURCE_DIR}\"\
     \"-aI${PAPIBINDING}\"\
     \"-aI${BARRIERBINDING}\"\
     \"-aI${STATSBINDING}\"\
     \"-aI${ATOMICINTRINSIC}/ada\"\
     \"-aI${TIMERBINDING}\"\
     \"-aI${CMAKE_SOURCE_DIR}/common/include\""
)


add_custom_command(
  TARGET ${TARGETBIN}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND}
    -E copy_if_different $<TARGET_FILE:${TARGETBIN}> ${BINDIR}
)

set_directory_properties(
  PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TARGETBIN}.dir/${TARGETBIN}.ali;\
   ${CMAKE_CURRENT_BINARY_DIR}/${TARGETBIN}.ali;\
   ${CMAKE_CURRENT_BINARY_DIR}/${TARGETBIN}.o"
)
