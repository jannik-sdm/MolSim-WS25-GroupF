# make doc_doxygen optional if someone does not have / like doxygen
# TODO: create CMake build option for the target.
option(ENABLE_DOXYGEN_TARGET "Enable doxygen Target" ON)
if(ENABLE_DOXYGEN_TARGET)
    find_program(DOXYGEN "doxygen")
    message(STATUS "doxygen Target is enabled")
    IF(DOXYGEN)
        message(STATUS "found doxygen command")
        # TODO: Add a custom target for building the documentation.
        add_custom_target(doc_doxygen
            COMMAND ${DOXYGEN} ..
            COMMENT "Generate a Doxygen documentation with make doc_doxygen"
        )
    else()
        message(FATAL_ERROR "Could not find doxygen")
    endif ()
endif ()