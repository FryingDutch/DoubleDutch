# base image
#!/bin/bash

FROM ubuntu:latest AS base

# install cmake, gcc, g++, boost, and git
FROM base AS dependencies
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
RUN git clone --branch 1.6.2 https://github.com/libcpr/cpr &&\
    cd cpr && mkdir build && cd build &&\
    cmake .. &&\
    make &&\
    make install

# get crow's include/ dir
RUN git clone --branch v0.3 https://github.com/CrowCpp/crow &&\
    cp -r crow/include include &&\
# make a directory we'll use to build
    mkdir build

# copy all of the source files to the image
COPY ./ ./

# build
FROM dependencies AS builder
WORKDIR /DoubleDutch/build
RUN cmake .. &&\
    make

# create a minimalized final image
FROM base AS finalimage
COPY --from=dependencies /DoubleDutch/cpr/build/lib/. /usr/lib/x86_64-linux-gnu/libsqlite3.so.0 /usr/lib/x86_64-linux-gnu/libhx509.so.5 /usr/lib/x86_64-linux-gnu/libheimbase.so.1 /usr/lib/x86_64-linux-gnu/libwind.so.0 /usr/lib/x86_64-linux-gnu/libroken.so.18 /usr/lib/x86_64-linux-gnu/libhcrypto.so.4 /usr/lib/x86_64-linux-gnu/libasn1.so.8 /usr/lib/x86_64-linux-gnu/libkrb5.so.26 /usr/lib/x86_64-linux-gnu/libheimntlm.so.0 /usr/lib/x86_64-linux-gnu/libkeyutils.so.1 /usr/lib/x86_64-linux-gnu/libbrotlicommon.so.1 /usr/lib/x86_64-linux-gnu/libgssapi.so.3 /usr/lib/x86_64-linux-gnu/libkrb5support.so.0 /usr/lib/x86_64-linux-gnu/libsasl2.so.2 /usr/lib/x86_64-linux-gnu/libkrb5support.so.0 /usr/lib/x86_64-linux-gnu/libk5crypto.so.3 /usr/lib/x86_64-linux-gnu/libkrb5.so.3 /usr/lib/x86_64-linux-gnu/libbrotlidec.so.1 /usr/lib/x86_64-linux-gnu/liblber-2.4.so.2 /usr/lib/x86_64-linux-gnu/libldap_r-2.4.so.2 /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so.2 /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1 /usr/lib/x86_64-linux-gnu/libpsl.so.5 /usr/lib/x86_64-linux-gnu/libssl.so.1.1 /usr/lib/x86_64-linux-gnu/libssh.so.4 /usr/lib/x86_64-linux-gnu/librtmp.so.1 /usr/lib/x86_64-linux-gnu/libnghttp2.so.14 /usr/lib/x86_64-linux-gnu/libcurl.so.4 /usr/local/lib/
COPY --from=builder /DoubleDutch/config.txt /DoubleDutch/build/src/server /
RUN ldconfig

# Run tests in a Python image based on ubuntu.
FROM fnndsc/ubuntu-python3:ubuntu20.04-python3.8.10 as test
COPY --from=finalimage /usr/local/lib/. /usr/local/lib/
COPY --from=finalimage /server /config.txt /
RUN ldconfig

WORKDIR /DoubleDutch/build
RUN pip install requests pytest
COPY tests/test_server.py test_server.py
RUN pytest
RUN echo "tests completed" >> /test_results.log

# Run
FROM finalimage AS production
COPY --from=test /test_results.log /test_results.log
ENTRYPOINT ["/server"]
