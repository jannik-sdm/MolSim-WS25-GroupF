#https://google.github.io/googletest/quickstart-cmake.html

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

# Now simply link against gtest or gtest_main as needed. Eg
target_link_libraries(MolSim gtest_main)
#add_test(NAME example_test COMMAND example)
enable_testing()

file(GLOB_RECURSE MY_TEST
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

add_executable(MolSimTester ${MY_TEST})

target_link_libraries(
        MolSimTester
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(MolSimTester)


