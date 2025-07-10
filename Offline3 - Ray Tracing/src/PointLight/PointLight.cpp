#include "PointLight.h"
#include <GL/glut.h>

// Constructor
PointLight::PointLight(Vector3 position, Color lightColor) {
    light_pos = position;
    color = lightColor;
}

// Default constructor
PointLight::PointLight() {
    light_pos = Vector3(0, 0, 0);
    color = Color(1.0, 1.0, 1.0);  // Default white light
}

// Draw method to render the point light as a colored point
void PointLight::draw() const {
    glPushMatrix();
    
    // Disable lighting for the light source itself
    glDisable(GL_LIGHTING);
    
    // Set the color of the point light
    glColor3f(color.r, color.g, color.b);
    
    // Set point size for better visibility
    glPointSize(8.0f);
    
    // Draw the point light
    glBegin(GL_POINTS);
    glVertex3f(light_pos.x, light_pos.y, light_pos.z);
    glEnd();
    
    // Re-enable lighting
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}
