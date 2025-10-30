# make doc_doxygen optional if someone does not have / like doxygen
find_package(Doxygen REQUIRED)
# TODO: create CMake build option for the target.
option(ENABLE_DOXYGEN_TARGET "Enable doxygen Target" ON)
if(ENABLE_DOXYGEN_TARGET)
    message(STATUS "doxygen Target is enabled")
    #https://cmake.org/cmake/help/latest/module/FindDoxygen.html
    find_package(Doxygen REQUIRED)

    IF(DOXYGEN_FOUND)
        message(STATUS "found doxygen Package")

        #This sets the Flags for the Doxyfile

        #Do not build the Documentation in the build Folder. Make a new one
        set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/doxys_documentation)
        # Set the README as mainpage
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
        #Generating LATEX
        set(DOXYGEN_GENERATE_LATEX YES)
        #Makes VTKWriter a part of the Documentation, if it is activated
        IF(ENABLE_VTK_OUTPUT)
            set(DOXYGEN_PREDEFINED ENABLE_VTK_OUTPUT)
        endif()
        # TODO: Add a custom target for building the documentation.
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