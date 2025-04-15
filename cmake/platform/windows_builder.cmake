include(cmake/utils/get_windows_version.cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Build as debug" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(
            /O2
            /Ob2
            /Oi
            /Ot
            /GL
            /Gy
            /Gw
            /GS-
            /Gy
            /W4
            /permissive-
            /volatile:iso
            /EHsc
            /GR
            /Z7
            /pdb:none
            /Zc:forScope
            /Zc:inline
            /Zc:rvalueCast
            /Zc:strictStrings
            /Zc:throwingNew
            /Zc:wchar_t
        )
    elseif(MINGW)
        add_compile_options(
            -O2
            -fomit-frame-pointer
            -funroll-loops
            -Wall
            -Wextra
            -Werror
            -fno-strict-aliasing
            -fno-common
            -D__USE_MINGW_ANSI_STDIO=1
        )
    elseif(CLANG)
        add_compile_options(
            -O2
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -fno-strict-aliasing
            -fno-common
        )
    endif()
else()
    if(MSVC)
        add_compile_options(
            /W4
            /WX-
            /MDd
        )
    elseif(MINGW)
        add_compile_options(
            -Wall
            -g
            -DDEBUG
        )
    elseif(CLANG)
        add_compile_options(
            -Wall
            -g
            -DDEBUG
        )
    endif()
endif()

# [EXECUTABLE]
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} WIN32 MANUAL_FINALIZATION ${PROJECT_MAIN_SRC_FILES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_MAIN_SRC_FILES} ${PROJECT_QRCS_FILES} ${PROJECT_QM_FILES})
endif()

# [INSTALLATION]

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

include(InstallRequiredSystemLibraries)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${Qt6_DIR}/../../../bin/windeployqt.exe $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Running windeployqt to gather Qt dependencies"
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/platforms
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/imageformats
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/generic
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/iconengines
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/networkinformation
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/platforminputcontexts
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/position
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/qml
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/qmltooling
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/resources
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/styles
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/tls
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/translations
        DESTINATION bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/
        DESTINATION bin
        FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "*.exe"
)

set(CPACK_GENERATOR "NSIS")

set(CPACK_NSIS_COMPRESSION_TYPE "lzma")
set(CPACK_NSIS_COMPRESSION_LEVEL 9)

set(CPACK_PACKAGE_NAME                "${PROJECT_APPLICATION_NAME}")
set(CPACK_PACKAGE_VERSION             "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "${PROJECT_ORGANIZATION_NAME}\\${PROJECT_APPLICATION_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SOA testing utility")
set(CPACK_RPM_PACKAGE_LICENSE         "MIT")
set(CPACK_PACKAGE_VENDOR              "${PROJECT_ORGANIZATION_NAME}")
set(CPACK_PACKAGE_CONTACT             "${PROJECT_DEVELOPER_EMAIL}")
set(CPACK_PACKAGE_EXECUTABLES         "${PROJECT_NAME}" "${PROJECT_NAME}")

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_APPLICATION_NAME} Installer")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\${PROJECT_NAME}.exe")
#set(CPACK_NSIS_ICON "${CMAKE_SOURCE_DIR}/installer_icon.ico")
set(CPACK_NSIS_CONTACT "${PROJECT_DEVELOPER_EMAIL}")
set(CPACK_NSIS_URL_INFO_ABOUT "${PROJECT_ORGANIZATION_WEBSITE}")
set(CPACK_NSIS_FILE_ASSOCIATIONS ".txt=${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")

include(CPack)
