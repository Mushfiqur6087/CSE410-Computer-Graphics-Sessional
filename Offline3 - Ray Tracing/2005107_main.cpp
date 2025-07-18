#include <bits/stdc++.h>
using namespace std;
#include "2005107_main.hpp"
#include "bitmap_image.hpp"
#include "header/Camera/2005107_Camera.h"

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

// Configuration constants
struct Config {
    static constexpr double EPSILON = 1e-6;
    static constexpr double DEFAULT_FOVY = 45.0;
    static constexpr double DEFAULT_ZNEAR = 1.0;
    static constexpr double DEFAULT_ZFAR = 700.0;
    static constexpr double CAMERA_MOVEMENT_SPEED = 1.0;
    static constexpr double CAMERA_ROTATION_SPEED = 2.0;
    static constexpr int DEFAULT_WINDOW_POS_X = 100;
    static constexpr int DEFAULT_WINDOW_POS_Y = 100;
};

// Global variables definitions
double epsilon = Config::EPSILON;
double recursionLevel = 1;
double fieldOfViewY = Config::DEFAULT_FOVY;
double zNear = Config::DEFAULT_ZNEAR; 
double zFar = Config::DEFAULT_ZFAR;
vector<Object *> objects;
vector<PointLight *> pointLights;
vector<SpotLight *> spotLights;

double cameraMovementSpeed = Config::CAMERA_MOVEMENT_SPEED;
double cameraRotationSpeed = Config::CAMERA_ROTATION_SPEED;

Vector3D initialCameraPosition = Vector3D(0, 300, 300);
Vector3D initialCameraUp = Vector3D(0, -1, 1);
Vector3D initialCameraLook = Vector3D(0, -1, -1);


double windowWidth, windowHeight;
double imageWidth, imageHeight;
double windowPositionX = Config::DEFAULT_WINDOW_POS_X;
double windowPositionY = Config::DEFAULT_WINDOW_POS_Y;

int captureCount = 0;
string outputFileDirectory;
string textureFilePath = "";

Camera camera;

// Forward declarations
void capture();
Color traceRay(const Ray& ray);
Object* createSphere(ifstream& in);
Object* createTriangle(ifstream& in);
Object* createGeneral(ifstream& in);
void initializeFloor();
void adjustMovementSpeed(bool increase);
void adjustRotationSpeed(bool increase);
void displayCameraInfo();
void displayHelp();

// Helper function to create sphere objects
Object* createSphere(ifstream& in) {
    double centerX, centerY, centerZ, radius;
    in >> centerX >> centerY >> centerZ >> radius;
    
    double r, g, b;
    in >> r >> g >> b;
    
    double ambient, diffuse, specular, reflection;
    in >> ambient >> diffuse >> specular >> reflection;
    
    int shininess;
    in >> shininess;
    
    Sphere* sphere = new Sphere(Vector3D(centerX, centerY, centerZ), radius);
    sphere->setColor(Color(r, g, b));
    sphere->setCoefficients(Coefficients(ambient, diffuse, specular, reflection));
    sphere->setShine(shininess);
    
    return sphere;
}

// Helper function to create triangle objects
Object* createTriangle(ifstream& in) {
    double x1, y1, z1, x2, y2, z2, x3, y3, z3;
    in >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
    
    double r, g, b;
    in >> r >> g >> b;
    
    double ambient, diffuse, specular, reflection;
    in >> ambient >> diffuse >> specular >> reflection;
    
    int shininess;
    in >> shininess;
    
    Triangle* triangle = new Triangle(Vector3D(x1, y1, z1), Vector3D(x2, y2, z2), Vector3D(x3, y3, z3));
    triangle->setColor(Color(r, g, b));
    triangle->setCoefficients(Coefficients(ambient, diffuse, specular, reflection));
    triangle->setShine(shininess);
    
    return triangle;
}

// Helper function to create general quadric objects
Object* createGeneral(ifstream& in) {
    double a, b, c, d, e, f, g, h, i, j;
    in >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j;
    
    Vector3D referencePoint;
    double height, width, length;
    in >> referencePoint.x >> referencePoint.y >> referencePoint.z >> height >> width >> length;
    
    double red, green, blue;
    in >> red >> green >> blue;
    
    double ambient, diffuse, specular, reflection;
    in >> ambient >> diffuse >> specular >> reflection;
    
    int shininess;
    in >> shininess;
    
    General* general = new General(a, b, c, d, e, f, g, h, i, j);
    general->setBoundingBox(referencePoint, height, width, length);
    general->setColor(Color(red, green, blue));
    general->setCoefficients(Coefficients(ambient, diffuse, specular, reflection));
    general->setShine(shininess);
    
    return general;
}

