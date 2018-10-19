/*
#include<iostream>
#include<stdlib.h>
#include<Windows.h>
#include "glut.h"
#include<time.h>
#include<Math.h>
#define d 6.0


void Light_Sun() {
	GLfloat ambient0[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat diffuse0[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat specular0[] = { 1.0, 0.0, 0.0, 1.0 };

	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

}

void Light_Earth() {
	GLfloat ambient0[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat diffuse0[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat specular0[] = { 0.0, 0.0, 1.0, 1.0 };

	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

}

void Light_Moon() {
	GLfloat ambient0[] = { 0.0, 1.0, 1.0, 1.0 };
	GLfloat diffuse0[] = { 0.0, 1.0, 1.0, 1.0 };
	GLfloat specular0[] = { 0.0, 1.0, 1.0, 1.0 };
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

}


double theta = 0;
int n = 60;
int Look = 0;

float ww, wh;

void SUN() {
	glColor3f(1.0, 0, 0);
	glutSolidSphere(1.2, 30, 30);
}
void Earth() {
	glColor3f(0, 0, 1.0);
	glutSolidSphere(0.4, 30, 30);
}
void Moon() {
	glColor3f(0, 1.0, 1.0);
	glutSolidSphere(0.24, 30, 30);
}

void mouse(int button, int state, int x, int y) {

	Look++;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glMatrixMode(GL_PROJECTION);
		if ((Look % 2) == 1) {
			gluLookAt(0, 1, 0, 0, 0, 0, 0, 0, -1);

			GLfloat light0_pos[] = { 3.0, 3.0, 3,0, 1.0 };
			glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

			glMatrixMode(GL_MODELVIEW);
		}
		else if ((Look % 2) == 0) {
			gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

			GLfloat light0_pos[] = { 3.0, 3.0, 3,0, 1.0 };
			glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

			Look %= 2;
			glMatrixMode(GL_MODELVIEW);
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_LIGHTING);

	glRotatef(theta, 0, 0, 1);

	Light_Sun();

	glRotatef(-theta, 0, 0, 1);

	SUN();

	glRotatef(theta, 0, 0, 1);
	glTranslatef(3.5, 0, 0);

	glRotatef(theta * 5, 0, 0, 1);

	Light_Earth();
	glRotatef(-(theta * 5), 0, 0, 1);

	Earth();
	glRotatef(theta * 5, 0, 0, 1);
	glTranslatef(1.2, 0, 0);

	Light_Moon();

	Moon();
	glutSwapBuffers();
}

void Timer(int v)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / n, Timer, v);
}

void Idle() {
	theta += 0.05;
	if (theta > 360.0) theta -= 360.0;
	glutPostRedisplay();
}

void myInit(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(0.0, 0.0, 0.5, 1.0);
	glOrtho(-d, d, -d, d, -d, d);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);
}

void main(int argc, char**argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Praise The Sun !!");
	glutMouseFunc(mouse);
	glutReshapeFunc(myInit);
	glutDisplayFunc(display);
	glutIdleFunc(Idle);
	glutTimerFunc(100, Timer, n);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}
*/