#include "Sphere.h"
#include <iostream>
#include <GL/glut.h>

// Constructor with center and radius
Sphere::Sphere(Vector3 center, double radius) {
    referencePoint = center;
    length = radius;  // Using length to store radius
    height = width = radius;  // For consistency
}

// Default constructor
Sphere::Sphere() {
    referencePoint = Vector3(0, 0, 0);
    length = height = width = 1.0;  // Default radius of 1
}

// Override the draw method
void Sphere::draw() {
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(referencePoint.x, referencePoint.y, referencePoint.z);
        glutSolidSphere(length, 200, 200);
    }
    glPopMatrix();
}

// Getter methods
Vector3 Sphere::getCenter() const {
    return referencePoint;
}

double Sphere::getRadius() const {
    return length;
}

// Setter methods
void Sphere::setCenter(const Vector3& center) {
    referencePoint = center;
}

void Sphere::setRadius(double radius) {
    length = height = width = radius;
}
