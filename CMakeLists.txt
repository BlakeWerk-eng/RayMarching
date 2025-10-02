cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(RayMarch)

add_executable(RayMarch src/main.cpp src/glad.c src/image.cpp)
target_include_directories(RayMarch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RayMarch glfw GL)