// Initialize the checkered floor
void initializeFloor() {
    Floor* checkeredFloor = new Floor(50, 20);
    checkeredFloor->setCoefficients(Coefficients(0.4, 0.2, 0.2, 0.2));
    checkeredFloor->height = 0;
    checkeredFloor->tileColor1 = Color(1, 1, 1);
    checkeredFloor->tileColor2 = Color(0, 0, 0);
    
    // Apply texture if specified
    if (!textureFilePath.empty()) {
        checkeredFloor->setTexture(textureFilePath);
    }
    
    objects.push_back(checkeredFloor);
}

void loadObjects(ifstream &in)
{
    initializeFloor();
    
    int numberOfObjects;
    in >> numberOfObjects;
    
    for (int i = 0; i < numberOfObjects; i++)
    {
        string objectType;
        in >> objectType;
        
        Object* newObject = nullptr;
        
        if (objectType == "sphere") {
            newObject = createSphere(in);
        } else if (objectType == "triangle") {
            newObject = createTriangle(in);
        } else if (objectType == "general") {
            newObject = createGeneral(in);
        } else if (objectType == "general") {
            newObject = createGeneral(in);
        }
        
        if (newObject != nullptr) {
            objects.push_back(newObject);
        }
    }
}

void loadPointLights(ifstream &in)
{
    int numberOfPointLights;
    in >> numberOfPointLights;
    
    for (int i = 0; i < numberOfPointLights; i++)
    {
        double x, y, z;
        in >> x >> y >> z;
        
        double r, g, b;
        in >> r >> g >> b;
        
        PointLight *pointLight = new PointLight();
        pointLight->setLightPosition(x, y, z);
        pointLight->setColor(Color(r, g, b));
        
        pointLights.push_back(pointLight);
    }
}

void loadSpotLights(ifstream &in)
{
    int numberOfSpotLights;
    in >> numberOfSpotLights;
    
    for (int i = 0; i < numberOfSpotLights; i++)
    {
        double x, y, z;
        in >> x >> y >> z;
        
        double r, g, b;
        in >> r >> g >> b;
        
        double directionX, directionY, directionZ;
        in >> directionX >> directionY >> directionZ;
        
        double cutoffAngle;
        in >> cutoffAngle;
        
        SpotLight *spotLight = new SpotLight();
        spotLight->setLightPosition(x, y, z);
        spotLight->setColor(Color(r, g, b));
        spotLight->setLightDirection(directionX, directionY, directionZ);
        spotLight->setCutoffAngle(cutoffAngle);
        
        spotLights.push_back(spotLight);
    }
}

void loadData(string fileName)
{
    ifstream in(fileName);
    double dimension;
    in >> recursionLevel >> dimension;
    windowHeight = windowWidth = dimension;
    imageHeight = imageWidth = dimension;
    loadObjects(in);
    loadPointLights(in);
    loadSpotLights(in);
}

void printInputs()
{
    for (int i = 0; i < objects.size(); i++)
    {
        cout << *objects[i] << endl;
    }
    for (int i = 0; i < pointLights.size(); i++)
    {
        cout << *pointLights[i] << endl;
    }
    for (int i = 0; i < spotLights.size(); i++)
    {
        cout << *spotLights[i] << endl;
    }
}

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLdouble aspectRatio = (GLdouble)windowWidth / (GLdouble)windowHeight;
    gluPerspective(fieldOfViewY, aspectRatio, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);

    // Initialize camera with the original camera values
    camera.setPosition(initialCameraPosition);
    camera.setLookDirection(initialCameraLook);
    camera.setUpDirection(initialCameraUp);
    camera.setMovementSpeed(cameraMovementSpeed);
    camera.setRotationSpeed(cameraRotationSpeed);
}

void drawObjects()
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
}

void drawLights()
{
    for (int i = 0; i < pointLights.size(); i++)
    {
        pointLights[i]->draw();
    }
    for (int i = 0; i < spotLights.size(); i++)
    {
        spotLights[i]->draw();
    }
}

void display()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.applyLookAt();

    drawObjects();
    drawLights();

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    if (height == 0) height = 1; // Prevent divide by zero

    GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfViewY, aspectRatio, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {
    // Capture image
    case '0':
        capture();
        break;
        
    // Rotation controls
    case '1':
        camera.lookLeft();
        cout << "Looking left" << endl;
        break;
    case '2':
        camera.lookRight();
        cout << "Looking right" << endl;
        break;
    case '3':
        camera.lookUp();
        cout << "Looking up" << endl;
        break;
    case '4':
        camera.lookDown();
        cout << "Looking down" << endl;
        break;
    case '5':
        camera.tiltCounterClockwise();
        cout << "Tilting counter-clockwise" << endl;
        break;
    case '6':
        camera.tiltClockwise();
        cout << "Tilting clockwise" << endl;
        break;
        
    // Speed controls
    case 'w':
    case 'W':
        adjustMovementSpeed(true);
        break;
    case 's':
    case 'S':
        adjustMovementSpeed(false);
        break;
    case 'a':
    case 'A':
        adjustRotationSpeed(true);
        break;
    case 'd':
    case 'D':
        adjustRotationSpeed(false);
        break;
        
    // Camera actions
    case 'r':
    case 'R':
        camera.reset();
        cout << "Camera reset to initial position" << endl;
        break;
    case 'c':
    case 'C':
        displayCameraInfo();
        break;
        
    // Help and quit
    case 'h':
    case 'H':
        displayHelp();
        break;
    case 'q':
    case 'Q':
        cout << "Exiting program..." << endl;
        exit(0);
        break;
        
    default:
        cout << "Unknown key: '" << key << "' - Press 'H' for help" << endl;
        break;
    }
}

void keyboardSpecialListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        camera.moveForward();
        cout << "Moving forward" << endl;
        break;
    case GLUT_KEY_DOWN:
        camera.moveBackward();
        cout << "Moving backward" << endl;
        break;
    case GLUT_KEY_RIGHT:
        camera.moveRight();
        cout << "Moving right" << endl;
        break;
    case GLUT_KEY_LEFT:
        camera.moveLeft();
        cout << "Moving left" << endl;
        break;
    case GLUT_KEY_PAGE_UP:
        camera.moveUp();
        cout << "Moving up" << endl;
        break;
    case GLUT_KEY_PAGE_DOWN:
        camera.moveDown();
        cout << "Moving down" << endl;
        break;
    default:
        cout << "Unknown special key - Press 'H' for help" << endl;
        break;
    }
}

void idle()
{
    glutPostRedisplay();
}

void freeMemory()
{
    for (int i = 0; i < objects.size(); i++)
    {
        delete objects[i];
    }
    for (int i = 0; i < pointLights.size(); i++)
    {
        delete pointLights[i];
    }
    for (int i = 0; i < spotLights.size(); i++)
    {
        delete spotLights[i];
    }
}

// Validation and initialization functions
bool validateArguments(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <input_file_path> <output_file_dir> [texture_file_path]" << endl;
        return false;
    }
    return true;
}

void initializeSystem(const string& inputFilePath, const string& outputDirectory) {
    outputFileDirectory = outputDirectory;
    loadData(inputFilePath);
}

void setupGraphics(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(768, 768);
    glutInitWindowPosition(windowPositionX, windowPositionY);
    glutCreateWindow("Ray Tracing");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(keyboardSpecialListener);
    glutIdleFunc(idle);
    
    init();
}

void runMainLoop() {
    glutMainLoop();
}

void cleanup() {
    freeMemory();
}

int main(int argc, char **argv)
{
    if (!validateArguments(argc, argv)) {
        return 1;
    }

    cout << "=== RAY TRACING APPLICATION STARTED ===" << endl;
    cout << "Loading scene data..." << endl;
    
    // Check if texture file path is provided
    if (argc >= 4) {
        textureFilePath = argv[3];
        cout << "Texture file specified: " << textureFilePath << endl;
    }
    
    initializeSystem(argv[1], argv[2]);
    
    cout << "Scene loaded successfully!" << endl;
    cout << "Objects loaded: " << objects.size() << endl;
    cout << "Point lights: " << pointLights.size() << endl;
    cout << "Spot lights: " << spotLights.size() << endl;
    
    setupGraphics(argc, argv);
    
    cout << "\nPress 'H' for help and controls information." << endl;
    cout << "Press '0' to capture/render an image." << endl;
    cout << "Starting interactive mode..." << endl;
    
    runMainLoop();
    cleanup();

    return 0;
}

// Ray tracing function
Color traceRay(const Ray& ray) {
    Color color(0, 0, 0);
    double minimumDistance = -1;
    Object* nearestObject = nullptr;

    // Find the nearest intersecting object
    for (auto object : objects) {
        double intersectionDistance = object->intersect(const_cast<Ray*>(&ray));
        if ((intersectionDistance > 0) && (intersectionDistance < minimumDistance || (nearestObject == nullptr))) {
            minimumDistance = intersectionDistance;
            nearestObject = object;
        }
    }

    // Check if intersection point is within render distance
    if (nearestObject != nullptr) {
        Vector3D intersectionPoint = ray.getOrigin() + (ray.getDirection() * minimumDistance);
        double distanceAlongLookDirection = (intersectionPoint - camera.getPosition()) * camera.getLookDirection();

        if (distanceAlongLookDirection > zFar || distanceAlongLookDirection < zNear) {
            nearestObject = nullptr;
        }
    }

    // Apply lighting if object found
    if (nearestObject != nullptr) {
        nearestObject->phongLighting(const_cast<Ray*>(&ray), &color, 0);
    }

    return color;
}

