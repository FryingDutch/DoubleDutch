# base image
FROM ubuntu:latest

# install cmake, gcc, g++, boost, and git
RUN apt-get update
RUN apt-get install -yq cmake gcc g++ 
RUN apt-get install -yq libboost-all-dev
RUN apt-get install -yq git

# get crow's include/ dir
RUN git clone --branch 0.2 https://github.com/CrowCpp/crow
RUN cp -r crow/include include

# make a directory we'll use to build
RUN mkdir build

# copy all of the source files to the image
COPY ./ /

# build the app
WORKDIR build
RUN cmake ..
RUN make 

# run the app!
ENTRYPOINT ["src/server"]




