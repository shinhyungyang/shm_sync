# src_lib/CMakeLists.txt

add_library(tagged STATIC tagged.c)

set(SOURCE_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/tagged_pointers.ads
  ${CMAKE_CURRENT_SOURCE_DIR}/tagged_pointers.adb
)

# These are Ada library information files built by gnat.  I am not
# sure whether the name suffixes correspond to the *.adb or *.ads files
# above or the union of them.  In any case, if any of the names change
# above, then this list will probably have to be changed as well.)

# N.B. the absolute location prefix of these files may have to be changed
# in the future since this is currently a CMake internal.
set(ALI_PREFIX
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/tagged_pointers.dir
)

# This variable references the Ada library information file for the library.
# and is useful for cleaning and/or installing the *.ali files.

set(ALI_LIST
  ${ALI_PREFIX}/tagged_pointers.ali
)

# CMake assumes compilation results only in object files being generated.
# However, gnatmake generates both object files and *.ali (Ada library
# information) files so it doesn't intrinsically know how to clean those
# additional *.ali files.
# Here is a workaround for this fundamental CMake limitation.

# Add generated .ali files to the list of additional files to be
# removed by make clean

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ALI_LIST}")

add_library(tagged_pointers ${SOURCE_LIST})

include_directories(tagged_pointers ${ATOMICINTRINSIC}/ada)

# Link to GNAT_LIB to avoid underlinking the tagged_pointers library (which causes
# link errors on at least the Cygwin platform), but use
# the PRIVATE keyword (on all platforms) to avoid overlinking Ada applications
# that link to tagged_pointers.
target_link_libraries(tagged_pointers PRIVATE ${GNAT_LIB})
target_link_libraries(tagged_pointers PRIVATE tagged)

set_target_properties(tagged_pointers
  PROPERTIES
  LINK_FLAGS
  "\"-aI${ATOMICINTRINSIC}/ada\""
  SOVERSION 0
  VERSION 0.0
  OUTPUT_NAME tagged_pointers
  POSITION_INDEPENDENT_CODE ON
)
