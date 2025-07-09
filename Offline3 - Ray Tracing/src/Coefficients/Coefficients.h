#ifndef COEFFICIENTS_H
#define COEFFICIENTS_H

#include <iostream>
using namespace std;

class Coefficients {
public:
    double ambient;
    double diffuse;
    double specular;
    double reflection;
    
    Coefficients() : ambient(0), diffuse(0), specular(0), reflection(0) {
    }
    
    Coefficients(double ambientCoeff, double diffuseCoeff, double specularCoeff, double reflectionCoeff) 
        : ambient(ambientCoeff), diffuse(diffuseCoeff), specular(specularCoeff), reflection(reflectionCoeff) {
    }
    
    // Getters
    double getAmbient() const { return ambient; }
    double getDiffuse() const { return diffuse; }
    double getSpecular() const { return specular; }
    double getReflection() const { return reflection; }
    
    // Setters
    void setAmbient(double ambientCoeff) { 
        ambient = ambientCoeff; 
    }
    
    void setDiffuse(double diffuseCoeff) { 
        diffuse = diffuseCoeff; 
    }
    
    void setSpecular(double specularCoeff) { 
        specular = specularCoeff; 
    }
    
    void setReflection(double reflectionCoeff) { 
        reflection = reflectionCoeff; 
    }
    
    // Set all coefficients at once
    void setCoefficients(double ambientCoeff, double diffuseCoeff, double specularCoeff, double reflectionCoeff) {
        ambient = ambientCoeff;
        diffuse = diffuseCoeff;
        specular = specularCoeff;
        reflection = reflectionCoeff;
    }
    
    // Friend function for output stream operator
    friend ostream &operator<<(ostream &out, const Coefficients &coefficients);
};

#endif // COEFFICIENTS_H
