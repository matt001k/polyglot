#!/bin/bash

if [ -z "$1" ]; then
    echo "No project name supplied please enter project name"
    exit 1
else
    PROJECT_NAME="$1"
fi

if [ -z "$OLD_PATH" ]; then
  export OLD_PATH="$PATH"
fi

# Create directory
ANEMONE_DIR="$HOME/anemone"
INSTALL_DIR="$ANEMONE_DIR/envs/$PROJECT_NAME"
GCC_DIR="$INSTALL_DIR/arm-none-eabi-gcc/bin"
CMAKE_DIR="$INSTALL_DIR/cmake/bin"
PYTHON_DIR="$INSTALL_DIR/python/bin"

# Remove other necessary paths from PATH
clean_path() {
    OLD_IFS=$IFS
    IFS=:
    NEW_PATH=""
    for dir in $PATH; do
        for arg in "$@"; do
            if [[ "$dir" != "$arg" ]]; then
                NEW_PATH="$NEW_PATH:$dir"
            fi
        done
    done
    IFS=$OLD_IFS
    # Remove leading colon
    export PATH="${NEW_PATH#:}"
}

# Clean and set PATH
clean_path "*arm-none-eabi*" "*cmake*" "*python*"

# Add desired toolchain paths
export PATH="$GCC_DIR:$PATH"

# CMake
export PATH="$CMAKE_DIR:$PATH"

# Python
export PATH="$PYTHON_DIR:$PATH"

echo "Embedded toolchain environment activated."
echo "ARM GCC path: $(which arm-none-eabi-gcc)"
echo "ARM GCC version: $(arm-none-eabi-gcc --version | head -n1)"
