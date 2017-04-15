#ifndef GRAPHER_CPP
#define GRAPHER_CPP

#include <iostream>
#include <cmath>

#include "grapher.h"

using namespace glot;
using namespace std;

int grapher::initialize(int argc, char ** argv, short int options, short int k_options) {
	
	display_options = options;
	keyboard_options = k_options;

	/* Initialize the rotation matrix */
	rotation = new double[16];
	for (unsigned int i = 0; i < 16; ++i) {
		rotation[i] = 0;
	}
	rotation[0] = rotation[5] = rotation[10] = rotation[15] = 1;
	
	#ifdef CGLX
		cglXInit(&argc, argv);
	#else
		glutInit(&argc, argv);
	#endif
	
	vpd = 800;
	
	/* size and placement hints to the window system */
	#ifdef CGLX
		cglXInitWindowSize(vpd, vpd);
		cglXInitWindowPosition(0, 0);

		/* double buffered, RGB color mode */
		cglXInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

		/* create a cglX window (not drawn until cglXMainLoop() is entered) */
		cglXCreateWindow("openGLot3D");

		/* register callbacks */

		/* window size changes */
		cglXReshapeFunc(reshape);

		/* keypress handling when the current window has input focus */
		cglXKeyboardFunc(keyboard);

		/* mouse event handling */
		cglXMouseFunc(mouse);           /* button press/release        */
		cglXMotionFunc(motion);         /* mouse motion w/ button down */

		/* handling of keyboard SHIFT, ALT, CTRL keys */
		cglXSpecialFunc(NULL);

		/* what to do when mouse cursor enters/exits the current window */
		cglXEntryFunc(NULL);
	#else
		glutInitWindowSize(vpd, vpd);
		glutInitWindowPosition(0, 0);

		/* double buffered, RGB color mode */
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

		/* create a cglX window (not drawn until cglXMainLoop() is entered) */
		wid = glutCreateWindow("openGLot3D");

		/* register callbacks */

		/* window size changes */
		glutReshapeFunc(reshape);

		/* keypress handling when the current window has input focus */
		glutKeyboardFunc(keyboard);

		/* mouse event handling */
		glutMouseFunc(mouse);           /* button press/release        */
		glutMotionFunc(motion);         /* mouse motion w/ button down */

		/* window obscured/revealed event handler */
		glutVisibilityFunc(NULL);

		/* handling of keyboard SHIFT, ALT, CTRL keys */
		glutSpecialFunc(NULL);

		/* what to do when mouse cursor enters/exits the current window */
		glutEntryFunc(NULL);
	#endif

	alpha = 4.0;
	culling = GL_FRONT;
	shading = GL_FLAT;

	d = 15.3;
	moving = false;

	/* what to do on each display loop iteration */
	#ifdef CGLX
		cglXDisplayFunc(display);
	#else
		glutDisplayFunc(display);
	#endif
	
	init_open_gl();
	init_texture();
	
	glewInit();
	
	if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_EXT_geometry_shader4)) {
		printf("Not totally ready :( \n");
		#ifdef CGLX
			cglx::exit(1);
		#else
			exit(1);
		#endif
	}
	
	wall.start();
	
	return wid;
}

// Initialize OpenGL
void grapher::init_open_gl() {
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	
	// Other smoothness and blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	/* automatically scale normals to unit length after transformation */
	glEnable(GL_NORMALIZE);

	/* clear to BLACK */
	glClearColor(0.0, 0.0, 0.0, 1.0);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);
}

void grapher::init_texture() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	glEnable(GL_TEXTURE_2D);
}

