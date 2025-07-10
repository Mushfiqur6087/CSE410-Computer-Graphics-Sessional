#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <GL/glut.h>
#include "src/Object/Object.h"
#include "src/Sphere/Sphere.h"
#include "src/Triangle/Triangle.h"
#include "src/GeneralQuadratic/GeneralQuadratic.h"
#include "src/Floor/Floor.h"
#include "src/PointLight/PointLight.h"
#include "src/SpotLight/SpotLight.h"
#include "src/Vector3/Vector3.h"
#include "src/Color/Color.h"
#include "src/Ray/Ray.h"
#include "src/Camera/Camera.h"
#include "Spec/bitmap_image.hpp"
#include "globals.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Global vectors for objects and lights
vector<Object*> objects;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;

// Global variables for scene configuration
int recursionLevel;
int imageWidth;

// Camera object
Camera camera;

// Window parameters
double windowWidth = 800;
double windowHeight = 600;
double viewAngle = 45.0;

// Image counter for naming output files
int imageCounter = 1;

void capture() {
    cout << "Capturing image..." << endl;
    
    // Initialize bitmap image and set background color
    bitmap_image image(imageWidth, imageWidth); // Square image
    image.set_all_channels(0, 0, 0); // Black background
    
    // Get camera vectors
    Vector3 eye = camera.getPosition();
    Vector3 l = camera.getLookDirection();
    Vector3 r = camera.getRightDirection();
    Vector3 u = camera.getUpDirection();
    
    // Calculate plane distance
    double planeDistance = (windowHeight / 2.0) / tan((viewAngle * M_PI / 180.0) / 2.0);
    
    // Calculate top-left corner of the image plane
    Vector3 topleft = eye + l * planeDistance - r * (windowWidth / 2.0) + u * (windowHeight / 2.0);
    
    // Calculate pixel spacing
    double du = windowWidth / imageWidth;
    double dv = windowHeight / imageWidth; // Using imageWidth for square image
    
    // Choose middle of the grid cell
    topleft = topleft + r * (0.5 * du) - u * (0.5 * dv);
    
    // Ray tracing loop
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageWidth; j++) {
            // Calculate current pixel position
            Vector3 curPixel = topleft + r * (i * du) + u * (-j * dv);
            
            // Create ray from eye to current pixel
            Vector3 rayDir = (curPixel - eye).normalized();
            Ray ray(eye, rayDir);
            
            // Find nearest intersection
            double tMin = -1;
            int nearest = -1;
            
            // Check intersection with all objects
            for (int k = 0; k < objects.size(); k++) {
                double t = objects[k]->intersect(&ray, nullptr, 0);
                if (t > 0 && (tMin < 0 || t < tMin)) {
                    tMin = t;
                    nearest = k;
                }
            }
            
            // Set pixel color
            if (nearest != -1) {
                double color[3];
                objects[nearest]->intersect(&ray, color, 1); // Start recursion at level 1
                
                // Simple clamping without gamma correction
                color[0] = std::max(0.0, std::min(1.0, color[0]));
                color[1] = std::max(0.0, std::min(1.0, color[1]));
                color[2] = std::max(0.0, std::min(1.0, color[2]));
                
                // Convert to [0, 255]
                int r = (int)(color[0] * 255);
                int g = (int)(color[1] * 255);
                int b = (int)(color[2] * 255);
                
                image.set_pixel(i, j, r, g, b);
            }
            // If no intersection, pixel remains black (background)
        }
    }
    
    // Save image
    string filename = "Output_" + to_string(imageCounter) + ".bmp";
    image.save_image(filename);
    cout << "Image saved as " << filename << endl;
    imageCounter++;
}

