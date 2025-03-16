execute_process(
    COMMAND cat /etc/os-release
    COMMAND grep -oP "^ID=\\K[a-zA-Z0-9]*"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE OS_RELEASE_RESULT
    OUTPUT_VARIABLE CMAKE_SYSTEM_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(OS_RELEASE_RESULT)
    message(WARNING "Unknown kernel ID, /etc/os-release not found.")
    set(CMAKE_SYSTEM_ID "unknown")
endif()

message(STATUS "Kernel Name:       ${CMAKE_SYSTEM_NAME}")
message(STATUS "Kernel Version:    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Kernel ID:         ${CMAKE_SYSTEM_ID}")
message(STATUS "Processor:         ${CMAKE_SYSTEM_PROCESSOR}")

execute_process(
    COMMAND bash -c "echo $XDG_CURRENT_DESKTOP $DESKTOP_SESSION $XDG_SESSION_DESKTOP $GDMSESSION"
    OUTPUT_VARIABLE WM_ENV
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if("${WM_ENV}" MATCHES ".*bspwm.*|.*i3.*|.*awesome.*|.*dwm.*|.*xmonad.*|.*herbstluftwm.*|.*qtile.*|.*spectrwm.*|.*Hyprland.*|.*sway.*")
    message(STATUS "Detected a tiling window manager")
    set(CMAKE_SYSTEM_TILING 1)
else()
    message(STATUS "No tiling window manager detected")
    set(CMAKE_SYSTEM_TILING 0)
endif()

if("${CMAKE_SYSTEM_ID}" STREQUAL "arch")
    message(STATUS "Oh, I see, you are the arch linux enjoyer after all :3")
elseif("${CMAKE_SYSTEM_ID}" STREQUAL "ubuntu")
    message(WARNING "I highly doubdt that you know what you are doing :/")
endif()
