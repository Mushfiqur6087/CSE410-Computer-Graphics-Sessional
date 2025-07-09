#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "../Vector3/Vector3.h"
#include "../Color/Color.h"

class PointLight {
public:
    Vector3 light_pos;
    Color color;
    
    // Constructor
    PointLight(Vector3 position, Color lightColor);
    
    // Default constructor
    PointLight();
};

#endif // POINTLIGHT_H
