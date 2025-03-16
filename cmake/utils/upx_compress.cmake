if(NOT CMAKE_CONFIGURATION_TYPES AND CMAKE_BUILD_TYPE)
  if(CMAKE_RELEASE AND CMAKE_UPX_COMPRESS)
    message(STATUS "Stripping with ${CMAKE_STRIP}")
    message(STATUS "Applying UPX compression")

    if(APPLE)
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}> && upx -9 $<TARGET_FILE:${PROJECT_NAME}>)
    else()
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${PROJECT_NAME}> && upx -9 $<TARGET_FILE:${PROJECT_NAME}>)
    endif()

  elseif(CMAKE_RELEASE)
    message(STATUS "Stripping with ${CMAKE_STRIP}")

    if(APPLE)
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
    else()
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${PROJECT_NAME}>)
    endif()

  elseif(CMAKE_UPX_COMPRESS)
    message(STATUS "Applying UPX compression")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND upx -9 $<TARGET_FILE:${PROJECT_NAME}>)
  endif()
endif()

