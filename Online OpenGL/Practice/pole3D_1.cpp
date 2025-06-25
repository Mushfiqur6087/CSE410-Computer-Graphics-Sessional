#include <GL/glut.h>

const int WIDTH = 800;
const int HEIGHT = 600;

// Rotation state
float poleAngle = 0.0f;       // Current rotation angle

// Draws the multicolored pole (same as before)
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
    gluLookAt(2.0, 3.0, 6.0,    // eye position
              0.0, 1.0, 0.0,    // look-at (center)
              0.0, 1.0, 0.0);   // up direction

    // Rotate pole around Y axis (XZ plane rotation)
    glRotatef(poleAngle, 0.0f, 1.0f, 0.0f);

    draw3DPole(0.2f, 0.2f, 2.0f);

    glutSwapBuffers();
}

// Timer for animation (auto-spin)
void timer(int value) {
    poleAngle += 1.0f;
    if (poleAngle >= 360.0f) poleAngle -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// Optional: keyboard control
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': poleAngle -= 5.0f; break;
        case 'd': poleAngle += 5.0f; break;
        case 27: exit(0); // ESC to exit
    }
    glutPostRedisplay();
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
    glutCreateWindow("3D Pole Rotating in XZ Plane");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
