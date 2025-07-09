#include "SpotLight.h"

// Constructor
SpotLight::SpotLight(PointLight pointLight, Vector3 direction, double cutoffAngle) {
    point_light = pointLight;
    light_direction = direction;
    cutoff_angle = cutoffAngle;
}

// Default constructor
SpotLight::SpotLight() {
    point_light = PointLight();
    light_direction = Vector3(0, 0, -1);  // Default pointing down
    cutoff_angle = 45.0;  // Default 45 degree cutoff
}
