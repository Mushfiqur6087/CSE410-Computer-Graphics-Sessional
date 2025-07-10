#include "Ray.h"

// Constructor that takes start point and direction
Ray::Ray(Vector3 startPoint, Vector3 direction) {
    start = startPoint;
    dir = direction.normalized(); // normalize for easier calculations
}

// Default constructor
Ray::Ray() {
    start = Vector3(0, 0, 0);
    dir = Vector3(0, 0, 1); // default direction along positive z-axis
}
