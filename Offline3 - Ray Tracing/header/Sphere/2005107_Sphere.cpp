#include "2005107_Sphere.h"
#include <algorithm>
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

Sphere::Sphere() : Object()
{
    referencePoint = Vector3D::zero();
    height = 0.0;
    width = 0.0;
    length = 0.0;
}

Sphere::Sphere(Vector3D center, double radius) : Object()
{
    referencePoint = center;
    height = radius;
    width = radius;
    length = radius;
}

void Sphere::draw()
{
    glPushMatrix();
    {
        glColor3f(color.getRed(), color.getGreen(), color.getBlue());
        glTranslatef(referencePoint.x, referencePoint.y, referencePoint.z);
        glutSolidSphere(length, 200, 200);
    }
    glPopMatrix();
}

Vector3D Sphere::computeNormal(Vector3D point)
{
    Vector3D normal = point - referencePoint;
    normal.normalize();
    return normal;
}

double Sphere::intersect(Ray *ray)
{
    Vector3D originalOrigin = ray->getOrigin();
    ray->setOrigin(ray->getOrigin() - referencePoint);
    double a = 1;
    double b = 2 * (ray->getDirection() * ray->getOrigin());
    double c = ray->getOrigin() * ray->getOrigin() - length * length;
    double discriminant = b * b - 4 * a * c;
    ray->setOrigin(originalOrigin);

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        double t;
        double t1 = (-b + sqrt(discriminant)) / (2 * a);
        double t2 = (-b - sqrt(discriminant)) / (2 * a);
        if (t1 > 0 && t2 > 0)
        {
            t = min(t1, t2);
        }
        else if (t1 > 0)
        {
            t = t1;
        }
        else if (t2 > 0)
        {
            t = t2;
        }
        else
        {
            t = -1.0;
        }
        return t;
    }
}
