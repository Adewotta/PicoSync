FROM ubuntu:latest
RUN apt-get update && apt-get -y upgrade && \
    # install dependencies
    DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y cmake gcc-arm-none-eabi git libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib wget && \

    # install pico SDK
    git clone https://github.com/raspberrypi/pico-sdk.git && \
    mv pico-sdk /usr/local/bin/pico-sdk && \

    # clean up
    rm -rf /tmp/*
ENV PICO_SDK_PATH=/usr/local/bin/pico-sdk