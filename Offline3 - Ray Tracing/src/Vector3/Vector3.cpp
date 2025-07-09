#include "Vector3.h"
#include <iostream>

// Constructors
Vector3::Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

Vector3::Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

// Assignment operator
Vector3& Vector3::operator=(const Vector3& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    return *this;
}

// Arithmetic operators
Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    if (std::abs(scalar) < 1e-6f) {
        std::cerr << "Warning: Division by zero in Vector3" << std::endl;
        return Vector3(0, 0, 0);
    }
    return Vector3(x / scalar, y / scalar, z / scalar);
}

// Compound assignment operators
Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    if (std::abs(scalar) < 1e-6f) {
        std::cerr << "Warning: Division by zero in Vector3" << std::endl;
        return *this;
    }
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

// Comparison operators
bool Vector3::operator==(const Vector3& other) const {
    const float epsilon = 1e-6f;
    return (std::abs(x - other.x) < epsilon &&
            std::abs(y - other.y) < epsilon &&
            std::abs(z - other.z) < epsilon);
}

bool Vector3::operator!=(const Vector3& other) const {
    return !(*this == other);
}

// Vector operations
float Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vector3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::lengthSquared() const {
    return x * x + y * y + z * z;
}

void Vector3::normalize() {
    float len = length();
    if (len > 1e-6f) {
        x /= len;
        y /= len;
        z /= len;
    }
}

Vector3 Vector3::normalized() const {
    Vector3 result(*this);
    result.normalize();
    return result;
}

Vector3 Vector3::rotate(const Vector3& axis, float degrees) const {
    float radians = degrees * (M_PI / 180.0f);
    Vector3 k = axis.normalized();
    Vector3 v = *this;
    
    return v * std::cos(radians) +
           k.cross(v) * std::sin(radians) +
           k * (k.dot(v) * (1 - std::cos(radians)));
}

// Static utility functions
Vector3 Vector3::zero() { return Vector3(0, 0, 0); }
Vector3 Vector3::one() { return Vector3(1, 1, 1); }
Vector3 Vector3::up() { return Vector3(0, 1, 0); }
Vector3 Vector3::down() { return Vector3(0, -1, 0); }
Vector3 Vector3::left() { return Vector3(-1, 0, 0); }
Vector3 Vector3::right() { return Vector3(1, 0, 0); }
Vector3 Vector3::forward() { return Vector3(0, 0, -1); }
Vector3 Vector3::backward() { return Vector3(0, 0, 1); }

void Vector3::print() const {
    std::cout << "Vector3(" << x << ", " << y << ", " << z << ")" << std::endl;
}

// Non-member operators
Vector3 operator*(float scalar, const Vector3& vector) {
    return vector * scalar;
}