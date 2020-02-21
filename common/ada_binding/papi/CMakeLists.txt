# src_lib/CMakeLists.txt

find_package(PAPI)

add_library(binding STATIC binding.cpp)
target_include_directories(binding PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
target_include_directories(binding PRIVATE ${PAPI_INCLUDE_DIR})
target_link_libraries(binding PRIVATE ${PAPI_LIBRARIES})

set(SOURCE_LIST
   ${CMAKE_CURRENT_SOURCE_DIR}/papi_binding.adb
   ${CMAKE_CURRENT_SOURCE_DIR}/papi_binding.ads
)

# These are Ada library information files built by gnat.  I am not
# sure whether the name suffixes correspond to the *.adb or *.ads files
# above or the union of them.  In any case, if any of the names change
# above, then this list will probably have to be changed as well.)

# N.B. the absolute location prefix of these files may have to be changed
# in the future since this is currently a CMake internal.
set(ALI_PREFIX
   ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/papi_binding.dir
)

# This variable references the Ada library information file for the library.
# and is useful for cleaning and/or installing the *.ali files.

set(ALI_LIST
   ${ALI_PREFIX}/papi_binding.ali
)

# CMake assumes compilation results only in object files being generated.
# However, gnatmake generates both object files and *.ali (Ada library
# information) files so it doesn't intrinsically know how to clean those
# additional *.ali files.
# Here is a workaround for this fundamental CMake limitation.

# Add generated .ali files to the list of additional files to be
# removed by make clean

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ALI_LIST}")

add_library(papi_binding ${SOURCE_LIST})

# Link to GNAT_LIB to avoid underlinking the papi_binding library (which causes
# link errors on at least the Cygwin platform), but use
# the PRIVATE keyword (on all platforms) to avoid overlinking Ada applications
# that link to papi_binding.
target_link_libraries(papi_binding PRIVATE ${GNAT_LIB})
target_link_libraries(papi_binding PRIVATE binding)

set_target_properties(papi_binding
PROPERTIES
SOVERSION 0
VERSION 0.0
OUTPUT_NAME papi_binding
POSITION_INDEPENDENT_CODE ON
)
