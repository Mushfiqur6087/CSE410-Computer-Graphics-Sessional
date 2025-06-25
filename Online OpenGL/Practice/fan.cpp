#include <GL/glut.h>
#include <cmath>

const int WIDTH = 600;
const int HEIGHT = 600;

// Draw a filled circle at (cx, cy, cz) with given radius
void drawCircle(float cx, float cy, float cz, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, cy, cz); // center
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cx + cos(angle) * radius;
        float y = cy + sin(angle) * radius;
        glVertex3f(x, y, cz);
    }
    glEnd();
}

// Draw a single fan blade as a triangle
void drawBlade(float bladeLength, float bladeWidth) {
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, 0.0f);                   // Center (hub)
        glVertex3f(bladeLength,  bladeWidth/2, 0.0f);   // Tip upper
        glVertex3f(bladeLength, -bladeWidth/2, 0.0f);   // Tip lower
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(0, 0, 10,  // eye (z=10)
              0, 0, 0,   // center
              0, 1, 0);  // up

    // Draw the hub (center circle)
    glColor3f(0.9f, 0.9f, 0.2f);
    drawCircle(0.0f, 0.0f, 0.0f, 0.25f, 40);

    // Draw 3 blades, each 120 degrees apart
    int numBlades = 3;
    float bladeLength = 1.7f;
    float bladeWidth = 0.35f;
    for (int i = 0; i < numBlades; ++i) {
        glPushMatrix();
            glRotatef(i * (360.0f / numBlades), 0, 0, 1); // Rotate each blade
            // Alternate colors for fun
            if(i % 3 == 0) glColor3f(0.1f, 0.8f, 1.0f);
            else if(i % 3 == 1) glColor3f(1.0f, 0.2f, 0.2f);
            else glColor3f(0.1f, 1.0f, 0.3f);
            drawBlade(bladeLength, bladeWidth);
        glPopMatrix();
    }

    glutSwapBuffers();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.18f, 0.16f, 0.20f, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (float)WIDTH/HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Fan (Hub + Blades) - Static");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
