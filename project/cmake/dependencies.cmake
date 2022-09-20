FetchContent_Declare(
    gcc-arm-none-eabi-10.3-2021.10-x86_64-linux
    URL https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
)

FetchContent_Declare(
    pico-sdk
    GIT_REPOSITORY https://github.com/raspberrypi/pico-sdk.git
    GIT_TAG master
)

FetchContent_Declare(
    pico-exemple
    GIT_REPOSITORY https://github.com/raspberrypi/pico-examples.git
    GIT_TAG master
)
