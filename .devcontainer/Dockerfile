FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends libopengl-dev libglx-dev mesa-common-dev \
	build-essential git make \
	pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
	libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
	libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
	libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
	libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev \
	libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev
# libgl-dev
