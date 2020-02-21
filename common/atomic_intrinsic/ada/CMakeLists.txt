# src_lib/CMakeLists.txt

#EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
include(DetermineTargetArchitecture REQUIRED)
Determine_Target_Architecture(FLAG)
if("${FLAG}" STREQUAL "x86_64")
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fences_x86_64.adb ${CMAKE_CURRENT_SOURCE_DIR}/fences.adb COPYONLY)
elseif("${FLAG}" STREQUAL "aarch64")
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fences_aarch64.adb ${CMAKE_CURRENT_SOURCE_DIR}/fences.adb COPYONLY)
else()
   message(FATAL_ERROR "Unsupported architecture: ${FLAG}")
endif()

set(FENCES_SRC_LIST
   ${CMAKE_CURRENT_SOURCE_DIR}/fences.adb
   ${CMAKE_CURRENT_SOURCE_DIR}/fences.ads
)
set(ATOMIC_SRC_LIST
   ${CMAKE_CURRENT_SOURCE_DIR}/atomic_intrinsic.ads
   ${CMAKE_CURRENT_SOURCE_DIR}/atomic_intrinsic.adb
)

add_library(gcc_atomics STATIC gcc_atomics.c)
target_link_libraries(gcc_atomics PRIVATE -latomic)

# These are Ada library information files built by gnat.  I am not
# sure whether the name suffixes correspond to the *.adb or *.ads files
# above or the union of them.  In any case, if any of the names change
# above, then this list will probably have to be changed as well.)

# N.B. the absolute location prefix of these files may have to be changed
# in the future since this is currently a CMake internal.
set(ATOMIC_ALI_PREFIX
   ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/atomic_intrinsic.dir
)
set(FENCES_ALI_PREFIX
   ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/fences.dir
)

# This variable references the Ada library information file for the library.
# and is useful for cleaning and/or installing the *.ali files.

set(ATOMIC_ALI_LIST
   ${ATOMIC_ALI_PREFIX}/atomic_intrinsic.ali
)
set(FENCES_ALI_LIST
   ${FENCES_ALI_PREFIX}/fences.ali
)

# CMake assumes compilation results only in object files being generated.
# However, gnatmake generates both object files and *.ali (Ada library
# information) files so it doesn't intrinsically know how to clean those
# additional *.ali files.
# Here is a workaround for this fundamental CMake limitation.

# Add generated .ali files to the list of additional files to be
# removed by make clean


add_library(atomic_intrinsic ${ATOMIC_SRC_LIST})
add_library(fences ${FENCES_SRC_LIST})

# Link to GNAT_LIB to avoid underlinking the atomic_intrinsic library (which causes
# link errors on at least the Cygwin platform), but use
# the PRIVATE keyword (on all platforms) to avoid overlinking Ada applications
# that link to atomic_intrinsic.
target_link_libraries(atomic_intrinsic PRIVATE ${GNAT_LIB})
target_link_libraries(atomic_intrinsic PRIVATE gcc_atomics)
target_link_libraries(fences PRIVATE ${GNAT_LIB})

set_target_properties(atomic_intrinsic
   PROPERTIES
   SOVERSION 0
   VERSION 0.0
   OUTPUT_NAME atomic_intrinsic
   POSITION_INDEPENDENT_CODE ON
)
set_target_properties(fences
   PROPERTIES
   SOVERSION 0
   VERSION 0.0
   OUTPUT_NAME fences
   POSITION_INDEPENDENT_CODE ON
)
