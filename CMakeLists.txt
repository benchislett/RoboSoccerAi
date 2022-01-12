cmake_minimum_required(VERSION 3.0)

project(roboai LANGUAGES CXX)

find_package(box2d REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
# find_package(Python COMPONENTS Interpreter Development REQUIRED)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -L/usr/lib  -lcrypt -lpthread -ldl  -lutil -lm -lm -I/usr/include/python3.10 -I/usr/include/python3.10")

if(SKBUILD)
  # Scikit-Build does not add your site-packages to the search path
  # automatically, so we need to add it _or_ the pybind11 specific directory
  # here.
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE _tmp_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
  list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif()

find_package(pybind11 CONFIG REQUIRED)

include_directories(include/)

file(GLOB sources src/*.cpp)

add_library(robolib STATIC ${sources})
target_link_libraries(robolib PUBLIC box2d)
target_link_libraries(robolib PUBLIC sfml-graphics)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE robolib)

pybind11_add_module(robopy MODULE src/pybind.cpp)
target_link_libraries(robopy PRIVATE robolib)

install(TARGETS robopy DESTINATION .)
