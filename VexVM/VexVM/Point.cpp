#include "Point.h"

Point::Point() : x(0.0f), y(0.0f), r(1.0f), g(1.0f), b(1.0f) { }

Point::Point(float x, float y, float r, float g, float b) {
	this->x = x;
	this->y = y;
	this->r = r;
	this->g = g;
	this->b = b;
}

void Point::pushData(float dataArray[], int* counter) {
	// TODO: this is fake, probably need to make a quad or something
	dataArray[*counter] = x;
	dataArray[*counter + 1] = y;
	dataArray[*counter + 2] = r;
	dataArray[*counter + 3] = g;
	dataArray[*counter + 4] = b;
	*counter += 5;
}
