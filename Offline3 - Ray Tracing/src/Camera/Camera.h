#ifndef CAMERA_H
#define CAMERA_H

#include "../Vector3/Vector3.h"

class Camera {
private:
    Vector3 position;
    Vector3 lookDirection;
    Vector3 upDirection;
    Vector3 rightDirection;
    
    float movementSpeed;
    float rotationSpeed;
    
    void updateBasisVectors();
    void rotateAboutAxis(const Vector3& axis, float degrees);

public:
    // Constructors
    Camera();
    Camera(const Vector3& pos, const Vector3& look, const Vector3& up);
    
    // Getters
    Vector3 getPosition() const;
    Vector3 getLookDirection() const;
    Vector3 getUpDirection() const;
    Vector3 getRightDirection() const;
    Vector3 getTarget() const;
    float getMovementSpeed() const;
    float getRotationSpeed() const;
    
    // Setters
    void setPosition(const Vector3& pos);
    void setLookDirection(const Vector3& look);
    void setUpDirection(const Vector3& up);
    void setMovementSpeed(float speed);
    void setRotationSpeed(float speed);
    void setLookAt(const Vector3& target);
    void setLookAt(const Vector3& target, const Vector3& up);
    
    // Movement methods
    void moveForward(float distance = -1.0f);
    void moveBackward(float distance = -1.0f);
    void moveLeft(float distance = -1.0f);
    void moveRight(float distance = -1.0f);
    void moveUp(float distance = -1.0f);
    void moveDown(float distance = -1.0f);
    void move(const Vector3& direction, float distance = -1.0f);
    
    // Rotation methods
    void lookLeft(float angle = -1.0f);
    void lookRight(float angle = -1.0f);
    void lookUp(float angle = -1.0f);
    void lookDown(float angle = -1.0f);
    void tiltClockwise(float angle = -1.0f);
    void tiltCounterClockwise(float angle = -1.0f);
    void rotate(const Vector3& axis, float angle);
    
    // Utility methods
    void reset();
    void update();
    void print() const;
    
    // OpenGL specific methods
    void applyLookAt() const;
};

#endif // CAMERA_H