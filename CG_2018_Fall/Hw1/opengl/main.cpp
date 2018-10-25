#include <Windows.h>
#include <gl/GLU.h>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>
#include "glut.h"

using namespace std;

void display();
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();
void SUN();
void EARTH();
void MOON();
void drawSphere(int nSides);
void SetupLight();

// ===========================
//		Global variables
//============================
int width = 400, height = 400;
int radius = 1;  // Y
float PI = 3.14159265359;
int sun_rad = 6 * radius;
int earth_rad = 2 * radius;
int moon_rad = radius;
int earth_to_sun = sun_rad + 20 + earth_rad;
int moon_to_earth = earth_rad + 3 + moon_rad;

// ===========================
//		For Rot and Rev
//============================
GLfloat degree = 0;  // X
GLfloat earth_rev = 0;
GLfloat earth_rot = 0;
GLfloat moon_rev = 0;
GLfloat moon_rot = 0;

// ===========================
//		For Lighting
//============================
GLfloat AMBIENT[] = {0.5, 0.5, 0.5, 1.0};
GLfloat DIFFUSE[] = {1.0, 1.0, 1.0, 1.0};
GLfloat SPECULAR[] = {1.0, 1.0, 1.0, 1.0};
GLfloat POSITION[] = {0, 10.0, 0, 1.0};

// ===========================
//		Material
//============================
GLfloat sun_mat_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat sun_mat_diffuse[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat sun_mat_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat sun_mat_emission[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat sun_mat_shininess = 1.0f;

GLfloat earth_mat_ambient[] = {0.2f, 0.2f, 0.8f, 1.0f};
GLfloat earth_mat_diffuse[] = {0.2f, 0.2f, 0.8f, 1.0f};
GLfloat earth_mat_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat earth_mat_emission[] = {0.0f, 0.2f, 0.8f, 1.0f};
GLfloat earth_mat_shininess = 10.0f;

GLfloat moon_mat_ambient[] = {0.2f, 0.2f, 0.0f, 1.0f};
GLfloat moon_mat_diffuse[] = {0.2f, 0.2f, 0.0f, 1.0f};
GLfloat moon_mat_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat moon_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat moon_mat_shininess = 10.0f;

// ===========================
//		Keyboard Control
//============================
bool pause = false;
bool switch_slice_stack = true;  // 0 for 360/240
                                 // 1 for 4/2
int nSides = 360;

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);

    // Set up the display window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("CG Hw1 - 0513328");

    // Specify resizing and refreshing routines
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Lighting objects
    SetupLight();
    glutMainLoop();

    return 0;
}

void display() {
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
    // Viewport Matrix
    glViewport(0, 0, width, height);

    // Initialize setting
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0);

    glDepthFunc(GL_LEQUAL);
    glClear(GL_DEPTH_BUFFER_BIT);

    // ===========================
    //			SUN
    //============================
    SUN();

    // ===========================
    //			EARTH
    //============================
    GLfloat tilt = 23.5;
    glRotatef(degree * 28, 0, 1, 0);  // Revolution
    glTranslatef(earth_to_sun, 0, 0);
    glPushMatrix();
    glRotatef(-degree * 28, 0, 1, 0);
    glRotatef(tilt, 0, 0, 1);               // tilt
    glRotatef(degree * 28 * 365, 0, 1, 0);  // Rotation
    EARTH();

    // ===========================
    //			MOON
    //============================
    glPopMatrix();
    glRotatef(degree * 365, 0, 1, 0);  // Revolution
    glTranslatef(moon_to_earth, 0.0, 0.0);
    glRotatef(degree * 365, 0, 1, 0);  // Rotation
    MOON();

    glutSwapBuffers();
}

void reshape(int _width, int _height) {
    width = _width;
    height = _height;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'P':
        case 'p':
            pause = !pause;
            break;
        case 'O':
        case 'o':
            switch_slice_stack = !switch_slice_stack;
            if (switch_slice_stack)
                nSides = 360;
            else
                nSides = 4;
            break;
        default:
            break;
    }
}

void idle() {
    if (!pause) {
        Sleep(20);
        degree += 0.005;
        if (degree >= 360.0) degree -= 360;
    }
    glutPostRedisplay();
}

void SUN() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess);

    // glColor3f(1.0, 0.3, 0.3);
    glPushMatrix();
    glScalef(sun_rad, sun_rad, sun_rad);
    if (nSides != 360) {
        int tmp = nSides;
        nSides = 360;
        drawSphere(360);
        nSides = tmp;
    } else if (nSides == 360) {
        drawSphere(360);
    }
    glPopMatrix();
}

void EARTH() {
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

    // glColor3f(0.3, 0.6, 1.0);
    if (switch_slice_stack) {
        glPushMatrix();
        glScalef(earth_rad, earth_rad, earth_rad);
        drawSphere(360);
        glPopMatrix();
    } else {
        // DrawPlanet(earth_rad, 4, 2);
        glPushMatrix();
        glScalef(earth_rad, earth_rad, earth_rad);
        drawSphere(4);
        glPopMatrix();
    }

    // Draw inclination
    glBegin(GL_LINES);
    glVertex3f(0, -4 * radius, 0);
    glVertex3f(0, 4 * radius, 0);
    glEnd();
    glPopMatrix();
}

void MOON() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, moon_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, moon_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, moon_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, moon_mat_shininess);

    // glColor3f(0.5, 0.5, 0);
    glPushMatrix();
    glScalef(moon_rad, moon_rad, moon_rad);
    if (nSides != 360) {
        int tmp = nSides;
        nSides = 360;
        drawSphere(360);
        nSides = tmp;
    } else
        drawSphere(360);
    glPopMatrix();
}

static void output_vertex(int lat, int lon) {
    float v[3];
    double la, lo;

    la = 2. * PI * lat / nSides;
    lo = 2. * PI * lon / nSides;
    v[0] = cos(lo) * sin(la);
    v[1] = sin(lo) * sin(la);
    v[2] = cos(la);
    glNormal3fv(v);
    glVertex3fv(v);
}

void drawSphere(int nSides) {
    int lat, lon;
    for (lat = 0; lat < nSides / 2; lat++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (lon = 0; lon <= nSides; lon++) {
            output_vertex(lat, lon);
            output_vertex(lat + 1, lon);
        }
        glEnd();
    }
}

void SetupLight() {
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    // Setup lighting
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AMBIENT);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, DIFFUSE);
    glLightfv(GL_LIGHT0, GL_POSITION, POSITION);
    glEnable(GL_LIGHT0);

    // Color tracking
    glEnable(GL_COLOR_MATERIAL);
}