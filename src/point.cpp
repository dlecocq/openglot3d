#ifndef POINT_CPP
#define POINT_CPP

#include "point.h"
#include "ray.h"

point::point() {
	x = 0;
	y = 0;
	z = 0;
}

point::point(double myx, double myy, double myz) {
	x = myx;
	y = myy;
	z = myz;
}

ray point::operator-(point other) {
	double newx, newy, newz;
	newx = x - other.x;
	newy = y - other.y;
	newz = z - other.z;
	return ray(newx, newy, newz);
}

#endif
