#ifndef RAY_H
#define RAY_H

class ray {
	
	public:
		double x, y, z;
		
		ray();
		
		ray(double myx, double myy, double myz);
		
		double length();
		
		void normalize();
		
		ray operator +(ray other);
		void operator +=(ray other);
		double operator *(ray other);
		ray operator /(ray other);
	
};

#endif
