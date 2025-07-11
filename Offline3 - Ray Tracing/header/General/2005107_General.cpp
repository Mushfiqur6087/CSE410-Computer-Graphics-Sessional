#include "2005107_General.h"
#include <algorithm>
#include <cmath>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

extern double epsilon;

General::General() : Object(), a(0), b(0), c(0), d(0), e(0), f(0), g(0), h(0), i(0), j(0)
{
    initializeDimensions();
}

General::General(double a, double b, double c, double d, double e, double f, 
                 double g, double h, double i, double j) : Object(), 
                 a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j)
{
    initializeDimensions();
}

void General::initializeDimensions()
{
    referencePoint = Vector3D::zero();
    height = 0.0;
    width = 0.0;
    length = 0.0;
}

void General::setBoundingBox(Vector3D referencePoint, double height, double width, double length)
{
    this->referencePoint = referencePoint;
    this->height = height;
    this->width = width;
    this->length = length;
}

bool General::isWithinDimension(double value, double minBound, double maxBound) const
{
    return (value >= minBound && value <= maxBound);
}

bool General::insideBoundingBox(Vector3D point) const
{
    // Check X dimension
    if (fabs(width) > epsilon) {
        if (!isWithinDimension(point.x, referencePoint.x, referencePoint.x + width)) {
            return false;
        }
    }
    
    // Check Y dimension
    if (fabs(height) > epsilon) {
        if (!isWithinDimension(point.y, referencePoint.y, referencePoint.y + height)) {
            return false;
        }
    }
    
    // Check Z dimension
    if (fabs(length) > epsilon) {
        if (!isWithinDimension(point.z, referencePoint.z, referencePoint.z + length)) {
            return false;
        }
    }
    
    return true;
}

void General::draw()
{
    // General quadrics are complex to draw, implementation skipped
    return;
}

Vector3D General::computeNormal(Vector3D point)
{
    // Compute partial derivatives for the normal vector
    double normalX = 2 * a * point.x + d * point.y + e * point.z + g;
    double normalY = 2 * b * point.y + d * point.x + f * point.z + h;
    double normalZ = 2 * c * point.z + e * point.x + f * point.y + i;
    
    Vector3D normal(normalX, normalY, normalZ);
    normal.normalize();
    return normal;
}

// Helper method to calculate quadratic equation coefficients
QuadraticSolution General::solveQuadraticEquation(double A, double B, double C) const
{
    QuadraticSolution solution;
    double discriminant = B * B - 4 * A * C;
    
    if (discriminant < 0) {
        solution.hasRealRoots = false;
        return solution;
    }
    
    solution.hasRealRoots = true;
    double sqrtDiscriminant = sqrt(discriminant);
    solution.t1 = (-B - sqrtDiscriminant) / (2 * A);
    solution.t2 = (-B + sqrtDiscriminant) / (2 * A);
    
    // Ensure t1 <= t2
    if (solution.t1 > solution.t2) {
        swap(solution.t1, solution.t2);
    }
    
    return solution;
}

// Calculate A coefficient for quadratic equation
double General::calculateCoefficientA(const Vector3D& direction) const
{
    return a * direction.x * direction.x + 
           b * direction.y * direction.y + 
           c * direction.z * direction.z + 
           d * direction.x * direction.y + 
           e * direction.y * direction.z + 
           f * direction.z * direction.x;
}

// Calculate B coefficient for quadratic equation
double General::calculateCoefficientB(const Vector3D& origin, const Vector3D& direction) const
{
    return 2 * (a * direction.x * origin.x + 
                b * direction.y * origin.y + 
                c * direction.z * origin.z) + 
           d * (direction.x * origin.y + direction.y * origin.x) + 
           e * (direction.y * origin.z + direction.z * origin.y) + 
           f * (direction.z * origin.x + direction.x * origin.z) + 
           g * direction.x + 
           h * direction.y + 
           i * direction.z;
}

// Calculate C coefficient for quadratic equation
double General::calculateCoefficientC(const Vector3D& origin) const
{
    return a * origin.x * origin.x + 
           b * origin.y * origin.y + 
           c * origin.z * origin.z + 
           d * origin.x * origin.y + 
           e * origin.y * origin.z + 
           f * origin.z * origin.x + 
           g * origin.x + 
           h * origin.y + 
           i * origin.z + 
           j;
}

// Find valid intersection within bounding box
double General::findValidIntersection(const Vector3D& origin, const Vector3D& direction, 
                                     double t1, double t2) const
{
    // Both intersection points are behind the ray origin
    if (t2 < epsilon) {
        return -1.0;
    }
    
    // First intersection is behind, check second
    if (t1 < epsilon) {
        Vector3D intersectionPoint2 = origin + direction * t2;
        return insideBoundingBox(intersectionPoint2) ? t2 : -1.0;
    }
    
    // Both intersections are in front, check closest first
    Vector3D intersectionPoint1 = origin + direction * t1;
    if (insideBoundingBox(intersectionPoint1)) {
        return t1;
    }
    
    Vector3D intersectionPoint2 = origin + direction * t2;
    if (insideBoundingBox(intersectionPoint2)) {
        return t2;
    }
    
    return -1.0;
}

double General::intersect(Ray *ray)
{
    Vector3D rayOrigin = ray->getOrigin();
    Vector3D rayDirection = ray->getDirection();
    
    // Calculate quadratic equation coefficients: At^2 + Bt + C = 0
    double A = calculateCoefficientA(rayDirection);
    double B = calculateCoefficientB(rayOrigin, rayDirection);
    double C = calculateCoefficientC(rayOrigin);
    
    // Solve quadratic equation
    QuadraticSolution solution = solveQuadraticEquation(A, B, C);
    
    if (!solution.hasRealRoots) {
        return -1.0;
    }
    
    // Find valid intersection point within bounding box
    return findValidIntersection(rayOrigin, rayDirection, solution.t1, solution.t2);
}
