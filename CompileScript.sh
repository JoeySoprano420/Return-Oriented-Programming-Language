#!/bin/bash

# === ROP-Lang IDE C++ Backend Compilation Script ===

# Set environment variables
export PROJECT_DIR="$(pwd)"
export BUILD_DIR="$PROJECT_DIR/build"
export SOURCE_DIR="$PROJECT_DIR/src"
export CMAKE_BUILD_TYPE="Release"
export CMAKE_GENERATOR="Unix Makefiles"

# Specify the paths to required dependencies
export LLVM_DIR="/path/to/llvm"  # Change this to your local LLVM installation path
export BOOST_DIR="/path/to/boost"  # If you're using Boost, change this path
export INOTIFY_DIR="/path/to/inotify"  # Path to the inotify library

# Ensure LLVM is installed and properly set up
if [ ! -d "$LLVM_DIR" ]; then
    echo "[ERROR] LLVM is not found in the specified directory: $LLVM_DIR"
    exit 1
fi

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "[INFO] Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Change to the build directory
cd "$BUILD_DIR" || exit 1

# === Run CMake to configure the build system ===
echo "[INFO] Configuring CMake..."
cmake -G "$CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
    -DLLVM_DIR="$LLVM_DIR" \
    -DBOOST_ROOT="$BOOST_DIR" \
    -DINOTIFY_DIR="$INOTIFY_DIR" \
    "$PROJECT_DIR"

# Check if CMake was successful
if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed. Please check the errors above."
    exit 1
fi

# === Build the project ===
echo "[INFO] Building the project..."
make -j$(nproc)

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed. Please check the errors above."
    exit 1
fi

# === Run tests (optional) ===
echo "[INFO] Running tests (if any)..."
make test

if [ $? -ne 0 ]; then
    echo "[ERROR] Tests failed. Please check the errors above."
    exit 1
fi

# === Run file watching process ===
echo "[INFO] Starting file watcher for live changes..."
inotifywait -m -r -e modify,create,delete "$SOURCE_DIR" |
while read -r directory events filename; do
    echo "[INFO] Detected change in: $directory$filename"
    # Rebuild the project if any source file changes
    make -j$(nproc)
    if [ $? -eq 0 ]; then
        echo "[INFO] Rebuild successful after file change."
    else
        echo "[ERROR] Rebuild failed after file change."
    fi
done
