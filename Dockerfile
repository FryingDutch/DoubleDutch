# base image
#!/bin/bash

FROM ubuntu:latest AS base

# install cmake, gcc, g++, boost, and git
FROM base as builder
RUN apt-get update
RUN apt-get install -yq cmake gcc g++
RUN apt-get install -yq libcurl4-openssl-dev
RUN apt-get install -yq libjsoncpp-dev
RUN apt-get install -yq libboost-all-dev
RUN apt-get install -yq libssl-dev
RUN apt-get install -yq git

# make a directory we will place DD in
RUN mkdir DoubleDutch
WORKDIR /DoubleDutch

# get crow's include/ dir
RUN git clone --branch v0.3 https://github.com/CrowCpp/crow
RUN cp -r crow/include include

# make a directory we'll use to build
RUN mkdir build

# copy all of the source files to the image
COPY ./ ./

# build
WORKDIR /DoubleDutch/build
RUN cmake ..
RUN make  

FROM builder as builddev
CMD ["/bin/bash"]

FROM ubuntu:latest as scratchtest
# run
COPY --from=builder /DoubleDutch /DoubleDutch
WORKDIR /DoubleDutch/build
CMD ["/bin/bash"]

FROM scratchtest
ENTRYPOINT ["src/server"]