void capture()
{
    cout << "Capturing image..." << endl;
    bitmap_image image(imageWidth, imageHeight);
    image.clear();

    double planeDistance = (windowHeight / 2.0) / tan(degreeToRadian(fieldOfViewY / 2.0));
    
    Vector3D cameraPosition = camera.getPosition();
    Vector3D cameraLookDirection = camera.getLookDirection();
    Vector3D cameraUpDirection = camera.getUpDirection();
    Vector3D cameraRightDirection = camera.getRightDirection();

    Vector3D topLeftCorner = cameraPosition + (cameraLookDirection * planeDistance) + 
                            (cameraUpDirection * (windowHeight / 2.0)) - 
                            (cameraRightDirection * (windowWidth / 2.0));

    double pixelWidth = windowWidth / imageWidth;
    double pixelHeight = windowHeight / imageHeight;

    topLeftCorner = topLeftCorner + (cameraRightDirection * pixelWidth * 0.5) - 
                   (cameraUpDirection * pixelHeight * 0.5);

    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            Vector3D currentPixel = topLeftCorner + (cameraRightDirection * i * pixelWidth) - 
                                   (cameraUpDirection * j * pixelHeight);

            Vector3D rayDirection = (currentPixel - cameraPosition).normalized();
            Ray ray(cameraPosition, rayDirection);

            Color pixelColor = traceRay(ray);
            
            // Convert to RGB values
            int red = round(pixelColor.getRed() * 255);
            int green = round(pixelColor.getGreen() * 255);
            int blue = round(pixelColor.getBlue() * 255);
            
            image.set_pixel(i, j, red, green, blue);
        }
    }

    captureCount++;
    string filename = outputFileDirectory + "/saved_image-" + to_string(captureCount) + ".bmp";
    image.save_image(filename);
    cout << "Image saved as: " << filename << endl;
}

// Helper functions for enhanced controls
void adjustMovementSpeed(bool increase) {
    if (increase) {
        cameraMovementSpeed = min(cameraMovementSpeed + 1.0, 20.0);
        cout << "Movement speed increased to: " << cameraMovementSpeed << endl;
    } else {
        cameraMovementSpeed = max(cameraMovementSpeed - 1.0, 1.0);
        cout << "Movement speed decreased to: " << cameraMovementSpeed << endl;
    }
    camera.setMovementSpeed(cameraMovementSpeed);
}

void adjustRotationSpeed(bool increase) {
    if (increase) {
        cameraRotationSpeed = min(cameraRotationSpeed + 2.0, 30.0);
        cout << "Rotation speed increased to: " << cameraRotationSpeed << endl;
    } else {
        cameraRotationSpeed = max(cameraRotationSpeed - 2.0, 2.0);
        cout << "Rotation speed decreased to: " << cameraRotationSpeed << endl;
    }
    camera.setRotationSpeed(cameraRotationSpeed);
}

// Display help information
void displayHelp() {
    cout << "\n=== RAY TRACING CONTROLS ===" << endl;
    cout << "CAMERA MOVEMENT:" << endl;
    cout << "  Arrow Keys    - Move Forward/Back/Left/Right" << endl;
    cout << "  Page Up/Down  - Move Up/Down" << endl;
    cout << "\nCAMERA ROTATION:" << endl;
    cout << "  1/2           - Look Left/Right" << endl;
    cout << "  3/4           - Look Up/Down" << endl;
    cout << "  5/6           - Tilt Counter-clockwise/Clockwise" << endl;
    cout << "\nSPEED CONTROLS:" << endl;
    cout << "  +/-           - Increase/Decrease Movement Speed" << endl;
    cout << "  [/]           - Increase/Decrease Rotation Speed" << endl;
    cout << "\nINFORMATION:" << endl;
    cout << "  i             - Display Camera Information" << endl;
    cout << "  h             - Display This Help" << endl;
    cout << "\nRENDERING:" << endl;
    cout << "  0             - Capture/Render Image" << endl;
    cout << "  q             - Quit Application" << endl;
    cout << "===========================\n" << endl;
}

// Display current camera information
void displayCameraInfo() {
    Vector3D pos = camera.getPosition();
    Vector3D look = camera.getLookDirection();
    Vector3D up = camera.getUpDirection();
    
    cout << "\n=== CAMERA INFO ===" << endl;
    cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << endl;
    cout << "Look Direction: (" << look.x << ", " << look.y << ", " << look.z << ")" << endl;
    cout << "Up Direction: (" << up.x << ", " << up.y << ", " << up.z << ")" << endl;
    cout << "Movement Speed: " << cameraMovementSpeed << endl;
    cout << "Rotation Speed: " << cameraRotationSpeed << endl;
    cout << "==================\n" << endl;
}