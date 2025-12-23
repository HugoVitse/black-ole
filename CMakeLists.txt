cmake_minimum_required(VERSION 3.21)
project(black++ole VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include)

set(SOURCES
    src/main.cpp
    src/vectors.cpp
    #src/blackhole.cpp
    src/skybox.cpp
    src/photon.cpp
    src/image.cpp
    src/pixel.cpp
)

add_executable(black++ole ${SOURCES})

# Performance: enable high optimization and OpenMP if available
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(black++ole PRIVATE -O3 -march=native -pipe)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(black++ole PRIVATE OpenMP::OpenMP_CXX)
endif()

# Optionnel : lier des librairies
# target_link_libraries(MyProject PRIVATE SomeLib)
