#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "src/Object/Object.h"
#include "src/Sphere/Sphere.h"
#include "src/Triangle/Triangle.h"
#include "src/GeneralQuadratic/GeneralQuadratic.h"
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
    
    cout << "Scene loaded successfully!" << endl;
    cout << "Objects: " << objects.size() << endl;
    cout << "Point Lights: " << pointLights.size() << endl;
    cout << "Spot Lights: " << spotLights.size() << endl;
}

int main() {
    cout << "Loading scene data..." << endl;
    loadData();
    
    // Your ray tracing implementation will go here
    
    return 0;
}