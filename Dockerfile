FROM ubuntu:20.04 as generic-cmake-env

LABEL maintainer="BCadet <https://github.com/BCadet>"

RUN apt-get update && \
    DEBIAN_FRONTEND=noninterractive apt-get install -y \
    make \
    cmake \
    gosu

# setup entrypoint
ADD https://gist.githubusercontent.com/BCadet/372702916a20b141cb78ea889e3dae59/raw/73822ba555bfbd75ab7c09c90d463585535e5a0e/container-entrypoint /container-entrypoint
RUN chmod +x /container-entrypoint
ENTRYPOINT [ "/container-entrypoint" ]

SHELL [ "/bin/bash", "-c" ]

# create user
RUN useradd --create-home builder

FROM ubuntu:20.04 as openocd-build-stage

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    git \
    make \
    libusb-1.0-0-dev \
    libtool \
    pkg-config \
    autoconf \
    automake \
    texinfo \
    libhidapi-dev

RUN git clone https://github.com/openocd-org/openocd.git -b v0.12.0  && \
    cd openocd && \
    ./bootstrap && \
    ./configure && \
    make && \
    make install

FROM ubuntu:20.04 as picotool-build-stage

ARG PICOTOOL_VERSION=1.1.1
ARG PICOTOOL_SDK_VERSION=1.5.0

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    git \
    build-essential \
    cmake \
    libusb-1.0-0-dev \
    pkg-config

RUN git clone https://github.com/raspberrypi/pico-sdk.git -b ${PICOTOOL_SDK_VERSION}

RUN git clone https://github.com/raspberrypi/picotool.git -b ${PICOTOOL_VERSION} && \
    mkdir build && \
    cd build && \
    PICO_SDK_PATH=/pico-sdk cmake /picotool && \
    make && \
    make install

FROM generic-cmake-env

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    git \
    libusb-1.0-0-dev \
    texinfo \
    libhidapi-dev\
    libncurses5 \
    vim \
    build-essential \
    minicom \
    cu \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    python3

# add openocd to the docker image
COPY --from=openocd-build-stage /usr/local/bin/openocd /usr/local/bin/openocd
COPY --from=openocd-build-stage /usr/local/share/openocd /usr/local/share/openocd

RUN adduser builder plugdev
RUN adduser builder dialout

COPY --from=picotool-build-stage /usr/local/bin /usr/local/bin