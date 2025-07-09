#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "../PointLight/PointLight.h"
#include "../Vector3/Vector3.h"

class SpotLight {
public:
    PointLight point_light;
    Vector3 light_direction;
    double cutoff_angle;
    
    // Constructor
    SpotLight(PointLight pointLight, Vector3 direction, double cutoffAngle);
    
    // Default constructor
    SpotLight();
};

#endif // SPOTLIGHT_H
