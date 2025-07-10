#ifndef SPHERE_H
#define SPHERE_H

#include "../Object/Object.h"

class Sphere : public Object {
public:
    // Constructor
    Sphere(Vector3 center, double radius);
    
    // Default constructor
    Sphere();
    
    // Override the draw method
    void draw() override;
    
    // Override the intersect method
    double intersect(Ray* r, double* color, int level) override;
    
    // Override the getColorAt method
    Color getColorAt(const Vector3& point) override;
    
    // Override the getNormalAt method
    Vector3 getNormalAt(const Vector3& point) override;
    
    // Getter methods
    Vector3 getCenter() const;
    double getRadius() const;
    
    // Setter methods
    void setCenter(const Vector3& center);
    void setRadius(double radius);
};

#endif // SPHERE_H
