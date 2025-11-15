find_package(yaml-cpp QUIET)

# Use fetchcontent if target does not exist
if(NOT TARGET yaml-cpp::yaml-cpp)
    message(STATUS "yaml-cpp not found — using FetchContent...")
    
    #https://cmake.org/cmake/help/latest/module/FetchContent.html
    include(FetchContent)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
            GIT_TAG yaml-cpp-0.8.x
    )

    FetchContent_MakeAvailable(yaml-cpp)
endif()

target_link_libraries(MolSimLib yaml-cpp::yaml-cpp)
