#include <GL/glut.h>
#include <cmath>

#define PI 3.14159265358979323846

int numSpokes = 6;
float radius = 4.0f;
float squareSize = 1.0f;

// Camera settings for 3D view (change these to get different angles!)
float camX = 8.0f, camY = 8.0f, camZ = 10.0f;

void drawCircleXZ(float y, float r, int segments) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * i / segments;
        float x = r * cos(theta);
        float z = r * sin(theta);
        glVertex3f(x, y, z);
    }
    glEnd();
}

void drawSquareXZ(float x, float y, float z, float size, float theta) {
    float half = size / 2.0f;
    float angle = theta + PI/4; // For "diamond" style

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; ++i) {
        float a = angle + i * PI/2;
        float sx = x + half * cos(a);
        float sz = z + half * sin(a);
        float sy = y;
        glVertex3f(sx, sy, sz);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1, 1, 1);

    // Camera: 3D perspective view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        camX, camY, camZ,  // Camera position
        0, 0, 0,           // Look at the origin
        0, 1, 0            // Up is +y
    );

    // Draw central circle at y=0 (XZ plane)
    drawCircleXZ(0.0f, radius, 100);

    for (int i = 0; i < numSpokes; ++i) {
        float theta = 2.0f * PI * i / numSpokes;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        float y = 0.0f;

        // Draw spoke (line from center to circle edge)
        glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(x, y, z);
        glEnd();

        // Draw outward-facing square at (x, y, z)
        drawSquareXZ(x, y, z, squareSize, theta);
    }

    glutSwapBuffers();
}

void init() {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 50.0); // FOV, aspect, near, far
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutCreateWindow("3D Spokes and Squares at y=0 (XZ plane)");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
