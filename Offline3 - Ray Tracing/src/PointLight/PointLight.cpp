#include "PointLight.h"

// Constructor
PointLight::PointLight(Vector3 position, Color lightColor) {
    light_pos = position;
    color = lightColor;
}

// Default constructor
PointLight::PointLight() {
    light_pos = Vector3(0, 0, 0);
    color = Color(1.0, 1.0, 1.0);  // Default white light
}
