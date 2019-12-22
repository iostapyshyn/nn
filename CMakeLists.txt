cmake_minimum_required(VERSION 3.12)
project(nn DESCRIPTION "Neural Network with back propagation learning.")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Library
add_library(nn STATIC
  src/neuralnetwork.c
  src/util.c
  src/activations.c
  src/matrix.c)

target_include_directories(nn INTERFACE include)
target_link_libraries(nn m)

# Examples
add_executable(digits examples/digits.c)
target_link_libraries(digits nn)

find_package(raylib)

add_executable(snake examples/snake.c)
target_link_libraries(snake nn)
target_link_libraries(snake raylib)
