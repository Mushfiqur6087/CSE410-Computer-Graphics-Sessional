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

const GLdouble clockRad = 1.1;
vector< GLdouble > _angles= { 0.0, 0.0, 0.0 };

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void drawCircle(GLdouble rad) {
    glBegin(GL_LINE_LOOP);
    for(int i = 0 ; i < 360 ; ++i) {
        GLdouble theta = (i*PI) / 180.0;
        GLdouble x = rad * cos(theta);
        GLdouble y = rad * sin(theta) ;
        glVertex2f(x, y);
    }
    glEnd(); 
}

void getTime(int &hour, int &min, int &sec, GLdouble &milis) {
    struct timeval _time; 
    gettimeofday(&_time, NULL);
    time_t now = _time.tv_sec; 
    struct tm* _time_info = localtime(&now); 
    hour = _time_info->tm_hour % 12;
    hour = (!hour ? 12 : hour);  
    min = _time_info->tm_min;
    sec = _time_info->tm_sec; 
    milis = _time.tv_usec / 1000.0; 
    // cout << hour << " " << min << " " << sec << " " << milis << "\n"; 
}

void drawSquare(GLdouble size) {
    glBegin(GL_QUADS); 
    {
        glVertex2f(-size/2, -size/2);
        glVertex2f(size/2, -size/2);
        glVertex2f(size/2, size/2);
        glVertex2f(-size/2, size/2);
    }
    glEnd(); 
}


void drawHand(GLdouble _angle, GLdouble len, GLdouble wid)
{
    glPushMatrix();
    glRotatef(_angle, 0.0 , 0.0 , 1.0);
    glLineWidth(wid); 
    glBegin(GL_LINES); 
    {
        glVertex2f(0.0, 0.0);
        glVertex2f(len,0.0); 
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

void updateAngle(int ind, GLdouble val) {
    _angles[ind] = fmod(_angles[ind] + val - 360.0, 360.0);
}

void recur(GLdouble len, vector< GLdouble > color, int cnt, int ind) {
    if(cnt > 2) return;
    glPushMatrix();
    glRotatef(_angles[ind], 0, 0, 1);       
    glTranslatef(len, 0.0, 0.0);
    glColor3f(color[0], color[1], color[2]);    
    drawCircle(len * 0.5);  
    drawHand(_angles[ind+1], len * 0.5, 1.0);
    // drawSquare(len);                    
    // drawHand(_angles[1], len, 1.0);  
    recur(len*0.5, {1.0, 1.0, 0.0}, cnt + 1,ind+1);  
    glPopMatrix();
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

    glColor3f(0.0f , 0.0f, 1.0f);
    drawCircle(clockRad);  

    drawHand(_angles[0], clockRad, 1.0);

    int cnt = 0;

    recur(clockRad, {1.0, 0.0, 0.0}, cnt + 1, 0); 
    

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
    updateAngle(0, 0.8);
    updateAngle(1, 1.2);
    updateAngle(2,1.5);
    glutPostRedisplay();
    glutTimerFunc(16, _timer, 0); 
}



int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Online");
    glutDisplayFunc(display);            
    glutReshapeFunc(reshape);
    glutTimerFunc(16, _timer, 0);     
    init();
    glutMainLoop();

    return 0;
}