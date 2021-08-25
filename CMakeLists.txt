cmake_minimum_required(VERSION 3.0.0)
project(ffmpeg_streamer VERSION 0.1.0)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_C_FLAGS "-O3 -fPIC")
add_definitions(-fPIC)

include_directories ("${PROJECT_SOURCE_DIR}/include")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
# include(spdlog)

add_subdirectory(test)
add_subdirectory(module/spdlog)
add_subdirectory(module/cpp-httplib-master)

add_library(
    ${PROJECT_NAME} 
    SHARED  
    ./src/ffmpeg_streamer.cpp
    ./src/save_stream_data_local.cpp
    ./include/timer.h
)

target_link_libraries(
    ffmpeg_streamer

    avfilter
    avformat
    avcodec
    avdevice
    swresample
    swscale
    avutil 
    spdlog::spdlog
    httplib::httplib
)
