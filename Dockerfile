FROM ubuntu:yakkety

RUN apt-get update && \
    apt-get -y install unzip git make gcc-arm-none-eabi

RUN mkdir /build
WORKDIR /build

CMD /bin/bash
