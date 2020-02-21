file(MAKE_DIRECTORY ${BINDIR}/blocking)
file(MAKE_DIRECTORY ${BINDIR}/nonblocking)

add_subdirectory(blocking)
add_subdirectory(nonblocking)

if (BUILD_ADA)
  file(MAKE_DIRECTORY ${BINDIR}/tsx)
  add_subdirectory(tsx)
endif()
