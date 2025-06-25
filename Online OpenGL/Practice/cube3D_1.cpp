#include <GL/glut.h>

const int WIDTH = 800;
const int HEIGHT = 600;

float angle1 = 0.0f; // Angle for first cube
float angle2 = 0.0f; // Angle for second cube

void draw3DCube(float side) {
    float h = side / 2.0f;

    glBegin(GL_QUADS);
        // Top face (white)
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-h, h, -h);
        glVertex3f( h, h, -h);
        glVertex3f( h, h,  h);
        glVertex3f(-h, h,  h);

        // Bottom face (gray)
        glColor3f(0.7f, 0.7f, 0.7f);
        glVertex3f(-h, -h, -h);
        glVertex3f( h, -h, -h);
        glVertex3f( h, -h,  h);
        glVertex3f(-h, -h,  h);

        // Front face (blue)
        glColor3f(0.0f, 0.5f, 1.0f);
        glVertex3f(-h, -h, h);
        glVertex3f( h, -h, h);
        glVertex3f( h,  h, h);
        glVertex3f(-h,  h, h);

        // Back face (red)
        glColor3f(1.0f, 0.1f, 0.1f);
        glVertex3f(-h, -h, -h);
        glVertex3f( h, -h, -h);
        glVertex3f( h,  h, -h);
        glVertex3f(-h,  h, -h);

        // Left face (green)
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-h, -h, -h);
        glVertex3f(-h, -h,  h);
        glVertex3f(-h,  h,  h);
        glVertex3f(-h,  h, -h);

        // Right face (yellow)
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f( h, -h, -h);
        glVertex3f( h, -h,  h);
        glVertex3f( h,  h,  h);
        glVertex3f( h,  h, -h);

    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    // Camera straight in front, at y=0.5, z=6
    gluLookAt(0.0, 0.5, 6.0,    // Eye position
              0.0, 0.5, 0.0,    // Look at the center
              0.0, 1.0, 0.0);   // Up is Y

    // Draw first cube at x = -1.0
    glPushMatrix();
        glTranslatef(-1.0f, 0.5f, 0.0f);         // Centered at y=0.5
        glRotatef(angle1, 0.0f, 1.0f, 0.0f);     // Rotate around Y axis
        draw3DCube(1.0f);
    glPopMatrix();

    // Draw second cube at x = +1.0
    glPushMatrix();
        glTranslatef(1.0f, 0.5f, 0.0f);
        glRotatef(angle2, 0.0f, 1.0f, 0.0f);     // Rotate around Y axis
        draw3DCube(1.0f);
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
    glutCreateWindow("Two Rotating 3D Cubes");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
