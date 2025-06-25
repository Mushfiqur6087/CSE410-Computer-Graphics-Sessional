#include <GL/glut.h>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;

float angle1 = 0.0f; // Triangle 1, X axis
float angle2 = 0.0f; // Triangle 2, Y axis
float angle3 = 0.0f; // Triangle 3, Z axis

// Draws an equilateral triangle centered at (0,0,0) in the XY plane
void drawTriangle(float size) {
    float h = sqrt(3.0f) / 2.0f * size; // height of the equilateral triangle

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        glVertex3f(-size / 2.0f, -h / 3.0f, 0.0f);

        glColor3f(0.0f, 1.0f, 0.0f); // Green
        glVertex3f(size / 2.0f, -h / 3.0f, 0.0f);

        glColor3f(0.0f, 0.0f, 1.0f); // Blue
        glVertex3f(0.0f, 2.0f * h / 3.0f, 0.0f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(0.0, 0.0, 7.0,    // Eye position
              0.0, 0.0, 0.0,    // Look at the center
              0.0, 1.0, 0.0);   // Up is Y

    // Triangle 1: rotates on X axis, placed at x = -1.5
    glPushMatrix();
        glTranslatef(-1.5f, 0.0f, 0.0f);
        glRotatef(angle1, 1.0f, 0.0f, 0.0f);
        drawTriangle(1.5f);
    glPopMatrix();

    // Triangle 2: rotates on Y axis, placed at x = 0
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(angle2, 0.0f, 1.0f, 0.0f);
        drawTriangle(1.5f);
    glPopMatrix();

    // Triangle 3: rotates on Z axis, placed at x = +1.5
    glPushMatrix();
        glTranslatef(1.5f, 0.0f, 0.0f);
        glRotatef(angle3, 0.0f, 0.0f, 1.0f);
        drawTriangle(1.5f);
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    angle1 += 2.0f; if (angle1 >= 360.0f) angle1 -= 360.0f;
    angle2 += 1.3f; if (angle2 >= 360.0f) angle2 -= 360.0f;
    angle3 += 3.2f; if (angle3 >= 360.0f) angle3 -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.12f, 0.12f, 0.15f, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)WIDTH/HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Three Rotating Triangles (X, Y, Z Axes)");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
