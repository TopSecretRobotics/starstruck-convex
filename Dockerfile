FROM ubuntu:yakkety

# Add dependencies
RUN apt-get update && \
    apt-get -y install make gcc-arm-none-eabi

RUN mkdir /build
WORKDIR /build

CMD /bin/bash
