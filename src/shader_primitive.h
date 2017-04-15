#ifndef SHADER_PRIMITIVE_H
#define SHADER_PRIMITIVE_H

#include <fstream>
#include <string>

#include <GL/glew.h>

#include "primitive.h"
#include "screen.h"
#include "color.h"

using namespace std;

namespace glot {
	
	class shader_primitive : public primitive{
	
		public:
		
			/** The color of the the point */
			color c;
					
			shader_primitive(const color& col, short int layout_opt = X_LIN | Y_LIN | CARTESIAN) : primitive(col, layout_opt) {};
			
			virtual ~shader_primitive() {};
			
			virtual void dl_gen(const screen& s) = 0;
			
			void printProgramInfoLog(GLuint obj);
			void printShaderInfoLog(GLuint obj);
			
			string read_file(const char * filename);
		
		protected:
			
			/** Shader source handles
			  *
			  * I'm not entirely sure if it's necessary to store these
			  * handles beyond the compiling and then linking the shader
			  * program, but these are for that purpose.
			  */
			GLhandleARB f;
			GLhandleARB g;
			GLhandleARB v;
		
		private:
	
	};

}

#endif
