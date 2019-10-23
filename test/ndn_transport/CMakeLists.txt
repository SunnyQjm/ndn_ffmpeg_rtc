project(ndn_transport)

add_executable(ndn_transport_client src/client.cpp)
target_link_libraries(ndn_transport_client ${SDL_HELPER_LIBRARY} ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS ndn_transport_client RUNTIME DESTINATION ${BIN_DIR})

add_executable(ndn_transport_source src/source.cpp)
target_link_libraries(ndn_transport_source ${SDL_HELPER_LIBRARY} ${FFMPEG_HELPER_LIBRARY} )
install(TARGETS ndn_transport_source RUNTIME DESTINATION ${BIN_DIR})