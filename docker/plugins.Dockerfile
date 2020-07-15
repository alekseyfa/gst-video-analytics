ARG DLDT_IMAGE
ARG GSTREAMER_IMAGE

FROM ${DLDT_IMAGE} AS dldt-build
FROM ${GSTREAMER_IMAGE} AS gst-build

FROM ubuntu:18.04
LABEL Description="This is the base image for GSTREAMER & DLDT Ubuntu 18.04 LTS"
LABEL Vendor="Intel Corporation"
WORKDIR /root

# Prerequisites
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y -q --no-install-recommends \
    libusb-1.0-0-dev libboost-all-dev libgtk2.0-dev lsb-release python-yaml \
    \
    clinfo libboost-all-dev libjson-c-dev \
    build-essential cmake ocl-icd-opencl-dev wget gcovr vim git gdb ca-certificates libssl-dev uuid-dev \
    libgirepository1.0-dev \
    python3-dev python3-wheel python3-pip python3-setuptools python-gi-dev python-yaml \
    \
    libglib2.0-dev libgmp-dev libgsl-dev gobject-introspection libcap-dev libcap2-bin gettext \
    \
    libx11-dev iso-codes libegl1-mesa-dev libgles2-mesa-dev libgl-dev gudev-1.0 libtheora-dev libcdparanoia-dev libpango1.0-dev libgbm-dev libasound2-dev libjpeg-dev \
    libvisual-0.4-dev libxv-dev libopus-dev libgraphene-1.0-dev libvorbis-dev \
    \
    libbz2-dev libv4l-dev libaa1-dev libflac-dev libgdk-pixbuf2.0-dev libmp3lame-dev libcaca-dev libdv4-dev libmpg123-dev libraw1394-dev libavc1394-dev libiec61883-dev \
    libpulse-dev libsoup2.4-dev libspeex-dev libtag-extras-dev libtwolame-dev libwavpack-dev \
    \
    libbluetooth-dev libusb-1.0.0-dev libass-dev libbs2b-dev libchromaprint-dev liblcms2-dev libssh2-1-dev libdc1394-22-dev libdirectfb-dev libssh-dev libdca-dev \
    libfaac-dev libfdk-aac-dev flite1-dev libfluidsynth-dev libgme-dev libgsm1-dev nettle-dev libkate-dev liblrdf0-dev libde265-dev libmjpegtools-dev libmms-dev \
    libmodplug-dev libmpcdec-dev libneon27-dev libopenal-dev libopenexr-dev libopenjp2-7-dev libopenmpt-dev libopenni2-dev libdvdnav-dev librtmp-dev librsvg2-dev \
    libsbc-dev libsndfile1-dev libsoundtouch-dev libspandsp-dev libsrtp2-dev libzvbi-dev libvo-aacenc-dev libvo-amrwbenc-dev libwebrtc-audio-processing-dev libwebp-dev \
    libwildmidi-dev libzbar-dev libnice-dev libxkbcommon-dev \
    \
    libmpeg2-4-dev libopencore-amrnb-dev libopencore-amrwb-dev liba52-0.7.4-dev \
    \
    libva-dev libxrandr-dev libudev-dev \
    \
    && rm -rf /var/lib/apt/lists/* \
    && python3.6 -m pip install numpy opencv-python pytest

# Install
COPY --from=dldt-build /opt/intel /opt/intel
COPY --from=gst-build /home/build /

# RUN cd /opt/intel/openvino/install_dependencies/ && \
#     ./install_NEO_OCL_driver.sh

ARG PREFIX=/
ARG LIBDIR=lib/
ARG LIBEXECDIR=bin/
ARG INCLUDEDIR=include/

ENV GSTREAMER_LIB_DIR=${PREFIX}/${LIBDIR}
ENV GST_PLUGIN_SCANNER=${PREFIX}/${LIBEXECDIR}/gstreamer-1.0/gst-plugin-scanner
ENV C_INCLUDE_PATH=${PREFIX}/${INCLUDEDIR}:${C_INCLUDE_PATH}
ENV CPLUS_INCLUDE_PATH=${PREFIX}/${INCLUDEDIR}:${CPLUS_INCLUDE_PATH}

RUN echo "\
    /usr/local/lib\n\
    ${GSTREAMER_LIB_DIR}/gstreamer-1.0\n\
    /opt/intel/mediasdk/lib64\n\
    /opt/intel/openvino/deployment_tools/inference_engine/lib/intel64\n\
    /opt/intel/openvino/deployment_tools/inference_engine/external/tbb/lib\n\
    /opt/intel/openvino/deployment_tools/ngraph/lib\n\
    /opt/intel/openvino/deployment_tools/inference_engine/external/hddl/lib\n\
    /opt/intel/openvino/opencv/lib/" > /etc/ld.so.conf.d/opencv-dldt-gst.conf && ldconfig

ENV GI_TYPELIB_PATH=${GSTREAMER_LIB_DIR}/girepository-1.0
ENV PYTHONPATH=${GSTREAMER_LIB_DIR}/python3.6/site-packages:${PYTHONPATH}

ENV PKG_CONFIG_PATH=${GSTREAMER_LIB_DIR}/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:/opt/intel/mediasdk/lib64/pkgconfig:${PKG_CONFIG_PATH}
ENV InferenceEngine_DIR=/opt/intel/openvino/deployment_tools/inference_engine/share
ENV OpenCV_DIR=/opt/intel/openvino/opencv/cmake
ENV LIBRARY_PATH=/opt/intel/mediasdk/lib64:/usr/lib:${GSTREAMER_LIB_DIR}:${LIBRARY_PATH}
ENV PATH=/usr/bin:${PREFIX}/${LIBEXECDIR}:/opt/intel/mediasdk/bin:${PATH}

ENV LIBVA_DRIVERS_PATH=/opt/intel/mediasdk/lib64
ENV LIBVA_DRIVER_NAME=iHD
ENV GST_VAAPI_ALL_DRIVERS=1
ENV DISPLAY=:0.0
ENV LD_LIBRARY_PATH=/usr/lib/gst-video-analytics:${LD_LIBRARY_PATH}
ENV HDDL_INSTALL_DIR=/opt/intel/openvino/deployment_tools/inference_engine/external/hddl

ARG GIT_INFO
ARG SOURCE_REV

COPY . gst-video-analytics
ARG ENABLE_PAHO_INSTALLATION=false
ARG ENABLE_RDKAFKA_INSTALLATION=false
ARG BUILD_TYPE=Release
ARG EXTERNAL_GVA_BUILD_FLAGS

RUN mkdir -p gst-video-analytics/build \
    && cd gst-video-analytics/build \
    && cmake \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DVERSION_PATCH=${SOURCE_REV} \
        -DGIT_INFO=${GIT_INFO} \
        -DENABLE_PAHO_INSTALLATION=${ENABLE_PAHO_INSTALLATION} \
        -DENABLE_RDKAFKA_INSTALLATION=${ENABLE_RDKAFKA_INSTALLATION} \
        -DHAVE_VAAPI=ON \
        -DENABLE_VAS_TRACKER=ON \
        ${EXTERNAL_GVA_BUILD_FLAGS} \
        .. \
    && make -j $(nproc) \
    && make install \
    && echo "/usr/lib/gst-video-analytics" >> /etc/ld.so.conf.d/opencv-dldt-gst.conf && ldconfig

ENV GST_PLUGIN_PATH=/usr/lib/gst-video-analytics/:/root/gst-video-analytics/
ENV PYTHONPATH=/root/gst-video-analytics/python:$PYTHONPATH
