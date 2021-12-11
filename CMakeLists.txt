cmake_minimum_required(VERSION 3.0)

project(roboai LANGUAGES CXX)

find_package(Torch REQUIRED)
find_package(box2d REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE box2d)
target_link_libraries(main PRIVATE sfml-graphics)
target_link_libraries(main PRIVATE "${TORCH_LIBRARIES}")
