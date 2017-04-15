/* 
Dan Lecocq (dlecocq@mines.edu)
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cassert>

//Mac Headers
//*
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
//*/

/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//*/

#include "triangle.h"
#include "point.h"
#include "ray.h"
#include "rgb.h"

using namespace std;

#define VPD_MIN     200
#define VPD_DEFAULT 800
#define VPD_MAX     1024

#define MENU_ZOOM_IN 1
#define MENU_ZOOM_OUT 2
#define MENU_SELECT_TORUS 3
#define MENU_SELECT_MIRROR 4
#define MENU_SELECT_SCULPT 5

#define TWOPI (2.0 * M_PI)
#define PI M_PI;

#define MIRROR 0
#define SCULPT 1
#define TORUS 2

GLint wid;					// GLUT window id
GLint vpd = VPD_DEFAULT; 	// (square) viewport dimensions

GLuint sceneID;      		// display list ID 

GLdouble alpha = 8.0;
GLenum culling = GL_FRONT;
GLenum shading = GL_FLAT;

triangle * triangles;
point * points;
ray * normals;

int nt, np;
double minx, miny, minz, maxx, maxy, maxz;
double d = 15.3;
bool moving = false;
point mouse_down;
point mouse_current;
ray axis;
double angle;
double rotation[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
GLuint surface_dl = 0;
rgb *tilable = NULL;

int tilable_x = 0, tilable_y = 0;
int spherical_x = 0, spherical_y = 0;
int vol_x = 128, vol_y = 128, vol_z = 128;

double maxd[3] = { 1, 1, 12 };
double midx[3] = { 0, 0, 0 };
double midy[3] = { 0, 0, 0 };
double midz[3] = { 0, 0, 0 };

void read_texture(char* fname, rgb*& array, int &width, int &height) {

	cout << "Reading " << fname << endl;
	
	ifstream ifs ( fname );
	if (!ifs) {
		cerr << "Can't open " << fname << endl;
		exit(1);
	}
	char c;
	ifs >> c;
	assert(c=='P');
	ifs >> c;
	assert(c=='6');   // binary PPMs start with the magic code `P6'
	ifs >> width >> height;
	int i;
	ifs >> i;
	assert(i==255);  // all images we'll use have 255 color levels
	ifs.get(); // need to skip one more byte
	array = new rgb[width*height];
	ifs.read((char*)array,width*height*sizeof(rgb));
}

double f(double x, double y) {
	return (cos((x * x + y * y) * 5)) / exp((x * x + y * y));
}

void init_surface() {
	
	if (tilable == NULL) {
		read_texture("tilable.ppm", tilable, tilable_x, tilable_y);
	}
	
	surface_dl = glGenLists(1);
	
	glNewList(surface_dl, GL_COMPILE);
	
		int i = 0, j = 0;
		
		glBegin(GL_LINES);
			glDisable(GL_TEXTURE_2D);
			glColor3d(1.0, 1.0, 0.0);
			glVertex3d(-1, 0, 0);
			glVertex3d(1, 0, 0);
			glVertex3d(0, -1, 0);
			glVertex3d(0, 1, 0);
			glVertex3d(0, 0, -1);
			glVertex3d(0, 0, 1);
			glEnable(GL_TEXTURE_2D);
		glEnd();
		
		glBegin(GL_QUADS);
		
		int n = 500;
		
		double cx = -2.0;
		for (i = 1; i <= n; ++i) {
			double x = -2.0 + i * (4.0) / n;
			double cy = -2.0;
			for (j = 0; j <= n; ++j) {
				double y = -2.0 + j * (4.0) / n;
				glTexCoord2d((cx + 1) * 4.0, (cy + 1) * 4.0);
				glVertex3d(cx, cy, f(cx, cy));
				glTexCoord2d((cx + 1) * 4.0, (y + 1) * 4.0);
				glVertex3d(cx, y, f(cx, y));
				glTexCoord2d((x + 1) * 4.0, (y + 1) * 4.0);
				glVertex3d(x, y, f(x, y));
				glTexCoord2d((x + 1) * 4.0, (cy + 1) * 4.0);
				glVertex3d(x, cy, f(x, cy));
				cy = y;
			}
			cx = x;
		}
		
		glEnd();
	
	glEndList();
}

void zoom_out() {
	double a = alpha / 0.9;
	if (a < 170) {
		alpha = a;
	}
}

void zoom_in() {
	alpha *= 0.9;
}

void init_lightsource ( void )
{
	GLfloat light_ambient[] = { .1, .1, .1, 1.0 };
	GLfloat light_diffuse[] = { .9, .9, .9, 1.0 };
	GLfloat light_specular[] = { 0, 0, 0, 1.0 };
	GLfloat light_position[] = { -2.0, -2.0, -2.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0);
	glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.0);
	glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void set_material_properties ( GLfloat r, GLfloat g, GLfloat b )
{
	GLfloat mat_specular[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient_and_diffuse[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_shininess[1] = { 0.0 };
	
	mat_specular[0] = mat_ambient_and_diffuse[0] = r;
	mat_specular[1] = mat_ambient_and_diffuse[1] = g;
	mat_specular[2] = mat_ambient_and_diffuse[2] = b;

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
 
}

GLuint draw_model() {
	
	glCallList(surface_dl);
	
}

GLuint draw_scene ( )
{	
	
	/* Draw the model */
	set_material_properties(1.0,1.0,1.0);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0.0, 0.0, -d);
	/* Rotate, yo */
	if (moving) {
		glRotated(angle, axis.x, axis.y, axis.z);
	}
	glMultMatrixd(rotation);
	draw_model();
	glPopMatrix();

}

/* --------------------------------------------- */

/* redraw the scene */

void draw(void)
{
	/* set the projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	d = 15;

	gluPerspective(alpha,1.0,d-5,d+5);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* initialize light */
	init_lightsource();

	/* ensure we're drawing to the correct GLUT window */
	glutSetWindow(wid);

	/* clear the color buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* DRAW WHAT IS IN THE DISPLAY LIST */
	draw_scene();

	/* flush the pipeline */
	glFlush();

	/* look at our handiwork */
	glutSwapBuffers();

	/*if (animate)
	  glutPostRedisplay();  
	*/
}

/* --------------------------------------------- */

/* handle mouse events */

point coordinates(GLint x, GLint y) {
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	double i = (2.0 * x) / width - 1;
	double j = - (2.0 * y) / height + 1;
	double l = sqrt(i * i + j * j) + 0.000001;
	if (l > 1) {
		i /= l;
		j /= l;
	}
	double k = sqrt(1 - i * i - j * j);
	//cout << "(" << i << ", " << j << ", " << k << ")" << endl;
	return point(i, j, k);
}

void mouse_button(GLint btn, GLint state, GLint mx, GLint my)
{
	switch( btn ) {
		case GLUT_LEFT_BUTTON:
			switch( state ) {
				case GLUT_DOWN: 
					mouse_down = coordinates(mx, my);
					axis.x = axis.y = axis.z = angle = 0;
					moving = true;
			  		break;
				case GLUT_UP:
		        	if ( moving ) {
		        		moving = false;
						glMatrixMode(GL_MODELVIEW);
						glPushMatrix();
						glLoadIdentity();
						glRotated(angle, axis.x, axis.y, axis.z);
						glMultMatrixd(rotation);
						glGetDoublev(GL_MODELVIEW_MATRIX, rotation);
						glPopMatrix();
				  		break;
		        	}
				}
			break;
	}
}

void button_motion(GLint mx, GLint my)
{
	if (moving) {
		mouse_current = coordinates(mx, my);
		ray a1 = mouse_down - point(0, 0, 0);
		ray a2 = mouse_current - point(0, 0, 0);
		axis = a1 / a2;
		angle = acos((a1 * a2) / (a1.length() * a2.length())) * 180 / PI;
		glutPostRedisplay();
	}
	return;
}

void passive_motion(GLint mx, GLint my)
{
	return;
}

/* --------------------------------------------- */

/* handle keyboard events; here, just exit if ESC is hit */

void keyboard(GLubyte key, GLint x, GLint y)
{
	switch(key) {
		case 27:  /* ESC */
			exit(0);
			break;
		case '+': /* + for zooming in */
			zoom_in();
			glutPostRedisplay();
			break;
		case '-': /* - for zooming out */
			zoom_out();
			glutPostRedisplay();
			break;
		default:
			break;
	}
}

/* --------------------------------------------- */

void menu ( int value )
{
	switch(value) {
		case MENU_ZOOM_IN:
			zoom_in();
			glutPostRedisplay();
			break;
		case MENU_ZOOM_OUT:
			zoom_out();
			glutPostRedisplay();
			break;
	}
	
}

/* --------------------------------------------- */

/* handle resizing the glut window */

void reshape(GLint vpw, GLint vph)
{
	glutSetWindow(wid);

	/* maintain a square viewport, not too small, not too big */
	if( vpw < vph ) vpd = vph;
	else            vpd = vpw;

	if( vpd < VPD_MIN ) vpd = VPD_MIN;
	if( vpd > VPD_MAX ) vpd = VPD_MAX;

	glViewport(0, 0, vpd, vpd);
	glutReshapeWindow(vpd, vpd);

	glutPostRedisplay();
}

/* --------------------------------------------- */

GLint init_glut(GLint *argc, char **argv)
{
	GLint id;

	glutInit(argc,argv);

	/* size and placement hints to the window system */
	glutInitWindowSize(vpd, vpd);
	glutInitWindowPosition(10,10);

	/* double buffered, RGB color mode */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/* create a GLUT window (not drawn until glutMainLoop() is entered) */
	id = glutCreateWindow("openGLot3D");    

	/* register callbacks */

	/* window size changes */
	glutReshapeFunc(reshape);

	/* keypress handling when the current window has input focus */
	glutKeyboardFunc(keyboard);

	/* mouse event handling */
	glutMouseFunc(mouse_button);           /* button press/release        */
	glutMotionFunc(button_motion);         /* mouse motion w/ button down */
	glutPassiveMotionFunc(passive_motion); /* mouse motion with button up */

	/* window obscured/revealed event handler */
	glutVisibilityFunc(NULL);

	/* handling of keyboard SHIFT, ALT, CTRL keys */
	glutSpecialFunc(NULL);

	/* what to do when mouse cursor enters/exits the current window */
	glutEntryFunc(NULL);

	/* what to do on each display loop iteration */
	glutDisplayFunc(draw);

	/* create menu */
	GLint menuID = glutCreateMenu(menu);
	glutAddMenuEntry("Zoom in (+)",MENU_ZOOM_IN);
	glutAddMenuEntry("Zoom out (-)",MENU_ZOOM_OUT);
	glutAddMenuEntry("Show Torus", MENU_SELECT_TORUS);
	glutAddMenuEntry("Show mirrored object", MENU_SELECT_MIRROR);
	glutAddMenuEntry("Show sculpted object", MENU_SELECT_SCULPT);
	glutSetMenu(menuID);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	return id;
}

/* --------------------------------------------- */

void init_opengl(void)
{
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glEnable(GL_TEXTURE_2D);
	
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,tilable_x,tilable_y,0,GL_RGB,GL_UNSIGNED_BYTE,tilable);

	/* automatically scale normals to unit length after transformation */
	glEnable(GL_NORMALIZE);

	/* clear to BLACK */
	glClearColor(0.0, 0.0, 0.0, 1.0);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);
}

void init_texture() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glEnable(GL_TEXTURE_2D);
}

/* --------------------------------------------- */

GLint main(GLint argc, char **argv)
{ 
	//read_input();
	//gouraud();
	//flat();
	
	/* initialize GLUT: register callbacks, etc */
	wid = init_glut(&argc, argv);
	
	init_surface();

	/* initialize light */
	init_lightsource();

	/* any OpenGL state initialization we need to do */
	init_opengl();

	glutMainLoop();

	return 0;
}

