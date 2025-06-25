#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>

static const float PI = 3.14159265f;
static float angle = 0.0f;   // current rotation angle (degrees)

// Draw a wire‐frame circle of radius r centered at the origin
void drawCircle(float r) {
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 360; ++i) {
        float theta = i * PI / 180.0f;
        glVertex2f(r * cosf(theta), r * sinf(theta));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // draw the big circle
    glColor3f(1,1,1);
    drawCircle(0.2f);

    glPushMatrix();
      // ——— First hand ———
      glRotatef(angle, 0,0,1);
      glLineWidth(2.0f);
      glBegin(GL_LINES);
        glVertex2f(0,0);
        glVertex2f(0.2f,0);
      glEnd();

      // ——— Move to tip of first hand and draw second circle + hand ———
      glPushMatrix();
        glTranslatef(0.2f, 0, 0);      // move to tip of first hand
        drawCircle(0.1f);              // small circle

        glRotatef(angle, 0,0,1);      // rotate for second hand
        glLineWidth(1.0f);
        glBegin(GL_LINES);
          glVertex2f(0,0);
          glVertex2f(0.1f,0);          // length = radius of small circle
        glEnd();
      glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
}


void timer(int /*unused*/) {
    angle += 1.0f;              // advance by 1° per frame
    if (angle >= 360.0f) angle -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Simple Clock Face");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
