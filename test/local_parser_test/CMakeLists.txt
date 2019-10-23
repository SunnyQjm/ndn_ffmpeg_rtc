project(local_parser_test)

add_executable(local_parser_test src/main.cpp)
target_link_libraries(local_parser_test ${SDL_HELPER_LIBRARY} ${FFMPEG_HELPER_LIBRARY} ${NDN_RTP_LIBRARY})
install(TARGETS local_parser_test RUNTIME DESTINATION ${BIN_DIR})