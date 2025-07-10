#include "Triangle.h"
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

// Constructor with three vertices
Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;
    
    // Set reference point as centroid
    referencePoint = Vector3(
        (v1.x + v2.x + v3.x) / 3.0,
        (v1.y + v2.y + v3.y) / 3.0,
        (v1.z + v2.z + v3.z) / 3.0
    );
}

// Default constructor
Triangle::Triangle() {
    vertex1 = Vector3(0, 0, 0);
    vertex2 = Vector3(1, 0, 0);
    vertex3 = Vector3(0, 1, 0);
    referencePoint = Vector3(0.33, 0.33, 0);
}

// Override the draw method
void Triangle::draw() {
    glBegin(GL_TRIANGLES);
    {
        glColor3f(color.r, color.g, color.b);
        glVertex3f(vertex1.x, vertex1.y, vertex1.z);
        glVertex3f(vertex2.x, vertex2.y, vertex2.z);
        glVertex3f(vertex3.x, vertex3.y, vertex3.z);
    }
    glEnd();
}

// Override the intersect method using Möller-Trumbore algorithm
double Triangle::intersect(Ray* r, double* color, int level) {
    const double EPSILON = 1e-8;
    
    // Edge vectors
    Vector3 edge1 = vertex2 - vertex1;
    Vector3 edge2 = vertex3 - vertex1;
    
    // Calculate determinant
    Vector3 h = r->dir.cross(edge2);
    double a = edge1.dot(h);
    
    if (a > -EPSILON && a < EPSILON) {
        return -1.0; // Ray is parallel to triangle
    }
    
    double f = 1.0 / a;
    Vector3 s = r->start - vertex1;
    double u = f * s.dot(h);
    
    if (u < 0.0 || u > 1.0) {
        return -1.0;
    }
    
    Vector3 q = s.cross(edge1);
    double v = f * r->dir.dot(q);
    
    if (v < 0.0 || u + v > 1.0) {
        return -1.0;
    }
    
    // Calculate t to find intersection point
    double t = f * edge2.dot(q);
    
    if (t <= EPSILON) {
        return -1.0; // Intersection behind ray origin
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
    
    // Handle recursive reflection
    if (level < recursionLevel) {
        // Calculate reflection direction
        Vector3 viewDir = r->dir;
        Vector3 reflectionDir = viewDir - normal * (2.0 * normal.dot(viewDir));
        reflectionDir = reflectionDir.normalized();
        
        // Create reflected ray (slightly offset to avoid self-intersection)
        Vector3 reflectionStart = intersectionPoint + normal * 1e-6;
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
            double colorReflected[3];
            objects[nearestReflected]->intersect(&reflectedRay, colorReflected, level + 1);
            
            // Add reflection component to color
            color[0] += colorReflected[0] * coEfficients.reflection;
            color[1] += colorReflected[1] * coEfficients.reflection;
            color[2] += colorReflected[2] * coEfficients.reflection;
        }
    }
    
    return t;
}

// Override the getColorAt method
Color Triangle::getColorAt(const Vector3& point) {
    return color;
}

// Override the getNormalAt method
Vector3 Triangle::getNormalAt(const Vector3& point) {
    return getNormal();
}

// Getter methods
Vector3 Triangle::getVertex1() const {
    return vertex1;
}

Vector3 Triangle::getVertex2() const {
    return vertex2;
}

Vector3 Triangle::getVertex3() const {
    return vertex3;
}

// Setter methods
void Triangle::setVertices(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
    vertex1 = v1;
    vertex2 = v2;
    vertex3 = v3;
    
    // Update reference point as centroid
    referencePoint = Vector3(
        (v1.x + v2.x + v3.x) / 3.0,
        (v1.y + v2.y + v3.y) / 3.0,
        (v1.z + v2.z + v3.z) / 3.0
    );
}

// Get normal vector of the triangle
Vector3 Triangle::getNormal() const {
    Vector3 edge1 = vertex2 - vertex1;
    Vector3 edge2 = vertex3 - vertex1;
    return edge1.cross(edge2).normalized();
}

// Get area of the triangle
double Triangle::getArea() const {
    Vector3 edge1 = vertex2 - vertex1;
    Vector3 edge2 = vertex3 - vertex1;
    return 0.5 * edge1.cross(edge2).length();
}
