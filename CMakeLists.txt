cmake_minimum_required(VERSION 3.20)
project(UETracesRecorder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(UETracesRecorder WIN32 main.cpp)
target_link_libraries(UETracesRecorder PRIVATE Ole32)
