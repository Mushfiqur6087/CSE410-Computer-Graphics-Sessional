#!/bin/zsh

# Compile with correct source file paths and include path

g++ -Isrc main.cpp \
    src/Vector3/Vector3.cpp \
    src/Camera/Camera.cpp \
    src/Color/Color.cpp \
    src/Coefficients/Coefficients.cpp \
    src/Object/Object.cpp \
    src/Sphere/Sphere.cpp \
    src/Triangle/Triangle.cpp \
    src/GeneralQuadratic/GeneralQuadratic.cpp \
    src/PointLight/PointLight.cpp \
    src/SpotLight/SpotLight.cpp \
    -o main -lGL -lGLU -lglut

# If compilation succeeds, run the program
if [[ $? -eq 0 ]]; then
    ./main
    rm -f ./main
else
    echo "Compilation failed."
fi