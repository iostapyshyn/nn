cmake_minimum_required(VERSION 3.12)
project(nn DESCRIPTION "Neural Network with back propagation learning.")

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
