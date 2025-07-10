#include "GeneralQuadratic.h"
#include "../Ray/Ray.h"
#include "../PointLight/PointLight.h"
#include "../SpotLight/SpotLight.h"
#include "../../globals.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
    // General quadratic surfaces are not drawn in OpenGL
    // They are only rendered in the BMP image file through ray tracing
    // This method is intentionally left empty as per the requirements
}

// Override the intersect method
double GeneralQuadratic::intersect(Ray* r, double* color, int level) {
    // General quadratic equation: Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0
    // Ray equation: P = start + t * dir
    // Substitute ray equation into quadratic equation and solve for t
    
    Vector3 start = r->start;
    Vector3 dir = r->dir;
    
    double x0 = start.x, y0 = start.y, z0 = start.z;
    double dx = dir.x, dy = dir.y, dz = dir.z;
    
    // Coefficients for the quadratic equation in t: at^2 + bt + c = 0
    double a = A*dx*dx + B*dy*dy + C*dz*dz + D*dx*dy + E*dx*dz + F*dy*dz;
    double b = 2*A*x0*dx + 2*B*y0*dy + 2*C*z0*dz + D*(x0*dy + y0*dx) + 
               E*(x0*dz + z0*dx) + F*(y0*dz + z0*dy) + G*dx + H*dy + I*dz;
    double c = A*x0*x0 + B*y0*y0 + C*z0*z0 + D*x0*y0 + E*x0*z0 + F*y0*z0 + 
               G*x0 + H*y0 + I*z0 + J;
    
    // Solve quadratic equation
    double discriminant = b*b - 4*a*c;
    
    if (discriminant < 0) {
        return -1.0; // No intersection
    }
    
    double sqrt_discriminant = sqrt(discriminant);
    double t1 = (-b - sqrt_discriminant) / (2.0 * a);
    double t2 = (-b + sqrt_discriminant) / (2.0 * a);
    
    // Choose the nearest positive intersection
    double t = -1.0;
    if (t1 > 0 && t2 > 0) {
        t = (t1 < t2) ? t1 : t2;
    } else if (t1 > 0) {
        t = t1;
    } else if (t2 > 0) {
        t = t2;
    }
    
    if (t < 0) {
        return -1.0; // No valid intersection
    }
    
    // If level is 0, just return distance for nearest object detection
    if (level == 0) {
        return t;
    }
    
    // Calculate intersection point
    Vector3 intersectionPoint = r->start + r->dir * t;
    
    // Get color at intersection point
    Color intersectionPointColor = getColorAt(intersectionPoint);
    
    // Initialize color with ambient component
    color[0] = intersectionPointColor.r * coEfficients.ambient;
    color[1] = intersectionPointColor.g * coEfficients.ambient;
    color[2] = intersectionPointColor.b * coEfficients.ambient;
    
    // Calculate normal at intersection point
    Vector3 normal = getNormalAt(intersectionPoint);
    
    // Process each point light
    for (const auto& pl : pointLights) {
        // Cast ray from light to intersection point
        Vector3 lightDir = (intersectionPoint - pl.light_pos);
        double lightDistance = lightDir.length();
        lightDir = lightDir.normalized();
        
        // Create shadow ray (from light to intersection point)
        Ray lightRay(pl.light_pos, lightDir);
        
        // Check if intersection point is in shadow
        bool inShadow = false;
        for (Object* obj : objects) {
            double shadowT = obj->intersect(&lightRay, nullptr, 0);
            if (shadowT > 0 && shadowT < lightDistance - 1e-6) {
                inShadow = true;
                break;
            }
        }
        
        if (!inShadow) {
            // Calculate Lambert value (diffuse component)
            Vector3 lightToPoint = (pl.light_pos - intersectionPoint).normalized();
            double lambertValue = std::max(0.0f, normal.dot(lightToPoint));
            
            // Calculate reflected ray direction
            Vector3 reflectedRay = lightToPoint - normal * (2.0 * normal.dot(lightToPoint));
            reflectedRay = reflectedRay.normalized();
            
            // Calculate Phong value (specular component)
            Vector3 viewDir = (r->start - intersectionPoint).normalized();
            double phongValue = std::max(0.0f, viewDir.dot(reflectedRay));
            phongValue = pow(phongValue, shine);
            
            // Add diffuse component
            color[0] += pl.color.r * coEfficients.diffuse * lambertValue * intersectionPointColor.r;
            color[1] += pl.color.g * coEfficients.diffuse * lambertValue * intersectionPointColor.g;
            color[2] += pl.color.b * coEfficients.diffuse * lambertValue * intersectionPointColor.b;
            
            // Add specular component
            color[0] += pl.color.r * coEfficients.specular * phongValue * intersectionPointColor.r;
            color[1] += pl.color.g * coEfficients.specular * phongValue * intersectionPointColor.g;
            color[2] += pl.color.b * coEfficients.specular * phongValue * intersectionPointColor.b;
        }
    }
    
    // Process each spot light
    for (const auto& sl : spotLights) {
        // Cast ray from light to intersection point
        Vector3 lightDir = (intersectionPoint - sl.point_light.light_pos);
        double lightDistance = lightDir.length();
        lightDir = lightDir.normalized();
        
        // Check if point is within spotlight cone
        double angle = acos(lightDir.dot(sl.light_direction.normalized())) * 180.0 / M_PI;
        if (angle > sl.cutoff_angle) {
            continue; // Point is outside spotlight cone
        }
        
        // Create shadow ray (from light to intersection point)
        Ray lightRay(sl.point_light.light_pos, lightDir);
        
        // Check if intersection point is in shadow
        bool inShadow = false;
        for (Object* obj : objects) {
            double shadowT = obj->intersect(&lightRay, nullptr, 0);
            if (shadowT > 0 && shadowT < lightDistance - 1e-6) {
                inShadow = true;
                break;
            }
        }
        
        if (!inShadow) {
            // Calculate Lambert value (diffuse component)
            Vector3 lightToPoint = (sl.point_light.light_pos - intersectionPoint).normalized();
            double lambertValue = std::max(0.0f, normal.dot(lightToPoint));
            
            // Calculate reflected ray direction
            Vector3 reflectedRay = lightToPoint - normal * (2.0 * normal.dot(lightToPoint));
            reflectedRay = reflectedRay.normalized();
            
            // Calculate Phong value (specular component)
            Vector3 viewDir = (r->start - intersectionPoint).normalized();
            double phongValue = std::max(0.0f, viewDir.dot(reflectedRay));
            phongValue = pow(phongValue, shine);
            
            // Add diffuse component
            color[0] += sl.point_light.color.r * coEfficients.diffuse * lambertValue * intersectionPointColor.r;
            color[1] += sl.point_light.color.g * coEfficients.diffuse * lambertValue * intersectionPointColor.g;
            color[2] += sl.point_light.color.b * coEfficients.diffuse * lambertValue * intersectionPointColor.b;
            
            // Add specular component
            color[0] += sl.point_light.color.r * coEfficients.specular * phongValue * intersectionPointColor.r;
            color[1] += sl.point_light.color.g * coEfficients.specular * phongValue * intersectionPointColor.g;
            color[2] += sl.point_light.color.b * coEfficients.specular * phongValue * intersectionPointColor.b;
        }
    }
    
    // Handle recursive reflection (following the spec exactly)
    if (level >= recursionLevel) {
        return t; // Stop recursion
    }
    
    // Calculate reflection direction
    Vector3 viewDir = r->dir;
    Vector3 reflectionDir = viewDir - normal * (2.0 * normal.dot(viewDir));
    reflectionDir = reflectionDir.normalized();
    
    // Create reflected ray (offset slightly forward in the reflection direction)
    Vector3 reflectionStart = intersectionPoint + reflectionDir * 1e-6;
    Ray reflectedRay(reflectionStart, reflectionDir);
    
    // Find nearest intersecting object for reflected ray
    double tMinReflected = -1;
    int nearestReflected = -1;
    
    for (int i = 0; i < objects.size(); i++) {
        double tReflected = objects[i]->intersect(&reflectedRay, nullptr, 0);
        if (tReflected > 0 && (tMinReflected < 0 || tReflected < tMinReflected)) {
            tMinReflected = tReflected;
            nearestReflected = i;
        }
    }
    
    if (nearestReflected != -1) {
        double colorReflected[3] = {0, 0, 0};
        objects[nearestReflected]->intersect(&reflectedRay, colorReflected, level + 1);
        
        // Add reflection component to color
        color[0] += colorReflected[0] * coEfficients.reflection;
        color[1] += colorReflected[1] * coEfficients.reflection;
        color[2] += colorReflected[2] * coEfficients.reflection;
    }
    
    return t;
}

// Override the getColorAt method
Color GeneralQuadratic::getColorAt(const Vector3& point) {
    return color;
}

// Override the getNormalAt method
Vector3 GeneralQuadratic::getNormalAt(const Vector3& point) {
    return getGradientAt(point).normalized();
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
