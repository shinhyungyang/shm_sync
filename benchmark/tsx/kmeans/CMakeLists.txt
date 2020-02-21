######################
# Benchmark Properties
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/inputs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(BENCHMARK KMeans)
set(TARGETBIN driver_kmeans)

file(MAKE_DIRECTORY ${BINDIR}/${BENCHMARK})
set(BINDIR ${BINDIR}/${BENCHMARK})

include_directories(
  ${PAPIBINDING}
  ${CMDARGS}
)

add_executable(${TARGETBIN} ${TARGETBIN}.adb)

target_link_libraries(${TARGETBIN} PRIVATE papi_binding)
target_link_libraries(${TARGETBIN} PRIVATE cmd_args)
target_link_libraries(${TARGETBIN} PRIVATE m)

set_target_properties(
    ${TARGETBIN}
    PROPERTIES
    LINK_FLAGS
    "\"-aI${CMAKE_CURRENT_SOURCE_DIR}\"\
     \"-aI${CMDARGS}\"\
     \"-aI${PAPIBINDING}\""
    INTERPROCEDURAL_OPTIMIZATION TRUE
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
