#ifdef __linux__
  #include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
  #include <windows.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>

const double PI        = acos(-1.0);
const double clockRad  = 0.6;    // radius of the circle
double       _angle    = 0.0;    // current rotation angle (degrees)
double       _center_x = -1.0;   // circle’s center X
double       _center_y =  0.0;   // circle’s center Y

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void drawCircle(double rad) {
    glPushMatrix();
      // move origin to circle center
      glTranslatef((float)_center_x, (float)_center_y, 0.0f);
      glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 360; ++i) {
            double theta = i * PI / 180.0;
            double x     = rad * std::cos(theta);
            double y     = rad * std::sin(theta);
            glVertex2f((float)x, (float)y);
        }
      glEnd();
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // position the camera
    gluLookAt(0.0, 0.0, 3.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // 1) draw the static circle
    drawCircle(clockRad);

    // 2) draw the rotating hand
    glPushMatrix();
      glTranslatef((float)_center_x, (float)_center_y, 0.0f);
      glRotatef((float)_angle, 0.0f, 0.0f, 1.0f);
      glLineWidth(2.0f);
      glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f((float)clockRad, 0.0f);
      glEnd();
    glPopMatrix();

    // 3) draw the second line parallel to the X-axis at the hand tip
    glPushMatrix();
      // move to circle center
      glTranslatef((float)_center_x, (float)_center_y, 0.0f);
      // rotate into hand direction
      glRotatef((float)_angle, 0.0f, 0.0f, 1.0f);
      // translate out to the tip
      glTranslatef((float)clockRad, 0.0f, 0.0f);
      // undo the rotation so the next line is in world X-direction
      glRotatef((float)-_angle, 0.0f, 0.0f, 1.0f);

      glLineWidth(1.5f);
      glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.5f, 0.0f);  // horizontal line 0.5 units long
      glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if(height == 0) height = 1;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,
                   (float)width / (float)height,
                   0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timerFunc(int /*value*/) {
    _angle = std::fmod(_angle + 2.0, 360.0);
    glutPostRedisplay();
    glutTimerFunc(16, timerFunc, 0);  // ~60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Circle + Rotating Hand + Horizontal Line");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timerFunc, 0);

    glutMainLoop();
    return 0;
}
