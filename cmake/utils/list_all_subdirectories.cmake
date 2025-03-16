function(list_all_subdirectories BASE_DIR OUTPUT_VAR)
    if(NOT EXISTS ${BASE_DIR})
        message(FATAL_ERROR "Directory '${BASE_DIR}' does not exist.")
    endif()

    file(GLOB LIST_DIRS LIST_DIRECTORIES true "${BASE_DIR}/*")

    set(SUBDIRS "")

    foreach(ITEM ${LIST_DIRS})
        if(IS_DIRECTORY ${ITEM})
            list(APPEND SUBDIRS ${ITEM})
            list_all_subdirectories(${ITEM} SUBDIRS_RECURSIVE)
            list(APPEND SUBDIRS ${SUBDIRS_RECURSIVE})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES SUBDIRS)

    set(${OUTPUT_VAR} ${SUBDIRS} PARENT_SCOPE)
endfunction()

