project(sdl_helper_test)

add_executable(sdl_helper_test src/main.cpp)
message(STATUS ${SDL2_LIBRARY})
message(STATUS ${SDL2_HELPER_LIBRARY})
target_link_libraries(sdl_helper_test ${SDL2_LIBRARY} ${SDL_HELPER_LIBRARY})
install(TARGETS sdl_helper_test RUNTIME DESTINATION ${BIN_DIR})
