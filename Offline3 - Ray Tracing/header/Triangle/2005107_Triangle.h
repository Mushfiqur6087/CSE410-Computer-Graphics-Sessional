#pragma once

#include "../Object/2005107_Object.h"

class Triangle : public Object
{
private:
    Vector3D vertexA, vertexB, vertexC;

public:
    Triangle();
    Triangle(Vector3D a, Vector3D b, Vector3D c);
    
    void draw() override;
    Vector3D computeNormal(Vector3D point) override;
    double intersect(Ray *ray) override;
};
