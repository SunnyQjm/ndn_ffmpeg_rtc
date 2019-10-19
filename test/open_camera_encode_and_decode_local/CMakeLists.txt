project(open_camera_encode_and_decode_local)

add_executable(open_camera_encode_and_decode_local src/main.cpp)
target_link_libraries(open_camera_encode_and_decode_local ${SDL2_LIBRARY} ${SDL_HELPER_LIBRARY} avcodec avformat swscale avdevice avutil ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS open_camera_encode_and_decode_local RUNTIME DESTINATION ${BIN_DIR})
