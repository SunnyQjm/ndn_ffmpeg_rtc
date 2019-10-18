project(video_encoder_test)

add_executable(video_encoder_test src/main.cpp)
target_link_libraries(video_encoder_test ${SDL2_LIBRARY} ${SDL_HELPER_LIBRARY} avcodec avformat swscale avdevice avutil ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS video_encoder_test RUNTIME DESTINATION ${BIN_DIR})
