#ifndef GRAPHER_H
#define GRAPHER_H

//#include <GL/glew.h>
//#include <OpenGL/gl.h>
//#include <GLUT/glut.h>

#include <GL/glew.h>

#ifdef CGLX
	#include <cglX/cglX.h>
	using namespace cglx;
#else
	#ifdef LINUX
		#include <GL/glut.h>
	#else
		#include <GLUT/glut.h>
	#endif
#endif

#ifdef LINUX
	#include <GL/gl.h>
	#include <GL/glu.h>
#else
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#endif

using namespace std;

#include <list>
#include <map>

#include "shader_primitive.h"
#include "stopwatch.h"
#include "primitive.h"
#include "p_surface.h"
#include "p_curve.h"
#include "surface.h"
#include "screen.h"
#include "point.h"
#include "flow.h"

using namespace std;

namespace glot {
	
	/** Enumeration for display options
	  *
	  * Bitwise or these to set the display options
	  */
	enum display_opt { AXES_OFF = 0, 
		GRID_OFF = 0,
		AXES_ON = 1,
		GRID_ON = 2 };

	/** Enumeration for keyboard action options
	  *
	  * Bitwise or these to set the keyboard action options
	  */
	enum keyboard_opt { ZOOM_KEYS_OFF = 0,
		AXES_KEYS_OFF = 0,
		GRID_KEYS_OFF = 0,
		QUIT_KEYS_OFF = 0,
		ZOOM_KEYS_ON = 1,
		AXES_KEYS_ON = 2,
		GRID_KEYS_ON = 4,
		QUIT_KEYS_ON = 8 };

	class grapher {
		
		public:

				/**	\brief Keyboard event handler typedef
			  *	\param key - unsigned char
			  *	\param x - GLint x coordinate
			  * \param y - GLint y coordinate
			  *
			  * A keyboard event handler accepts a key and x, y coordinates
			  */
			typedef void (*keyboard_function)(unsigned char key, GLint x, GLint y);

			/** \brief Click even handler typedef
			  * \param button - GLint button pressed
			  * \param x - GLint x coordinate
			  * \param y - GLint y coordinate
			  *
			  * See set_click_function(...) for more details
			  */
			typedef void (*click_function)(GLint button, GLint x, GLint y);
			
			/** \brief The idle function typedef
			  *
			  * See set_idle_funciton(...) for more details
			  */
			typedef void (*idle_function)(void);
		
			/** \brief Initialize the grapher
			  * \param argc - same as argc used for OpenGL initialization
			  * \param argv - same as argv used for OpenGL initialization
			  * \param options - startup options
			  * \param k_options - the default keyboard actions options
			  *
			  * Use a bitwise or to select startup options:
			  * AXES_ON, AXES_OFF
			  * GRID_ON, GRID_OFF
			  * X_LIN, X_LOG (linear x scale or logarithmic)
			  * Y_LIN, Y_LOG (linear y scale or logarithmic)
			  */
			static int initialize(int argc, char ** argv, short int options = AXES_ON | GRID_OFF | X_LIN | Y_LIN, short int k_options = ZOOM_KEYS_ON | AXES_KEYS_ON | GRID_KEYS_ON | QUIT_KEYS_ON);
		
			/** \brief Enter the OpenGL main loop after initialization
			  *
			  *	In general, you will set up your event handlers, main
			  * code, etc., and then when you've gotten everything in
			  * place, you call grapher::run() to start the program's
			  * OpenGL portion.
			  */
			static void run();
		
			/**	\brief User-requested redraw
			  *	
			  *	If you create an event handler that will make some
			  * changes to the graph, you can make those changes and
			  * then request a redraw with this function.
			  */
			static void redraw();
		
			/**	\brief Add a curve to the plot
			  *	\param c - The curve you wish to add to the plot
			  *
			  *	If you instantiate a curve, you can add it to the plot
			  * with this function.  NOTE: This does not automatically
			  * request a redisplay.  The idea here is that you may be
			  * adding several curves at once, and so after adding all 
			  * of your curves, etc., you should then call
			  * grapher::redraw()
			  */
			static void add(primitive& p);
		
			/**	\brief Delete a curve from the plot
			  *	\param c - The curve you wish to delete from the plot
			  *
			  *	If you've plotted a curve, but wish to remove it, you
			  * can do so with this function.  As with add_curve(...),
			  * this does not automatically request a redisplay.
			  */
			static void remove(primitive& p);
		
