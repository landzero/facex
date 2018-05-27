cmake_minimum_required(VERSION 2.8.12)
project(facex)

find_package(dlib REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(facex src/main.cpp src/result.cpp src/server.cpp src/capture.cpp)
target_link_libraries(facex dlib::dlib ${OpenCV_LIBS})
