#!/bin/bash

# ===================================================
#    ROP-Lang Full Compile & Build Script
#    Compiles the entire language into a monolithic
#    executable using CMake, Clang, and LLVM
# ===================================================

set -e

APP_NAME="roplang"
BUILD_DIR="build"
SRC_DIR="src"
INSTALL_DIR="bin"
CMAKE_FILE="CMakeLists.txt"
LLVM_VERSION="17"  # Update as needed
USE_LLVM=true

# 1. Clean previous build artifacts
echo "🧹 Cleaning previous builds..."
rm -rf $BUILD_DIR $INSTALL_DIR
mkdir -p $BUILD_DIR $INSTALL_DIR

# 2. Create full CMakeLists.txt if not already present
echo "🛠️  Preparing CMakeLists..."
if [ ! -f $CMAKE_FILE ]; then
  echo "CMakeLists.txt not found!"
  exit 1
fi

# 3. Configure the build with CMake
echo "🔧 Configuring CMake..."
cd $BUILD_DIR

cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DLLVM_DIR=$(llvm-config --cmakedir) \
         -DROPLANG_USE_LLVM=$USE_LLVM

# 4. Build the application using all cores
echo "⚙️  Building the application..."
cmake --build . --target $APP_NAME -- -j$(nproc)

# 5. Copy the final binary to the install directory
echo "📦 Packaging executable..."
cp $APP_NAME ../$INSTALL_DIR/$APP_NAME

cd ..

# 6. Summary
echo "✅ ROP-Lang built successfully!"
echo "📁 Output: $INSTALL_DIR/$APP_NAME"
