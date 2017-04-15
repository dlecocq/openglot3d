#ifndef P_CURVE_H
#define P_CURVE_H

#include <GL/glew.h>
#include <string>

#include "shader_primitive.h"
#include "texture.h"
#include "color.h"

namespace glot {
	
	class p_curve : public shader_primitive {
		
		public:
			
			color c;
			
			p_curve(const string& f, const color& col, const string& filename, short int layout_opt = X_LIN | Y_LIN | CARTESIAN) : shader_primitive(col, layout_opt), func(f), t(filename) {
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
