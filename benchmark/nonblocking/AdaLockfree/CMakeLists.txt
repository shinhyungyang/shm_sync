######################
# Benchmark Properties
set(BENCHMARK AdaLockfree)
set(TARGET adalockfree)

file(MAKE_DIRECTORY ${BINDIR}/${BENCHMARK})
set(BINDIR ${BINDIR}/${BENCHMARK})

if (BUILD_DWATOMIC)
  set(TARGETS "adalockfree" "adalockfree_16")
else()
  set(TARGETS "adalockfree")
endif()

foreach (TARGETBIN ${TARGETS})
   if (${TARGETBIN} MATCHES "adalockfree_16")
    set(QUEUE_IMPL ${CMAKE_CURRENT_SOURCE_DIR}/CAS_16)
  else()
    set(QUEUE_IMPL ${CMAKE_CURRENT_SOURCE_DIR}/CAS_8)
  endif()
  add_executable(${TARGETBIN} ${TARGETBIN}.adb)
  
  target_include_directories(${TARGETBIN} PRIVATE ${PAPIBINDING})
  target_include_directories(${TARGETBIN} PRIVATE ${BARRIERBINDING})
  target_include_directories(${TARGETBIN} PRIVATE ${STATSBINDING})
  target_include_directories(${TARGETBIN} PRIVATE ${TIMERBINDING})
  target_include_directories(${TARGETBIN} PRIVATE ${TAGGEDPOINTERS})
  target_include_directories(${TARGETBIN} PRIVATE ${ATOMICINTRINSIC}/ada)
  target_include_directories(${TARGETBIN} PRIVATE ${QUEUE_IMPL})
  
  target_link_libraries(${TARGETBIN} PRIVATE papi_binding)
  target_link_libraries(${TARGETBIN} PRIVATE barrier_binding)
  target_link_libraries(${TARGETBIN} PRIVATE stats_binding)
  target_link_libraries(${TARGETBIN} PRIVATE timer_binding)
  target_link_libraries(${TARGETBIN} PRIVATE tagged_pointers)
  target_link_libraries(${TARGETBIN} PRIVATE atomic_intrinsic)
  
  set_target_properties(
    ${TARGETBIN}
    PROPERTIES
    LINK_FLAGS
    "\"-aI${CMAKE_CURRENT_SOURCE_DIR}\"\
     \"-aI${PAPIBINDING}\"\
     \"-aI${BARRIERBINDING}\"\
     \"-aI${STATSBINDING}\"\
     \"-aI${TIMERBINDING}\"\
     \"-aI${TAGGEDPOINTERS}\"\
     \"-aI${ATOMICINTRINSIC}/ada\"\
     \"-aI${QUEUE_IMPL}\""
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
endforeach()
