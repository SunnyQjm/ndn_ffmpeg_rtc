project(sdl_helper)

add_library(sdl_helper SHARED SDL2Helper.cpp SDL2Helper.h FFmpegFailedException.cpp FFmpegFailedException.h)
add_library(sdl_helper_static STATIC SDL2Helper.cpp SDL2Helper.h FFmpegFailedException.cpp FFmpegFailedException.h)
set_target_properties(sdl_helper_static PROPERTIES OUTPUT_NAME "sdl_helper")
target_link_libraries(sdl_helper ${SDL2_LIBRARY})

add_library(ffmpeg_helper SHARED FFmpegHelper.cpp EasyVideoEncoder.cpp EasyVideoDecoder.cpp
        EasyCamera.cpp FFmpegUtil.cpp FFmpegFailedException.cpp FFmpegFailedException.h
        EasyMicroPhone.h EasyMicroPhone.cpp EasyAudioEncoder.h EasyAudioEncoder.cpp)
add_library(ffmpeg_helper_static STATIC FFmpegHelper.cpp EasyVideoEncoder.cpp EasyVideoDecoder.cpp
        EasyCamera.cpp FFmpegUtil.cpp FFmpegFailedException.cpp FFmpegFailedException.h
        EasyMicroPhone.h EasyMicroPhone.cpp EasyAudioEncoder.h EasyAudioEncoder.cpp)
set_target_properties(ffmpeg_helper_static PROPERTIES OUTPUT_NAME "ffmpeg_helper")

target_link_libraries(ffmpeg_helper avcodec avformat swscale avdevice avutil)


