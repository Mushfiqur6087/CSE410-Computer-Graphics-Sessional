#include "Sphere.h"
#include "../Ray/Ray.h"
#include "../PointLight/PointLight.h"
#include "../SpotLight/SpotLight.h"
#include "../../globals.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor with center and radius
Sphere::Sphere(Vector3 center, double radius) {
    referencePoint = center;
    length = radius;  // Using length to store radius
    height = width = radius;  // For consistency
}

// Default constructor
Sphere::Sphere() {
    referencePoint = Vector3(0, 0, 0);
    length = height = width = 1.0;  // Default radius of 1
}

// Override the draw method
void Sphere::draw() {
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(referencePoint.x, referencePoint.y, referencePoint.z);
        glutSolidSphere(length, 200, 200);
    }
    glPopMatrix();
}

// Override the intersect method
double Sphere::intersect(Ray* r, double* color, int level) {
    // Ray-sphere intersection
    // Ray: P = start + t * dir
    // Sphere: (P - center)^2 = radius^2
    // Substituting: (start + t*dir - center)^2 = radius^2
    
    Vector3 oc = r->start - referencePoint;  // Vector from center to ray start
    double a = r->dir.dot(r->dir);           // Should be 1 if normalized
    double b = 2.0 * oc.dot(r->dir);
    double c = oc.dot(oc) - length * length; // length stores radius
    
    double discriminant = b * b - 4 * a * c;
    
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
            if (shadowT > 0 && shadowT < lightDistance - 1e-6) { // Small epsilon to avoid self-intersection
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
            if (shadowT > 0 && shadowT < lightDistance - 1e-6) { // Small epsilon to avoid self-intersection
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
Color Sphere::getColorAt(const Vector3& point) {
    return color;
}

// Override the getNormalAt method
Vector3 Sphere::getNormalAt(const Vector3& point) {
    return (point - referencePoint).normalized();
}

// Getter methods
Vector3 Sphere::getCenter() const {
    return referencePoint;
}

double Sphere::getRadius() const {
    return length;
}

// Setter methods
void Sphere::setCenter(const Vector3& center) {
    referencePoint = center;
}

void Sphere::setRadius(double radius) {
    length = height = width = radius;
}
