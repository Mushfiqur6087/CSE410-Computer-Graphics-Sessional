#include <GL/glut.h>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;

// Angles for animation (degrees)
float planetAngle = 0.0f;
float moonAngle = 0.0f;

void drawCircle(float radius) {
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 100; ++i) {
        float angle = 2 * M_PI * i / 100;
        glVertex3f(radius * cos(angle), 0.0f, radius * sin(angle));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera setup: slightly above and back
    gluLookAt(0.0, 8.0, 18.0,    // Eye position
              0.0, 0.0, 0.0,     // Look at origin
              0.0, 1.0, 0.0);    // Up is Y

    // Draw Sun at origin
    glPushMatrix();
        glColor3f(1.0f, 0.8f, 0.0f); // Yellow
        glutSolidSphere(1.0f, 50, 50);
    glPopMatrix();

    // Draw planet orbit path
    glColor3f(0.5f, 0.5f, 1.0f); // Light blue
    drawCircle(5.0f);

    // Move to planet's position in orbit
    glPushMatrix();
        glRotatef(planetAngle, 0.0f, 1.0f, 0.0f); // Rotate around sun
        glTranslatef(5.0f, 0.0f, 0.0f);           // Orbit radius

        // Draw Planet
        glColor3f(0.2f, 0.4f, 1.0f); // Blue
        glutSolidSphere(0.5f, 30, 30);

        // Draw moon's orbit path around planet
        glColor3f(0.6f, 0.6f, 0.6f);
        drawCircle(1.2f);

        // Move to moon's position in orbit
        glPushMatrix();
            glRotatef(moonAngle, 0.0f, 1.0f, 0.0f);
            glTranslatef(1.2f, 0.0f, 0.0f);

            // Draw Moon
            glColor3f(0.7f, 0.7f, 0.7f); // Gray
            glutSolidSphere(0.18f, 20, 20);
        glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    planetAngle += 0.6f; if (planetAngle >= 360.0f) planetAngle -= 360.0f;
    moonAngle   += 2.8f; if (moonAngle >= 360.0f) moonAngle -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.07f, 0.06f, 0.15f, 1); // Space-like background

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)WIDTH/HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Solar System (Sun, Planet, Moon)");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
