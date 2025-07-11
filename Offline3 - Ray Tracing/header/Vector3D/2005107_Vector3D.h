#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <iostream>

using namespace std;

class Vector3D {
public:
    double x, y, z;

    // Constructors
    Vector3D(double x = 0, double y = 0, double z = 0);
    Vector3D(const Vector3D& other);

    // Assignment operator
    Vector3D& operator=(const Vector3D& other);

    // Arithmetic operators
    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D operator*(double scalar) const;
    Vector3D operator/(double scalar) const;

    // Compound assignment operators
    Vector3D& operator+=(const Vector3D& other);
    Vector3D& operator-=(const Vector3D& other);
    Vector3D& operator*=(double scalar);
    Vector3D& operator/=(double scalar);

    // Comparison operators
    bool operator==(const Vector3D& other) const;
    bool operator!=(const Vector3D& other) const;

    // Vector operations
    double dot(const Vector3D& other) const;
    Vector3D cross(const Vector3D& other) const;
    double length() const;
    double lengthSquared() const;
    void normalize();
    Vector3D normalized() const;
    Vector3D rotate(const Vector3D& axis, double degrees) const;

    // Legacy operators for backward compatibility
    double operator*(const Vector3D& v) const; // dot product
    Vector3D operator^(const Vector3D& v) const; // cross product

    // Static utility functions
    static Vector3D zero();
    static Vector3D one();
    static Vector3D up();
    static Vector3D down();
    static Vector3D left();
    static Vector3D right();
    static Vector3D forward();
    static Vector3D backward();

    // Friend function for output stream operator
    friend ostream& operator<<(ostream& out, const Vector3D& vector);
};

// Utility functions
double degreeToRadian(double degree);
double radianToDegree(double radian);

#endif // VECTOR3D_H
