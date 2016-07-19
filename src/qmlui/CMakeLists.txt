set(SOURCES
    chatlinemodel.cpp
    chatlinemodelitem.cpp
    qmlui.cpp
    qmluiaccountmodel.cpp
    qmluiapplication.cpp
    qmluibuffermodel.cpp
    qmluimessagemodel.cpp
    qmluimessageprocessor.cpp
)
if (USE_UBUNTU)
    qt_add_resources(SOURCES ubuntu/qml.qrc)
    add_subdirectory(ubuntu)
    list(APPEND SOURCES mainwin.cpp)
else()
    qt_add_resources(SOURCES controls/qml.qrc)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/client
                    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(mod_qmlui STATIC ${SOURCES} ${SPSRC} ${UI})
qt_use_modules(mod_qmlui Core Network Qml Quick)
target_link_libraries(mod_qmlui mod_client mod_common)


