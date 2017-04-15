#ifndef P_CURVE_CPP
#define P_CURVE_CPP

#include <iostream>

#include "p_curve.h"

using namespace glot;
using namespace std;

void p_curve::dl_gen(const screen& scr) {
	
	double width = 1.0;
	
	double a = 0;
	
	int count = 500;
	
	double dx = width / count;
	
	glBegin(GL_LINE_STRIP);
	
	for (int i = 0; i < count; ++i) {
		glVertex3d(a, 0.0, 0);
		a += dx;
	}
		
	glEnd();
}

string p_curve::get_vert_shader() {
	string program = read_file("shaders/p_curve.vert.1") + func + read_file("shaders/p_curve.vert.2");
	
	if (layout & CYLINDRICAL) {
		program += "gl_Position = vec4(gl_Position.y * cos(gl_Position.x), gl_Position.y * sin(gl_Position.x), gl_Position.z, 1.0);";
	} else if (layout & SPHERICAL) {
		program += "gl_Position = vec4(gl_Position.x * sin(gl_Position.y) * cos(gl_Position.z), gl_Position.x * sin(gl_Position.y) * sin(gl_Position.z), gl_Position.x * cos(gl_Position.y), 1.0);";		
	}
	
	program += read_file("shaders/p_curve.vert.3");
	
	return program;
}

string p_curve::get_geom_shader() {
	return read_file("shaders/p_curve.geom");
}

void p_curve::gen_shader() {
	
	cout << "Creating shader" << endl;
	f = glCreateShader(GL_FRAGMENT_SHADER);
	v = glCreateShader(GL_VERTEX_SHADER);
	//g = glCreateShader(GL_GEOMETRY_SHADER_EXT);

	//string frag_src = read_file("shaders/surface.frag.front") + func + read_file("shaders/surface.frag.back");
	string frag_src = read_file("shaders/p_curve.frag");
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
