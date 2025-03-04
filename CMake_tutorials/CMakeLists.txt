# 1. create source cpp and CMakeFile
# 2. mkdir build
# 3. cmake .. - Generating the Build Files / Configure the Project
# 4. cmake --build .
# 5. ./Executable

cmake_minimum_required(VERSION 3.31)

project(CppProjectTemplate VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(app)