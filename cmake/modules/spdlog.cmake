find_package(spdlog QUIET)

# Wenn kein Target existiert, FetchContent als Fallback
if(NOT TARGET spdlog::spdlog)
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

target_link_libraries(MolSimLib spdlog::spdlog)

set(LOG_LEVEL "INFO"
    CACHE STRING "Set spdlog log level"
)

set_property(CACHE LOG_LEVEL PROPERTY STRINGS
    TRACE DEBUG INFO WARN ERROR OFF
)


if(LOG_LEVEL STREQUAL "TRACE")
    set(SPDLOG_LEVEL SPDLOG_LEVEL_TRACE)
elseif(LOG_LEVEL STREQUAL "DEBUG")
    set(SPDLOG_LEVEL SPDLOG_LEVEL_DEBUG)
elseif(LOG_LEVEL STREQUAL "INFO")
    set(SPDLOG_LEVEL SPDLOG_LEVEL_INFO)
elseif(LOG_LEVEL STREQUAL "WARN")
    set(SPDLOG_LEVEL SPDLOG_LEVEL_WARN)
elseif(LOG_LEVEL STREQUAL "ERROR")
    set(SPDLOG_LEVEL SPDLOG_LEVEL_ERROR)
elseif(LOG_LEVEL STREQUAL "OFF")
    set(SPDLOG_LEVEL SPDLOG_LEVEL_OFF)
else()
    message(FATAL_ERROR "Invalid LOG_LEVEL '${LOG_LEVEL}'. Must be one of: DEBUG, INFO, WARN, ERROR, OFF")
endif()

if(TARGET MolSimLib)
    target_compile_definitions(MolSimLib
        PRIVATE SPDLOG_ACTIVE_LEVEL=${SPDLOG_LEVEL}
    )
endif()
if(TARGET MolSim)
    target_compile_definitions(MolSim
        PRIVATE SPDLOG_ACTIVE_LEVEL=${SPDLOG_LEVEL}
    )
endif()
if(TARGET MolSimTester)
    target_compile_definitions(MolSimTester
        PRIVATE SPDLOG_ACTIVE_LEVEL=${SPDLOG_LEVEL}
    )
endif()