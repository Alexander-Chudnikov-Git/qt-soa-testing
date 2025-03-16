set(CURRENT_LIBRARY_NAME fmt)

FetchContent_Declare(
  ${CURRENT_LIBRARY_NAME}
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        11.0.2
)
FetchContent_MakeAvailable(${CURRENT_LIBRARY_NAME})

list(APPEND PROJECT_LIBRARIES_LIST fmt::fmt)

