project(brainfuck)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE
  SOURCE_FILES
  *.cc
  *.h
  )

add_executable(
  bf
  ${SOURCE_FILES}
  )
