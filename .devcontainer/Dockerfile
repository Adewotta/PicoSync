FROM ubuntu:latest
RUN apt-get update && apt-get -y upgrade && \
    # install dependencies
    DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y cmake && \

    # clean up
    rm -rf /tmp/*