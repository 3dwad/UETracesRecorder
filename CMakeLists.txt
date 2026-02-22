cmake_minimum_required(VERSION 3.20)
project(UETracesRecorder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(UETracesRecorder WIN32 main.cpp)

target_compile_definitions(UETracesRecorder PRIVATE UNICODE _UNICODE)
target_link_libraries(UETracesRecorder PRIVATE Ole32)

if(MSVC)
    target_compile_options(UETracesRecorder PRIVATE /W4 /permissive-)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UETracesRecorder)
endif()
