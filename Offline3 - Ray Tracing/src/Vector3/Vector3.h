#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>

using namespace std;

class Vector3 {
public:
    float x, y, z;

    // Constructors
    Vector3(float X = 0, float Y = 0, float Z = 0);
    Vector3(const Vector3& other);

    // Assignment operator
    Vector3& operator=(const Vector3& other);

    // Arithmetic operators
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    // Compound assignment operators
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    // Comparison operators
    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;

    // Vector operations
    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;
    float length() const;
    float lengthSquared() const;
    void normalize();
    Vector3 normalized() const;
    Vector3 rotate(const Vector3& axis, float degrees) const;

    // Static utility functions
    static Vector3 zero();
    static Vector3 one();
    static Vector3 up();
    static Vector3 down();
    static Vector3 left();
    static Vector3 right();
    static Vector3 forward();
    static Vector3 backward();

    // Friend function for output stream operator
    friend ostream &operator<<(ostream &out, const Vector3 &vector);
};

// Non-member operators
Vector3 operator*(float scalar, const Vector3& vector);

#endif // VECTOR3_H