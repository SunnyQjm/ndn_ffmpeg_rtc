cmake_minimum_required(VERSION 3.15)
project(ndn_ffmpeg_rtc)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Bump up warning levels appropriately for clang, gcc & msvc and build in debug mode
if (MSVC)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif ()
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif ()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
#include_directories(helper)
#include_directories(helper/exception)

#add_library(sdl_helper SHARED helper/SDL2Helper.cpp)
#target_include_directories(sdl_helper PUBLIC helper)
#add_library(sdl_helper_static STATIC helper/SDL2Helper.cpp)
#set_target_properties(sdl_helper_static PROPERTIES OUTPUT_NAME "sdl_helper")

#add_library(ffmpeg_helper SHARED helper/FFmpegHelper.cpp helper/EasyEncoder.cpp helper/EasyDecoder.cpp
#        helper/EasyCamera.cpp helper/FFmpegUtil.cpp)
#add_library(ffmpeg_helper_static STATIC helper/FFmpegHelper.cpp helper/FFmpegHelper.cpp helper/EasyEncoder.cpp
#        helper/EasyDecoder.cpp helper/EasyCamera.cpp helper/FFmpegUtil.cpp)
#set_target_properties(ffmpeg_helper_static PROPERTIES OUTPUT_NAME "ffmpeg_helper")



add_subdirectory(helper)
include_directories(helper)
include_directories(helper/exception)
set(SDL_HELPER_LIBRARY sdl_helper)
set(FFMPEG_HELPER_LIBRARY ffmpeg_helper)

add_subdirectory(ndn_rtp)
include_directories(ndn_rtp)
set(NDN_RTP_LIBRARY ndn_rtp)

add_subdirectory(test/sdl_helper_test)
add_subdirectory(test/open_camera_test)
add_subdirectory(test/video_encoder_test)
add_subdirectory(test/open_camera_encode_and_decode_local)
add_subdirectory(test/ndn_transport)
add_subdirectory(test/local_parser_test)
