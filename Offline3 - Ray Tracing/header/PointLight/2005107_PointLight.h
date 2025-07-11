#pragma once

#include <iostream>
using namespace std;

#include "../Vector3D/2005107_Vector3D.h"
#include "../Color/2005107_Color.h"

class PointLight
{
    Vector3D lightPosition;
    Color color;

public:
    PointLight();
    PointLight setLightPosition(double x, double y, double z);
    PointLight setColor(Color color);
    Vector3D getLightPosition() const;
    Color getColor() const;
    void draw();
    friend ostream &operator<<(ostream &out, const PointLight &p);
};
