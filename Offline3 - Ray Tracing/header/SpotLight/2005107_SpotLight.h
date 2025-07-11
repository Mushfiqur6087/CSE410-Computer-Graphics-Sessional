#pragma once

#include <iostream>
using namespace std;

#include "../Vector3D/2005107_Vector3D.h"
#include "../Color/2005107_Color.h"
#include "../PointLight/2005107_PointLight.h"

class SpotLight
{
    PointLight pointLight;
    Vector3D lightDirection;
    double cutoffAngle;

public:
    SpotLight();
    SpotLight setLightPosition(double x, double y, double z);
    SpotLight setColor(Color color);
    SpotLight setLightDirection(double x, double y, double z);
    SpotLight setCutoffAngle(double cutoffAngle);

    Vector3D getLightPosition() const;
    Color getColor() const;
    Vector3D getLightDirection() const;
    double getCutoffAngle() const;
    void draw();
    friend ostream &operator<<(ostream &out, const SpotLight &s);
};
