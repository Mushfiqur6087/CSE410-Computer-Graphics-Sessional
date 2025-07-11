#include "Floor.h"
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

using namespace std;

// Constructor
Floor::Floor(double floorWidth, double tileWidth) : floorWidth(floorWidth), tileWidth(tileWidth) {
    // Set reference point to center the floor at origin
    referencePoint = Vector3(-floorWidth / 2, -floorWidth / 2, 0);
    length = tileWidth;
    width = tileWidth;
    height = 0; // Floor has no height
    
    // Initialize tile colors
    tile_color1 = Color(1.0, 1.0, 1.0); // White tiles
    tile_color2 = Color(0.0, 0.0, 0.0); // Black tiles
}

// Default constructor
Floor::Floor() : floorWidth(1000), tileWidth(20) {
    referencePoint = Vector3(-floorWidth / 2, -floorWidth / 2, 0);
    length = tileWidth;
    width = tileWidth;
    height = 0;
    
    // Initialize tile colors
    tile_color1 = Color(1.0, 1.0, 1.0); // White tiles
    tile_color2 = Color(0.0, 0.0, 0.0); // Black tiles
}

// Override the draw method
void Floor::draw() {
    int tile_count = static_cast<int>(floorWidth / tileWidth);
    int tile_size = tileWidth;
    int start_x = -tile_count * tile_size / 2;
    int start_y = -tile_count * tile_size / 2;
    
    glBegin(GL_QUADS);
    {
        for (int i = 0; i < tile_count; i++)
        {
            for (int j = 0; j < tile_count; j++)
            {
                if ((i + j) % 2 == 0)
                {
                    glColor3f(tile_color1.r, tile_color1.g, tile_color1.b);
                }
                else
                {
                    glColor3f(tile_color2.r, tile_color2.g, tile_color2.b);
                }

                glVertex3f(start_x + i * tile_size, start_y + j * tile_size, height);
                glVertex3f(start_x + (i + 1) * tile_size, start_y + j * tile_size, height);
                glVertex3f(start_x + (i + 1) * tile_size, start_y + (j + 1) * tile_size, height);   
                glVertex3f(start_x + i * tile_size, start_y + (j + 1) * tile_size, height);
            }
        }
    }
    glEnd();
}

// Override the intersect method
double Floor::intersect(Ray* r, double* color, int level) {
    // Floor is at z = 0 plane
    double t = -r->start.z / r->dir.z;
    
    if (t <= 0) {
        return -1.0; // Behind ray origin or parallel to plane
    }
    
    // Calculate intersection point
    Vector3 intersectionPoint = r->start + r->dir * t;
    
    // Check if intersection is within floor bounds
    double halfWidth = floorWidth / 2.0;
    if (intersectionPoint.x < -halfWidth || intersectionPoint.x > halfWidth ||
        intersectionPoint.y < -halfWidth || intersectionPoint.y > halfWidth) {
        return -1.0; // Outside floor bounds
    }
    
    // If level is 0, just return distance for nearest object detection
    if (level == 0) {
        return t;
    }
    
    // Get color at intersection point
    Color intersectionPointColor = getColorAt(intersectionPoint);
    
    // Initialize color with ambient component
    color[0] = intersectionPointColor.r * coEfficients.ambient;
    color[1] = intersectionPointColor.g * coEfficients.ambient;
    color[2] = intersectionPointColor.b * coEfficients.ambient;
    
    // Calculate normal at intersection point (always pointing up for floor)
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
            // Use negative dot product because lightDir points FROM light TO intersection
            double lambertValue = std::max(0.0, -(double)lightDir.dot(normal));
            
            // Calculate reflected light ray direction (following senior's formula)
            Vector3 reflectedLightRay = lightDir - normal * (2.0 * lightDir.dot(normal));
            reflectedLightRay = reflectedLightRay.normalized();
            
            // Calculate Phong value (specular component)
            // viewDir should be the incoming ray direction (eye to intersection)
            Vector3 viewDir = r->dir;  // This is already eye-to-intersection direction
            double phongValue = std::max(0.0, -(double)reflectedLightRay.dot(viewDir));
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
            // Use negative dot product because lightDir points FROM light TO intersection
            double lambertValue = std::max(0.0, -(double)lightDir.dot(normal));
            
            // Calculate reflected light ray direction (following senior's formula)
            Vector3 reflectedLightRay = lightDir - normal * (2.0 * lightDir.dot(normal));
            reflectedLightRay = reflectedLightRay.normalized();
            
            // Calculate Phong value (specular component)
            // viewDir should be the incoming ray direction (eye to intersection)
            Vector3 viewDir = r->dir;  // This is already eye-to-intersection direction
            double phongValue = std::max(0.0, -(double)reflectedLightRay.dot(viewDir));
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
    Vector3 viewDir = r->dir;  // This is the incoming ray direction (eye to intersection)
    Vector3 reflectionDir = viewDir - normal * (2.0 * viewDir.dot(normal));
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
    
    // Clamp color values to [0, 1] range (borrowed from senior's approach)
    color[0] = std::min(1.0, color[0]);
    color[1] = std::min(1.0, color[1]);
    color[2] = std::min(1.0, color[2]);
    color[0] = std::max(0.0, color[0]);
    color[1] = std::max(0.0, color[1]);
    color[2] = std::max(0.0, color[2]);
    
    return t;
}

