# Use the official Ubuntu 20.04 image as a base
FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=UTC

##########################################
#           INSTALL COMMON LIB           #
##########################################

    RUN apt-get update && \
        apt-get install -y \
        build-essential \
        pkg-config \
        mesa-utils \
        git \
        make \
        wget \
        cmake \
        ffmpeg \
        python3 \
        python3-pip \
        --fix-missing \
        && rm -rf /var/lib/apt/lists/*


##########################################
#           INSTALL GSTREAMER            #
##########################################

    # Update package lists and install necessary packages
    RUN apt-get update && \
        apt-get install -y \
        gstreamer-1.0 \
        gstreamer1.0-dev \
        libgstreamer1.0-0 \
        gstreamer1.0-plugins-base \
        gstreamer1.0-plugins-good \
        gstreamer1.0-plugins-bad \
        gstreamer1.0-plugins-ugly \
        gstreamer1.0-libav \
        gstreamer1.0-tools \
        gstreamer1.0-x \
        gstreamer1.0-alsa \
        gstreamer1.0-gl \
        gstreamer1.0-gtk3 \
        gstreamer1.0-qt5 \
        gstreamer1.0-pulseaudio \
        gir1.2-gstreamer-1.0 \
        gstreamer1.0-python3-plugin-loader \
        libgstreamer1.0-dev \
        libgstreamer-plugins-base1.0-dev \
        libgstreamer-plugins-good1.0-dev \
        libgstreamer-plugins-bad1.0-dev \
        libgstrtspserver-1.0-dev \
        libglib2.0-dev \
        libglibmm-2.4-dev \
        libboost-all-dev \
        && apt-get clean \
        && rm -rf /var/lib/apt/lists/*

##########################################
#            SETUP WORKSPACE             #
##########################################

    WORKDIR /gstreamer-plugins