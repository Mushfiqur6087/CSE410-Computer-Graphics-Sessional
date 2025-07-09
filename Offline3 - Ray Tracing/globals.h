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

#endif // GLOBALS_H
