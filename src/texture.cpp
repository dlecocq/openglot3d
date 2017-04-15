#ifndef TEXTURE_CPP
#define TEXTURE_CPP

#include <iostream>
#include <fstream>
#include <cassert>

#include "texture.h"

using namespace glot;
using namespace std;

void texture::read_ppm(const char * filename) {
	char c;
	
	cout << "Reading " << filename << endl;

	ifstream ifs ( filename );
	if (!ifs) {
		cerr << "Can't open " << filename << endl;
		exit(1);
	}

	// Read the Magic code.  Should be P6
	ifs >> c;
	assert(c=='P');
	ifs >> c;
	assert(c=='6');   // binary PPMs start with the magic code `P6'
	
	// Read the width and height of the image
	ifs >> width >> height;
	
	int i;
	ifs >> i;
	// all images we'll use have 255 color levels
	assert(i==255);
	
	// need to skip one more byte
	ifs.get();
	
	data = (char*) malloc(width * height * 3 * sizeof(char));
	ifs.read(data, width * height * 3 * sizeof(char));
	ifs.close();
}

#endif
