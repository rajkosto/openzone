configure_file(config.hh.in config.hh @ONLY)
configure_file(ozEngine.pc.in ozEngine.pc @ONLY)

add_library(ozEngine
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  AL.hh
  common.hh
  Cursor.hh
  GL.hh
  Lua.hh
  ozEngine.hh
  Window.hh
  AL.cc
  Cursor.cc
  GL.cc
  Lua.cc
  Window.cc
#END SOURCES
)

target_link_libraries(ozEngine ${LUA_LIBRARIES} ${GL_LIBRARIES} ${OPENAL_LIBRARY} ${JPEG_LIBRARY}
                               ${VORBISFILE_LIBRARIES} ${SDL_LIBRARIES} ${SDL_TTF_LIBRARIES})

if(NOT NACL)
#  target_link_libraries(ozEngine ${SDL_NET_LIBRARIES})
endif()

target_link_libraries(ozEngine ozCore)

set_target_properties(ozEngine PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0)

install(TARGETS ozEngine RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
                         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
                         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozEngine)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ozEngine.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig)
