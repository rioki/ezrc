cmake_minimum_required(VERSION 3.21)
project(ezrc VERSION 0.4.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_path(TINYFORMAT_INCLUDE_DIRS "tinyformat.h")
find_package(fkYAML CONFIG REQUIRED)

add_executable(ezrc ezrc/main.cpp)
target_include_directories(ezrc PRIVATE ${TINYFORMAT_INCLUDE_DIRS})
target_link_libraries(ezrc PRIVATE
    fkYAML::fkYAML
)

install(TARGETS ezrc RUNTIME DESTINATION bin)

# Optional unit tests (off by default)
option(EZRC_BUILD_TESTS "Build ezrc unit tests" OFF)

if(EZRC_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    find_package(FreeImage CONFIG REQUIRED)

    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test/resources.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test/resources.cpp
        COMMAND ezrc ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test/resources.yml
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test/resources.yml
            ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test/math.glsl
            ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test/lena.png
            ezrc
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test
        COMMENT "Generating resources.h / resources.cpp via ezrc"
    )

    add_executable(ezrc-test
        ezrc-test/main.cpp
        ezrc-test/resources.cpp
    )

    target_compile_definitions(ezrc-test PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test" )

    target_link_libraries(ezrc-test PRIVATE
        GTest::gtest
        fkYAML::fkYAML
        freeimage::FreeImage
    )

    target_include_directories(ezrc-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ezrc-test)

    include(GoogleTest)
    gtest_discover_tests(ezrc-test)
endif()
