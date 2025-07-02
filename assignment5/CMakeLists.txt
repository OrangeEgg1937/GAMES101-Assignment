cmake_minimum_required(VERSION 3.10)
project(RayTracing)

# Set the C++ standard, this is the modern way to do it
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add your executable and its source files
add_executable(RayTracing 
    main.cpp 
    Object.hpp 
    Vector.hpp 
    Sphere.hpp 
    global.hpp 
    Triangle.hpp 
    Scene.cpp 
    Scene.hpp 
    Light.hpp 
    Renderer.cpp
)

# Set compiler options in a cross-platform way
# MSVC (Visual Studio) doesn't use the same warning flags as GCC/Clang
if(MSVC)
    # /W4 is similar to -Wall and -Wextra. /permissive- enforces standards compliance.
    target_compile_options(RayTracing PRIVATE /W4 /permissive-)
else()
    # These are the flags for GCC and Clang
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -pedantic -Wshadow -Wreturn-type)
endif()