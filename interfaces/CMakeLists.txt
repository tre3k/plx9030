cmake_minimum_required(VERSION 3.16)

project(plx9030detector LANGUAGES CXX)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../include/)
set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/../lib/)

include_directories(${INCLUDE_PATH})
link_directories(${LIBRARY_PATH})

add_library(plx9030detector SHARED
  plx9030c.cpp
  plx9030detector.cpp
  )

add_executable(test main.cpp)
target_link_libraries(test plx9030detector)
