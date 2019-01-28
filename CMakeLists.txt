cmake_minimum_required(VERSION 3.13)
project(Chip8)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
set(CMAKE_ARCHVE_OUTPUT_DIRECTORY bin/)
set(CMAKE_LIBRARIES_OUTPUT_DIRECTORY bin/)

file(GLOB SOURCE_FILES
  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/Chip8.cpp)

include_directories(${SOURCE_DIR})
add_executable(Chip8 ${SOURCE_FILES})
set_target_properties(Chip8 PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

find_package(SFML 2 REQUIRED COMPONENTS audio graphics system)

if(SFML_FOUND)
  target_link_libraries(Chip8 sfml-graphics sfml-audio sfml-system)
else(SFML_FOUND)
  message("SFML was not found!")
endif(SFML_FOUND)

