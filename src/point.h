#ifndef POINT_H
#define POINT_H

#include "ray.h"

class point {
	public:
		double x,y,z;
		
		point();
		
		point(double myx, double myy, double myz);
		
		ray operator -(point other);
};

#endif