			/**	\brief Add a point to the plot
			  * \param p - The Point you wish to add to the plot
			  *
			  * Instantiate a point and then add it to the plot with
			  * this function.  NOTE: Does not automatically request a
			  * redisplay
			  */
			//static void add_point(point& p);
		
			/**	\brief Delete a point from the plot
			  * \param p - The point you wish to delete from the plot
			  *
			  *	If you've plotted a point, you can remove it from the
			  *	plot with this function.  NOTE: Does not automatically
			  *	request a redisplay.
			  */
			//static void delete_point(point& p);
		
			/** \brief Set up a keyboard event handler
			  *	\param k - the function you'd like to handle key events
			  *
			  *	If you'd like to set up a function to handle key events,
			  * you can register them with this function.  NOTE: there are
			  * default behaviors for certain keys that are not overridden
			  * here.  For example, '+' zooms in, but if you use that
			  * key as well, both a zoom and your operation will take
			  * place.
			  *
			  * The idea behind this is that these default behaviors are
			  * not the programmer's responsibility to code up as well.
			  * They are interface freebies.
			  *
			  *	Bitwise or options together from the set:
			  * ZOOM_KEYS_ON / ZOOM_KEYS_OFF
			  * AXES_KEYS_ON / AXES_KEYS_OFF
			  * GRID_KEYS_ON / GRID_KEYS_OFF
			  * QUIT_KEYS_ON / QUIT_KEYS_OFF
			  */
			static void set_keyboard_function(keyboard_function k);
		
			/**	\brief Set up a click event handler
			  *	\param c - the function you'd like to handle click events
			  *
			  * If you'd like to set up a function to handle click events
			  * (which are when a user presses down and then releases a
			  * button at the same spot), you can register it with this
			  * function.  NOTE: this is different from a motion function.
			  */
			static void set_click_function(click_function c);
			
			/** \brief Set up an idle function handler
				* \param i - the function you'd like to handle idle time
				*
				* This is primarily used for animation, and will often end
				* up being just setting the idle function to redraw:
				* set_idle_function(grapher::redraw)
				*/
			static void set_idle_function(idle_function i);
		
			/**	\brief Zoom in / out by a scale
			  *	\param scale - the scale by which to zoom
			  *
			  *	This determines the center of the plot as it is now, and
			  * scales in / out by a factor of scale outward / inward from
			  * that center point.
			  */
			static void zoom(double scale);
		
			/**	\brief Transform a screen x coordinate to a world one
			  *	\param x - the screen x coordinate to transform
			  *
			  * It's the grapher's responsibility to know how to
			  * transform a screen coordinate into a world coordinate
			  * with this function.
			  */
			//static double get_x_coord(GLint x);
		
			/**	\brief Transform a screen y coordinate to a world one
			  * \param y - the screen y coordinate to transform
			  * 
			  *	It's the grapher's repsonsibility to know how to
			  *	transform a screen coordinate into a world coordinate
			  *	with this function.
			  */
			//static double get_y_coord(GLint y);

		private:
		
			/**	\brief Transform a y coordinate
			  *	\param y - the y coordinate to transform
			  *
			  * This grapher gives the option of using linear or 
			  * logarithmic scales, and so this function transforms
			  * a real (linear) y coordinate into the coordinate
			  * appropriate for this plot.  If it's linear, it will
			  * remain linear, but if the plot has a logarithmic y
			  *	scale, it will be transformed by log10(abs(y)).
			  */
			//static double y_coord_transform(double y);
		
			/**	\brief Transform a x coordinate
			  *	\param x - the x coordinate to transform
			  *
			  * This grapher gives the optiopn of using linear or
			  *	logarithmic scales, and so this function transforms
			  * a world x coordinate into the correct plot x
			  * coordinate.  For example, if X_LOG is active in the
			  *	display options, then it will transform x to be 
			  * exp(10, x).
			  */
			//static double x_coord_transform(double x);
		
			/**	\brief Generate a display list for the axes
			  *	
			  *	For the current configuration, generate a display
			  * list for the axes.
			  */
			//static GLint axes_dl_gen();
		
