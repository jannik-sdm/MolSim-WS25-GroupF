find_package(yaml-cpp QUIET)

# Use fetchcontent if target does not exist
if(NOT TARGET yaml-cpp::yaml-cpp)
    message(STATUS "yaml-cpp not found — using FetchContent...")
    
    # Using following commit:
    # https://github.com/jbeder/yaml-cpp/commit/c9371de7836d113c0b14bfa15ca70f00ebb3ac6f
    # The latest release (0.8.0) has a low CMake version that causes build errors.
    include(FetchContent)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
            GIT_TAG c9371de
    )

    FetchContent_MakeAvailable(yaml-cpp)
endif()

target_link_libraries(MolSimLib yaml-cpp::yaml-cpp)
