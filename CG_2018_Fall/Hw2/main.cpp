#include "glew.h"
#include <Windows.h>
#include <iostream>
#include "FreeImage.h"
#include "glut.h"

using namespace std;

GLuint texID[2];  // earth, moon
const char *textureFileNames[2] = {"./earth.jpg", "./moon.jpg"};

int height = 400, width = 400;
GLfloat theta = 0;    // X * 28
GLdouble radius = 1;  // Y

// ======================================
//            Keyboard Control
// ======================================
bool pause = false;
bool switch_slice_stack = true;  // 0 for 360/180
                                 // 1 for 4/2
int e_slice = 360;
int e_stack = 180;

void display();
void keyboard(unsigned char, int, int);
void idle();
void init();
void loadTextures();
void reshape(int, int);
void lighting();
void EARTH();
void MOON();

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("ICG Hw2");

    glewInit();

    init();
    glEnable(GL_TEXTURE_2D);
    loadTextures();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}

void display() {
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
    // Viewport Matrix
    glViewport(0, 0, width, height);

    // clear
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    lighting();

    // Draw earth
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    EARTH();
    glBindTexture(GL_TEXTURE_2D, 0);

    // Draw moon
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    MOON();
    glBindTexture(GL_TEXTURE_2D, 0);

    glutSwapBuffers();
}

void reshape(int _width, int _height) {
    width = _width;
    height = _height;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'p':
        case 'P':
            pause = !pause;
            break;
        case 'o':
        case 'O':
            switch_slice_stack = !switch_slice_stack;
            if (switch_slice_stack) {
                e_slice = 360;
                e_stack = 180;
            } else {
                e_slice = 4;
                e_stack = 2;
            }
            break;
        default:
            break;
    }
}

void idle() {
    if (!pause) {
        Sleep(20);
        theta += 0.3;
        if (theta >= 360) theta -= 360;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);

    // enable lighting
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
}

void lighting() {
    float position[] = {0.0f, 10.0f, 0.0f, 1.0f};
    float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
}

void loadTextures() {
    glGenTextures(2, texID);
    for (int i = 0; i < 2; i++) {
        FREE_IMAGE_FORMAT fif =
            FreeImage_GetFIFFromFilename(textureFileNames[i]);
        FIBITMAP *pImage = FreeImage_Load(fif, textureFileNames[i], 0);
        FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
        FreeImage_Unload(pImage);
        int iWidth = FreeImage_GetWidth(p32BitsImage);
        int iHeight = FreeImage_GetHeight(p32BitsImage);

        glBindTexture(GL_TEXTURE_2D, texID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void EARTH() {
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    GLUquadric *earth = gluNewQuadric();
    gluQuadricTexture(earth, GL_TRUE);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);                     // fix sphere direction
    glRotatef(23.5, 0, 1, 0);                    // tilt
    glRotatef(theta * 28, 0, 0, 1);              // rotation  X
    gluSphere(earth, radius, e_slice, e_stack);  // radius Y
    glPopMatrix();
}

void MOON() {
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    GLUquadric *moon = gluNewQuadric();
    gluQuadricTexture(moon, GL_TRUE);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);    // fix sphere direction
    glRotatef(theta, 0, 0, 1);  // revolution X/28
    glTranslatef((3 * radius + 0.5 * radius), 0,
                 0);                         // revolution radius 3*Y+0.5*Y
    glRotatef(theta, 0, 0, 1);               // rotation  X/28
    gluSphere(moon, radius * 0.5, 240, 60);  // radius 0.5*Y
    glPopMatrix();
}