#include "2005107_Triangle.h"
#include <algorithm>
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

extern double epsilon;

Triangle::Triangle() : Object()
{
    vertexA = Vector3D::zero();
    vertexB = Vector3D::zero();
    vertexC = Vector3D::zero();
    referencePoint = Vector3D::zero();
    height = 0.0;
    width = 0.0;
    length = 0.0;
}

Triangle::Triangle(Vector3D a, Vector3D b, Vector3D c) : Object(), vertexA(a), vertexB(b), vertexC(c)
{
    Vector3D normal = (vertexB - vertexA) ^ (vertexC - vertexA);
    normal.normalize();
    referencePoint = vertexA;
    height = normal.x;
    width = normal.y;
    length = normal.z;
}

void Triangle::draw()
{
    glBegin(GL_TRIANGLES);
    {
        glColor3f(color.getRed(), color.getGreen(), color.getBlue());
        glVertex3f(vertexA.x, vertexA.y, vertexA.z);
        glVertex3f(vertexB.x, vertexB.y, vertexB.z);
        glVertex3f(vertexC.x, vertexC.y, vertexC.z);
    }
    glEnd();
}

Vector3D Triangle::computeNormal(Vector3D point)
{
    Vector3D normal = (vertexB - vertexA) ^ (vertexC - vertexA);
    normal.normalize();
    return normal;
}

double Triangle::intersect(Ray *ray)
{
    Vector3D normal = computeNormal(referencePoint);
    double denominator = normal * ray->getDirection();
    
    if (fabs(denominator) < epsilon)
    {
        return -1.0;
    }
    
    double t = ((referencePoint - ray->getOrigin()) * normal) / denominator;
    if (t < 0)
    {
        return -1.0;
    }
    
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * t;
    
    // Check if point is inside triangle using cross product method
    Vector3D crossProduct0 = (vertexB - vertexA) ^ (intersectionPoint - vertexA);
    Vector3D crossProduct1 = (vertexC - vertexB) ^ (intersectionPoint - vertexB);
    Vector3D crossProduct2 = (vertexA - vertexC) ^ (intersectionPoint - vertexC);
    
    if ((crossProduct0 * crossProduct1 >= 0) && (crossProduct1 * crossProduct2 >= 0))
    {
        return t;
    }
    
    return -1.0;
}
