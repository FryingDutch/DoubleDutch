# base image
#!/bin/bash

FROM ubuntu:latest AS base

# install cmake, gcc, g++, boost, and git
FROM base AS builder
RUN apt-get update &&\
    apt-get install -yq cmake gcc g++ &&\
    apt-get install -yq libcurl4-openssl-dev &&\
    apt-get install -yq libjsoncpp-dev &&\
    apt-get install -yq libboost-all-dev &&\
    apt-get install -yq libssl-dev &&\
    apt-get install -yq git &&\
# make a directory we will place DD in
    mkdir DoubleDutch
WORKDIR /DoubleDutch

# get crow's include/ dir
RUN git clone --branch v0.3 https://github.com/CrowCpp/crow &&\
    cp -r crow/include include &&\
# make a directory we'll use to build
    mkdir build

# copy all of the source files to the image
COPY ./ ./

# build
WORKDIR /DoubleDutch/build
RUN cmake .. &&\
    make

FROM base AS finalimage
COPY --from=builder /DoubleDutch/SSL/ /DoubleDutch/SSL/
COPY --from=builder /DoubleDutch/config.txt /DoubleDutch/config.txt
COPY --from=builder /DoubleDutch/build/src/server /DoubleDutch/build/ 
WORKDIR /DoubleDutch/build/

FROM finalimage AS dev
CMD [ "/bin/bash" ]

FROM finalimage AS production
ENTRYPOINT ["./server"]