#include "MyLine.h"
#include <cmath>

Myline::Myline()
{
	startX = 0;
	endX = 30;
	startY = 0;
	endY = 40;
}

void Myline::move(unsigned int direction, float moving)
{
	switch (direction)
	{
	case AXIS_X:
		startX += moving;
		endX += moving;
		break;
	case AXIS_Y:
		startY += moving;
		endY += moving;
		break;
	default:
		break;
	}
}

//Вращает отрезок относительно начальной точки на angle (angle в радинах)
void Myline::rotate(double angle)
{
	int tmp = endX;
	endX = (endX - startX) * cos(angle) - (endY - startY) * sin(angle) + startX;
	endY = (tmp - startX) * sin(angle) + (endY - startY) * cos(angle) + startY;
}

//Меняет размер отрезка в scale раз
void Myline::scale(float scale)
{

	if (!(abs(endX - startX) < 1 && abs(endY - startY) < 1 && scale < 1))
	{
		endX = (endX - startX) * scale + startX;
		endY = (endY - startY) * scale + startY;
	}
}

//Меняет местами начальную и конечную точку отрезка
void Myline::swap()
{
	float tmp = startX;
	startX = endX;
	endX = tmp;

	tmp = startY;
	startY = endY;
	endY = tmp;
}

//Возращает структуру POINT с целочисленными координатами начальной точки 
POINT Myline::getStartPoint()
{
	POINT result;
	result.x = static_cast<int>(startX);
	result.y = static_cast<int>(startY);
	return result;
}

//Возращает структуру POINT с целочисленными координатами конечной точки 
POINT Myline::getEndPoint()
{
	POINT result;
	result.x = static_cast<int>(endX);
	result.y = static_cast<int>(endY);
	return result;
}