#! /bin/bash
if [ -z "$2" ]
then
    echo "usage: ./run.sh <input_file_path> <output_file_directory> [texture_file_path]"
    exit 1
fi
input_file_path=$1
output_file_directory=$2
texture_file_path=$3

if [ ! -d "$output_file_directory" ]
then
    mkdir -p $output_file_directory
fi

g++ -std=c++11 header/Camera/2005107_Camera.cpp header/Vector3D/2005107_Vector3D.cpp header/Color/2005107_Color.cpp header/Coefficients/2005107_Coefficients.cpp header/Ray/2005107_Ray.cpp header/Object/2005107_Object.cpp header/Floor/2005107_Floor.cpp header/Sphere/2005107_Sphere.cpp header/Triangle/2005107_Triangle.cpp header/General/2005107_General.cpp header/PointLight/2005107_PointLight.cpp header/SpotLight/2005107_SpotLight.cpp 2005107_main.cpp -o 2005107_main -lGL -lGLU -lglut

if [ -z "$texture_file_path" ]
then
    echo "Running without texture (checkerboard pattern)"
    ./2005107_main $input_file_path $output_file_directory
else
    echo "Running with texture: $texture_file_path"
    ./2005107_main $input_file_path $output_file_directory $texture_file_path
fi

rm 2005107_main