void grapher::init_lightsource() {
	GLfloat light_ambient[] = { .1, .1, .1, 1.0 };
	GLfloat light_diffuse[] = { .9, .9, .9, 1.0 };
	GLfloat light_specular[] = { 0, 0, 0, 1.0 };
	GLfloat light_position[] = { -20.0, -20.0, -20.0, 0.0 };

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

void grapher::set_material_properties ( GLfloat r, GLfloat g, GLfloat b ) {
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

// Our callback function registered with OpenGL
void grapher::display() {
	
	/* set the projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	d = 30;

	#ifdef CGLX
		cglx::gluPerspective(alpha,1.0,d-25,d+25);
	#else
		gluPerspective(alpha,1.0,d-25,d+25);
	#endif

	/* initialize light */
	init_lightsource();
	
	float t = float(wall.time());
	scr.time = wall.time();

	/* ensure we're drawing to the correct cglX window */
	#ifndef CGLX
		glutSetWindow(wid);
	#endif

	/* clear the color buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* DRAW WHAT IS IN THE DISPLAY LIST */
	/* Draw the model */
	set_material_properties(0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0.0, 0.0, -d);
	/* Rotate, yo */
	if (moving) {
		glRotated(angle, axis.x, axis.y, axis.z);
	}
	glMultMatrixd(rotation);
	
	map<primitive*, GLint>::iterator it;
	for (it = primitives.begin(); it != primitives.end(); ++it) {
		/*
		glUseProgram(it->first->p);
		glCallList(it->second);
		glUseProgram(0);
		//*/
		if (it->first->pattern) {
			it->first->pattern->enable();
		}
		
		// Call it's shader program, defaulted to 0
		glUseProgram(it->first->p);
		
		GLint location = glGetUniformLocation(it->first->p, "t");
		
		glUniform1f(location, t);
		
		glColor4d(it->first->c.r, it->first->c.g, it->first->c.b, it->first->c.a);
		// Call the actual draw list
		glCallList(it->second);
		//it->first->dl_gen(scr);
		
		// Re-set the shader program to 0
		glUseProgram(0);

		/*
		glUseProgram(it->first->p);
		it->first->dl_gen(scr);
		glUseProgram(0);
		*/
		//*/
	}
	
	glPopMatrix();

	/* flush the pipeline */
	glFlush();

	/* look at our handiwork */
	#ifdef CGLX
		cglXSwapBuffers();
	#else
		glutSwapBuffers();
	#endif
}

// Our reshaping function, registered with OpenGL
void grapher::reshape(int w, int h) {
	#ifndef CGLX
		glutSetWindow(wid);
	#endif
	
	if (w > h) {
		vpd = h;
	} else {
		vpd = w;
	}
	
	glViewport(0, 0, vpd, vpd);
	#ifndef CGLX
		glutReshapeWindow(vpd, vpd);
		glutPostRedisplay();
	#else
		cglXPostRedisplay();
	#endif
}

void grapher::zoom(double scale) {
	double a = alpha * scale;
	if (a > 0.01 && a < 170) {
		alpha = a;
		#ifdef CGLX
			cglXPostRedisplay();
		#else
			glutPostRedisplay();
		#endif
	}
}

// Our keyboard function registered with OpenGL
void grapher::keyboard(unsigned char key, GLint x, GLint y) {
	switch (key) {
		case '+':
			if (keyboard_options & ZOOM_KEYS_ON) {
				zoom(0.9);
			}
			break;
		case '-':
			if (keyboard_options & ZOOM_KEYS_ON) {
				zoom(1.0 / 0.9);
			}
			break;
		case 'a':
			if (keyboard_options & AXES_KEYS_ON) {
				display_options = AXES_ON ^ display_options;
				#ifdef CGLX
					cglXPostRedisplay();
				#else
					glutPostRedisplay();
				#endif
			}
			break;
		case 'g':
			if (keyboard_options & GRID_KEYS_ON) {
				display_options = GRID_ON ^ display_options;
				#ifdef CGLX
					cglXPostRedisplay();
				#else
					glutPostRedisplay();
				#endif
			}
			break;
		case 'q':
			if (keyboard_options & QUIT_KEYS_ON) {
				#ifdef CGLX
					cglx::exit(0);
				#else
					exit(0);
				#endif
			}
			break;
	}
	
	/** If the user has defined their own keyboard event
	  * handler, execute it here, after we've completed
	  * our own.  They may turn on and off the default
	  * functionality by changing keyboard_options
	  */
	if (user_keyboard_function) {
		user_keyboard_function(key, x, y);
	}
}

// Our mouse funciton, registered with OpenGL
void grapher::mouse(GLint button, GLint state, GLint x, GLint y) {
	switch( button ) {
		case GLUT_LEFT_BUTTON:
			switch( state ) {
				case GLUT_DOWN: 
					mouse_down = coordinates(x, y);
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
						#ifdef CGLX
							cglXPostRedisplay();
						#else
							glutPostRedisplay();
						#endif
			  		break;
        	}
				}
			break;
	}
}

point grapher::coordinates(GLint x, GLint y) {
	#ifdef CGLX
		int width = 500;//cglXGet(GLUT_WINDOW_WIDTH);
		int height = 500;//cglXGet(GLUT_WINDOW_HEIGHT);
	#else
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);
	#endif
	
	double i = (2.0 * x) / width - 1;
	double j = - (2.0 * y) / height + 1;
	double l = sqrt(i * i + j * j) + 0.000001;
	if (l > 1) {
		i /= l;
		j /= l;
	}
	double k = sqrt(1 - i * i - j * j);
	return point(i, j, k);
}

