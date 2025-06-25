#include <GL/glut.h>
#include <cmath>

// Simple 3D vector
struct Vector3 {
    float x, y, z;
    Vector3(float X=0, float Y=0, float Z=0): x(X), y(Y), z(Z) {}

    // Addition
    Vector3 operator+(const Vector3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    // Subtraction
    Vector3 operator-(const Vector3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    // Compound addition
    Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    // Compound subtraction
    Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

    // Scalar multiplication
    Vector3 operator*(float s) const { return {x*s, y*s, z*s}; }
    // Dot product
    float dot(const Vector3& o) const { return x*o.x + y*o.y + z*o.z; }
    // Cross product
    Vector3 cross(const Vector3& o) const {
        return {
            y*o.z - z*o.y,
            z*o.x - x*o.z,
            x*o.y - y*o.x
        };
    }
    // Normalize
    void normalize() {
        float len = std::sqrt(x*x + y*y + z*z);
        if (len > 1e-6f) { x /= len; y /= len; z /= len; }
    }
    // Rotate around axis by degrees
    Vector3 rotate(const Vector3& axis, float deg) const {
        float rad = deg * (M_PI/180.0f);
        Vector3 k = axis; k.normalize();
        Vector3 v = *this;
        return v * std::cos(rad)
             + k.cross(v) * std::sin(rad)
             + k * (k.dot(v) * (1 - std::cos(rad)));
    }
};

// Camera with position & basis vectors
struct Camera {
    Vector3 pos   = {0, 0, 10};
    Vector3 look  = {0, 0, -1};
    Vector3 up    = {0, 1, 0};
    Vector3 right;

    float speed    = 0.3f;
    float rotSpeed = 4.0f;

    Camera() {
        look.normalize();
        right = look.cross(up);
        right.normalize();
    }

    void moveForward()  { pos += look * speed; }
    void moveBackward() { pos -= look * speed; }
    void moveLeft()     { pos -= right * speed; }
    void moveRight()    { pos += right * speed; }
    void moveUp()       { pos += up * speed; }
    void moveDown()     { pos -= up * speed; }

    void lookLeft()      { rotateAbout(up,  rotSpeed); }
    void lookRight()     { rotateAbout(up, -rotSpeed); }
    void lookUp()        { rotateAbout(right,  rotSpeed); }
    void lookDown()      { rotateAbout(right, -rotSpeed); }
    void tiltClockwise() { rotateAbout(look,  rotSpeed); }
    void tiltCounter()   { rotateAbout(look, -rotSpeed); }

private:
    void rotateAbout(const Vector3& axis, float deg) {
        look  = look.rotate(axis, deg);
        up    = up.rotate(axis, deg);
        right = look.cross(up);
        right.normalize();
    }
} cam;

// Draw a simple colored cube
void drawCube() {
    float s = 1.0f;
    glBegin(GL_QUADS);
    // +Z face (blue)
    glColor3f(0,0,1);
    glVertex3f(-s,-s, s);
    glVertex3f( s,-s, s);
    glVertex3f( s, s, s);
    glVertex3f(-s, s, s);
    // -Z face (green)
    glColor3f(0,1,0);
    glVertex3f(-s,-s,-s);
    glVertex3f(-s, s,-s);
    glVertex3f( s, s,-s);
    glVertex3f( s,-s,-s);
    // -X face (red)
    glColor3f(1,0,0);
    glVertex3f(-s,-s,-s);
    glVertex3f(-s,-s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s,-s);
    // +X face (yellow)
    glColor3f(1,1,0);
    glVertex3f( s,-s,-s);
    glVertex3f( s, s,-s);
    glVertex3f( s, s, s);
    glVertex3f( s,-s, s);
    // +Y face (cyan)
    glColor3f(0,1,1);
    glVertex3f(-s, s,-s);
    glVertex3f(-s, s, s);
    glVertex3f( s, s, s);
    glVertex3f( s, s,-s);
    // -Y face (magenta)
    glColor3f(1,0,1);
    glVertex3f(-s,-s,-s);
    glVertex3f( s,-s,-s);
    glVertex3f( s,-s, s);
    glVertex3f(-s,-s, s);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
        cam.pos.x, cam.pos.y, cam.pos.z,
        cam.pos.x + cam.look.x,
        cam.pos.y + cam.look.y,
        cam.pos.z + cam.look.z,
        cam.up.x, cam.up.y, cam.up.z
    );
    drawCube();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if(h == 0) h = 1;
    float ar = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ar, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int, int) {
    switch(key) {
        case GLUT_KEY_UP:       cam.moveForward();  break;
        case GLUT_KEY_DOWN:     cam.moveBackward(); break;
        case GLUT_KEY_LEFT:     cam.moveLeft();     break;
        case GLUT_KEY_RIGHT:    cam.moveRight();    break;
        case GLUT_KEY_PAGE_UP:  cam.moveUp();       break;
        case GLUT_KEY_PAGE_DOWN:cam.moveDown();     break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    switch(key) {
        case '1': cam.lookLeft();      break;
        case '2': cam.lookRight();     break;
        case '3': cam.lookUp();        break;
        case '4': cam.lookDown();      break;
        case '5': cam.tiltClockwise(); break;
        case '6': cam.tiltCounter();   break;
        case 27:  exit(0);             break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Minimal Cube + Camera");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
