cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../tools)
set(FETCHCONTENT_QUIET FALSE)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake)

FetchContent_MakeAvailable(gcc-arm-none-eabi-10.3-2021.10-x86_64-linux)
set(PICO_TOOLCHAIN_PATH ${gcc-arm-none-eabi-10.3-2021.10-x86_64-linux_SOURCE_DIR})

set(PICO_SDK_PATH ${pico-sdk_SOURCE_DIR})
set(PICO_TOOLCHAIN_PATH ${gcc-arm-none-eabi-10.3-2021.10-x86_64-linux_SOURCE_DIR}) #require to help pico_find_compiler
FetchContent_Populate(pico-sdk)

include(${pico-sdk_SOURCE_DIR}/pico_sdk_init.cmake)

# FetchContent_Populate(pico-exemple)

set(CMAKE_BUILD_TYPE Debug)

set(PICO_BOARD "pico_w") # set board target

project (rpi_xmodem)
pico_sdk_init()
# find gdb for debugger (pico_sdk_init forget to set it).
# see build/_deps/pico-sdk-src/cmake/preload/toolchains/pico_arm_gcc.cmake)
pico_find_compiler(PICO_COMPILER_GDB arm-none-eabi-gdb)
set(CMAKE_DEBUGGER ${PICO_COMPILER_GDB} CACHE FILEPATH "path to gdb") #require by launch.json

# FetchContent_MakeAvailable(pico-exemple)

set(PROJECT_SOURCES
    rpi_xmodem/blink_led.c
)

add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})

# pull in common dependencies
target_link_libraries(${CMAKE_PROJECT_NAME} 
    pico_cyw43_arch_none
    pico_stdlib
    pico_multicore
    hardware_adc
    hardware_uart
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

#Enbale output on usb and disable on uart
# pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
# pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)


# add url via pico_set_program_url
# example_auto_set_url(blink)