void grapher::motion(GLint x, GLint y) {
	if (moving) {
		mouse_current = coordinates(x, y);
		ray a1 = mouse_down - point(0, 0, 0);
		ray a2 = mouse_current - point(0, 0, 0);
		axis = a1 / a2;
		angle = acos((a1 * a2) / (a1.length() * a2.length())) * 180 / M_PI;
		#ifdef CGLX
			cglXPostRedisplay();
		#else
			glutPostRedisplay();
		#endif
	}
}

void grapher::set_keyboard_function(keyboard_function k) {
	user_keyboard_function = k;
}

void grapher::set_idle_function(idle_function i) {
	user_idle_function = i;
	#ifdef CGLX
		cglXIdleFunc(user_idle_function);
	#else
		glutIdleFunc(user_idle_function);
	#endif
}

// void grapher::set_click_function(click_function c) {
// 	user_click_function = c;
// }

// double grapher::get_x_coord(GLint x) {
// 	return minx + (maxx - minx) * ((double)x / width);
// }
// 
// double grapher::get_y_coord(GLint y) {
// 	return maxy - (maxy - miny) * (double)y / height;
// }

// GLint grapher::axes_dl_gen() {
// 	GLint dl = glGenLists(1);
// 	
// 	glNewList(dl, GL_COMPILE);
// 	
// 		glColor4d(0.0, 0.0, 0.0, 1.0);
// 		
// 		glBegin(GL_LINES);
// 		
// 			glVertex3d(0, miny, 0);
// 			glVertex3d(0, maxy, 0);
// 			glVertex3d(minx, 0, 0);
// 			glVertex3d(maxx, 0, 0);
// 		
// 		glEnd();
// 	
// 	glEndList();
// 	
// 	return dl;
// }

void grapher::add(primitive& p) {
	primitives[&p] = glGenLists(1);
	glNewList(primitives[&p], GL_COMPILE);
		(&p)->dl_gen(scr);
	glEndList();
}

void grapher::remove(primitive& p) {
	primitives.erase(&p);
}

void grapher::refresh_dls() {
	
	// axes_dl = axes_dl_gen();
	// grid_dl = grid_dl_gen();
	
	map<primitive*, GLint>::iterator it;
	for (it = primitives.begin(); it != primitives.end(); ++it) {
		glDeleteLists(it->second, 1);
		glNewList(it->second, GL_COMPILE);
			it->first->dl_gen(scr);
		glEndList();
	}
}

// double grapher::y_coord_transform(double y) {
// 	if (Y_LOG & display_options) {
// 		return log10(abs(y));
// 	} else {
// 		return y;
// 	}
// }

// double grapher::x_coord_transform(double x) {
// 	if (X_LOG & display_options) {
// 		return pow(10, x);
// 	} else {
// 		return x;
// 	}
// }

void grapher::redraw() {
	if (framecount == 0) {
		timer.start();
	}
	glFinish();
	
	#ifdef CGLX
		cglXPostRedisplay();
	#else
		glutPostRedisplay();
	#endif

	++framecount;
	if (framecount == 500) {
		framecount = 0;
		cout << 500 / timer.stop() << " fps" << endl;
	}
}

void grapher::run() {
	#ifdef CGLX
		cglXMainLoop();
	#else
		glutMainLoop();
	#endif
}

ray grapher::axis;
point grapher::mouse_down;
point grapher::mouse_current;
GLint grapher::vpd;
GLint grapher::wid;
double grapher::alpha;
GLenum grapher::culling;
GLenum grapher::shading;
screen grapher::scr;
double grapher::d;
bool grapher::moving;
double grapher::startx;
double grapher::starty;
//ray grapher::axis;
double grapher::angle;
double *grapher::rotation;
short int grapher::keyboard_options;
short int grapher::display_options;
map<primitive*, GLint> grapher::primitives;
stopwatch grapher::wall;
stopwatch grapher::timer;
int grapher::framecount;

grapher::keyboard_function grapher::user_keyboard_function;
grapher::idle_function grapher::user_idle_function;

#endif
