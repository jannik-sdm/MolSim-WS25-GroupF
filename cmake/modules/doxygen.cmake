# make doc_doxygen optional if someone does not have / like doxygen
find_package(Doxygen REQUIRED)
# TODO: create CMake build option for the target.
option(ENABLE_DOXYGEN_TARGET "Enable doxygen Target" ON)
if(ENABLE_DOXYGEN_TARGET)
    message(STATUS "doxygen Target is enabled")
    find_package(Doxygen REQUIRED)

    IF(DOXYGEN_FOUND)
        message(STATUS "found doxygen Package")
        # TODO: Add a custom target for building the documentation.

        set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/doxys_documentation)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

        doxygen_add_docs(
                doc_doxygen
                src/ README.md
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                COMMENT "Generate a Doxygen documentation with make doc_doxygen"
        )
    else()
        message(FATAL_ERROR "Could not find doxygen")
    endif ()
endif ()