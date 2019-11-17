project(open_microphone_test)

add_executable(open_microphone_test src/main.cpp)
target_link_libraries(open_microphone_test ${SDL_HELPER_LIBRARY} ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS open_microphone_test RUNTIME DESTINATION ${BIN_DIR})
