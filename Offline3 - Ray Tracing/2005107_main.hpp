#pragma once

#include <iostream>
#include <vector>
using namespace std;

#include "header/Vector3D/2005107_Vector3D.h"
#include "header/Color/2005107_Color.h"
#include "header/Coefficients/2005107_Coefficients.h"
#include "header/Ray/2005107_Ray.h"
#include "header/Object/2005107_Object.h"
#include "header/Floor/2005107_Floor.h"
#include "header/Sphere/2005107_Sphere.h"
#include "header/Triangle/2005107_Triangle.h"
#include "header/General/2005107_General.h"
#include "header/PointLight/2005107_PointLight.h"
#include "header/SpotLight/2005107_SpotLight.h"

extern double epsilon;
extern double recursionLevel;
extern double fieldOfViewY, zNear, zFar;
extern vector<Object *> objects;
extern vector<PointLight *> pointLights;
extern vector<SpotLight *> spotLights;

extern double cameraMovementSpeed;
extern double cameraRotationSpeed;

extern Vector3D initialCameraPosition;
extern Vector3D initialCameraUp;
extern Vector3D initialCameraLook;
