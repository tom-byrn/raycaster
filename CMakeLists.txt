cmake_minimum_required(VERSION 3.10)

project(Raycaster)

# Find OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Add the executable
add_executable(raycaster code.c)

# Link libraries
target_link_libraries(raycaster OpenGL::GL GLUT::GLUT)
