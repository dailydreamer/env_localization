cmake_minimum_required(VERSION 3.1.0)
project(env_localization)

set(CMAKE_CXX_STANDARD 14)

# set(OpenCV_DIR /usr/local/opencv3.3.0)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(env_localization main.cpp ipm.cpp ipm.h)
target_link_libraries(env_localization ${OpenCV_LIBS})

add_executable(gen_marker gen_marker.cpp)
target_link_libraries(gen_marker ${OpenCV_LIBS})

add_executable(compute_extrinsic compute_extrinsic.cpp ipm.cpp ipm.h)
target_link_libraries(compute_extrinsic ${OpenCV_LIBS})

add_executable(recorder recorder.cpp)
target_link_libraries(recorder ${OpenCV_LIBS})