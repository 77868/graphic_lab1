#ifndef MYLINE_H
#define MYLINE_H

#include  <windows.h>

#define AXIS_Y 0
#define AXIS_X 1

class Myline
{
public:
	Myline();
	void move(unsigned int direction, float speed);
	void rotate(double angle);
	void scale(float scale);
	void swap();
	POINT getStartPoint();
	POINT getEndPoint();
private:
	float startX, startY, endX, endY;
};
#endif 

