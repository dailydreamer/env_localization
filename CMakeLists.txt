cmake_minimum_required(VERSION 3.1.0)
project(env_localization)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp ipm.cpp ipm.h)
add_executable(env_localization ${SOURCE_FILES})
target_link_libraries(env_localization ${OpenCV_LIBS})