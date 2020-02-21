# - Find python libraries
# This module finds if Python is installed and determines where the
# include files and libraries are. It also determines what the name of
# the library is. This code sets the following variables:
#
#  PYTHON_FOUND           - have the Python libs been found
#  PYTHON_LIBRARIES       - path to the python library
#  PYTHON_INCLUDE_DIRS    - path to where Python.h is found
#

unset(_Python_NAMES)

set(_PYTHON3_VERSIONS 3.8 3.7 3.6 3.5 3.4 3.3 3.2 3.1 3.0)

if(Python3_FIND_VERSION) #if version is specified
  if(Python3_FIND_VERSION_COUNT GREATER 1) # if minor version is mentioned
    set(_PYTHON_FIND_MAJ_MIN "${Python3_FIND_VERSION_MAJOR}.${Python3_FIND_VERSION_MINOR}")
    list(APPEND _Python_NAMES python${_PYTHON_FIND_MAJ_MIN})

    unset(_PYTHON_FIND_OTHER_VERSIONS)
    if(NOT Python3_FIND_VERSION_EXACT) # if newer versions are accepted
      foreach(_PYTHON_V ${_PYTHON${Python3_FIND_VERSION_MAJOR}_VERSIONS})
        if(NOT _PYTHON_V VERSION_LESS _PYTHON_FIND_MAJ_MIN)
          list(APPEND _PYTHON_FIND_OTHER_VERSIONS ${_PYTHON_V})
        endif()
      endforeach()
    endif()
    unset(_PYTHON_FIND_MAJ_MIN)
  else() # if minor version is not mentioned (we already know we look for Python3 though)
    list(APPEND _Python_NAMES python${Python3_FIND_VERSION_MAJOR})
    set(_PYTHON_FIND_OTHER_VERSIONS ${_PYTHON${Python3_FIND_VERSION_MAJOR}_VERSIONS})
  endif()
else() # if version is not specified
    set(_PYTHON_FIND_OTHER_VERSIONS ${_PYTHON3_VERSIONS})
endif()
find_program(PYTHON_EXECUTABLE NAMES ${_Python_NAMES}) # search for target version

# create a list of other versions to look for
set(_Python_VERSIONS ${_PYTHON_FIND_OTHER_VERSIONS}) # grab additional versions 

unset(_PYTHON_FIND_OTHER_VERSIONS)
unset(_PYTHON3_VERSIONS)

# Search for newest python version if python executable isn't found
if(NOT PYTHON_EXECUTABLE)
  foreach(_CURRENT_VERSION IN LISTS _Python_VERSIONS)
    set(_Python_NAMES python${_CURRENT_VERSION})
    find_program(PYTHON_EXECUTABLE NAMES ${_Python_NAMES})
  endforeach()
endif()

EXECUTE_PROCESS(
    COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils import sysconfig; print(sysconfig.get_python_inc())"
    OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

EXECUTE_PROCESS(
    COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils import sysconfig; print(sysconfig.get_python_version())"
    OUTPUT_VARIABLE _CURRENT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

EXECUTE_PROCESS (
    COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils import sysconfig; print(sysconfig.get_config_var('LIBDIR'))"
    OUTPUT_VARIABLE _LIBDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

EXECUTE_PROCESS (
    COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils import sysconfig; print(sysconfig.get_config_var('prefix'))"
    OUTPUT_VARIABLE _PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

EXECUTE_PROCESS (
  COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils import sysconfig; print(sysconfig.get_config_var('LDVERSION'))"
  OUTPUT_VARIABLE _LD_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
IF(_CURRENT_VERSION MATCHES "\\d*\\.\\d*")
    STRING(REPLACE "." "" _CURRENT_VERSION_NO_DOTS ${_CURRENT_VERSION})
ENDIF()

FIND_LIBRARY(PYTHON_LIBRARY
    NAMES python${_CURRENT_VERSION_NO_DOTS} python${_CURRENT_VERSION} python${_LD_VERSION}
    PATHS
        ${_LIBDIR}
        ${_PREFIX}/lib
        ${_PREFIX}/libs
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs
    # Avoid finding the .dll in the PATH.  We want the .lib.
    NO_DEFAULT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
)

# Python Should be built and installed as a Framework on OSX
IF(APPLE)
    message(${PYTHON_INCLUDE_DIR})
    STRING(REGEX MATCH ".*\\.framework" _FRAMEWORK_PATH "${PYTHON_INCLUDE_DIR}")
    IF(_FRAMEWORK_PATH)
        SET (PYTHON_LIBRARY "${_FRAMEWORK_PATH}/Python")
    ENDIF()
ENDIF(APPLE)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python DEFAULT_MSG PYTHON_EXECUTABLE PYTHON_LIBRARY PYTHON_INCLUDE_DIR)

MARK_AS_ADVANCED(
    PYTHON_EXECUTABLE
    PYTHON_LIBRARY
    PYTHON_INCLUDE_DIR
)

MACRO(ADD_PYTHON_MODULE name)

    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})
    ADD_LIBRARY(${name} MODULE ${ARGN})
    TARGET_LINK_LIBRARIES(${name} ${PYTHON_LIBRARY})

    SET_TARGET_PROPERTIES(${name} PROPERTIES PREFIX "")
    IF(CMAKE_HOST_WIN32)
        SET_TARGET_PROPERTIES(${name} PROPERTIES SUFFIX ".pyd")
    ENDIF(CMAKE_HOST_WIN32)

    # Store the module in the source tree
    GET_TARGET_PROPERTY(FILEPATH ${name} LOCATION)
    ADD_CUSTOM_COMMAND(
        TARGET ${name} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} 
        ARGS -E copy ${FILEPATH} ${CMAKE_CURRENT_SOURCE_DIR})

ENDMACRO(ADD_PYTHON_MODULE)