void loadData() {
    ifstream file("Spec/scene.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open scene.txt" << endl;
        return;
    }

    // Read recursion level and image width
    file >> recursionLevel;
    file >> imageWidth;

    // Read number of objects
    int numObjects;
    file >> numObjects;

    // Read objects
    for (int i = 0; i < numObjects; i++) {
        string objectType;
        file >> objectType;

        if (objectType == "sphere") {
            // Read sphere data
            Vector3 center;
            double radius;
            Color color;
            double ambient, diffuse, specular, reflection;
            int shine;

            file >> center.x >> center.y >> center.z;
            file >> radius;
            file >> color.r >> color.g >> color.b;
            file >> ambient >> diffuse >> specular >> reflection;
            file >> shine;

            // Create sphere object
            Object* temp = new Sphere(center, radius);
            temp->setColor(color.r, color.g, color.b);
            temp->setCoEfficients(ambient, diffuse, specular, reflection);
            temp->setShine(shine);
            objects.push_back(temp);
        }
        else if (objectType == "triangle") {
            // Read triangle data
            Vector3 v1, v2, v3;
            Color color;
            double ambient, diffuse, specular, reflection;
            int shine;

            file >> v1.x >> v1.y >> v1.z;
            file >> v2.x >> v2.y >> v2.z;
            file >> v3.x >> v3.y >> v3.z;
            file >> color.r >> color.g >> color.b;
            file >> ambient >> diffuse >> specular >> reflection;
            file >> shine;

            // Create triangle object
            Object* temp = new Triangle(v1, v2, v3);
            temp->setColor(color.r, color.g, color.b);
            temp->setCoEfficients(ambient, diffuse, specular, reflection);
            temp->setShine(shine);
            objects.push_back(temp);
        }
        else if (objectType == "general") {
            // Read general quadratic data
            double A, B, C, D, E, F, G, H, I, J;
            Vector3 refPoint;
            double length, width, height;
            Color color;
            double ambient, diffuse, specular, reflection;
            int shine;

            file >> A >> B >> C >> D >> E >> F >> G >> H >> I >> J;
            file >> refPoint.x >> refPoint.y >> refPoint.z >> length >> width >> height;
            file >> color.r >> color.g >> color.b;
            file >> ambient >> diffuse >> specular >> reflection;
            file >> shine;

            // Create general quadratic object
            Object* temp = new GeneralQuadratic(A, B, C, D, E, F, G, H, I, J);
            temp->setColor(color.r, color.g, color.b);
            temp->setCoEfficients(ambient, diffuse, specular, reflection);
            temp->setShine(shine);
            temp->setReferencePoint(refPoint);
            temp->setDimensions(height, width, length);
            objects.push_back(temp);
        }
    }

    // Read number of point lights
    int numPointLights;
    file >> numPointLights;

    // Read point lights
    for (int i = 0; i < numPointLights; i++) {
        Vector3 position;
        Color lightColor;

        file >> position.x >> position.y >> position.z;
        file >> lightColor.r >> lightColor.g >> lightColor.b;

        PointLight pl(position, lightColor);
        pointLights.push_back(pl);
    }

    // Read number of spot lights
    int numSpotLights;
    file >> numSpotLights;

    // Read spot lights
    for (int i = 0; i < numSpotLights; i++) {
        Vector3 position;
        Color lightColor;
        Vector3 direction;
        double cutoffAngle;

        file >> position.x >> position.y >> position.z;
        file >> lightColor.r >> lightColor.g >> lightColor.b;
        file >> direction.x >> direction.y >> direction.z;
        file >> cutoffAngle;

        PointLight pl(position, lightColor);
        SpotLight sl(pl, direction, cutoffAngle);
        spotLights.push_back(sl);
    }

    file.close();
    
    // Create and add floor
    Object* floorObj = new Floor(1000, 20);
    floorObj->setColor(1.0, 1.0, 1.0); // Default white color (will be overridden by checkerboard pattern)
    floorObj->setCoEfficients(0.2, 0.4, 0.2, 0.3); // ambient, diffuse, specular, reflection (increased reflection)
    floorObj->setShine(10);
    objects.push_back(floorObj);
    
    cout << "Scene loaded successfully!" << endl;
}

// Function to draw all objects in the scene
void drawScene() {
    // Clear the screen first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Apply camera transformation
    camera.applyLookAt();
    
    // Enable lighting for better visualization
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Set up a basic light (using first point light if available)
    if (!pointLights.empty()) {
        GLfloat light_position[] = {
            (float)pointLights[0].light_pos.x, 
            (float)pointLights[0].light_pos.y, 
            (float)pointLights[0].light_pos.z, 
            1.0f
        };
        GLfloat light_color[] = {
            (float)pointLights[0].color.r, 
            (float)pointLights[0].color.g, 
            (float)pointLights[0].color.b, 
            1.0f
        };
        
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    } else {
        // Default light if no point lights
        GLfloat light_position[] = {100.0f, 100.0f, 100.0f, 1.0f};
        GLfloat light_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
        
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    }
    
    // Set ambient light
    GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    
    // Debug: Print drawing status
    static bool printed = false;
    if (!printed) {
        cout << "Drawing " << objects.size() << " objects and " << pointLights.size() << " point lights" << endl;
        printed = true;
    }
    
    // Draw all objects
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]) {
            objects[i]->draw();
        }
    }
    
    // Draw all point lights as small spheres
    glDisable(GL_LIGHTING); // Draw lights without lighting
    for (const PointLight& light : pointLights) {
        glPushMatrix();
        glColor3f(light.color.r, light.color.g, light.color.b);
        glTranslatef(light.light_pos.x, light.light_pos.y, light.light_pos.z);
        glutSolidSphere(2.0, 10, 10); // Small sphere to represent light
        glPopMatrix();
    }
    glEnable(GL_LIGHTING); // Re-enable lighting
}

