cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0135 NEW)

project(image_processor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
        ${SOURCE_DIR}/Filters.cpp
        ${SOURCE_DIR}/PictureInfo.cpp
        ${SOURCE_DIR}/image_processor.cpp
        ${SOURCE_DIR}/input_control/ControlParameters.cpp
        ${SOURCE_DIR}/input_control/Input_OutputProcessing.cpp
)

set(HEADERS
        ${INCLUDE_DIR}/PictureInfo.h
        ${INCLUDE_DIR}/Exceptions.h
        ${INCLUDE_DIR}/Filters.h
        ${INCLUDE_DIR}/input_control/ControlParameters.h
        ${INCLUDE_DIR}/input_control/Input_OutputProcessing.h
)

add_library(image_processor_lib ${SOURCES} ${HEADERS})

target_include_directories(image_processor_lib PUBLIC ${INCLUDE_DIR})

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(image_processor ${SOURCE_DIR}/image_processor.cpp)
target_link_libraries(image_processor image_processor_lib)

add_executable(unit_tests test_script/unit_tests.cpp)
target_link_libraries(unit_tests image_processor_lib gtest_main)

add_test(NAME UnitTests COMMAND unit_tests)