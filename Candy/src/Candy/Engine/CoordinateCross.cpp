#include "CoordinateCross.h"
#include <GL/glew.h>

CoordinateCross::CoordinateCross()
{
}

CoordinateCross::~CoordinateCross()
{
}

void CoordinateCross::Render()
{
	glUseProgram(0);

	float a=1.0f;
	bool ticks=true;
//	bool arrows=true;

	// thicker lines
	glLineWidth(3);
	// render axes
	glBegin(GL_LINES);
	// x-axis
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-a*0.1f,0,0);
	glVertex3f(a,0,0);
	// y-axis
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0,-a*0.1f,0);
	glVertex3f(0,a,0);
	// z-axis
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0,0,-a*0.1f);
	glVertex3f(0,0,a);
	glEnd();

	if(ticks) {
		float TICKS_LENGTH = 0.01f * a;
		float TICKS_LENGTH_QSRT2 = TICKS_LENGTH * 0.707f;

		glBegin(GL_LINES);
		for(float x=0.1f; x<a; x+=0.1f*a) {
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(x, -TICKS_LENGTH, 0);
			glVertex3f(x, +TICKS_LENGTH, 0);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-TICKS_LENGTH, x, 0);
			glVertex3f(+TICKS_LENGTH, x, 0);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-TICKS_LENGTH_QSRT2, -TICKS_LENGTH_QSRT2, x);
			glVertex3f(+TICKS_LENGTH_QSRT2, +TICKS_LENGTH_QSRT2, x);
		}
		glEnd();
	}

	/*if(arrows) {
		float ARROW_RADIUS = 0.01f * a;
		float ARROW_LENGTH = ARROW_RADIUS * 2.33;
		// render end of axes
		glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
		glRotatef(90, 0,1,0);
		glTranslatef(0,0,a);
		Primitives::RenderPyramid(ARROW_RADIUS, ARROW_LENGTH, 12, true);
		glPopMatrix();

		glColor3f(0.0f, 1.0f, 0.0f);
		glPushMatrix();
		glRotatef(-90, 1,0,0);
		glTranslatef(0,0,a);
		Primitives::RenderPyramid(ARROW_RADIUS, ARROW_LENGTH, 12, true);
		glPopMatrix();

		glColor3f(0.0f, 0.0f, 1.0f);
		glPushMatrix();
		glTranslatef(0,0,a);
		Primitives::RenderPyramid(ARROW_RADIUS, ARROW_LENGTH, 12, true);
		glPopMatrix();
	}*/
}
