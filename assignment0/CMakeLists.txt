cmake_minimum_required(VERSION 3.10)
project (Transformation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the path to the Eigen3 headers
# Assuming Eigen3 is in project_root/dependency/Eigen3
set(EIGEN3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../dependency/Eigen3")

# Verify that the Eigen3 directory exists
if(NOT EXISTS "${EIGEN3_INCLUDE_DIR}")
    message(FATAL_ERROR "Eigen3 not found at ${EIGEN3_INCLUDE_DIR}. Please ensure the submodule is initialized and updated.")
endif()

# Add the executable for task1
add_executable(Transformation main.cpp)

# Include the Eigen3 headers
target_include_directories(Transformation PRIVATE ${EIGEN3_INCLUDE_DIR})

target_link_libraries(Transformation PRIVATE Eigen3)