FROM ubuntu:yakkety

RUN apt-get update && \
    apt-get -y install unzip git make gcc-arm-none-eabi

RUN mkdir /build
WORKDIR /build

RUN mkdir /build/jpearman && \
    git clone git://github.com/jpearman/convex.git /build/jpearman/convex && \
    unzip -q /build/jpearman/convex/ChibiOS_2.6.2.zip -d /build && \
    ln -svf /build/jpearman/convex/convex /build/convex

CMD /bin/bash
