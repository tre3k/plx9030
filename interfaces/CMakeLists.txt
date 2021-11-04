cmake_minimum_required(VERSION 3.16)

project(plx9030detector LANGUAGES CXX)


add_library(plx9030detector SHARED
  plx9030c.cpp plx9030c.h
  plx9030detector.cpp plx9030detector.h
  )

add_executable(test main.cpp)
target_link_libraries(test plx9030detector)
