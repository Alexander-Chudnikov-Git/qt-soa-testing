execute_process(
    COMMAND cmd /C "wmic os get BuildNumber /format:table | more +1"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE OS_RELEASE_RESULT
    OUTPUT_VARIABLE CMAKE_SYSTEM_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(OS_RELEASE_RESULT)
    message(WARNING "Unknown windows build, wmic not found.")
    set(CMAKE_SYSTEM_ID "unknown")
endif()

message(STATUS "Kernel Name:       ${CMAKE_SYSTEM_NAME}")
message(STATUS "Kernel Version:    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Kernel ID:         ${CMAKE_SYSTEM_ID}")
message(STATUS "Processor:         ${CMAKE_SYSTEM_PROCESSOR}")

message(STATUS "No tiling window manager detected")
set(CMAKE_SYSTEM_TILING 0)
