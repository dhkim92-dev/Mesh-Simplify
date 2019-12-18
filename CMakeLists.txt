CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(Mesh-Simplify)
set(PROJECT_VERSION_MINOR 0)
set(PROEJCT_VERSION_MAJOR 1)

include_directories(./src)

add_definitions("-Wall -std=c++17")
add_subdirectory(src)
add_executable(mesh_simplify main.cpp)

link_directories(./src)
target_link_libraries(mesh_simplify lag)
target_link_libraries(mesh_simplify mesim)
