#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" &>/dev/null
}

# Check for and install CMake
if ! command_exists cmake; then
    echo "CMake not found. Installing CMake..."
    brew install cmake
else
    echo "CMake is already installed."
fi

# Check for and install Ninja
if ! command_exists ninja; then
    echo "Ninja not found. Installing Ninja..."
    brew install ninja
else
    echo "Ninja is already installed."
fi

# Check for and install entr
if ! command_exists entr; then
    echo "entr not found. Installing entr..."
    brew install entr
else
    echo "entr is already installed."
fi

# Set up build directory with Ninja if it doesn't exist
if [ ! -d "build" ]; then
    echo "Generating build files with Ninja..."
    cmake -B ./build -G Ninja
else
    echo "Build directory already exists. Skipping CMake generation."
fi

# Start the file watcher and incremental build process
echo "Starting file watcher. Press Ctrl+C to stop."
find src -name '*.cpp' | entr -c cmake --build ./build