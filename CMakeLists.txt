cmake_minimum_required(VERSION 3.12)
project(nn C)

set(CMAKE_BUILD_TYPE Debug)

add_executable(digits digits.c
  neuralnetwork.c
  neuralnetwork.h
  util.c
  util.h
  activations.c
  activations.h
  matrix.c
  matrix.h)

target_link_libraries(digits m)