// Override the getColorAt method
Color Floor::getColorAt(const Vector3& point) {
    return getTileColor(point.x, point.y);
}

// Override the getNormalAt method
Vector3 Floor::getNormalAt(const Vector3& point) {
    return Vector3(0, 0, 1); // Floor normal always points up
}

// Getter methods
double Floor::getFloorWidth() const {
    return floorWidth;
}

double Floor::getTileWidth() const {
    return tileWidth;
}

// Setter methods
void Floor::setFloorWidth(double width) {
    floorWidth = width;
    referencePoint = Vector3(-floorWidth / 2, -floorWidth / 2, 0);
}

void Floor::setTileWidth(double width) {
    tileWidth = width;
    length = tileWidth;
    this->width = tileWidth;
}

// Utility method to determine if a tile should be white (true) or black (false)
bool Floor::isWhiteTile(double x, double y) const {
    // Convert world coordinates to tile indices
    // Add floorWidth/2 to shift origin to corner, then divide by tileWidth
    double adjustedX = x + floorWidth / 2.0;
    double adjustedY = y + floorWidth / 2.0;
    
    int tileX = (int)floor(adjustedX / tileWidth);
    int tileY = (int)floor(adjustedY / tileWidth);
    
    // Ensure we're within bounds
    if (adjustedX < 0 || adjustedX >= floorWidth || adjustedY < 0 || adjustedY >= floorWidth) {
        return false; // Black for out of bounds
    }
    
    // Checkerboard pattern: white if sum of indices is even, black if odd
    return (tileX + tileY) % 2 == 0;
}

// Get the color of a tile at given coordinates
Color Floor::getTileColor(double x, double y) const {
    // Convert world coordinates to tile indices
    // Add floorWidth/2 to shift origin to corner, then divide by tileWidth
    double adjustedX = x + floorWidth / 2.0;
    double adjustedY = y + floorWidth / 2.0;
    
    int tileX = (int)floor(adjustedX / tileWidth);
    int tileY = (int)floor(adjustedY / tileWidth);
    
    // Ensure we're within bounds
    if (adjustedX < 0 || adjustedX >= floorWidth || adjustedY < 0 || adjustedY >= floorWidth) {
        return tile_color2; // Black for out of bounds
    }
    
    if ((tileX + tileY) % 2 == 0) {
        return tile_color1; // White tile
    } else {
        return tile_color2; // Black tile
    }
}
