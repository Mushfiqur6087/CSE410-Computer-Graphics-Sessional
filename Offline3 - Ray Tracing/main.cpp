#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
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
#include "globals.h"

using namespace std;

// Global vectors for objects and lights
vector<Object*> objects;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;

// Global variables for scene configuration
int recursionLevel;
int imageWidth;

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
    floorObj->setCoEfficients(0.4, 0.2, 0.2, 0.2); // ambient, diffuse, specular, reflection
    floorObj->setShine(10);
    objects.push_back(floorObj);
    
    cout << "Scene loaded successfully!" << endl;
    cout << endl;
    
    // Print detailed scene information
    printSceneDetails();
}

// Function to print detailed object information
void printObjectDetails(Object* obj, int index) {
    cout << "Object " << index << ":" << endl;
    
    // Try to determine object type using dynamic_cast
    if (dynamic_cast<Sphere*>(obj)) {
        Sphere* sphere = dynamic_cast<Sphere*>(obj);
        cout << "  Type: Sphere" << endl;
        cout << "  Center: (" << sphere->getCenter().x << ", " << sphere->getCenter().y << ", " << sphere->getCenter().z << ")" << endl;
        cout << "  Radius: " << sphere->getRadius() << endl;
    }
    else if (dynamic_cast<Triangle*>(obj)) {
        Triangle* triangle = dynamic_cast<Triangle*>(obj);
        cout << "  Type: Triangle" << endl;
        cout << "  Vertex 1: (" << triangle->getVertex1().x << ", " << triangle->getVertex1().y << ", " << triangle->getVertex1().z << ")" << endl;
        cout << "  Vertex 2: (" << triangle->getVertex2().x << ", " << triangle->getVertex2().y << ", " << triangle->getVertex2().z << ")" << endl;
        cout << "  Vertex 3: (" << triangle->getVertex3().x << ", " << triangle->getVertex3().y << ", " << triangle->getVertex3().z << ")" << endl;
    }
    else if (dynamic_cast<GeneralQuadratic*>(obj)) {
        GeneralQuadratic* quad = dynamic_cast<GeneralQuadratic*>(obj);
        cout << "  Type: General Quadratic" << endl;
        double a, b, c, d, e, f, g, h, i, j;
        quad->getCoefficients(a, b, c, d, e, f, g, h, i, j);
        cout << "  Equation: " << a << "x² + " << b << "y² + " << c << "z² + " << d << "xy + " << e << "xz + " << f << "yz + " << g << "x + " << h << "y + " << i << "z + " << j << " = 0" << endl;
        cout << "  Reference Point: (" << quad->getReferencePoint().x << ", " << quad->getReferencePoint().y << ", " << quad->getReferencePoint().z << ")" << endl;
    }
    else if (dynamic_cast<Floor*>(obj)) {
        Floor* floor = dynamic_cast<Floor*>(obj);
        cout << "  Type: Checkerboard Floor" << endl;
        cout << "  Floor Width: " << floor->getFloorWidth() << endl;
        cout << "  Tile Width: " << floor->getTileWidth() << endl;
        cout << "  Reference Point: (" << floor->getReferencePoint().x << ", " << floor->getReferencePoint().y << ", " << floor->getReferencePoint().z << ")" << endl;
    }
    else {
        cout << "  Type: Unknown Object" << endl;
    }
    
    // Print common properties
    cout << "  Color: (" << obj->color.r << ", " << obj->color.g << ", " << obj->color.b << ")" << endl;
    cout << "  Coefficients - Ambient: " << obj->coEfficients.ambient << ", Diffuse: " << obj->coEfficients.diffuse 
         << ", Specular: " << obj->coEfficients.specular << ", Reflection: " << obj->coEfficients.reflection << endl;
    cout << "  Shine: " << obj->shine << endl;
    cout << endl;
}

// Function to print detailed point light information
void printPointLightDetails(const PointLight& light, int index) {
    cout << "Point Light " << index << ":" << endl;
    cout << "  Position: (" << light.light_pos.x << ", " << light.light_pos.y << ", " << light.light_pos.z << ")" << endl;
    cout << "  Color: (" << light.color.r << ", " << light.color.g << ", " << light.color.b << ")" << endl;
    cout << endl;
}

// Function to print detailed spot light information
void printSpotLightDetails(const SpotLight& light, int index) {
    cout << "Spot Light " << index << ":" << endl;
    cout << "  Position: (" << light.point_light.light_pos.x << ", " << light.point_light.light_pos.y << ", " << light.point_light.light_pos.z << ")" << endl;
    cout << "  Color: (" << light.point_light.color.r << ", " << light.point_light.color.g << ", " << light.point_light.color.b << ")" << endl;
    cout << "  Direction: (" << light.light_direction.x << ", " << light.light_direction.y << ", " << light.light_direction.z << ")" << endl;
    cout << "  Cutoff Angle: " << light.cutoff_angle << " degrees" << endl;
    cout << endl;
}

// Function to print comprehensive scene information
void printSceneDetails() {
    cout << "========================================" << endl;
    cout << "           SCENE DETAILS" << endl;
    cout << "========================================" << endl;
    cout << "Recursion Level: " << recursionLevel << endl;
    cout << "Image Width: " << imageWidth << endl;
    cout << "Total Objects: " << objects.size() << endl;
    cout << "Total Point Lights: " << pointLights.size() << endl;
    cout << "Total Spot Lights: " << spotLights.size() << endl;
    cout << "========================================" << endl;
    cout << endl;
    
    // Print detailed object information
    cout << "OBJECTS:" << endl;
    cout << "--------" << endl;
    for (int i = 0; i < objects.size(); i++) {
        printObjectDetails(objects[i], i + 1);
    }
    
    // Print detailed point light information
    if (!pointLights.empty()) {
        cout << "POINT LIGHTS:" << endl;
        cout << "-------------" << endl;
        for (int i = 0; i < pointLights.size(); i++) {
            printPointLightDetails(pointLights[i], i + 1);
        }
    }
    
    // Print detailed spot light information
    if (!spotLights.empty()) {
        cout << "SPOT LIGHTS:" << endl;
        cout << "------------" << endl;
        for (int i = 0; i < spotLights.size(); i++) {
            printSpotLightDetails(spotLights[i], i + 1);
        }
    }
    
    cout << "========================================" << endl;
}

// Function to draw all objects in the scene
void drawScene() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw all objects
    for (Object* obj : objects) {
        if (obj) {
            obj->draw();
        }
    }
    
    // Flush OpenGL commands
    glFlush();
}

int main(int argc, char** argv) {
    cout << "Loading scene data..." << endl;
    loadData();
    
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ray Tracing Scene");
    
    // Set up OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Main display loop
    glutDisplayFunc([]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw the scene
        drawScene();
        
        glutSwapBuffers();
    });
    
    // Enter the GLUT main loop
    glutMainLoop();
    
    return 0;
}