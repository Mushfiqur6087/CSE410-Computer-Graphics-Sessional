/**
 * Advanced OpenGL Demo Application
 *
 * This program demonstrates basic OpenGL functionality including:
 * - Drawing and transformations
 * - Keyboard input (normal and special keys)
 * - Mouse interaction
 * - Animation using both idle and timer functions
 *
 * Use as a learning template for OpenGL with GLUT
 */

// Platform-specific includes
#ifdef __linux__
#include <GL/glut.h> // For Linux systems
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <GL/glut.h> // For Windows systems
#elif defined(__APPLE__)
#include <GLUT/glut.h> // For macOS systems
#else
#include <GL/glut.h> // Default fallback
#endif

#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include <iostream>
#include <vector>
#define PI acos(-1)
using namespace std;

const GLdouble clockRad = 0.6 ;

GLdouble _angle = 0.0;
GLdouble _center_x = -1.0, _waveStart = 2*clockRad + _center_x;
GLdouble _center_y = 0.0;   
vector< GLdouble > allY;
GLint _max = 500; 

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void drawCircle(GLdouble rad) {
    glPushMatrix();
    glTranslatef(_center_x, _center_y, 0.0); 
    glBegin(GL_LINE_LOOP);
    for(int i = 0 ; i < 360 ; ++i) {
        GLdouble theta = (i*PI) / 180.0;
        GLdouble x = rad * cos(theta);
        GLdouble y = rad * sin(theta) ;
        glVertex2f(x, y);
    }
    glEnd(); 
    glPopMatrix(); 
}


void drawLine(GLdouble strt, GLdouble end) {
    glBegin(GL_LINES);
    {
        glVertex2f(0.0, strt); 
        glVertex2f(0.0, end);
    }
    glEnd(); 
}



void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        0.0, 0.0, 3.0,
        0.0, 0.0, 0.0, 
        0.0, 1.0, 0.0  
    );

    allY.insert(allY.begin(), _center_y + clockRad * sin(_angle * PI/ 180.0));
    if(allY.size() > _max) {
        allY.pop_back();
    }
    drawCircle(clockRad); 

    glBegin(GL_LINES);
    {
        glVertex2f(_center_x, _center_y);
        glVertex2f(_center_x + clockRad * cos(_angle * PI/ 180.0), _center_y + clockRad * sin(_angle * PI/ 180.0)); 
    }
    glEnd(); 
    glBegin(GL_LINES);
    {
        glVertex2f(_center_x + clockRad * cos(_angle* PI/ 180.0), _center_y + clockRad * sin(_angle * PI/ 180.0));
        glVertex2f(_waveStart, _center_y + clockRad * sin(_angle * PI/ 180.0)); 
    }
    glEnd();
    glBegin(GL_LINE_STRIP); 
    for(int i = 0 ; i < allY.size() ; ++i) {
        glVertex2f(_waveStart + i * 0.01, allY[i]); 
    }
    glEnd(); 
    glutSwapBuffers();
}




void reshape(int width, int height)
{
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = (float)width / (float)height;
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}


void _timer(int value) {
    _angle = fmod(_angle + 2, 360.0);
    glutPostRedisplay();
    glutTimerFunc(16, _timer, 0); 
}



int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(1800, 800);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Online");
    glutDisplayFunc(display);            
    glutReshapeFunc(reshape);
    glutTimerFunc(16, _timer, 0);     
    init();
    glutMainLoop();

    return 0;
}