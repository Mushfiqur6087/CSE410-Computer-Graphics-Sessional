#include <GL/glut.h>
#include <cmath>

const int WIDTH = 600;
const int HEIGHT = 600;

// Rotation angles
float angleY = 0.0f;
float angleZ = 0.0f;
// Rotation speeds
float speedY = 1.0f;
float speedZ = 2.0f;

// Draw a filled circle (hub)
void drawCircle(float cx, float cy, float cz, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, cy, cz);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cx + cos(angle) * radius;
        float y = cy + sin(angle) * radius;
        glVertex3f(x, y, cz);
    }
    glEnd();
}

void drawBlade(float bladeLength, float bladeWidth) {
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(bladeLength,  bladeWidth/2, 0.0f);
        glVertex3f(bladeLength, -bladeWidth/2, 0.0f);
    glEnd();
}

void drawFan() {
    // Draw the hub
    glColor3f(0.9f, 0.9f, 0.2f);
    drawCircle(0.0f, 0.0f, 0.0f, 0.25f, 40);

    // Draw 3 blades, each 120 degrees apart
    int numBlades = 3;
    float bladeLength = 1.7f;
    float bladeWidth = 0.35f;
    for (int i = 0; i < numBlades; ++i) {
        glPushMatrix();
            glRotatef(i * (360.0f / numBlades), 0, 0, 1);
            if(i % 3 == 0) glColor3f(0.1f, 0.8f, 1.0f);
            else if(i % 3 == 1) glColor3f(1.0f, 0.2f, 0.2f);
            else glColor3f(0.1f, 1.0f, 0.3f);
            drawBlade(bladeLength, bladeWidth);
        glPopMatrix();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(0, 0, 8,  0, 0, 0,  0, 1, 0);

    glPushMatrix();
        glRotatef(angleY, 0, 1, 0); // Y-axis rotation
        glRotatef(angleZ, 0, 0, 1); // Z-axis rotation
        drawFan();
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    angleY += speedY; if (angleY >= 360.0f) angleY -= 360.0f;
    angleZ += speedZ; if (angleZ >= 360.0f) angleZ -= 360.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': speedY += 0.3f; break;   // Increase Y-axis speed
        case 's': speedY -= 0.3f; break;   // Decrease Y-axis speed
        case 'e': speedZ += 0.3f; break;   // Increase Z-axis speed
        case 'd': speedZ -= 0.3f; break;   // Decrease Z-axis speed
        case 27:  exit(0); break;          // ESC to quit
    }
    // Clamp speeds to reasonable values (optional)
    if (speedY < 0.0f) speedY = 0.0f;
    if (speedZ < 0.0f) speedZ = 0.0f;
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.15f, 0.13f, 0.19f, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (float)WIDTH/HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Keyboard-controlled Fan (Y & Z Axes)");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
