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

#include <vector>
#include <cmath>

const int     MAX_POINTS = 600;        // number of samples on screen
std::vector<float> samples;            // buffer of sine values
float         phase      = 0.0f;       // running phase
const float   PHASE_INC  = 0.05f;      // smaller step → slower speed & more spread
const float   AMPLITUDE  = 0.3f;       // lower amplitude → shorter height

void init() {
    glClearColor(0, 0, 0, 1);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1) generate new sample
    samples.insert(samples.begin(), AMPLITUDE * std::sin(phase));
    if (samples.size() > MAX_POINTS)
        samples.pop_back();

    // 2) draw the sine‐wave as a line strip
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
      for (int i = 0; i < (int)samples.size(); ++i) {
          // map i=0..MAX_POINTS→x=-1..+1
          float x = -1.0f + 2.0f * i / (MAX_POINTS - 1);
          float y = samples[i];
          glVertex2f(x, y);
      }
    glEnd();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // keep [-1,+1] both axes, with aspect correction
    if (w <= h) {
        float a = (float)h / w;
        glOrtho(-1, 1, -a, +a, -1, 1);
    } else {
        float a = (float)w / h;
        glOrtho(-a, +a, -1, 1, -1, 1);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timerFunc(int /*unused*/) {
    phase += PHASE_INC;            // slower advance
    if (phase > 2*M_PI) phase -= 2*M_PI;
    glutPostRedisplay();
    glutTimerFunc(33, timerFunc, 0);  // ~30 FPS for even slower motion
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 400);
    glutCreateWindow("Spread-out Low-Amplitude Sine Wave");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timerFunc, 0);
    glutMainLoop();
    return 0;
}
