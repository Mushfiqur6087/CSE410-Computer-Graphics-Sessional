#pragma once

#include <iostream>
using namespace std;

#include "../Vector3D/2005107_Vector3D.h"
#include "../Color/2005107_Color.h"
#include "../Coefficients/2005107_Coefficients.h"
#include "../Ray/2005107_Ray.h"

class Object
{
protected:
    Vector3D referencePoint;
    Color color;
    Coefficients materialCoefficients;
    int shine;

public:
    double height, width, length;

    Object();
    Object setColor(Color color);
    Object setShine(int shine);
    Object setCoefficients(Coefficients coefficients);
    Coefficients getCoefficients();
    virtual Color getSurfaceColor(Vector3D point);
    virtual Vector3D computeNormal(Vector3D point);
    virtual void draw();
    virtual double intersect(Ray *ray);
    void phongLighting(Ray *ray, Color *color, int level);
    
    // New helper methods
    void computePointLightContribution(Vector3D intersectionPoint, Ray *observerRay, Color *color, Color intersectionPointColor);
    void computeSpotLightContribution(Vector3D intersectionPoint, Ray *observerRay, Color *color, Color intersectionPointColor);
    void computeReflection(Vector3D intersectionPoint, Ray *observerRay, Color *color, int level);
    bool isInShadow(Vector3D intersectionPoint, Vector3D lightPosition, double lightDistance);
    double computeDiffuseComponent(Vector3D incidentDirection, Vector3D normalDirection);
    double computeSpecularComponent(Vector3D reflectedDirection, Vector3D observerDirection, int shininess);
    bool isPointVisible(Vector3D point);
    Vector3D getReflectionDirection(Vector3D incidentDirection, Vector3D normalDirection);
    
    friend ostream &operator<<(ostream &out, const Object &o);
};
