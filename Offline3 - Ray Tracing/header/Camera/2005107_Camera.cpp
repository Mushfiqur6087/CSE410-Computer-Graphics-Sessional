#include "2005107_Camera.h"

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#include <iostream>

// Constructors
Camera::Camera() 
    : position(0, 0, 10), 
      lookDirection(0, 0, -1), 
      upDirection(0, 1, 0),
      movementSpeed(0.3f),
      rotationSpeed(4.0f) {
    updateBasisVectors();
}

Camera::Camera(const Vector3D& pos, const Vector3D& look, const Vector3D& up)
    : position(pos),
      lookDirection(look),
      upDirection(up),
      movementSpeed(0.3f),
      rotationSpeed(4.0f) {
    updateBasisVectors();
}

// Private methods
void Camera::updateBasisVectors() {
    lookDirection.normalize();
    rightDirection = lookDirection.cross(upDirection);
    rightDirection.normalize();
    upDirection = rightDirection.cross(lookDirection);
    upDirection.normalize();
}

void Camera::rotateAboutAxis(const Vector3D& axis, float degrees) {
    lookDirection = lookDirection.rotate(axis, degrees);
    upDirection = upDirection.rotate(axis, degrees);
    updateBasisVectors();
}

// Getters
Vector3D Camera::getPosition() const { return position; }
Vector3D Camera::getLookDirection() const { return lookDirection; }
Vector3D Camera::getUpDirection() const { return upDirection; }
Vector3D Camera::getRightDirection() const { return rightDirection; }
Vector3D Camera::getTarget() const { return position + lookDirection; }
float Camera::getMovementSpeed() const { return movementSpeed; }
float Camera::getRotationSpeed() const { return rotationSpeed; }

// Setters
void Camera::setPosition(const Vector3D& pos) {
    position = pos;
}

void Camera::setLookDirection(const Vector3D& look) {
    lookDirection = look;
    updateBasisVectors();
}

void Camera::setUpDirection(const Vector3D& up) {
    upDirection = up;
    updateBasisVectors();
}

void Camera::setMovementSpeed(float speed) {
    movementSpeed = speed;
}

void Camera::setRotationSpeed(float speed) {
    rotationSpeed = speed;
}

void Camera::setLookAt(const Vector3D& target) {
    lookDirection = (target - position).normalized();
    updateBasisVectors();
}

void Camera::setLookAt(const Vector3D& target, const Vector3D& up) {
    lookDirection = (target - position).normalized();
    upDirection = up;
    updateBasisVectors();
}

// Movement methods
void Camera::moveForward(float distance) {
    if (distance < 0) distance = movementSpeed;
    position += lookDirection * distance;
}

void Camera::moveBackward(float distance) {
    if (distance < 0) distance = movementSpeed;
    position -= lookDirection * distance;
}

void Camera::moveLeft(float distance) {
    if (distance < 0) distance = movementSpeed;
    position -= rightDirection * distance;
}

void Camera::moveRight(float distance) {
    if (distance < 0) distance = movementSpeed;
    position += rightDirection * distance;
}

void Camera::moveUp(float distance) {
    if (distance < 0) distance = movementSpeed;
    position += upDirection * distance;
}

void Camera::moveDown(float distance) {
    if (distance < 0) distance = movementSpeed;
    position -= upDirection * distance;
}

void Camera::move(const Vector3D& direction, float distance) {
    if (distance < 0) distance = movementSpeed;
    position += direction.normalized() * distance;
}

// Rotation methods
void Camera::lookLeft(float angle) {
    if (angle < 0) angle = rotationSpeed;
    rotateAboutAxis(upDirection, angle);
}

void Camera::lookRight(float angle) {
    if (angle < 0) angle = rotationSpeed;
    rotateAboutAxis(upDirection, -angle);
}

void Camera::lookUp(float angle) {
    if (angle < 0) angle = rotationSpeed;
    rotateAboutAxis(rightDirection, angle);
}

void Camera::lookDown(float angle) {
    if (angle < 0) angle = rotationSpeed;
    rotateAboutAxis(rightDirection, -angle);
}

void Camera::tiltClockwise(float angle) {
    if (angle < 0) angle = rotationSpeed;
    rotateAboutAxis(lookDirection, angle);
}

void Camera::tiltCounterClockwise(float angle) {
    if (angle < 0) angle = rotationSpeed;
    rotateAboutAxis(lookDirection, -angle);
}

void Camera::rotate(const Vector3D& axis, float angle) {
    rotateAboutAxis(axis, angle);
}

// Utility methods
void Camera::reset() {
    position = Vector3D(0, 0, 10);
    lookDirection = Vector3D::backward();
    upDirection = Vector3D::up();
    updateBasisVectors();
}

void Camera::update() {
    updateBasisVectors();
}

// OpenGL specific methods
void Camera::applyLookAt() const {
    Vector3D target = position + lookDirection;
    gluLookAt(
        position.x, position.y, position.z,
        target.x, target.y, target.z,
        upDirection.x, upDirection.y, upDirection.z
    );
}

ostream &operator<<(ostream &out, const Camera &camera) {
    out << "Camera Status:" << endl;
    out << "Position: " << camera.position << endl;
    out << "Look Dir: " << camera.lookDirection << endl;
    out << "Up Dir:   " << camera.upDirection << endl;
    out << "Right Dir:" << camera.rightDirection << endl;
    out << "Movement Speed: " << camera.movementSpeed << endl;
    out << "Rotation Speed: " << camera.rotationSpeed << endl;
    return out;
}
