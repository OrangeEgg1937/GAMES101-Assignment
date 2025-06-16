cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

set(CMAKE_CXX_STANDARD 17)

# Specify the path to the Eigen3 headers
# Assuming Eigen3 is in project_root/dependency/Eigen3
set(EIGEN3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../dependency/Eigen3")

if(WIN32)
    message(STATUS "Configuring for Windows")
    set(OpenCV_DIR "E:/opencv/build")
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
elseif(APPLE)
    find_package(OpenCV REQUIRED)
    include_directories(/usr/local/include)
endif()

add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp global.hpp Triangle.hpp Triangle.cpp Texture.hpp Texture.cpp Shader.hpp OBJ_Loader.h)
target_link_libraries(Rasterizer ${OpenCV_LIBRARIES})
#target_compile_options(Rasterizer PUBLIC -Wall -Wextra -pedantic)