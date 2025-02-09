cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0069 NEW) 
cmake_policy(SET CMP0048 NEW)
project(CachePipes LANGUAGES CXX)

if(NOT (DEFINED ENV{CPM_SOURCE_CACHE} OR DEFINED CPM_SOURCE_CACHE))
  set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_BINARY_DIR}/cmake CACHE PATH "Location of CPM cache")
endif()

set(CPM_USE_LOCAL_PACKAGES ON)

if(NOT EXISTS ${CPM_SOURCE_CACHE}/CPM.cmake)
  file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.5/CPM.cmake
    ${CPM_SOURCE_CACHE}/CPM.cmake
    EXPECTED_HASH SHA256=c46b876ae3b9f994b4f05a4c15553e0485636862064f1fcc9d8b4f832086bc5d
  )
endif()
message(STATUS "Including CPM from: ${CPM_SOURCE_CACHE}/CPM.cmake")
include(${CPM_SOURCE_CACHE}/CPM.cmake)
message(STATUS "CPM included")

CPMAddPackage(
    NAME rapidhash
    GITHUB_REPOSITORY Nicoshev/rapidhash
    GIT_TAG rapidhash_v1.0
)

if(rapidhash_ADDED)
    add_library(rapidhash INTERFACE)
    target_include_directories(rapidhash INTERFACE ${rapidhash_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME Swl.Variant
    GITHUB_REPOSITORY adihodos/swl-variant
    GIT_TAG main
    OPTIONS
        "CMAKE_PROJECT_NAME not_swl_variant"
)

find_package(Boost 1.87.0 COMPONENTS hana unordered QUIET)

if(NOT Boost_FOUND)
    CPMAddPackage(
        NAME Boost.Unordered
        GITHUB_REPOSITORY MikePopoloski/boost_unordered
        GIT_TAG v1.87.1
        OPTIONS
            "BUS_SYSTEM_INCLUDE ON"
    )

    CPMAddPackage(
        NAME Boost.Hana
        GITHUB_REPOSITORY boostorg/hana
        GIT_TAG v1.7.0
        OPTIONS
            "BOOST_HANA_ENABLE_STRING_UDL OFF"
            "BOOST_HANA_ENABLE_EXCEPTIONS OFF"
    )
endif()

target_compile_options(hana INTERFACE 
    $<$<CXX_COMPILER_ID:Clang>:
        -Wno-unused-value
    >
)
set_target_properties(hana PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES 
    $<TARGET_PROPERTY:hana,INTERFACE_INCLUDE_DIRECTORIES>
)

CPMAddPackage(
    NAME SFML
    GITHUB_REPOSITORY SFML/SFML
    GIT_TAG 3.0.0
    OPTIONS
        "SFML_BUILD_EXAMPLES OFF"
        "SFML_BUILD_TEST_SUITE OFF"
        "SFML_BUILD_DOC OFF"
        "SFML_SHARED_LIBS ON"
        "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        $<$<PLATFORM_ID:Windows>:"SFML_GENERATE_PDB ON">
        $<$<AND:$<PLATFORM_ID:Darwin>,$<CONFIG:Release>>:
            "CMAKE_INSTALL_PREFIX=/Library/Frameworks"
            "SFML_BUILD_FRAMEWORKS ON"
        >
        $<$<AND:$<PLATFORM_ID:Darwin>,$<CONFIG:Debug>>:
            "SFML_BUILD_FRAMEWORKS OFF"
        >
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_compile_options(${PROJECT_NAME} PRIVATE 
    $<$<CXX_COMPILER_ID:Clang>:
        -Wall
        -Wextra
    >
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/dev1
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        hana
        boost_unordered
        swl-variant
        rapidhash
        sfml-graphics 
        sfml-audio
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    EXECUTABLE_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}>
    $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)