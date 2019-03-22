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
set(APP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install CACHE PATH "Application install directory")
set(CMAKE_INSTALL_PREFIX ${APP_INSTALL_DIR})

set(APP_BUILD_GEN OFF CACHE BOOL "Build SAT file generator")

# Find ACIS headers and libraries
find_package(ACIS REQUIRED)

# Include ACIS includes if ACIS is installed
if(ACIS_FOUND)
  include_directories(${ACIS_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "ACIS not found")
endif()

# Set source files
set(SRC_FILES
  src/ACIS.h
  src/json/json.h
  src/json/json-forwards.h
  src/jsoncpp.cpp
  src/common.h
  src/common.cpp
  src/extract.h
  src/extract.cpp
  src/main.cpp
)

# Create the executable
add_executable(rwsat ${SRC_FILES})
target_link_libraries(rwsat ${ACIS_LINK_LIBRARIES})
set_target_properties(rwsat PROPERTIES DEBUG_POSTFIX "_d")

# Install the binary
install(
  TARGETS rwsat
  DESTINATION ${APP_INSTALL_DIR}
)

if(APP_BUILD_GEN)
  # Set source files for the generator application
  set(GEN_SRC_FILES
    src/ACIS.h
    src/common.h
    src/common.cpp
    src/main_gen.cpp
  )

  # Create the executable for the generator application
  add_executable(satgen ${GEN_SRC_FILES})
  target_link_libraries(satgen ${ACIS_LINK_LIBRARIES})
  set_target_properties(satgen PROPERTIES DEBUG_POSTFIX "_d")
  
  # Install the generator application
  install(
    TARGETS satgen
    DESTINATION ${APP_INSTALL_DIR}
  )
endif(APP_BUILD_GEN)

# On Windows, it might be good to copy required DLL files into the module directory
if(MSVC)
  install(
      FILES ${ACIS_REDIST_RELEASE}
      DESTINATION ${APP_INSTALL_DIR}/${APP_MODULE_NAME}
      CONFIGURATIONS Release RelWithDebInfo MinSizeRel
  )

  install(
      FILES ${ACIS_REDIST_DEBUG}
      DESTINATION ${APP_INSTALL_DIR}/${APP_MODULE_NAME}
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
