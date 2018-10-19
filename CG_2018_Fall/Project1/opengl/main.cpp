
#include "glut.h"
#include <iostream>

using namespace std;

void display();
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void passiveMouseMotion(int x, int y);
void idle();
void SUN();
void EARTH();
void MOON();

// Global Variables
int width = 1000, height = 1000;
int degree = 0;	// X
float radius = 0.8;	// Y

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Solar System");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(passiveMouseMotion);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

void display()
{
	//ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Camera
	gluLookAt(0.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
	//Viewport Matrix
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Sun
	SUN();
	glPushMatrix();	// Push Sun System
	
	// Earth
	glPopMatrix();
	glTranslatef(20, 0, 0);	// Revolution Radius: 20
	EARTH();
	glPushMatrix();	// Push Earth System
	
	// Moon
	glPopMatrix();
	glTranslatef(3, 0, 0);	// Revolution Radius: 3
	MOON();

	glutSwapBuffers();
}

void reshape(int _width, int _height) {
	width = _width;
	height = _height;
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {
}

void mouseMotion(int x, int y) {
}

void passiveMouseMotion(int x, int y) {
}

void idle() {
	degree = (degree + 1) % 360;
	glutPostRedisplay();
}

void SUN() {
	glColor3f(1.0, 0, 0);
	// Radius: 6 * y
	glutWireSphere(6 * radius, 18, 18);
}

void EARTH() {
	glColor3f(0, 0, 1.0);
	// Radius: 2 * y
	glutWireSphere(2 * radius, 18, 18);
}

void MOON() {
	glColor3f(0, 1.0, 0);
	glScalef(0.5, 0.5, 0.5);
	// Radius: y
	glutWireSphere(radius, 18, 18);
}