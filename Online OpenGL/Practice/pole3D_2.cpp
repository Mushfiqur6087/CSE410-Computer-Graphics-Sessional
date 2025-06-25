#include <GL/glut.h>

const int WIDTH = 800;
const int HEIGHT = 600;

float angle1 = 0.0f; // Angle for first pole
float angle2 = 0.0f; // Angle for second pole

void draw3DPole(float width, float depth, float height) {
    float x = width / 2.0f;
    float z = depth / 2.0f;

    glBegin(GL_QUADS);

        // Bottom face (gray)
        glColor3f(0.7f, 0.7f, 0.7f);
        glVertex3f(-x, 0.0f, -z);
        glVertex3f( x, 0.0f, -z);
        glVertex3f( x, 0.0f,  z);
        glVertex3f(-x, 0.0f,  z);

        // Top face (white)
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-x, height, -z);
        glVertex3f( x, height, -z);
        glVertex3f( x, height,  z);
        glVertex3f(-x, height,  z);

        // Front face (blue)
        glColor3f(0.0f, 0.5f, 1.0f);
        glVertex3f(-x, 0.0f,  z);
        glVertex3f( x, 0.0f,  z);
        glVertex3f( x, height,  z);
        glVertex3f(-x, height,  z);

        // Back face (red)
        glColor3f(1.0f, 0.1f, 0.1f);
        glVertex3f(-x, 0.0f, -z);
        glVertex3f( x, 0.0f, -z);
        glVertex3f( x, height, -z);
        glVertex3f(-x, height, -z);

        // Left face (green)
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-x, 0.0f, -z);
        glVertex3f(-x, 0.0f,  z);
        glVertex3f(-x, height,  z);
        glVertex3f(-x, height, -z);

        // Right face (yellow)
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f( x, 0.0f, -z);
        glVertex3f( x, 0.0f,  z);
        glVertex3f( x, height,  z);
        glVertex3f( x, height, -z);

    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // Set the camera for a straight front view along Z, looking at X axis.
    gluLookAt(0.0, 1.0, 6.0,    // Eye position (in front, at height 1, 6 units away in Z)
              0.0, 1.0, 0.0,    // Look-at (origin at Y=1)
              0.0, 1.0, 0.0);   // Up is +Y

    // Draw first pole at x = -1.0
    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 0.0f);       // Move to left along X, bottom on Y=0
        glRotatef(angle1, 0.0f, 1.0f, 0.0f);   // Rotate around its Y axis (vertical)
        draw3DPole(0.2f, 0.2f, 2.0f);
    glPopMatrix();

    // Draw second pole at x = +1.0
    glPushMatrix();
        glTranslatef(1.0f, 0.0f, 0.0f);        // Move to right along X, bottom on Y=0
        glRotatef(angle2, 0.0f, 1.0f, 0.0f);   // Rotate around its Y axis (vertical)
        draw3DPole(0.2f, 0.2f, 2.0f);
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    angle1 += 1.5f; if (angle1 >= 360.0f) angle1 -= 360.0f;
    angle2 -= 2.5f; if (angle2 < 0.0f) angle2 += 360.0f;
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
    glutCreateWindow("Two Vertical Rotating Poles - Along X Axis");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
