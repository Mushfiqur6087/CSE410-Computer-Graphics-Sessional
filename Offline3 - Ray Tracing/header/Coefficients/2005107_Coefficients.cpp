#include "2005107_Coefficients.h"
#include <iostream>

using namespace std;

Coefficients::Coefficients() : ambient(0), diffuse(0), specular(0), reflection(0) {}

Coefficients::Coefficients(double ambient, double diffuse, double specular, double reflection) 
    : ambient(ambient), diffuse(diffuse), specular(specular), reflection(reflection) {}

void Coefficients::setCoefficients(double ambientCoeff, double diffuseCoeff, double specularCoeff, double reflectionCoeff) {
    ambient = ambientCoeff;
    diffuse = diffuseCoeff;
    specular = specularCoeff;
    reflection = reflectionCoeff;
}

// Getter methods
double Coefficients::getAmbient() const {
    return ambient;
}

double Coefficients::getDiffuse() const {
    return diffuse;
}

double Coefficients::getSpecular() const {
    return specular;
}

double Coefficients::getReflection() const {
    return reflection;
}

ostream &operator<<(ostream &out, const Coefficients &coefficients) {
    out << "Coefficients(ambient: " << coefficients.ambient 
        << ", diffuse: " << coefficients.diffuse 
        << ", specular: " << coefficients.specular 
        << ", reflection: " << coefficients.reflection << ")";
    return out;
}
