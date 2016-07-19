# Substitute variables in the desktop file
set(DESKTOP_DIR ${DATA_DIR})
set(DESKTOP_FILE_NAME "${CMAKE_PROJECT_NAME}.desktop")
set(ICON "quassel.png")
set(EXEC "bin/quasselclient")
configure_file(${DESKTOP_FILE_NAME}.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME} DESTINATION ${DESKTOP_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/icons/oxygen/256x256/apps/${ICON} DESTINATION ${DESKTOP_DIR})
