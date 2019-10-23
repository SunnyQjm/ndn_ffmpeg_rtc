project(sdl_helper)

add_library(sdl_helper SHARED SDL2Helper.cpp SDL2Helper.h FFmpegFailedException.cpp FFmpegFailedException.h)
target_link_libraries(sdl_helper ${SDL2_LIBRARY})
#target_link_directories(sdl_helper PUBLIC ${PROJECT_SOURCE_DIR})
add_library(ffmpeg_helper SHARED FFmpegHelper.cpp EasyEncoder.cpp EasyDecoder.cpp
        EasyCamera.cpp FFmpegUtil.cpp FFmpegFailedException.cpp FFmpegFailedException.h)
target_link_libraries(ffmpeg_helper avcodec avformat swscale avdevice avutil)
#target_link_directories(ffmpeg_helper PUBLIC ${PROJECT_SOURCE_DIR})


