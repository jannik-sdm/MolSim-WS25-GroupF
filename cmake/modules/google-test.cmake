#https://google.github.io/googletest/quickstart-cmake.html

option(ENABLE_TEST_TARGET "Enable test Target" ON)
if (ENABLE_TEST_TARGET)
    include(FetchContent)
    message(STATUS "including google Test")
    #Notwendig um eine Warnung, dass google-test bald cMake < 3.5 nicht mehr unterstützt zu unterdrücken. Da wir cMake 3.10 vorraussetzen, ist das aber eine unnötige Warnung
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)

    FetchContent_Declare(
            googletest
            #https://cmake.org/cmake/help/latest/module/FetchContent.html
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        main
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    #Exclude main(). Otherwise the tester would try to execute the main function
    list(REMOVE_ITEM MY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/MolSim.cpp")
    #Necessary for Tester
    add_library(MolSimLib ${MY_SRC})
    target_include_directories(MolSimLib
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # Now simply link against gtest or gtest_main as needed. Eg
    target_link_libraries(MolSim gtest_main)
    add_test(NAME MolSimTester COMMAND ${CMAKE_PROJECT_NAME})
endif()

