# src_lib/CMakeLists.txt

set(SOURCE_LIST
   ${CMAKE_CURRENT_SOURCE_DIR}/cmd_args.ads
   ${CMAKE_CURRENT_SOURCE_DIR}/cmd_args.adb
   ${CMAKE_CURRENT_SOURCE_DIR}/cpu_affinity.ads
   ${CMAKE_CURRENT_SOURCE_DIR}/cpu_affinity.adb
)

# These are Ada library information files built by gnat.  I am not
# sure whether the name suffixes correspond to the *.adb or *.ads files
# above or the union of them.  In any case, if any of the names change
# above, then this list will probably have to be changed as well.)

# N.B. the absolute location prefix of these files may have to be changed
# in the future since this is currently a CMake internal.
set(ALI_PREFIX
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cmd_args.dir
)

# This variable references the Ada library information file for the library.
# and is useful for cleaning and/or installing the *.ali files.

set(ALI_LIST
${ALI_PREFIX}/cmd_args.ali
${ALI_PREFIX}/cpu_affinity.ali
)

# CMake assumes compilation results only in object files being generated.
# However, gnatmake generates both object files and *.ali (Ada library
# information) files so it doesn't intrinsically know how to clean those
# additional *.ali files.
# Here is a workaround for this fundamental CMake limitation.

# Add generated .ali files to the list of additional files to be
# removed by make clean

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ALI_LIST}")

add_library(cmd_args ${SOURCE_LIST})

target_link_libraries(cmd_args PRIVATE ${GNAT_LIB})

set_target_properties(cmd_args
PROPERTIES
SOVERSION 0
VERSION 0.0
OUTPUT_NAME cmd_args
POSITION_INDEPENDENT_CODE ON
)
