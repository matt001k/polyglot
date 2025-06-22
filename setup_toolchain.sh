#!/bin/bash

# Exit on error
set -e

if [ -z "$1" ]; then
    echo "No project name supplied please enter project name"
    exit 1
else
    PROJECT_NAME="$1"
fi

# Get system architecture
arch=$(uname -m)

# Create directory
ANEMONE_DIR="$HOME/anemone"
INSTALL_DIR="$ANEMONE_DIR/envs/$PROJECT_NAME"
mkdir -p "$INSTALL_DIR"

# Update and install dependencies
echo "Installing dependencies"
sudo apt update && sudo apt upgrade -y
sudo apt install -y build-essential software-properties-common wget unzip tar libusb-1.0-0-dev git

# Install ARM GCC Toolchain
echo "\n\nInstalling ARM GCC Toolchain...\n\n"
ARM_GCC_VERS="14.2.Rel1"
ARM_GCC_VERS_LOWER="${ARM_GCC_VERS,,}"
ARM_GCC_ARCHIVE="arm-gnu-toolchain-$ARM_GCC_VERS_LOWER-$arch-arm-none-eabi.tar.xz"
ARM_GCC_ARCHIVE_DIR="$INSTALL_DIR/$ARM_GCC_ARCHIVE"
ARM_GCC_DIR="$INSTALL_DIR/arm-none-eabi-gcc"
ARM_GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu/$ARM_GCC_VERS/binrel/$ARM_GCC_ARCHIVE"
wget -c -P "$INSTALL_DIR" "$ARM_GCC_URL"
mkdir -p "$ARM_GCC_DIR"
tar -xvf "$ARM_GCC_ARCHIVE_DIR" -C "$ARM_GCC_DIR" --strip-components 1
rm -rf "$ARM_GCC_ARCHIVE_DIR"

# Install CMake
echo "\n\nInstalling CMake...\n\n"
CMAKE_VERSION="4.0.3"
CMAKE_FOLDER="cmake-$CMAKE_VERSION-linux-$arch"
CMAKE_ARCHIVE="$CMAKE_FOLDER.tar.gz"
CMAKE_ARCHIVE_DIR="$INSTALL_DIR/$CMAKE_ARCHIVE"
CMAKE_DIR="$INSTALL_DIR/cmake"
CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_ARCHIVE"
wget -c -P "$INSTALL_DIR" "$CMAKE_URL"
mkdir -p "$CMAKE_DIR"
tar -xzf "$CMAKE_ARCHIVE_DIR" -C "$CMAKE_DIR" --strip-components 1
rm -rf "$CMAKE_ARCHIVE_DIR"

# Install Python
PYTHON_DIR="$INSTALL_DIR/python"
PYTHON3_EXE="$PYTHON_DIR/bin/python3"
PYTHON_EXE="$PYTHON_DIR/bin/python"

install_packages() {
    "$PYTHON_EXE" -m ensurepip --upgrade
    "$PYTHON_EXE" -m pip install --upgrade pip
    for arg in "$@"; do
        "$PYTHON_EXE" -m pip install "$arg"
    done
}

echo "\n\nInstalling Python...\n\n"
PYTHON_VERSION=3.12.2
PYTHON_ARCHIVE="Python-$PYTHON_VERSION.tgz"
PYTHON_ARCHIVE_DIR="$INSTALL_DIR/$PYTHON_ARCHIVE"
PYTHON_URL="https://www.python.org/ftp/python/$PYTHON_VERSION/$PYTHON_ARCHIVE"
PYTHON_SETUP_DIR="$INSTALL_DIR/Python-$PYTHON_VERSION"
wget -c -P "$INSTALL_DIR" "$PYTHON_URL"
tar -xf "$PYTHON_ARCHIVE_DIR" -C "$INSTALL_DIR"
cd "$PYTHON_SETUP_DIR"
./configure --prefix="$PYTHON_DIR" --enable-optimizations
make -j$(nproc)
make install
cd .. && rm -rf "$PYTHON_SETUP_DIR" && rm "$PYTHON_ARCHIVE_DIR"
ln -sf "$PYTHON3_EXE" "$PYTHON_EXE"
install_packages "cryptography==45.0.4"


# Install Make
MAKE_VERSION=4.4.1

exit 0
