// Windmill Simulation in OpenGL
// Controls: 'a'/'d' to rotate windmill, 'w'/'s' to change blade speed

#include <GL/glut.h>
#include <cmath>

#define PI 3.14159265358979323846

// Global variables
float windmillAngle = 0.0f;
float bladeAngle = 0.0f;
float bladeSpeed = 1.0f;
const float poleHeight = 1.5f;

void drawPole() {
    glColor3f(0.0f, 0.5f, 1.0f); // Blue
    glBegin(GL_QUADS);
        // Front face
        glVertex3f(-0.1f, 0.0f, 0.1f);
        glVertex3f(0.1f, 0.0f, 0.1f);
        glVertex3f(0.1f, poleHeight, 0.1f);
        glVertex3f(-0.1f, poleHeight, 0.1f);

        // Back face
        glVertex3f(-0.1f, 0.0f, -0.1f);
        glVertex3f(0.1f, 0.0f, -0.1f);
        glVertex3f(0.1f, poleHeight, -0.1f);
        glVertex3f(-0.1f, poleHeight, -0.1f);

        // Left face
        glVertex3f(-0.1f, 0.0f, -0.1f);
        glVertex3f(-0.1f, 0.0f, 0.1f);
        glVertex3f(-0.1f, poleHeight, 0.1f);
        glVertex3f(-0.1f, poleHeight, -0.1f);

        // Right face
        glVertex3f(0.1f, 0.0f, -0.1f);
        glVertex3f(0.1f, 0.0f, 0.1f);
        glVertex3f(0.1f, poleHeight, 0.1f);
        glVertex3f(0.1f, poleHeight, -0.1f);
    glEnd();
}

void drawBlade() {
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.5f, 0.1f, 0.0f);
        glVertex3f(0.5f, -0.1f, 0.0f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0, 1.0, 5.0,  // Eye
              0.0, 0.5, 0.0,  // Center
              0.0, 1.0, 0.0); // Up

    glPushMatrix(); {
        // Rotate entire windmill
        glRotatef(windmillAngle, 0.0f, 1.0f, 0.0f);

        drawPole();

        glPushMatrix(); {
            glTranslatef(0.0f, poleHeight, 0.0f); // Move to top of pole
            glRotatef(bladeAngle, 0.0f, 0.0f, 1.0f); // Blade rotation

            for (int i = 0; i < 3; ++i) {
                drawBlade();
                glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
            }
        } glPopMatrix();
    } glPopMatrix();

    glutSwapBuffers();
}

void update(int value) {
    bladeAngle += bladeSpeed;
    if (bladeAngle >= 360.0f) bladeAngle -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': windmillAngle -= 5.0f; break;
        case 'd': windmillAngle += 5.0f; break;
        case 'w': bladeSpeed += 0.5f; break;
        case 's': bladeSpeed -= 0.5f; break;
    }
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Rotating Windmill - CSE 410 Assignment");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}