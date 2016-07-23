# Adds a target to create an .apk

# Depends on the Android toolchain from extra-cmake-modules.

# Based heavily on the code in the Android toolchain in extra-cmake-modules, copyright as follows:
# =============================================================================
# Copyright 2014 Aleix Pol i Gonzalez <aleixpol@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file COPYING-CMAKE-SCRIPTS for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of extra-cmake-modules, substitute the full
#  License text for the above reference.)


if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    # Work around bug in extra-cmake-modules
    SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    function(add_apk _target)
        set(APK_SOURCE "${CMAKE_SOURCE_DIR}/android/")
        if(NOT EXISTS "${APK_SOURCE}/${_target}/AndroidManifest.xml")
            message(FATAL_ERROR "Missing APK source dir for target ${_target}")
        endif()

        set(APK_TARGET_NAME "${_target}-apk")

        set(EXPORT_DIR "${CMAKE_BINARY_DIR}/${_target}_build_apk/")
        set(EXECUTABLE_DESTINATION_PATH "${EXPORT_DIR}/libs/${ANDROID_ABI}/lib${_target}.so")
        configure_file("${APK_SOURCE}/deployment-file.json.in" "${_target}-deployment.json.in")
        add_custom_target(${APK_TARGET_NAME}
            COMMAND cmake -E echo "Generating $<TARGET_NAME:${_target}> with $<TARGET_FILE_DIR:Qt5::qmake>/androiddeployqt"
            COMMAND cmake -E remove_directory "${EXPORT_DIR}"
            COMMAND cmake -E copy_directory "${APK_SOURCE}/${_target}" "${EXPORT_DIR}"
            COMMAND cmake -E copy "$<TARGET_FILE:${_target}>" "${EXECUTABLE_DESTINATION_PATH}"
            COMMAND cmake -DINPUT_FILE="${_target}-deployment.json.in" -DOUTPUT_FILE="${_target}-deployment.json" "-DTARGET_DIR=$<TARGET_FILE_DIR:${_target}>" "-DTARGET_NAME=${_target}" "-DEXPORT_DIR=${CMAKE_INSTALL_PREFIX}" -P ${_CMAKE_ANDROID_DIR}/specifydependencies.cmake
            COMMAND $<TARGET_FILE_DIR:Qt5::qmake>/androiddeployqt --input "${_target}-deployment.json" --output "${EXPORT_DIR}" --deployment bundled "\\$(ARGS)"
            )
    endfunction()
else()
    # Dummy target
function(add_apk _target)
endfunction()
endif()
