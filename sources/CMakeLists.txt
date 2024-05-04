cmake_minimum_required(VERSION 3.14)

project (RD3303)

set (TARGET_NAME )

set(CMAKE_UNITY_BUILD ON)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_subdirectory(1-Device)
add_subdirectory(2-Reader771_1)
add_subdirectory(3-Accelerometer)
add_subdirectory(4-Sound)
add_subdirectory(5-Programmer)
add_subdirectory(5-LoaderSTM)
add_subdirectory(6-Reader/vs_keil771)
add_subdirectory(6-Reader/vs_keil777)
add_subdirectory(6-Reader/vs_keil799)
add_subdirectory(7-Device799)
add_subdirectory(8-Loader799)
add_subdirectory(Sample)
add_subdirectory(CodeGenerator)
add_subdirectory(WaveGenerator)
