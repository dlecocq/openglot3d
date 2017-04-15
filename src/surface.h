#ifndef SURFACE_H
#define SURFACE_H

#include <GL/glew.h>
#include <string>

#include "shader_primitive.h"
#include "texture.h"
#include "color.h"

namespace glot {
	
	class surface : public shader_primitive {
		
		public:
			
			color c;
			
			surface(const string& f, const color& col, const string& filename) : shader_primitive(col), func(f), t(filename) {
				gen_shader();
			};
		
			void dl_gen(const screen& s);
		
		private:

			GLhandleARB f;
			GLhandleARB g;
			GLhandleARB v;
			
			string func;
			texture t;
			
			void gen_shader();
			
			string get_vert_shader();
			string get_geom_shader();
	
	};
	
}

#endif