			/**	\brief Generate a display list for the grid
			  *
			  *	For the current configuration, generate a display
			  *	list for the grid.
			  */
			//static GLint grid_dl_gen();
		
			/**	\brief Initialize OpenGL
			  */
			static void init_open_gl();
			
			static void init_lightsource();
			
			static void init_texture();
			
			static void set_material_properties ( GLfloat r, GLfloat g, GLfloat b );
		
			/**	\brief Display the plot
			  *
			  *	This is the grapher's event handler it registers with
			  *	OpenGL to display the plot.  It iterates over all its
			  *	primitives and displays them in turn.
			  */
			static void display();
		
			/**	\brief Reshape function
			  *	\param w - new width
			  *	\param h - new height
			  *
			  *	This is the grapher's reshape event handler is registers
			  *	with OpenGL.
			  */
			static void reshape(int w, int h);
		
			/** \brief Keyboard event function
			  *	\param key - the key depressed
			  *	\param x - the x screen coordinate of the mouse
			  *	\param y - the y screen coordinate of the mouse
			  *
			  * This encapsulates certain default key behavior (like
			  *	zooming in or zooming out), and is what grapher registers
			  * with OpenGL to handle keyboard events.  If the user
			  *	defines their own keyboard event handler, it will execute
			  * that function AFTER it's done.
			  */
			static void keyboard(unsigned char key, GLint x, GLint y);
		
			/**	\brief Mouse event function
			  *	\param button - the button pressed
			  *	\param state - the state (UP / DOWN) of the button
			  * \param x - the screen x coordinate of the mouse
			  * \param y - the screen y coordinate of the mouse
			  *
			  * This function keeps track of where a mouse event begins
			  * and where it ends, and if those are the same coordinates,
			  * it registers it as a click.  In that event, it fires the
			  * user-defined click-event-handler (if it's registered).
			  *
			  * In future versions, if the mouse clicks, moves, returns
			  * to the original spot and releases, that will not be
			  * registered as a click.
			  */
			static void mouse(GLint button, GLint state, GLint x, GLint y);
		
			/**	\brief Motion event function
			  *	\param x - the screen x coordinate of the mouse
			  *	\param y - the screen y coordinate of the mouse
			  *
			  * This if the function this grapher registers with OpenGL
			  * to handle motion events.  It will move the current plot
			  * around the window, and when it stops, it will regenerate
			  * the display lists for the elements for the new window
			  * bounds, and then redraws the plot.
			  */
			static void motion(GLint x, GLint y);
			
			static point coordinates(GLint x, GLint y);
		
			/**	\brief Regenerate all display lists
			  *
			  *	If the plot moves, or for whatever reason you need to 
			  * recalculate all the display lists, you can call this
			  * function.
			  */
			static void refresh_dls();

			/** \brief The display options for the grapher
			  *	
			  *	It's a bitwise or'ing of the options AXES_ON / AXES_OFF,
			  * GRID_ON / GRID_OFF, etc.
			  */
			static short int display_options;
		
			/** \brief The default keyboard actions options
			  *
			  * It's a bitwise or'ing of the options ZOOM_KEYS_ON /
			  *	ZOOM_KEYS_OFF, AXES_KEYS_ON / AXES_KEYS_OFF,
			  *	GRID_KEYS_ON / GRID_KEYS_OFF
			  */
			static short int keyboard_options;
			
			static GLint wid;
			static GLint vpd;

			static double alpha;
			static GLenum culling;
			static GLenum shading;
			
			static screen scr;

			static double d;
			static bool moving;
			static double startx;
			static double starty;
			
			static point mouse_current;
			static point mouse_down;
			
			static ray axis;
			static double angle;
			static double *rotation;
			
			//rgb *tilable = NULL;

			/** The display list for the axes */
			// static GLint axes_dl;
			/** The display list for the grid */
			// static GLint grid_dl;
		
			/** The user-defined keyboard-event fuction */
			static keyboard_function user_keyboard_function;
			
			/** The user-defined idle-even function */
			static idle_function user_idle_function;
		
			/** The user-defined click-event function */
			// static click_function user_click_function;
		
			/** All the curves */
			// static map<curve*, GLint> curves;
		
			/** All the points */
			// static list<point*> points;
			
			static map<primitive*, GLint> primitives;
			
			static stopwatch wall;
			static stopwatch timer;
			
			static int framecount;

	};
	
}

#endif
