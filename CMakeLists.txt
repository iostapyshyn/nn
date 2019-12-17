cmake_minimum_required(VERSION 3.12)
project(nn VERSION 1.0.1 DESCRIPTION "Neural Network with back propagation learning.")

# Library
add_library(nn STATIC
  src/neuralnetwork.c
  src/util.c
  src/activations.c
  src/matrix.c)

set_target_properties(nn PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(nn INTERFACE include)
target_link_libraries(nn m)

# Demo
add_executable(digits examples/digits.c)
target_link_libraries(digits nn)
