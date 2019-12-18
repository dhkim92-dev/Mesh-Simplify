# Mesh-Simplify/src/CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lag Vector.cpp Matrix.cpp)
add_library(mesim Mesh.cpp Face.cpp)
