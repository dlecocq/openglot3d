#ifndef P_SURFACE_CPP
#define P_SURFACE_CPP

#include <iostream>

#include "p_surface.h"

using namespace glot;
using namespace std;

void p_surface::dl_gen(const screen& scr) {
	
	double width = 1.0;
	
	double a = 0;
	double b = 0;
	
	int count = 200;
	
	double dx = width / count;
	
	double ap = a + dx;
	double bp = b + dx;
	
	glBegin(GL_QUADS);
	
	for (int i = 0; i < count; ++i) {
		b = -width / 2;
		bp = b + dx;
		for (int j = 0; j < count; ++j) {
			glTexCoord2d(a * 50,  b  * 50);
				glVertex3d(a,  			b,  	0);
			glTexCoord2d(ap * 50,	b	 * 50);
				glVertex3d(ap, 			b,  	0);
			glTexCoord2d(ap * 50, bp * 50);
				glVertex3d(ap, 			bp, 	0);
			glTexCoord2d(a * 50,	bp * 50);
				glVertex3d(a,  			bp, 	0);
			b = bp;
			bp += dx;
		}
		a = ap;
		ap += dx;
	}
		
	glEnd();
}

string p_surface::get_vert_shader() {
	string program = read_file("shaders/p_surface.vert.1") + func + read_file("shaders/p_surface.vert.2");
	
	if (layout & CYLINDRICAL) {
		program += "gl_Position = vec4(gl_Position.y * cos(gl_Position.x), gl_Position.y * sin(gl_Position.x), gl_Position.z, 1.0);";
	} else if (layout & SPHERICAL) {
		program += "gl_Position = vec4(gl_Position.x * sin(gl_Position.y) * cos(gl_Position.z), gl_Position.x * sin(gl_Position.y) * sin(gl_Position.z), gl_Position.x * cos(gl_Position.y), 1.0);";		
	}
	
	program += read_file("shaders/p_surface.vert.3");
	
	return program;
}

string p_surface::get_geom_shader() {
	return read_file("shaders/p_surface.geom");
}

void p_surface::gen_shader() {
	
	cout << "Creating shader" << endl;
	f = glCreateShader(GL_FRAGMENT_SHADER);
	v = glCreateShader(GL_VERTEX_SHADER);
	//g = glCreateShader(GL_GEOMETRY_SHADER_EXT);

	//string frag_src = read_file("shaders/surface.frag.front") + func + read_file("shaders/surface.frag.back");
	string frag_src = read_file("shaders/p_surface.frag");
	string vert_src = get_vert_shader();
	//string geom_src = get_geom_shader();
	
	//cout << frag_src << endl;
	//cout << vert_src << endl;
	
	const GLchar * fs_src = frag_src.c_str();
	const GLchar * vs_src = vert_src.c_str();
	//const GLchar * gs_src = geom_src.c_str();	

	glShaderSource(f, 1, &fs_src, NULL);
	glShaderSource(v, 1, &vs_src, NULL);
	//glShaderSource(g, 1, &gs_src, NULL);
	
	glCompileShader(f);
	glCompileShader(v);
	//glCompileShader(g);
	
	p = glCreateProgram();
	
	glAttachShader(p,f);
	glAttachShader(p,v);
	//glAttachShader(p,g);
	
	// It has to be GL_LINES as input, GL_LINE_STRIP as output?
	//glProgramParameteriEXT(p, GL_GEOMETRY_INPUT_TYPE_EXT,  GL_LINES);
	//glProgramParameteriEXT(p, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_POINTS);
	//glProgramParameteriEXT(p, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);
	/*
	GLint temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &temp);
	//*/
	//glProgramParameteriEXT(p, GL_GEOMETRY_VERTICES_OUT_EXT, 40);
	
	glLinkProgram(p);
	
	cout << "Fragment shader : ";
	printShaderInfoLog(f);
	
	cout << "Vertex shader : ";
	printShaderInfoLog(v);
	
	//cout << "Geometry shader : ";
	//printShaderInfoLog(g);
	
	cout << "Program : ";
	printProgramInfoLog(p);
}

#endif
