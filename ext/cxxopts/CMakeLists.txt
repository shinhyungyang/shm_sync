##############################
# External Project description
  set(ProjectName cxxopts)
  set(Version v2.1.2)

include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_CURRENT_BINARY_DIR})

ExternalProject_Add(${ProjectName}
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts
    GIT_TAG ${Version}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

