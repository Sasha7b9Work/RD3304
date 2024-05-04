# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
project (8-Loader799)

set(CMAKE_UNITY_BUILD OFF)

include_directories(
    src
    src/Hardware/HAL
    ../common/_VS_KEIL/CMSIS
    ../common/GD32E23x_Firmware_Library/Firmware/CMSIS/GD/GD32E23x/Include
    ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include
)

add_definitions(-DSTM32F103xB -DPLOADER)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(
    /wd4002    # too many arguments for function-like macro invocation ###
    /wd4365
    /wd4820    # 'N' bytes padding added after data member
    /wd5045
    /wd5220    # a non-static data member with a volatile qualified type no longer implies
)

file(GLOB GD32E23x          ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include/*.*
                            ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Source/*.*)
file(GLOB SRC               src/*.*)
file(GLOB HARDWARE          src/Hardware/*.*)
file(GLOB HARDWARE_HAL      src/Hardware/HAL/*.*)

set_source_files_properties(${GD32E23x} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(GD32E23x       FILES ${GD32E23x})
source_group(src            FILES ${SRC})
source_group(Hardware       FILES ${HARDWARE})
source_group(Hardware/HAL   FILES ${HARDWARE_HAL})

add_library(
    ${PROJECT_NAME}
    ${GD32E23x}
    ${SRC}
    ${HARDWARE}
    ${HARDWARE_HAL}
)
