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
double epsilon = 1e-6;
int recursionLevel = 1;  // Default value matching senior's code
int imageWidth, imageHeight;
double fovy = 45.0;
double znear = 1.0;
double zfar = 700.0;

// Camera object
Camera camera;

// Window parameters (matching senior's naming)
double window_width = 800;
double window_height = 600;
double image_width, image_height;
double viewAngle = 45.0;

// Image counter for naming output files
int imageCounter = 1;

void capture() {
    cout << "Capturing image..." << endl;
    
    // Initialize bitmap image and set background color
    bitmap_image image(imageWidth, imageHeight);
    image.set_all_channels(0, 0, 0); // Black background
    
    // Get camera vectors
    Vector3 eye = camera.getPosition();
    Vector3 l = camera.getLookDirection();
    Vector3 r = camera.getRightDirection();
    Vector3 u = camera.getUpDirection();
    
    // Calculate plane distance (matching senior's formula)
    double planeDistance = (window_height / 2.0) / tan((fovy * M_PI / 180.0) / 2.0);
    
    // Calculate top-left corner of the image plane
    Vector3 topleft = eye + l * planeDistance + u * (window_height / 2.0) - r * (window_width / 2.0);
    
    // Calculate pixel spacing (matching senior's approach exactly)
    double du = window_width / imageWidth;
    double dv = window_height / imageHeight;
    
    // Choose middle of the grid cell (matching senior's approach)
    topleft = topleft + r * (du * 0.5) - u * (dv * 0.5);
    
    // Ray tracing loop (single sample per pixel to match senior's approach)
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            Vector3 curPixel = topleft + r * (i * du) - u * (j * dv);
            
            // Create ray from eye to current pixel (matching senior's approach)
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
                
                // Convert to [0, 255] with proper rounding (matching senior's approach)
                int r = (int)round(color[0] * 255);
                int g = (int)round(color[1] * 255);
                int b = (int)round(color[2] * 255);
                
                image.set_pixel(i, j, r, g, b);
            }
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
    imageHeight = imageWidth; // Square image as per senior's code

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
    
    // Create and add floor (matching scene requirements exactly)
    Object* floorObj = new Floor(1000, 20); // floorWidth = 1000 (500 across each side from origin), tileWidth = 20
    floorObj->setColor(1.0, 1.0, 1.0); // Default white color (will be overridden by checkerboard pattern)
    floorObj->setCoEfficients(0.4, 0.2, 0.2, 0.2); // Matching scene coefficients
    floorObj->setShine(10);
    objects.push_back(floorObj);
    
    cout << "Scene loaded successfully!" << endl;
}

// Function to draw all objects in the scene
void drawScene() {
    // Clear the screen first
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Apply camera transformation
    camera.applyLookAt();
    
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
    for (const PointLight& light : pointLights) {
        glPushMatrix();
        glColor3f(light.color.r, light.color.g, light.color.b);
        glTranslatef(light.light_pos.x, light.light_pos.y, light.light_pos.z);
        glutSolidSphere(2.0, 10, 10); // Small sphere to represent light
        glPopMatrix();
    }
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
        case '0': // PDF specification requires '0' key
            capture();
            break;
        case 'c':
        case 'C':
        case ' ': // Space bar
            capture();
            break;
            
        // Number key controls as per PDF specification
        case '1':
            camera.lookLeft();
            break;
        case '2':
            camera.lookRight();
            break;
        case '3':
            camera.lookUp();
            break;
        case '4':
            camera.lookDown();
            break;
        case '5':
            camera.tiltClockwise();
            break;
        case '6':
            camera.tiltCounterClockwise();
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
    cout << "0: Capture ray-traced image (PDF specification)" << endl;
    cout << "C/Space: Capture ray-traced image (alternative)" << endl;
    cout << "\nMovement:" << endl;
    cout << "W/S: Move forward/backward" << endl;
    cout << "A/D: Move left/right" << endl;
    cout << "E/X: Move up/down" << endl;
    cout << "Arrow keys: Alternative movement" << endl;
    cout << "PageUp/PageDown: Move up/down" << endl;
    cout << "\nRotation:" << endl;
    cout << "I/K: Look up/down" << endl;
    cout << "J/L: Look left/right" << endl;
    cout << "U/O: Tilt counter-clockwise/clockwise" << endl;
    cout << "\nNumber Keys (PDF specification):" << endl;
    cout << "1: Look left" << endl;
    cout << "2: Look right" << endl;
    cout << "3: Look up" << endl;
    cout << "4: Look down" << endl;
    cout << "5: Tilt clockwise" << endl;
    cout << "6: Tilt counter-clockwise" << endl;
    cout << "================" << endl;
}

int main(int argc, char** argv) {
    cout << "Loading scene data..." << endl;
    loadData();
    
    // Initialize camera with same position as senior's code
    camera.setPosition(Vector3(0, 300, 300));
    // Senior's look direction is (0, -1, -1), so target should be position + look_direction
    Vector3 lookTarget = Vector3(0, 300, 300) + Vector3(0, -1, -1);
    camera.setLookAt(lookTarget, Vector3(0, -1, 1));
    camera.setMovementSpeed(5.0f);    // Matching senior's camera_change = 5
    camera.setRotationSpeed(10.0f);   // Matching senior's camera_angle_change = 10
    
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