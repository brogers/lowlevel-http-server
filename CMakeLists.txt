cmake_minimum_required(VERSION 3.27)
project(myhttp)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE DEBUG)

add_executable(${PROJECT_NAME} src/main.c src/tcp.c)

target_include_directories(
  ${PROJECT_NAME} PRIVATE .
  ${CMAKE_SOURCE_DIR}/include
)
