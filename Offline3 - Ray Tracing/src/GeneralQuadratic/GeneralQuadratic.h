#ifndef GENERALQUADRATIC_H
#define GENERALQUADRATIC_H

#include "../Object/Object.h"

class GeneralQuadratic : public Object {
public:
    // Coefficients for the general quadratic equation:
    // Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0
    double A, B, C, D, E, F, G, H, I, J;
    
    // Constructor
    GeneralQuadratic(double a, double b, double c, double d, double e, double f,
                    double g, double h, double i, double j);
    
    // Default constructor
    GeneralQuadratic();
    
    // Override the draw method
    void draw() override;
    
    // Override the intersect method
    double intersect(Ray* r, double* color, int level) override;
    
    // Override the getColorAt method
    Color getColorAt(const Vector3& point) override;
    
    // Override the getNormalAt method
    Vector3 getNormalAt(const Vector3& point) override;
    
    // Setter methods
    void setCoefficients(double a, double b, double c, double d, double e, double f,
                        double g, double h, double i, double j);
    
    // Getter methods
    void getCoefficients(double& a, double& b, double& c, double& d, double& e, double& f,
                        double& g, double& h, double& i, double& j) const;
    
    // Utility methods
    double evaluateAt(const Vector3& point) const;
    Vector3 getGradientAt(const Vector3& point) const;
};

#endif // GENERALQUADRATIC_H
