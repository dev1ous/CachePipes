cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0069 NEW) 
cmake_policy(SET CMP0048 NEW)
project(CachePipes LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

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
    GIT_TAG rapidhash_v3
)

if(rapidhash_ADDED)
    add_library(rapidhash INTERFACE)
    target_include_directories(rapidhash INTERFACE ${rapidhash_SOURCE_DIR})
endif()

 CPMAddPackage(
    NAME Boost.Unordered
    GITHUB_REPOSITORY MikePopoloski/boost_unordered
    GIT_TAG v1.88.0
    OPTIONS
        "BUS_SYSTEM_INCLUDE ON"
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

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} 
    PUBLIC 
        cxx_std_23
)

target_compile_options(${PROJECT_NAME} 
    PRIVATE 
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
            -Wall
            -Wextra
        >
)

target_sources(${PROJECT_NAME} 
    PRIVATE 
        src/main.cpp
        src/map.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/dev1
)

target_precompile_headers(${PROJECT_NAME} 
    PRIVATE 
        "${CMAKE_SOURCE_DIR}/include/dev1/detail/pch.hpp"
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        boost_unordered
        rapidhash
        sfml-graphics 
        sfml-audio
)

 target_compile_definitions(${PROJECT_NAME}
     PRIVATE
         EXECUTABLE_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# add_custom_command(
#     TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/resources
#     ${CMAKE_BINARY_DIR}/resources
# )

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}>
    $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)