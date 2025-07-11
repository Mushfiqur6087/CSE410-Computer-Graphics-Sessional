#pragma once

#include <iostream>
using namespace std;

#include "../Vector3D/2005107_Vector3D.h"


class Camera
{
private:
    Vector3D position;
    Vector3D lookDirection;
    Vector3D upDirection;
    Vector3D rightDirection;
    float movementSpeed;
    float rotationSpeed;

    void updateBasisVectors();
    void rotateAboutAxis(const Vector3D& axis, float degrees);

public:
    // Constructors
    Camera();
    Camera(const Vector3D& pos, const Vector3D& look, const Vector3D& up);

    // Getters
    Vector3D getPosition() const;
    Vector3D getLookDirection() const;
    Vector3D getUpDirection() const;
    Vector3D getRightDirection() const;
    Vector3D getTarget() const;
    float getMovementSpeed() const;
    float getRotationSpeed() const;

    // Setters
    void setPosition(const Vector3D& pos);
    void setLookDirection(const Vector3D& look);
    void setUpDirection(const Vector3D& up);
    void setMovementSpeed(float speed);
    void setRotationSpeed(float speed);
    void setLookAt(const Vector3D& target);
    void setLookAt(const Vector3D& target, const Vector3D& up);

    // Movement methods
    void moveForward(float distance = -1);
    void moveBackward(float distance = -1);
    void moveLeft(float distance = -1);
    void moveRight(float distance = -1);
    void moveUp(float distance = -1);
    void moveDown(float distance = -1);
    void move(const Vector3D& direction, float distance = -1);

    // Rotation methods
    void lookLeft(float angle = -1);
    void lookRight(float angle = -1);
    void lookUp(float angle = -1);
    void lookDown(float angle = -1);
    void tiltClockwise(float angle = -1);
    void tiltCounterClockwise(float angle = -1);
    void rotate(const Vector3D& axis, float angle);

    // Utility methods
    void reset();
    void update();

    // OpenGL specific methods
    void applyLookAt() const;

    // Stream output
    friend ostream& operator<<(ostream& out, const Camera& camera);
};
