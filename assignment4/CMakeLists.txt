cmake_minimum_required(VERSION 3.10)
project(BezierCurve)

set(CMAKE_CXX_STANDARD 14)

message("Current OS is: ${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(OpenCV_DIR "E:/opencv/build")
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    find_package(OpenCV REQUIRED)
    include_directories(/usr/local/include)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(OpenCV REQUIRED)
endif()

add_executable(BezierCurve main.cpp)

target_link_libraries(BezierCurve ${OpenCV_LIBRARIES})
