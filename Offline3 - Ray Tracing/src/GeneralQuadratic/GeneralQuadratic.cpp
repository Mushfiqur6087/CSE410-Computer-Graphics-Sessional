#include "GeneralQuadratic.h"
#include <iostream>

// Constructor with coefficients
GeneralQuadratic::GeneralQuadratic(double a, double b, double c, double d, double e, double f,
                                  double g, double h, double i, double j) {
    A = a; B = b; C = c; D = d; E = e;
    F = f; G = g; H = h; I = i; J = j;
    
    // Set reference point to origin by default
    referencePoint = Vector3(0, 0, 0);
}

// Default constructor
GeneralQuadratic::GeneralQuadratic() {
    // Default to unit sphere: x^2 + y^2 + z^2 - 1 = 0
    A = B = C = 1.0;
    D = E = F = G = H = I = 0.0;
    J = -1.0;
    
    referencePoint = Vector3(0, 0, 0);
}

// Override the draw method
void GeneralQuadratic::draw() {
    // Write codes for drawing general quadratic surface
    std::cout << "Drawing general quadratic surface with equation:" << std::endl;
    std::cout << A << "x² + " << B << "y² + " << C << "z² + " 
              << D << "xy + " << E << "xz + " << F << "yz + "
              << G << "x + " << H << "y + " << I << "z + " << J << " = 0" << std::endl;
}

// Set coefficients
void GeneralQuadratic::setCoefficients(double a, double b, double c, double d, double e, double f,
                                      double g, double h, double i, double j) {
    A = a; B = b; C = c; D = d; E = e;
    F = f; G = g; H = h; I = i; J = j;
}

// Get coefficients
void GeneralQuadratic::getCoefficients(double& a, double& b, double& c, double& d, double& e, double& f,
                                      double& g, double& h, double& i, double& j) const {
    a = A; b = B; c = C; d = D; e = E;
    f = F; g = G; h = H; i = I; j = J;
}

// Evaluate the quadratic equation at a given point
double GeneralQuadratic::evaluateAt(const Vector3& point) const {
    double x = point.x, y = point.y, z = point.z;
    return A*x*x + B*y*y + C*z*z + D*x*y + E*x*z + F*y*z + G*x + H*y + I*z + J;
}

// Get gradient at a given point (for normal calculation)
Vector3 GeneralQuadratic::getGradientAt(const Vector3& point) const {
    double x = point.x, y = point.y, z = point.z;
    
    double dx = 2*A*x + D*y + E*z + G;
    double dy = 2*B*y + D*x + F*z + H;
    double dz = 2*C*z + E*x + F*y + I;
    
    return Vector3(dx, dy, dz);
}
