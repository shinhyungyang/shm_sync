######################
# Benchmark Properties
set(BENCHMARK ArrayLock)

enable_language(CXX)

find_package(Threads)
find_package(PAPI)

file(MAKE_DIRECTORY ${BINDIR}/${BENCHMARK})
set(BINDIR ${BINDIR}/${BENCHMARK})

foreach (MEMORDER ${MEMORDERS})

  set(TARGETBIN arraylock_${MEMORDER})

  set(SRC_FILES
    arraylock_${MEMORDER}.cpp
    driver.cpp
#   barrier.cpp
#   stats.cpp
  )

  add_executable(${TARGETBIN} ${SRC_FILES})
  add_dependencies(${TARGETBIN} cxxopts)
  target_link_libraries(${TARGETBIN} PRIVATE barrier)
  target_link_libraries(${TARGETBIN} PRIVATE stats)
  target_link_libraries(${TARGETBIN} PRIVATE timer)

  target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
  target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/barrier)
  target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/stats)
  target_include_directories(${TARGETBIN} PRIVATE ${CMAKE_SOURCE_DIR}/common/timer)
  target_include_directories(${TARGETBIN} PRIVATE ${CXXOPTS}/include)
  target_include_directories(${TARGETBIN} PRIVATE ${PAPI_INCLUDE_DIR})

  target_link_libraries(${TARGETBIN} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(${TARGETBIN} PRIVATE ${PAPI_LIBRARIES})


  add_custom_command(
    TARGET ${TARGETBIN}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND}
      -E copy_if_different $<TARGET_FILE:${TARGETBIN}> ${BINDIR}
  )
endforeach ()
