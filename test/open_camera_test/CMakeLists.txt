project(open_camera_test)

add_executable(open_camera_test src/main.cpp)
target_link_libraries(open_camera_test ${SDL_HELPER_LIBRARY} ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS open_camera_test RUNTIME DESTINATION ${BIN_DIR})
