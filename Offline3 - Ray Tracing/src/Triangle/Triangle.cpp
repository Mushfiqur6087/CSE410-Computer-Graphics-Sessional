#include "Triangle.h"
#include <iostream>

// Constructor with three vertices
Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;
    
    // Set reference point as centroid
    referencePoint = Vector3(
        (v1.x + v2.x + v3.x) / 3.0,
        (v1.y + v2.y + v3.y) / 3.0,
        (v1.z + v2.z + v3.z) / 3.0
    );
}

// Default constructor
Triangle::Triangle() {
    vertex1 = Vector3(0, 0, 0);
    vertex2 = Vector3(1, 0, 0);
    vertex3 = Vector3(0, 1, 0);
    referencePoint = Vector3(0.33, 0.33, 0);
}

// Override the draw method
void Triangle::draw() {
    // Write codes for drawing triangle
    std::cout << "Drawing triangle with vertices:" << std::endl;
    std::cout << "  V1: (" << vertex1.x << ", " << vertex1.y << ", " << vertex1.z << ")" << std::endl;
    std::cout << "  V2: (" << vertex2.x << ", " << vertex2.y << ", " << vertex2.z << ")" << std::endl;
    std::cout << "  V3: (" << vertex3.x << ", " << vertex3.y << ", " << vertex3.z << ")" << std::endl;
}

// Getter methods
Vector3 Triangle::getVertex1() const {
    return vertex1;
}

Vector3 Triangle::getVertex2() const {
    return vertex2;
}

Vector3 Triangle::getVertex3() const {
    return vertex3;
}

// Setter methods
void Triangle::setVertices(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;
    
    // Update reference point as centroid
    referencePoint = Vector3(
        (v1.x + v2.x + v3.x) / 3.0,
        (v1.y + v2.y + v3.y) / 3.0,
        (v1.z + v2.z + v3.z) / 3.0
    );
}

// Get normal vector of the triangle
Vector3 Triangle::getNormal() const {
    Vector3 edge1 = vertex2 - vertex1;
    Vector3 edge2 = vertex3 - vertex1;
    return edge1.cross(edge2).normalized();
}

// Get area of the triangle
double Triangle::getArea() const {
    Vector3 edge1 = vertex2 - vertex1;
    Vector3 edge2 = vertex3 - vertex1;
    return 0.5 * edge1.cross(edge2).length();
}
