cmake_minimum_required(VERSION 3.0)

project(roboai LANGUAGES CXX)

find_package(box2d REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fsanitize=address")

include_directories(include/)

add_executable(main src/main.cpp src/agent.cpp src/environment.cpp src/misc.cpp src/visualize.cpp)

target_link_libraries(main PRIVATE box2d)
target_link_libraries(main PRIVATE sfml-graphics)
