option(ENABLE_VTK_OUTPUT "Enable VTK output" OFF)

if(ENABLE_VTK_OUTPUT)
    message(STATUS "VTK output enabled")
    # VTK Library, only add required components for less unnecessary includes
    find_package(VTK REQUIRED COMPONENTS
            CommonCore
            CommonDataModel
            IOXML
    )
    #Generates the Macro ENABLE_VTK_OUTPUT, which makes VTKWriter.* visible to the rest of the Project
    target_compile_definitions(MolSimLib PRIVATE ENABLE_VTK_OUTPUT)

    if(VTK_FOUND)
        message (STATUS "Found VTK Version: ${VTK_VERSION}")
    else ()
        message(FATAL_ERROR "VTK not found")
    endif ()

    if(VTK_VERSION VERSION_GREATER_EQUAL 8.9)
         include_directories(${VTK_INCLUDE_DIRS})
    else()
        include(${VTK_USE_FILE})
    endif ()

    target_link_libraries(MolSimLib
                ${VTK_LIBRARIES}
    )
endif()
