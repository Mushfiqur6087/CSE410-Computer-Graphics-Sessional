#pragma once

#include "../Object/2005107_Object.h"

class Sphere : public Object
{
public:
    Sphere();
    Sphere(Vector3D center, double radius);
    
    void draw() override;
    Vector3D computeNormal(Vector3D point) override;
    double intersect(Ray *ray) override;
};
