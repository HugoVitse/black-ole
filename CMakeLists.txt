#global

cmake_minimum_required(VERSION 3.21)
project(black++ole VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories




#cpu 
file(GLOB CPU_SOURCES "src/cpu/*.cpp")
add_executable(black++ole ${CPU_SOURCES})
target_include_directories(black++ole PRIVATE include/cpu)

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



#gpu


#opengl
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB GPU_SOURCES "src/gpu/*.cpp")
add_executable(blackhole_gpu ${GPU_SOURCES})
target_include_directories(blackhole_gpu PRIVATE include/gpu)


if(WIN32)
    target_link_libraries(blackhole_gpu glfw GLEW::GLEW opengl32)
else()
    target_link_libraries(blackhole_gpu glfw GLEW::GLEW OpenGL::GL)
endif()



# Commande pour copier le shader dans le dossier de build
add_custom_command(TARGET blackhole_gpu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/src/gpu/*.glsl"
    "$<TARGET_FILE_DIR:blackhole_gpu>/"
)