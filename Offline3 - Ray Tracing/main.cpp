#include <GL/glut.h>
#include <iostream>
#include "Vector3/Vector3.h"
#include "Camera/Camera.h"

// Global objects
Camera camera;

// Cube rendering variables
float cubeSize = 1.0f;
Vector3 cubePosition(0, 0, 0);
Vector3 cubeRotation(0, 0, 0);
bool wireframeMode = false;

// Window dimensions
int windowWidth = 800;
int windowHeight = 600;

// Function declarations
void initializeOpenGL();
void display();
void reshape(int width, int height);
void specialKeys(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void printControls();
void drawCube();
void drawCubeFace(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, 
                  float r, float g, float b);

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Refactored Cube Viewer with Camera System");
    
    // Initialize OpenGL settings
    initializeOpenGL();
    
    // Print control instructions
    printControls();
    
    // Set up callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    
    // Start the main loop
    glutMainLoop();
    
    return 0;
}

void initializeOpenGL() {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Set background color to dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Enable face culling for better performance
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Set initial camera position
    camera.setPosition(Vector3(0, 0, 10));
    camera.setLookDirection(Vector3(0, 0, -1));
}

void display() {
    // Clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset the modelview matrix
    glLoadIdentity();
    
    // Apply camera transformation
    camera.applyLookAt();
    
    // Draw the cube
    drawCube();
    
    // Swap buffers for smooth animation
    glutSwapBuffers();
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    // Prevent division by zero
    if (height == 0) height = 1;
    
    // Calculate aspect ratio
    float aspectRatio = (float)width / (float)height;
    
    // Set the viewport
    glViewport(0, 0, width, height);
    
    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspectRatio, 0.1, 1000.0);
    
    // Switch back to modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int x, int y) {
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
    // Redraw the scene
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        // Camera rotation controls
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
            
        // Reset and utility controls
        case 'c':
        case 'C':
            camera.reset();
            std::cout << "Camera reset" << std::endl;
            break;
        case 'p':
        case 'P':
            camera.print();
            break;
        case 'h':
        case 'H':
            printControls();
            break;
            
        // Exit
        case 27: // ESC key
        case 'q':
        case 'Q':
            std::cout << "Exiting..." << std::endl;
            exit(0);
            break;
    }
    
    // Redraw the scene
    glutPostRedisplay();
}

void drawCubeFace(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
                  float r, float g, float b) {
    glColor3f(r, g, b);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
    glVertex3f(v4.x, v4.y, v4.z);
}

void drawCube() {
    glPushMatrix();
    
    // Apply cube transformations
    glTranslatef(cubePosition.x, cubePosition.y, cubePosition.z);
    glRotatef(cubeRotation.x, 1, 0, 0);
    glRotatef(cubeRotation.y, 0, 1, 0);
    glRotatef(cubeRotation.z, 0, 0, 1);
    
    float s = cubeSize;
    
    if (wireframeMode) {
        // Draw wireframe cube
        glColor3f(1, 1, 1); // White wireframe
        glBegin(GL_LINES);
        
        // Bottom face
        glVertex3f(-s, -s, -s); glVertex3f( s, -s, -s);
        glVertex3f( s, -s, -s); glVertex3f( s, -s,  s);
        glVertex3f( s, -s,  s); glVertex3f(-s, -s,  s);
        glVertex3f(-s, -s,  s); glVertex3f(-s, -s, -s);
        
        // Top face
        glVertex3f(-s,  s, -s); glVertex3f( s,  s, -s);
        glVertex3f( s,  s, -s); glVertex3f( s,  s,  s);
        glVertex3f( s,  s,  s); glVertex3f(-s,  s,  s);
        glVertex3f(-s,  s,  s); glVertex3f(-s,  s, -s);
        
        // Vertical edges
        glVertex3f(-s, -s, -s); glVertex3f(-s,  s, -s);
        glVertex3f( s, -s, -s); glVertex3f( s,  s, -s);
        glVertex3f( s, -s,  s); glVertex3f( s,  s,  s);
        glVertex3f(-s, -s,  s); glVertex3f(-s,  s,  s);
        
        glEnd();
    } else {
        // Draw solid colored cube
        glBegin(GL_QUADS);
        
        // +Z face (blue)
        drawCubeFace(Vector3(-s, -s,  s), Vector3( s, -s,  s), 
                     Vector3( s,  s,  s), Vector3(-s,  s,  s), 0, 0, 1);
        
        // -Z face (green)
        drawCubeFace(Vector3(-s, -s, -s), Vector3(-s,  s, -s), 
                     Vector3( s,  s, -s), Vector3( s, -s, -s), 0, 1, 0);
        
        // -X face (red)
        drawCubeFace(Vector3(-s, -s, -s), Vector3(-s, -s,  s), 
                     Vector3(-s,  s,  s), Vector3(-s,  s, -s), 1, 0, 0);
        
        // +X face (yellow)
        drawCubeFace(Vector3( s, -s, -s), Vector3( s,  s, -s), 
                     Vector3( s,  s,  s), Vector3( s, -s,  s), 1, 1, 0);
        
        // +Y face (cyan)
        drawCubeFace(Vector3(-s,  s, -s), Vector3(-s,  s,  s), 
                     Vector3( s,  s,  s), Vector3( s,  s, -s), 0, 1, 1);
        
        // -Y face (magenta)
        drawCubeFace(Vector3(-s, -s, -s), Vector3( s, -s, -s), 
                     Vector3( s, -s,  s), Vector3(-s, -s,  s), 1, 0, 1);
        
        glEnd();
    }
    
    glPopMatrix();
}

void printControls() {
    std::cout << "\n=== CONTROLS ===" << std::endl;
    std::cout << "Camera Movement:" << std::endl;
    std::cout << "  Arrow Keys    - Move forward/back/left/right" << std::endl;
    std::cout << "  Page Up/Down  - Move up/down" << std::endl;
    std::cout << "\nCamera Rotation:" << std::endl;
    std::cout << "  1/2           - Look left/right" << std::endl;
    std::cout << "  3/4           - Look up/down" << std::endl;
    std::cout << "  5/6           - Tilt clockwise/counter-clockwise" << std::endl;
    std::cout << "\nCube Controls:" << std::endl;
    std::cout << "  W             - Toggle wireframe mode" << std::endl;
    std::cout << "  R             - Rotate around Y-axis" << std::endl;
    std::cout << "  T             - Rotate around X-axis" << std::endl;
    std::cout << "  Y             - Rotate around Z-axis" << std::endl;
    std::cout << "  +/-           - Increase/decrease cube size" << std::endl;
    std::cout << "\nUtility:" << std::endl;
    std::cout << "  C             - Reset camera" << std::endl;
    std::cout << "  P             - Print camera status" << std::endl;
    std::cout << "  H             - Show this help" << std::endl;
    std::cout << "  ESC/Q         - Exit" << std::endl;
    std::cout << "================\n" << std::endl;
}