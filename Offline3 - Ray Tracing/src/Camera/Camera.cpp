#include "Camera.h"
#include <GL/glut.h>
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

Camera::Camera(const Vector3& pos, const Vector3& look, const Vector3& up)
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

void Camera::rotateAboutAxis(const Vector3& axis, float degrees) {
    lookDirection = lookDirection.rotate(axis, degrees);
    upDirection = upDirection.rotate(axis, degrees);
    updateBasisVectors();
}

// Getters
Vector3 Camera::getPosition() const { return position; }
Vector3 Camera::getLookDirection() const { return lookDirection; }
Vector3 Camera::getUpDirection() const { return upDirection; }
Vector3 Camera::getRightDirection() const { return rightDirection; }
Vector3 Camera::getTarget() const { return position + lookDirection; }
float Camera::getMovementSpeed() const { return movementSpeed; }
float Camera::getRotationSpeed() const { return rotationSpeed; }

// Setters
void Camera::setPosition(const Vector3& pos) {
    position = pos;
}

void Camera::setLookDirection(const Vector3& look) {
    lookDirection = look;
    updateBasisVectors();
}

void Camera::setUpDirection(const Vector3& up) {
    upDirection = up;
    updateBasisVectors();
}

void Camera::setMovementSpeed(float speed) {
    movementSpeed = speed;
}

void Camera::setRotationSpeed(float speed) {
    rotationSpeed = speed;
}

void Camera::setLookAt(const Vector3& target) {
    lookDirection = (target - position).normalized();
    updateBasisVectors();
}

void Camera::setLookAt(const Vector3& target, const Vector3& up) {
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

void Camera::move(const Vector3& direction, float distance) {
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

void Camera::rotate(const Vector3& axis, float angle) {
    rotateAboutAxis(axis, angle);
}

// Utility methods
void Camera::reset() {
    position = Vector3(0, 0, 10);
    lookDirection = Vector3(0, 0, -1);
    upDirection = Vector3(0, 1, 0);
    updateBasisVectors();
}

void Camera::update() {
    updateBasisVectors();
}

void Camera::print() const {
    std::cout << "Camera Status:" << std::endl;
    std::cout << "Position: "; position.print();
    std::cout << "Look Dir: "; lookDirection.print();
    std::cout << "Up Dir:   "; upDirection.print();
    std::cout << "Right Dir:"; rightDirection.print();
    std::cout << "Movement Speed: " << movementSpeed << std::endl;
    std::cout << "Rotation Speed: " << rotationSpeed << std::endl;
}

// OpenGL specific methods
void Camera::applyLookAt() const {
    Vector3 target = position + lookDirection;
    gluLookAt(
        position.x, position.y, position.z,
        target.x, target.y, target.z,
        upDirection.x, upDirection.y, upDirection.z
    );
}