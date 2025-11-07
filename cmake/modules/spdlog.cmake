find_package(spdlog QUIET)

# Wenn kein Target existiert, FetchContent als Fallback
if(NOT TARGET spdlog_FOUND)
    message(STATUS "spdlog not found — using FetchContent...")
    #https://cmake.org/cmake/help/latest/module/FetchContent.html
    include(FetchContent)
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.x
    )

    FetchContent_MakeAvailable(spdlog)
endif()

target_link_libraries(MolSim spdlog::spdlog)
