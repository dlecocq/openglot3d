#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <GL/glew.h>

#include "texture.h"
#include "screen.h"
#include "color.h"

namespace glot {
	
	enum layout_opt { 
		X_LIN 			= 0,
		Y_LIN 			= 0,
		CARTESIAN 	= 0,
		X_LOG 			= 1,
		Y_LOG 			= 2,
		CYLINDRICAL = 4,
		SPHERICAL		= 8 };
	
	class primitive {
	
		public:
		
			/** The color of the the point */
			color c;
					
			primitive(const color& col, short int layout_opt = X_LIN | Y_LIN | CARTESIAN) : c(col), p(0), pattern(NULL), layout(layout_opt) {};
			
			virtual ~primitive() {
				if (pattern == NULL) {
					delete pattern;
				}
			};
			
			virtual void dl_gen(const screen& s) = 0;
			
			void load_texture(const string& filename) {
				if (pattern == NULL) {
					delete pattern;
				}
				pattern = new texture(filename);
			}
			
			/*
			y_coord_transform();
			x_coord_transform();
			*/
			
			GLenum p;
			texture* pattern;
		
		protected:
			
			short int layout;
		
		private:
	
	};

}

#endif
