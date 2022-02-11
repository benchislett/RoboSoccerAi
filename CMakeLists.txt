cmake_minimum_required(VERSION 3.0)

project(roboai LANGUAGES C CXX)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)
find_package(SDL2 REQUIRED)
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
include_directories(imagecapture/)
include_directories(robo_api/)
include_directories(roboai/)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
include_directories( ${SDL2_INCLUDE_DIRS} )

file(GLOB imagecap_sources imagecapture/*.c)

add_library(imagecapture STATIC ${imagecap_sources})
target_link_libraries(imagecapture PUBLIC ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${SDL2_LIBRARIES} -lbluetooth -ljpeg )
set_target_properties(imagecapture PROPERTIES LINKER_LANGUAGE C)

file(GLOB roboapi_sources robo_api/*.c)

add_library(roboapi STATIC ${roboapi_sources})
set_target_properties(roboapi PROPERTIES LINKER_LANGUAGE C)

file(GLOB roboai_sources roboai/*.c)

add_library(roboai_main STATIC ${roboai_sources})
set_target_properties(roboai_main PROPERTIES LINKER_LANGUAGE C)

file(GLOB sources src/*.cpp)

add_library(robolib STATIC ${sources})
target_link_libraries(robolib PUBLIC box2d)
target_link_libraries(robolib PUBLIC sfml-graphics)
target_link_libraries(robolib PUBLIC roboapi)
target_link_libraries(robolib PUBLIC imagecapture)
target_link_libraries(robolib PUBLIC roboai_main)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC robolib)

pybind11_add_module(robopy MODULE src/pybind.cpp)
target_link_libraries(robopy PRIVATE robolib)

install(TARGETS robopy DESTINATION .)
