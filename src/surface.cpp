#ifndef SURFACE_CPP
#define SURFACE_CPP

#include <iostream>

#include "surface.h"

using namespace glot;
using namespace std;

void surface::dl_gen(const screen& scr) {
	
	double width = 8;
	
	double a = -width/2;
	double b = -width/2;
	
	int count = 200;
	
	double dx = width / count;
	
	double ap = a + dx;
	double bp = b + dx;
	
	glBegin(GL_QUADS);
	
	for (int i = 0; i < count; ++i) {
		b = -width / 2;
		bp = b + dx;
		for (int j = 0; j < count; ++j) {
			glTexCoord2d(a,		b);
				glVertex3d(a,  	b,  scr.time);
			glTexCoord2d(ap,	b);
				glVertex3d(ap, 	b,  scr.time);
			glTexCoord2d(ap,	bp);
				glVertex3d(ap, 	bp, scr.time);
			glTexCoord2d(a,		bp);
				glVertex3d(a,  	bp, scr.time);
			b = bp;
			bp += dx;
		}
		a = ap;
		ap += dx;
	}
		
	glEnd();
}

string surface::get_vert_shader() {
	return read_file("shaders/surface.vert.front") + func + read_file("shaders/surface.vert.back");
	//return read_file("shaders/surface.vert.front") + read_file("shaders/surface.vert.back");
	//return read_file("shaders/surface.vert");
}

string surface::get_geom_shader() {
	return read_file("shaders/surface.geom");
}

void surface::gen_shader() {
	
	cout << "Creating shader" << endl;
	f = glCreateShader(GL_FRAGMENT_SHADER);
	v = glCreateShader(GL_VERTEX_SHADER);
	//g = glCreateShader(GL_GEOMETRY_SHADER_EXT);

	//string frag_src = read_file("shaders/surface.frag.front") + func + read_file("shaders/surface.frag.back");
	string frag_src = read_file("shaders/surface.frag");
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
