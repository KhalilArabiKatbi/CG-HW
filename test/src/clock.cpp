#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>

// Constants
#define HOUR_HAND_LENGTH 50.0
#define MINUTE_HAND_LENGTH 70.0
#define SECOND_HAND_LENGTH 75.0
#define CLOCK_RADIUS 80.0
#define PI 3.14159265358979323846

// Global variables
double secondAngle = 0, minuteAngle = 0, hourAngle = 0;
float rotateX = 20.0f;
float rotateY = 20.0f;

// Camera variables
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 200.0f;

// Function prototypes
void drawCylinder(float radius, float height, int slices);
void drawClockFace(float radius);
void drawHand(float length, float angle, float z);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

// Initialize OpenGL settings
void init(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
}

// Draw the clock face
void drawClockFace(float radius) {
    glBegin(GL_LINES);
    for (int i = 0; i < 60; i++) {
        float angle = i * 6 * PI / 180;
        float x = radius * sin(angle);
        float y = radius * cos(angle);

        if (i % 5 == 0) {
            // Hour marks
            glVertex3f(0.9 * x, 0.9 * y, 10.1);
            glVertex3f(x, y, 10.1);
        }
        else {
            // Minute marks
            glVertex3f(0.95 * x, 0.95 * y, 10.1);
            glVertex3f(x, y, 10.1);
        }
    }
    glEnd();
}

// Draw a clock hand
void drawHand(float length, float angle, float z) {
    glPushMatrix();
    glRotatef(-angle * 180 / PI, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, z);
    glVertex3f(0, length, z);
    glEnd();
    glPopMatrix();
}

// Draw the clock with cylinder and hands
void drawClock(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, 1, 0);

    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

    // Draw cylinder
    glColor3f(0.8, 0.2, 0.2);
    drawCylinder(CLOCK_RADIUS, 10.0, 32);

    // Draw clock face
    glColor3f(1.0, 1.0, 1.0);
    drawClockFace(CLOCK_RADIUS);

    // Draw clock hands
    glPushMatrix();
    glTranslatef(0, 0, 10); // Move hands to the top of the cylinder

    glLineWidth(5.0);
    glColor3f(1.0f, 0.0f, 1.0f);
    drawHand(HOUR_HAND_LENGTH, hourAngle, 0.3);

    glLineWidth(3.0);
    glColor3f(0.0f, 1.0f, 1.0f);
    drawHand(MINUTE_HAND_LENGTH, minuteAngle, 0.2);

    glLineWidth(1.0);
    glColor3f(0.0f, 0.0f, 1.0f);
    drawHand(SECOND_HAND_LENGTH, secondAngle, 0.1);

    glPopMatrix();

    glutSwapBuffers();
}

// Draw a cylinder
void drawCylinder(float radius, float height, int slices) {
    float x, y, angle;

    // Draw the tube
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        angle = 2.0 * PI * i / slices;
        x = radius * cos(angle);
        y = radius * sin(angle);
        glNormal3f(cos(angle), sin(angle), 0.0);
        glVertex3f(x, y, height);
        glVertex3f(x, y, 0.0);
    }
    glEnd();

    // Draw the top circle
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, height);
    for (int i = 0; i <= slices; i++) {
        angle = 2.0 * PI * i / slices;
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
    }
    glEnd();

    // Draw the bottom circle
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(0.0, 0.0, 0.0);
    for (int i = slices; i >= 0; i--) {
        angle = 2.0 * PI * i / slices;
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, 0.0);
    }
    glEnd();
}

// Handle window reshaping
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

// Update clock hands
void redraw(int timerID) {
    struct timeb tb;
    time_t tim = time(0);
    struct tm* t;
    t = localtime(&tim);
    ftime(&tb);

    secondAngle = (double)(t->tm_sec + (double)tb.millitm / 1000.0) / 30.0 * PI;
    minuteAngle = (double)(t->tm_min) / 30.0 * PI + secondAngle / 60.0;
    hourAngle = (double)(t->tm_hour > 12 ? t->tm_hour - 12 : t->tm_hour) / 6.0 * PI + minuteAngle / 12.0;

    glutPostRedisplay();
    glutTimerFunc(15, redraw, timerID);
}

// Handle keyboard input for camera zoom
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
    case 'W':
        cameraZ -= 5.0f;
        break;
    case 's':
    case 'S':
        cameraZ += 5.0f;
        break;
    }
    glutPostRedisplay();
}

// Handle special keys for camera movement
void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += 5.0f;
        break;
    case GLUT_KEY_UP:
        cameraY += 5.0f;
        break;
    case GLUT_KEY_DOWN:
        cameraY -= 5.0f;
        break;
    }
    glutPostRedisplay();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Clock with Camera Movement");
    init();
    glutDisplayFunc(drawClock);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(15, redraw, 1);
    glutMainLoop();
    return 0;
}