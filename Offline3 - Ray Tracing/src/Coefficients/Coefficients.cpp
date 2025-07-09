#include "Coefficients.h"

ostream &operator<<(ostream &out, const Coefficients &coefficients) {
    out << "Coefficients - Ambient: " << coefficients.ambient 
        << ", Diffuse: " << coefficients.diffuse 
        << ", Specular: " << coefficients.specular 
        << ", Reflection: " << coefficients.reflection;
    return out;
}
