#pragma once

#include "../Object/2005107_Object.h"

// Forward declaration for quadratic solution structure
struct QuadraticSolution {
    bool hasRealRoots;
    double t1, t2;
};

class General : public Object
{
private:
    double a, b, c, d, e, f, g, h, i, j; // Quadric coefficients
    
    // Helper methods
    void initializeDimensions();
    bool isWithinDimension(double value, double minBound, double maxBound) const;
    QuadraticSolution solveQuadraticEquation(double A, double B, double C) const;
    double calculateCoefficientA(const Vector3D& direction) const;
    double calculateCoefficientB(const Vector3D& origin, const Vector3D& direction) const;
    double calculateCoefficientC(const Vector3D& origin) const;
    double findValidIntersection(const Vector3D& origin, const Vector3D& direction, 
                               double t1, double t2) const;

public:
    General();
    General(double a, double b, double c, double d, double e, double f, 
            double g, double h, double i, double j);
    
    void setBoundingBox(Vector3D referencePoint, double height, double width, double length);
    bool insideBoundingBox(Vector3D point) const;
    
    void draw() override;
    Vector3D computeNormal(Vector3D point) override;
    double intersect(Ray *ray) override;
};
