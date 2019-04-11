cmake_minimum_required(VERSION 2.8.11)
project(rwsat)

# Extend CMake module path for loading custom modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake-modules")

# Generate a Visual Studio filter "CMakePredefinedTargets"
if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(PREDEFINED_TARGETS_FOLDER "CustomTargets")
  # Silence fopen warnings
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Use C++11
set(CMAKE_CXX_STANDARD 11)

# Set RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set install directory
set(RWSAT_INSTALL_DIR ${PROJECT_BINARY_DIR}/install CACHE PATH "Application install directory")
set(CMAKE_INSTALL_PREFIX ${RWSAT_INSTALL_DIR})

set(RWSAT_BUILD_SATGEN OFF CACHE BOOL "Build satgen")

# Find ACIS headers and libraries
find_package(ACIS REQUIRED)

# Include ACIS includes if ACIS is installed
if(ACIS_FOUND)
  include_directories(${ACIS_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "ACIS not found")
endif()

# Set source files
set(SOURCE_FILES_SAT2JSON
  src/ACIS.h
  src/json/json.h
  src/json/json-forwards.h
  src/jsoncpp.cpp
  src/common.h
  src/common.cpp
  src/extract.h
  src/extract.cpp
  src/sat2json.cpp
)

# Create the executable
add_executable(sat2json ${SOURCE_FILES_SAT2JSON})
target_link_libraries(sat2json ${ACIS_LINK_LIBRARIES})
set_target_properties(sat2json PROPERTIES DEBUG_POSTFIX "_d")

# Install the binary
install(
  TARGETS sat2json
  DESTINATION ${RWSAT_INSTALL_DIR}
)

if(RWSAT_BUILD_SATGEN)
  # Set source files for the generator application
  set(SOURCE_FILES_SATGEN
    src/ACIS.h
    src/common.h
    src/common.cpp
    src/satgen.cpp
  )

  # Create the executable for the generator application
  add_executable(satgen ${SOURCE_FILES_SATGEN})
  target_link_libraries(satgen ${ACIS_LINK_LIBRARIES})
  set_target_properties(satgen PROPERTIES DEBUG_POSTFIX "_d")
  
  # Install the generator application
  install(
    TARGETS satgen
    DESTINATION ${RWSAT_INSTALL_DIR}
  )
endif(RWSAT_BUILD_SATGEN)

# On Windows, it would be wise copy required DLL files into the app directory
if(MSVC)
  install(
      FILES ${ACIS_REDIST_RELEASE}
      DESTINATION ${RWSAT_INSTALL_DIR}
      CONFIGURATIONS Release RelWithDebInfo MinSizeRel
  )

  install(
      FILES ${ACIS_REDIST_DEBUG}
      DESTINATION ${RWSAT_INSTALL_DIR}
      CONFIGURATIONS Debug
  )
endif()

# Create uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
