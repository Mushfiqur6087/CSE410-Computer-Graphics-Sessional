#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

// --- Window Dimensions ---
const int WIDTH  = 600;
const int HEIGHT = 600;

// Convert degrees to radians
constexpr float DEG_TO_RAD = 3.14159265f / 180.0f;

// Initialize OpenGL state
void initializeOpenGL() {
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

// Draw a radial background gradient
void drawRadialGradientBackground() {
    const int slices = 100;

    glBegin(GL_TRIANGLE_FAN);
        // Center bright color
        glColor3f(0.2f, 0.2f, 0.4f);
        glVertex2f(0.0f, 0.0f);

        // Edge dark color
        glColor3f(0.05f, 0.05f, 0.1f);
        for (int i = 0; i <= slices; ++i) {
            float angle = i * 2.0f * M_PI / slices;
            glVertex2f(cosf(angle), sinf(angle));
        }
    glEnd();
}

// Draw the clock face and markers
void drawClockFaceWithMarkers() {
    glColor4f(0.2f, 0.4f, 0.8f, 0.9f);  // Soft Blue Color
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 360; ++i) {
            float theta = i * DEG_TO_RAD;
            glVertex2f(cosf(theta) * 0.9f, sinf(theta) * 0.9f);
        }
    glEnd();

    // Outer rim glow with updated color
    glLineWidth(5.0f);
    glColor4f(0.6f, 0.6f, 0.8f, 0.6f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; ++i) {
            float theta = i * DEG_TO_RAD;
            glVertex2f(cosf(theta) * 0.92f, sinf(theta) * 0.92f);
        }
    glEnd();

    // Hour and minute ticks with updated color
    for (int i = 0; i < 60; ++i) {
        float angle = (90.0f - i * 6.0f) * DEG_TO_RAD;
        float outer = 0.9f;
        float inner = (i % 5 == 0) ? 0.78f : 0.85f;
        glLineWidth((i % 5 == 0) ? 5.0f : 2.0f);
        glColor3f(1.0f, 0.9f, 0.9f);
        glBegin(GL_LINES);
            glVertex2f(cosf(angle) * inner, sinf(angle) * inner);
            glVertex2f(cosf(angle) * outer, sinf(angle) * outer);
        glEnd();
    }
}

// Draw clock hands with updated color
void drawClockHand(float angleDeg, float length, float width, float red, float green, float blue) {
    float angle = (90.0f - angleDeg) * DEG_TO_RAD;

    glColor3f(red, green, blue);
    glLineWidth(width);
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(cosf(angle) * length, sinf(angle) * length);
    glEnd();
}

// Draw the central hub of the clock
void drawClockHub() {
    glColor3f(0.8f, 0.4f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        const int segments = 20;
        for (int i = 0; i <= segments; ++i) {
            float theta = i * 2.0f * M_PI / segments;
            glVertex2f(cosf(theta) * 0.03f, sinf(theta) * 0.03f);
        }
    glEnd();
}

// Draw digital time display with updated colors
void drawDigitalClock(int hour, int minute, int second) {
    std::ostringstream timeStream;
    int h12 = hour % 12 == 0 ? 12 : hour % 12;
    bool isPM = hour >= 12;
    timeStream << std::setw(2) << std::setfill('0') << h12
               << ":" << std::setw(2) << std::setfill('0') << minute
               << ":" << std::setw(2) << std::setfill('0') << second
               << (isPM ? " PM" : " AM");
    std::string timeText = timeStream.str();

    // Background box with updated transparency
    glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
    glBegin(GL_QUADS);
        glVertex2f(-0.22f, -0.98f);
        glVertex2f(0.22f, -0.98f);
        glVertex2f(0.22f, -0.90f);
        glVertex2f(-0.22f, -0.90f);
    glEnd();

    // Text in updated yellow color
    glColor3f(1.0f, 1.0f, 0.2f);
    glRasterPos2f(-0.20f, -0.95f);
    for (char c : timeText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Main drawing routine
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawRadialGradientBackground();
    drawClockFaceWithMarkers();

    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    int hours = localTime->tm_hour;
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;

    float secAngle = seconds * 6.0f;
    float minAngle = minutes * 6.0f + seconds * 0.1f;
    float hrAngle  = (hours % 12) * 30.0f + minutes * 0.5f;

    drawClockHand(hrAngle, 0.5f, 8.0f, 0.9f, 0.9f, 0.6f);
    drawClockHand(minAngle, 0.7f, 5.0f, 0.6f, 0.9f, 0.9f);
    drawClockHand(secAngle, 0.85f, 3.0f, 1.0f, 0.3f, 0.3f);

    drawClockHub();
    drawDigitalClock(hours, minutes, seconds);

    glutSwapBuffers();
}

// Window reshape handler
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = width / (float)height;
    if (aspect >= 1.0f)
        gluOrtho2D(-aspect, aspect, -1.0f, 1.0f);
    else
        gluOrtho2D(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect);
}

// Timer callback for real-time updates
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Beautiful Analog Clock");
    initializeOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
