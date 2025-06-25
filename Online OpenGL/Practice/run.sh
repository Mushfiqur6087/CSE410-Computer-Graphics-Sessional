#!/bin/bash

# Exit immediately if a command fails
set -e

# Check if a file name was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <OpenGL_cpp_file.cpp>"
    exit 1
fi

FILE="$1"
EXE="${FILE%.cpp}"

# Check if file exists
if [[ ! -f "$FILE" ]]; then
    echo "Error: File '$FILE' not found!"
    exit 1
fi

echo "Compiling $FILE with OpenGL support..."
g++ "$FILE" -o "$EXE" -lGL -lGLU -lglut

echo "Compilation successful. Running $EXE..."
./"$EXE"

echo "Cleaning up..."
rm -f "$EXE"
echo "Executable $EXE removed."
