set(CURRENT_LIBRARY_NAME spdlog)

FetchContent_Declare(
  ${CURRENT_LIBRARY_NAME}
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.0
)

#set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library" FORCE)
#set(SPDLOG_FMT_HEADER_ONLY OFF CACHE BOOL "Disable header only mode" FORCE)

FetchContent_MakeAvailable(${CURRENT_LIBRARY_NAME})

list(APPEND PROJECT_LIBRARIES_LIST spdlog::spdlog)

#target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ENABLE_SYSLOG)

