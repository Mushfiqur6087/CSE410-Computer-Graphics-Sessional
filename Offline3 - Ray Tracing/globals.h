#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "src/Object/Object.h"
#include "src/PointLight/PointLight.h"
#include "src/SpotLight/SpotLight.h"

// External declarations for global vectors
extern std::vector<Object*> objects;
extern std::vector<PointLight> pointLights;
extern std::vector<SpotLight> spotLights;

// External declarations for scene configuration
extern int recursionLevel;
extern int imageWidth;

// Function declarations
void loadData();
void printObjectDetails(Object* obj, int index);
void printPointLightDetails(const PointLight& light, int index);
void printSpotLightDetails(const SpotLight& light, int index);
void printSceneDetails();
void drawScene();

#endif // GLOBALS_H
