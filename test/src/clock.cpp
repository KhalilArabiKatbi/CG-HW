#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <SOIL/SOIL.h>
#include <iostream>

// Constants
#define HOUR_HAND_LENGTH 50.0
#define MINUTE_HAND_LENGTH 70.0
#define SECOND_HAND_LENGTH 75.0
#define CLOCK_RADIUS 80.0
#define PI 3.14159265358979323846
#define GL_CLAMP_TO_EDGE 0x812F
#define TEX_SIZE 1000.0f

GLfloat skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f
};

// Texture IDs for skybox
GLuint textureID[6];
// Global variables
double secondAngle = 0, minuteAngle = 0, hourAngle = 0;
float rotateX = 0.0f;
float rotateY = 0.0f;

// Camera variables
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 500.0f;

// Function prototypes
void loadSkyboxTextures();
void drawSkybox();
void drawCylinder(float radius, float height, int slices);
void drawClockFace(float radius);
void drawHand(float length, float angle, float z);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

void init(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = {
        .5,.5,.5 , .5
    };

    glLightfv(GL_LIGHT2, GL_POSITION, light_position);

    glClearColor(.5, .5, .5, .5);

    loadSkyboxTextures(); 
}
void loadSkyboxTextures() {
    const char* faces[6] = {
     "C:\\Users\\TECH SHOP\\Documents\\Uni\\CG\\test\\test\\skybox\\right.png",
     "C:\\Users\\TECH SHOP\\Documents\\Uni\\CG\\test\\test\\skybox\\left.png",
     "C:\\Users\\TECH SHOP\\Documents\\Uni\\CG\\test\\test\\skybox\\bottom.png",
     "C:\\Users\\TECH SHOP\\Documents\\Uni\\CG\\test\\test\\skybox\\top.png",
     "C:\\Users\\TECH SHOP\\Documents\\Uni\\CG\\test\\test\\skybox\\front.png",
     "C:\\Users\\TECH SHOP\\Documents\\Uni\\CG\\test\\test\\skybox\\back.png"
    };
    glGenTextures(6, textureID);
    for (int i = 0; i < sizeof(faces) / sizeof(faces[0]); i++) {
        glBindTexture(GL_TEXTURE_2D, textureID[i]); 
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        int width, height;
        unsigned char* image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        if (image) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            SOIL_free_image_data(image);
            std::cout << "Texture loaded: " << faces[i] << std::endl;
        }
        else {
            std::cout << "Failed to load texture: " << faces[i] << std::endl;
            std::cout << "SOIL error: " << SOIL_last_result() << std::endl;
        }

    }
    
}

void drawSkybox() {
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(cameraX, cameraY, cameraZ);
    glDisable(GL_DEPTH_TEST); // Disable depth testing for skybox

    // Front
    glBindTexture(GL_TEXTURE_2D, textureID[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    // Back
    glBindTexture(GL_TEXTURE_2D, textureID[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // Left
    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // Right
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glEnd();

    // Top
    glBindTexture(GL_TEXTURE_2D, textureID[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(TEX_SIZE, TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(-TEX_SIZE, TEX_SIZE, TEX_SIZE);
    glEnd();

    // Bottom
    glBindTexture(GL_TEXTURE_2D, textureID[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glTexCoord2f(1.0, 0.0); glVertex3f(-TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(1.0, 1.0); glVertex3f(TEX_SIZE, -TEX_SIZE, TEX_SIZE);
    glTexCoord2f(0.0, 1.0); glVertex3f(TEX_SIZE, -TEX_SIZE, -TEX_SIZE);
    glEnd();

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
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
            glVertex3f(0.9 * x, 0.9 *    y, 10.1);
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

    drawSkybox();

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
    gluPerspective(45.0, (float)w / h, 5.0, 5000.0); // Set near to 5.0 instead of 1.0
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
        cameraX -= 10.0f;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += 10.0f;
        break;
    case GLUT_KEY_UP:
        cameraY += 10.0f;
        break;
    case GLUT_KEY_DOWN:
        cameraY -= 10.0f;
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