#include <bits/stdc++.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;

// Vector3 structure for handling 3D vector operations
struct Vector3
{
    float x, y, z;

    // Constructor
    Vector3(float X = 0.0f, float Y = 0.0f, float Z = 0.0f) : x(X), y(Y), z(Z) {}

    // Addition
    Vector3 operator+(const Vector3 &other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Subtraction
    Vector3 operator-(const Vector3 &other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    Vector3 &operator+=(const Vector3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3 &operator-=(const Vector3 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // Scalar multiplication
    Vector3 operator*(float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Dot product
    float dot(const Vector3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector3 cross(const Vector3 &other) const
    {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    // Normalize the vector
    void normalize()
    {
        float len = sqrt(x * x + y * y + z * z);
        if (len > 0)
        {
            x /= len;
            y /= len;
            z /= len;
        }
    }
    Vector3 rotate(Vector3 &axis, double deg) const
    {
        double th = deg * M_PI / 180.0;
        axis.normalize();
        Vector3 k = axis;
        Vector3 v1 = (*this) * cos(th);
        Vector3 v2 = k.cross(*this) * sin(th);
        Vector3 v3 = k * (k.dot(*this) * (1 - cos(th)));
        Vector3 res = v1 + v2 + v3;
        return res;
    }

    float norm()
    {
        float n = sqrt(x * x + y * y + z * z);
        return n;
    }

};

struct Camera
{
    Vector3 pos;
    Vector3 look;
    Vector3 up;
    Vector3 right;
    double speed = 0.45;
    double rotSpeed = 4.0;

    Camera()
        : pos(1, 1, 1), look(-3, -3, -3), up(0, 1, 0)
    {
        look.normalize();
        if (fabs(look.dot(up)) < 1e-5)
        {
            up.normalize();
            right = look.cross(up);
            right.normalize();
        }
        else
        {
            right = Vector3(look.y, -look.x, 0);
            right.normalize();
            up = right.cross(look);
            up.normalize();
        }
    }

    // Translation
    void moveForward() { pos += look * speed; }
    void moveBackward() { pos -= look * speed; }
    void moveLeft() { pos -= right * speed; }
    void moveRight() { pos += right * speed; }
    void moveUp() { pos += up * speed; }
    void moveDown() { pos -= up * speed; }
    void moveUpSameRef()
    {
        double prev = pos.norm();
        pos.z += speed;
        double curr = pos.norm();
        double angle = acos((prev * prev + curr * curr - speed * speed) / (2 * prev * curr)) * 180.0 / M_PI;
        look = look.rotate(right, -angle);
        up = up.rotate(right, -angle);
        right = look.cross(up);
        right.normalize();
    }
    void moveDownSameRef()
    {
        double prev = pos.norm();
        pos.z -= speed;
        double curr = pos.norm();
        double angle = acos((prev * prev + curr * curr - speed * speed) / (2 * prev * curr)) * 180.0 / M_PI;
        look = look.rotate(right, angle);
        up = up.rotate(right, angle);
        right = look.cross(up);
        right.normalize();
    }

    // Rotation
    void lookLeft()
    {
        look = look.rotate(up, rotSpeed);
        right = right.rotate(up, rotSpeed);
    }
    void lookRight()
    {
        look = look.rotate(up, -rotSpeed);
        right = right.rotate(up, -rotSpeed);
    }
    void lookUp()
    {
        look = look.rotate(right, rotSpeed);
        up = up.rotate(right, rotSpeed);
    }
    void lookDown()
    {
        look = look.rotate(right, -rotSpeed);
        up = up.rotate(right, -rotSpeed);
    }
    void tiltClockwise()
    {
        right = right.rotate(look, rotSpeed);
        up = up.rotate(look, rotSpeed);
    }
    void tiltCounterClock()
    {
        right = right.rotate(look, -rotSpeed);
        up = up.rotate(look, -rotSpeed);
    }
};

Camera cam;

///////////////////////////////////////////
float speed = 0.2f;
float rotSpeed = 2.0f;
float ballRadius = 0.5f;
Vector3 ball(0.0f, 3.0f, 0.0f); // Ball position
float initialSpeed = 0.5f;
float angle = (rand() % 360) * M_PI / 180.0f;
Vector3 velocity(initialSpeed *cos(angle), 3.0f, initialSpeed *sin(angle));
float gravity = -9.8f, restitution = 0.7f;
bool simulate = false, showArrow = true;
float dt = 0.016f;
float ballAngle = 0.0f;
Vector3 rotationAxis(1.0f, 0.0f, 0.0f); // Axis of rotation for the ball
// Convert degrees to radians
float toRad(float angle)
{
    return angle * M_PI / 180.0f;
}
// Convert radians to degrees
float toDeg(float angle)
{
    return angle * 180.0f / M_PI;
}

// ---------- Floor, Cube, and Ball ----------
void drawCheckeredFloor()
{
    float floorSize = 8.0f;
    int numTiles = 8;
    float tileSize = floorSize / numTiles;
    float start = -floorSize / 2.0f;
    float y = -4.0f;
    for (int i = 0; i < numTiles; ++i)
    {
        for (int j = 0; j < numTiles; ++j)
        {
            glColor3f((i + j) % 2 == 0 ? 1.0f : 0.0f, (i + j) % 2 == 0 ? 1.0f : 0.0f, (i + j) % 2 == 0 ? 1.0f : 0.0f);
            float x = start + i * tileSize;
            float z = start + j * tileSize;
            glBegin(GL_QUADS);
            glVertex3f(x, y, z);
            glVertex3f(x + tileSize, y, z);
            glVertex3f(x + tileSize, y, z + tileSize);
            glVertex3f(x, y, z + tileSize);
            glEnd();
        }
    }
}

void drawColoredCube()
{
    float size = 4.0f;
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glVertex3f(size, size, size);
    glVertex3f(-size, size, size);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, -size, -size);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, size, size);
    glVertex3f(-size, size, -size);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(size, -size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, size, size);
    glVertex3f(size, -size, size);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-size, size, -size);
    glVertex3f(-size, size, size);
    glVertex3f(size, size, size);
    glVertex3f(size, size, -size);
    glEnd();
}

void drawBall()
{
    glPushMatrix();
    glTranslatef(ball.x, ball.y, ball.z);
    glRotatef(ballAngle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
    int stacks = 30;
    int sectors = 30;
    for (int i = 0; i < stacks; ++i)
    {
        float lat0 = M_PI * (-0.5 + (float)i / stacks);
        float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);
        float z0 = ballRadius * sin(lat0);
        float zr0 = ballRadius * cos(lat0);
        float z1 = ballRadius * sin(lat1);
        float zr1 = ballRadius * cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sectors; ++j)
        {
            float lng = 2 * M_PI * (float)(j == sectors ? 0 : j) / sectors;
            float x = cos(lng);
            float y = sin(lng);
            glColor3f((j % 2), 1 - (j % 2), 0);
            glVertex3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
    glPopMatrix();
}

void drawVelocityArrow()
{
    if (!showArrow)
        return;
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(ball.x, ball.y, ball.z);
    glVertex3f(ball.x + velocity.x * 2, ball.y + velocity.y * 2, ball.z + velocity.z * 2);
    glEnd();
}

// Function to reset the ball
void resetBall()
{
    float randomX = (rand() % 8) - 4.0f;
    float randomY = 3.0f;
    float randomZ = (rand() % 8) - 4.0f;
    ball = Vector3(randomX, randomY, randomZ);
    float randomSpeed = (rand() % 5 + 2) * 0.2f;
    float randomAngle = (rand() % 360) * M_PI / 180.0f;
    velocity = Vector3(randomSpeed * cos(randomAngle), 3.0f, randomSpeed * sin(randomAngle));
    ballAngle = 0.0f;
    rotationAxis = Vector3(1.0f, 0.0f, 0.0f);
}

void updateBall()
{
    if (!simulate)
        return;

    velocity.y += gravity * dt;
    ball = ball + velocity * dt;

    Vector3 v = velocity;
    float velMag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

    // Stop the velocity if it is very low
    float velocityThreshold = 0.1f;  // Adjust this threshold as needed
    // printf("Velocity: %f %f %f\n", v.x, v.y, v.z);
    if (sqrt(v.x*v.x) < velocityThreshold)
    {
        velocity.x = 0;
        velocity.y = 0;
        velocity.z = 0;
        // printf("Ball stopped\n");
    }
    else
    {
        if (velMag > 0.001f)
        {
            Vector3 axis = {v.z, 0, -v.x};
            axis.normalize();
            float axislen = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
            if (axislen != 0)
            {
                rotationAxis = axis;
            }
            ballAngle += (velMag / ballRadius) * (180.0f / M_PI) * dt;
        }
    }

    Vector3 min = {-4.0f + ballRadius, -4.0f + ballRadius, -4.0f + ballRadius};
    Vector3 max = {4.0f - ballRadius, 4.0f - ballRadius, 4.0f - ballRadius};

    // Collision handling with the cube boundaries
    if (ball.x < min.x)
    {
        ball.x = min.x;
        velocity.x *= -restitution;
    }
    if (ball.x > max.x)
    {
        ball.x = max.x;
        velocity.x *= -restitution;
    }
    if (ball.y < min.y)
    {
        ball.y = min.y;
        velocity.y *= -restitution;
    }
    if (ball.y > max.y)
    {
        ball.y = max.y;
        velocity.y *= -restitution;
    }
    if (ball.z < min.z)
    {
        ball.z = min.z;
        velocity.z *= -restitution;
    }
    if (ball.z > max.z)
    {
        ball.z = max.z;
        velocity.z *= -restitution;
    }
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    float aspect = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
        cam.pos.x, cam.pos.y, cam.pos.z,
        cam.pos.x + cam.look.x,
        cam.pos.y + cam.look.y,
        cam.pos.z + cam.look.z,
        cam.up.x, cam.up.y, cam.up.z);
    drawCheckeredFloor();
    drawColoredCube();
    drawBall();
    drawVelocityArrow();
    glutSwapBuffers();
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        cam.moveForward();
        break;
    case GLUT_KEY_DOWN:
        cam.moveBackward();
        break;
    case GLUT_KEY_LEFT:
        cam.moveLeft();
        break;
    case GLUT_KEY_RIGHT:
        cam.moveRight();
        break;
    case GLUT_KEY_PAGE_UP:
        cam.moveUp();
        break;
    case GLUT_KEY_PAGE_DOWN:
        cam.moveDown();
        break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':
        cam.lookLeft();
        break;
    case '2':
        cam.lookRight();
        break;
    case '3':
        cam.lookUp();
        break;
    case '4':
        cam.lookDown();
        break;
    case '5':
        cam.tiltClockwise();
        break;
    case '6':
        cam.tiltCounterClock();
        break;
    case 'w':
        cam.moveUpSameRef();
        break;
    case 's':
        cam.moveDownSameRef();
        break;

    case ' ':
        simulate = !simulate;
        break;
    case 'r':
        resetBall();
        break;
    case '+':
        if (!simulate)
        {
            initialSpeed += 1.1f;
            if (initialSpeed < 0)
                initialSpeed = 0;
            // recompute vel dir
            angle = atan2(velocity.z, velocity.x);
            velocity.x = initialSpeed * cos(angle);
            velocity.z = initialSpeed * sin(angle);
        }

        break;
    case '-':
        if (!simulate)
        {
            initialSpeed -= 1.1f;
            if (initialSpeed < 0)
                initialSpeed = 0;
            angle = atan2(velocity.z, velocity.x);
            velocity.x = initialSpeed * cos(angle);
            velocity.z = initialSpeed * sin(angle);
        }

        break;
    case 'v':
        showArrow = !showArrow;
        break;
    case 27:
        exit(0);
    }
    glutPostRedisplay();
}

void idle()
{
    updateBall();
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Camera with Bouncing Ball");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}