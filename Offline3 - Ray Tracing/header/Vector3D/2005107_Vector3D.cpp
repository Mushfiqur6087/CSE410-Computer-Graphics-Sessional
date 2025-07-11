#include <iostream>
#include <cmath>
using namespace std;

#include "2005107_Vector3D.h"

// Constructors
Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3D::Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}

// Assignment operator
Vector3D& Vector3D::operator=(const Vector3D& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    return *this;
}

// Arithmetic operators
Vector3D Vector3D::operator+(const Vector3D& other) const {
    return Vector3D(x + other.x, y + other.y, z + other.z);
}

Vector3D Vector3D::operator-(const Vector3D& other) const {
    return Vector3D(x - other.x, y - other.y, z - other.z);
}

Vector3D Vector3D::operator*(double scalar) const {
    return Vector3D(x * scalar, y * scalar, z * scalar);
}

Vector3D Vector3D::operator/(double scalar) const {
    return Vector3D(x / scalar, y / scalar, z / scalar);
}

// Compound assignment operators
Vector3D& Vector3D::operator+=(const Vector3D& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3D& Vector3D::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3D& Vector3D::operator/=(double scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

// Comparison operators
bool Vector3D::operator==(const Vector3D& other) const {
    const double epsilon = 1e-9;
    return abs(x - other.x) < epsilon && abs(y - other.y) < epsilon && abs(z - other.z) < epsilon;
}

bool Vector3D::operator!=(const Vector3D& other) const {
    return !(*this == other);
}

// Vector operations
double Vector3D::dot(const Vector3D& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3D Vector3D::cross(const Vector3D& other) const {
    return Vector3D(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

double Vector3D::length() const {
    return sqrt(x * x + y * y + z * z);
}

double Vector3D::lengthSquared() const {
    return x * x + y * y + z * z;
}

void Vector3D::normalize() {
    double len = length();
    if (len > 0) {
        x /= len;
        y /= len;
        z /= len;
    }
}

Vector3D Vector3D::normalized() const {
    double len = length();
    if (len > 0) {
        return Vector3D(x / len, y / len, z / len);
    }
    return Vector3D::zero();
}

Vector3D Vector3D::rotate(const Vector3D& axis, double degrees) const {
    Vector3D v = *this;
    Vector3D k = axis.normalized();
    double theta = degreeToRadian(degrees);
    double cosTheta = cos(theta);
    double sinTheta = sin(theta);
    double dotProduct = k.dot(v);
    
    // Rodrigues' rotation formula: v*cos(θ) + (k × v)*sin(θ) + k*(k · v)*(1 - cos(θ))
    return v * cosTheta + k.cross(v) * sinTheta + k * dotProduct * (1 - cosTheta);
}

// Legacy operators for backward compatibility
double Vector3D::operator*(const Vector3D& v) const {
    return dot(v);
}

Vector3D Vector3D::operator^(const Vector3D& v) const {
    return cross(v);
}

// Static utility functions
Vector3D Vector3D::zero() {
    return Vector3D(0, 0, 0);
}

Vector3D Vector3D::one() {
    return Vector3D(1, 1, 1);
}

Vector3D Vector3D::up() {
    return Vector3D(0, 1, 0);
}

Vector3D Vector3D::down() {
    return Vector3D(0, -1, 0);
}

Vector3D Vector3D::left() {
    return Vector3D(-1, 0, 0);
}

Vector3D Vector3D::right() {
    return Vector3D(1, 0, 0);
}

Vector3D Vector3D::forward() {
    return Vector3D(0, 0, 1);
}

Vector3D Vector3D::backward() {
    return Vector3D(0, 0, -1);
}

// Friend function for output stream operator
ostream& operator<<(ostream& out, const Vector3D& vector) {
    out << "< " << vector.x << " " << vector.y << " " << vector.z << " >";
    return out;
}

// Utility functions
double degreeToRadian(double degree) {
    return degree * M_PI / 180;
}

double radianToDegree(double radian) {
    return radian * 180 / M_PI;
}

