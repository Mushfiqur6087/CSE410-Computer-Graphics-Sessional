#ifndef RAY_H
#define RAY_H

#include "../Vector3/Vector3.h"

class Ray {
public:
    Vector3 start;
    Vector3 dir; // normalize for easier calculations
    
    // Constructor that takes start point and direction
    Ray(Vector3 startPoint, Vector3 direction);
    
    // Default constructor
    Ray();
};

#endif // RAY_H
