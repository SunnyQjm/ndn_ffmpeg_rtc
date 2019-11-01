project(audio_player_test)

add_executable(audio_player_test src/main.cpp)
target_link_libraries(audio_player_test ${SDL_HELPER_LIBRARY} ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS audio_player_test RUNTIME DESTINATION ${BIN_DIR})
