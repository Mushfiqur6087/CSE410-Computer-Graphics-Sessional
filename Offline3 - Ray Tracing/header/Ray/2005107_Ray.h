#pragma once

#include <iostream>
#include "../Vector3D/2005107_Vector3D.h"

using namespace std;

class Ray
{
public:
    Vector3D origin, direction;
    Ray();
    Ray(Vector3D originParam, Vector3D directionParam);
    Ray setOrigin(Vector3D originParam);
    Ray setDirection(Vector3D directionParam);
    Vector3D getOrigin() const;
    Vector3D getDirection() const;
    friend ostream &operator<<(ostream &out, const Ray &r);
};