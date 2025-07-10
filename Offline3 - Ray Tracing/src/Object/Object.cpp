#include "Object.h"
#include "../Ray/Ray.h"

// Constructor
Object::Object() {
    referencePoint = Vector3(0, 0, 0);
    height = width = length = 0.0;
    
    // Initialize color to default (black)
    color = Color(0.0, 0.0, 0.0);
    
    // Initialize coefficients to default values
    coEfficients = Coefficients(0.0, 0.0, 0.0, 0.0);
    
    shine = 0;
}

// Virtual destructor
Object::~Object() {
    // Empty destructor - derived classes can override if needed
}

// Set color values
void Object::setColor(double r, double g, double b) {
    color = Color(r, g, b);
}

// Set shine value
void Object::setShine(int shineValue) {
    shine = shineValue;
}

// Set coefficients
void Object::setCoEfficients(double ambient, double diffuse, double specular, double reflection) {
    coEfficients = Coefficients(ambient, diffuse, specular, reflection);
}

// Get reference point
Vector3 Object::getReferencePoint() const {
    return referencePoint;
}

// Set reference point
void Object::setReferencePoint(const Vector3& point) {
    referencePoint = point;
}

// Set dimensions
void Object::setDimensions(double h, double w, double l) {
    height = h;
    width = w;
    length = l;
}
