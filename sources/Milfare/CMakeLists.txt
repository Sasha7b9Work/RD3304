# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Milfare)

#set(CMAKE_UNITY_BUILD OFF)

message("------------------------------------------------------------------------ ${PROJECT_NAME}")

include_directories(
    ../common/_VS_KEIL/CMSIS
    ../common/_VS_KEIL/std_includes
    src/Hardware/HAL/GD
    ../common/GD32E23x_Firmware_Library/Firmware/CMSIS/GD/GD32E23x/Include
    ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include
    src
)

add_definitions(-DGD32E230 -DTYPE_BOARD_799)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(
    /wd4201    # nonstandard extension used : nameless struct/union
    /wd4255
    /wd4514    # 'function' : unreferenced inline function has been removed
    /wd4626    # assignment operator was implicitly defined as deleted
    /wd4710
    /wd4820    # 'xxx': '4' bytes padding added after data member 'yyy'
    /wd5027    # move assignment operator was implicitly defined as deleted
    /wd5045    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    /wd5220    # a non-static data member with a volatile qualified type no longer implies
    /wd5264

    /wd4365
    /wd4100
    /wd4244
    /wd4189
    /wd4242
)


file(GLOB GD32E23x                      ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include/*.*
                                        ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Source/*.*)
file(GLOB DOC                           ../doc/*.*)
file(GLOB SRC                           src/*.*)
file(GLOB DEVICE                        src/Device/*.*)
file(GLOB HARDWARE                      src/Hardware/*.*)
file(GLOB HARDWARE_CMSIS                src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL                  src/Hardware/HAL/*.*
                                        src/Hardware/HAL/GD/*.*)
file(GLOB MFRC630                       src/MFRC630/*.*)
file(GLOB MODULES_CLRC66303HN           src/Modules/CLRC66303HN/*.*)
file(GLOB MODULES_INDICATOR             src/Modules/Indicator/*.*)
file(GLOB MODULES_INDICATOR_WS2812B     src/Modules/Indicator/WS2812B/*.*)
file(GLOB MODULES_LIS2DH12              src/Modules/LIS2DH12/*.*)
file(GLOB MODULES_MEMORY                src/Modules/Memory/*.*)
file(GLOB MODULES_PLAYER                src/Modules/Player/*.*
                                        src/Modules/Player/GD/*.*)
file(GLOB MODULES_SENSOR                src/Modules/Sensor/*.*)
file(GLOB NODES                         src/Nodes/*.*)
file(GLOB NODES_OSDP                    src/Nodes/OSDP/*.*)
file(GLOB READER                        src/Reader/*.*)
file(GLOB READER_TASK                   src/Reader/Task/*.*)
file(GLOB SETTINGS                      src/Settings/*.*)
file(GLOB UTILS                         src/Utils/*.*)

source_group(GD32E23x                   FILES ${GD32E23x})
source_group(doc                        FILES ${DOC})
source_group(src                        FILES ${SRC})
source_group(Device                     FILES ${DEVICE})
source_group(Hardware                   FILES ${HARDWARE})
source_group(Hardware/CMSIS             FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL               FILES ${HARDWARE_HAL})
source_group(MFRC630                    FILES ${MFRC630})
source_group(Modules/CLRC66303HN        FILES ${MODULES_CLRC66303HN})
source_group(Modules/Indicator          FILES ${MODULES_INDICATOR})
source_group(Modules/Indicator/WS2812B  FILES ${MODULES_INDICATOR_WS2812B})
source_group(Modules/LIS2DH12           FILES ${MODULES_LIS2DH12})
source_group(Modules/Memory             FILES ${MODULES_MEMORY})
source_group(Modules/Player             FILES ${MODULES_PLAYER})
source_group(Modules/Sensor             FILES ${MODULES_SENSOR})
source_group(Nodes                      FILES ${NODES})
source_group(Nodes/OSDP                 FILES ${NODES_OSDP})
source_group(Reader                     FILES ${READER})
source_group(Reader/Task                FILES ${READER_TASK})
source_group(Settings                   FILES ${SETTINGS})
source_group(Utils                      FILES ${UTILS})

set_source_files_properties(
    ${GD32E23x} ${HARDWARE_CMSIS}
    src/Hardware/CDC/usbd_desc.c
    src/Hardware/CDC/usb_device.c
    src/Hardware/CDC/usbd_conf.c
    PROPERTIES HEADER_FILE_ONLY TRUE
)

add_library(${PROJECT_NAME}
    ${GD32E23x}
    ${DOC}
    ${SRC}
    ${DEVICE}
    ${HARDWARE}
    ${HARDWARE_CMSIS}
    ${HARDWARE_HAL}
    ${MFRC630}
    ${MODULES_CLRC66303HN}
    ${MODULES_INDICATOR}
    ${MODULES_INDICATOR_WS2812B}
    ${MODULES_LIS2DH12}
    ${MODULES_MEMORY}
    ${MODULES_PLAYER}
    ${MODULES_SENSOR}
    ${NODES}
    ${NODES_OSDP}
    ${READER}
    ${READER_TASK}
    ${SETTINGS}
    ${UTILS}
)
