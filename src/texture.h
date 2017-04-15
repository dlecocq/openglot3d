#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include <iostream>
#include <string>

using namespace std;

namespace glot {
	
	class texture {
		
		public:
			
			texture(const string& fname) : filename(fname) {
				read_ppm(fname.c_str());
			};
			
			~texture() {
				free(data);
			}
			
			void enable() {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			
		private:
			
			string filename;
			int width;
			int height;
			char * data;
			
			void read_ppm(const char* fname);
		
	};
	
}

#endif
