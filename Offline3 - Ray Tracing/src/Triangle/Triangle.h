#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../Object/Object.h"

class Triangle : public Object {
public:
    Vector3 vertex1, vertex2, vertex3;
    
    // Constructor
    Triangle(Vector3 v1, Vector3 v2, Vector3 v3);
    
    // Default constructor
    Triangle();
    
    // Override the draw method
    void draw() override;
    
    // Getter methods
    Vector3 getVertex1() const;
    Vector3 getVertex2() const;
    Vector3 getVertex3() const;
    
    // Setter methods
    void setVertices(const Vector3& v1, const Vector3& v2, const Vector3& v3);
    
    // Utility methods
    Vector3 getNormal() const;
    double getArea() const;
};

#endif // TRIANGLE_H
