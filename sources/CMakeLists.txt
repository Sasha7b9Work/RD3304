cmake_minimum_required(VERSION 3.14)

project (RD3304)

set (TARGET_NAME )

set(CMAKE_UNITY_BUILD ON)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_subdirectory(Reader/vs_keil771)
add_subdirectory(Reader/vs_keil777)
add_subdirectory(Reader/vs_keil799)
add_subdirectory(Loader-799)