void display() {
    // Draw the scene (includes clearing buffers)
    drawScene();
    
    glutSwapBuffers();
}

void keyboardHandler(unsigned char key, int x, int y) {
    switch(key) {
        // Exit controls
        case 'q':
        case 'Q':
        case 27: // ESC key
            exit(0);
            break;
            
        // Camera movement controls
        case 'w':
        case 'W':
            camera.moveForward();
            break;
        case 's':
        case 'S':
            camera.moveBackward();
            break;
        case 'a':
        case 'A':
            camera.moveLeft();
            break;
        case 'd':
        case 'D':
            camera.moveRight();
            break;
        case 'e':
        case 'E':
            camera.moveUp();
            break;
        case 'x':
        case 'X':
            camera.moveDown();
            break;
            
        // Camera rotation controls
        case 'i':
        case 'I':
            camera.lookUp();
            break;
        case 'k':
        case 'K':
            camera.lookDown();
            break;
        case 'j':
        case 'J':
            camera.lookLeft();
            break;
        case 'l':
        case 'L':
            camera.lookRight();
            break;
        case 'u':
        case 'U':
            camera.tiltCounterClockwise();
            break;
        case 'o':
        case 'O':
            camera.tiltClockwise();
            break;
            
        // Reset camera
        case 'r':
        case 'R':
            camera.reset();
            break;
            
        // Capture image
        case 'c':
        case 'C':
        case ' ': // Space bar
            capture();
            break;
    }
    
    glutPostRedisplay();
}

void specialKeyHandler(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            camera.moveForward();
            break;
        case GLUT_KEY_DOWN:
            camera.moveBackward();
            break;
        case GLUT_KEY_LEFT:
            camera.moveLeft();
            break;
        case GLUT_KEY_RIGHT:
            camera.moveRight();
            break;
        case GLUT_KEY_PAGE_UP:
            camera.moveUp();
            break;
        case GLUT_KEY_PAGE_DOWN:
            camera.moveDown();
            break;
    }
    
    glutPostRedisplay();
}

void printControls() {
    cout << "\n=== CONTROLS ===" << endl;
    cout << "ESC/Q: Exit" << endl;
    cout << "R: Reset camera" << endl;
    cout << "C/Space: Capture ray-traced image" << endl;
    cout << "\nMovement:" << endl;
    cout << "W/S: Move forward/backward" << endl;
    cout << "A/D: Move left/right" << endl;
    cout << "E/X: Move up/down" << endl;
    cout << "Arrow keys: Alternative movement" << endl;
    cout << "\nRotation:" << endl;
    cout << "I/K: Look up/down" << endl;
    cout << "J/L: Look left/right" << endl;
    cout << "U/O: Tilt counter-clockwise/clockwise" << endl;
    cout << "================" << endl;
}

int main(int argc, char** argv) {
    cout << "Loading scene data..." << endl;
    loadData();
    
    // Initialize camera with a good starting position to see the scene
    camera.setPosition(Vector3(150, 150, 80));
    camera.setLookAt(Vector3(0, 30, 25), Vector3(0, 0, 1));
    camera.setMovementSpeed(5.0f);
    camera.setRotationSpeed(2.0f);
    
    cout << "Scene loaded successfully!" << endl;
    printControls();
    
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ray Tracing Scene with Camera Control");
    
    // Set up OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE); // Automatically normalize normals
    glEnable(GL_AUTO_NORMAL); // Automatically generate normals for quadrics
    glShadeModel(GL_SMOOTH); // Smooth shading
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0/600.0, 1.0, 1000.0);
    
    // Set initial modelview matrix
    glMatrixMode(GL_MODELVIEW);
    
    // Set up callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(specialKeyHandler);
    
    // Enter the GLUT main loop
    glutMainLoop();
    
    return 0;
}