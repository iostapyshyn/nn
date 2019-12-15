cmake_minimum_required(VERSION 3.12)
project(nn C)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG_PRINT")

add_executable(nn nn_main.c
  neuralnetwork.c neuralnetwork.h
  util.c util.h
  activations.c activations.h
  matrix.c matrix.h)
