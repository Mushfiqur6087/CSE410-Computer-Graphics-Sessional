#ifndef OBJECT_H
#define OBJECT_H

#include "../Vector3/Vector3.h"
#include "../Color/Color.h"
#include "../Coefficients/Coefficients.h"

class Object {
public:
    Vector3 referencePoint;  // should have x, y, z
    double height, width, length;
    Color color;
    Coefficients coEfficients;   // ambient, diffuse, specular, reflection coefficients
    int shine;               // exponent term of specular component

    // Constructor
    Object();

    // Virtual destructor for proper inheritance
    virtual ~Object();

    // Virtual draw method to be overridden by derived classes
    virtual void draw() {}

    // Setter methods
    void setColor(double r, double g, double b);
    void setShine(int shineValue);
    void setCoEfficients(double ambient, double diffuse, double specular, double reflection);

    // Getter methods
    Vector3 getReferencePoint() const;
    void setReferencePoint(const Vector3& point);
    void setDimensions(double h, double w, double l);
};

#endif // OBJECT_H
