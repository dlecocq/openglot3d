#ifndef RAY_CPP
#define RAY_CPP

#include <math.h>

#include "ray.h"
		
ray::ray() {
	x = 0;
	y = 0;
	z = 0;
}

ray::ray(double myx, double myy, double myz) {
	x = myx;
	y = myy;
	z = myz;
}

double ray::length() {
	return sqrt(x * x + y * y + z * z);
}

void ray::normalize() {
	double l = length();
	x /= l;
	y /= l;
	z /= l;
}

ray ray::operator+(ray other) {			
	double newx, newy, newz;
	newx = x + other.x;
	newy = y + other.y;
	newz = z + other.z;
	return ray(newx, newy, newz);
}

void ray::operator+=(ray other) {
	x += other.x;
	y += other.y;
	z += other.z;
}

double ray::operator*(ray other) {
	return (x * other.x + y * other.y + z * other.z);
}

ray ray::operator/(ray other) {
	double newx, newy, newz;
	newx = y * other.z - z * other.y;
	newy = z * other.x - x * other.z;
	newz = x * other.y - y * other.x;
	return ray(newx, newy, newz);
}

#endif
