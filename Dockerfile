FROM ubuntu:yakkety

# Add dependencies
RUN apt-get update && \
    apt-get -y install make gcc-arm-none-eabi

RUN apt-get -y install python3-pip

RUN mkdir /build
WORKDIR /build

CMD /bin/bash
