#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cmath>

const double PI      = acos(-1.0);
const double RAD     = 6.0;     // circle radius
const int    N_LINES = 12;      // number of spokes

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // black background
    glEnable(GL_DEPTH_TEST);
}

void drawCircle() {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; ++i) {
        double theta = i * PI / 180.0;
        double x     = RAD * std::cos(theta);
        double y     = RAD * std::sin(theta);
        glVertex3f((float)x, (float)y, 0.0f);
    }
    glEnd();
}

void drawSpokes() {
    glBegin(GL_LINES);
    for (int i = 0; i < N_LINES; ++i) {
        double theta = (2.0 * PI * i) / N_LINES;
        double x     = RAD * std::cos(theta);
        double y     = RAD * std::sin(theta);
        // from center (0,0,0) to rim (x,y,0)
        glVertex3f(0.0f,   0.0f, 0.0f);
        glVertex3f((float)x, (float)y, 0.0f);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // simple camera straight on
    gluLookAt(0.0, 0.0, 20.0,   // eye at (0,0,20)
              0.0, 0.0,  0.0,   // look at origin
              0.0, 1.0,  0.0);  // up = +Y

    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle();

    glColor3f(1.0f, 0.0f, 0.0f);
    drawSpokes();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 640);
    glutCreateWindow("Static Circle + Spokes");

    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
