cmake_minimum_required(VERSION 3.23...3.30)

project(myhttp
  VERSION 0.1.0
  DESCRIPTION "A minimal HTTP server written in C from scratch"
  LANGUAGES C
)

# Project-wide settings (only when built directly, not as a subproject)
if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_C_STANDARD 23)
  set(CMAKE_C_STANDARD_REQUIRED ON)
  set(CMAKE_C_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

  # Fallback for single-config generators when no preset set a build type.
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      Debug Release RelWithDebInfo MinSizeRel)
  endif()
endif()

option(MYHTTP_BUILD_TESTS "Build the test suite" ${PROJECT_IS_TOP_LEVEL})
option(MYHTTP_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

add_subdirectory(src)

if(MYHTTP_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
