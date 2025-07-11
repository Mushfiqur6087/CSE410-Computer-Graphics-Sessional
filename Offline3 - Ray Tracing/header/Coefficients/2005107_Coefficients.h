#pragma once

#include <iostream>

using namespace std;

class Coefficients
{
public:
    double ambient, diffuse, specular, reflection;
    
    Coefficients();
    Coefficients(double ambient, double diffuse, double specular, double reflection);
    
    void setCoefficients(double ambientCoeff, double diffuseCoeff, double specularCoeff, double reflectionCoeff);
    
    // Getter methods
    double getAmbient() const;
    double getDiffuse() const;
    double getSpecular() const;
    double getReflection() const;
    
    // Friend function for output stream operator
    friend ostream &operator<<(ostream &out, const Coefficients &coefficients);
};
