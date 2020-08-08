#include "Line.h"
#include <iostream>

Line::Line() : x1(0.0f), y1(0.0f), x2(0.0f), y2(0.0f) { }

Line::Line(float x1, float y1, float x2, float y2) {
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;

	this->xd = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.01f;
	this->yd = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.01f;
}

void Line::pushData(float dataArray[], int* counter) {
	dataArray[*counter] = x1;
	dataArray[*counter + 1] = y1;
	dataArray[*counter + 2] = x2;
	dataArray[*counter + 3] = y2;
	*counter += 4;
}

void Line::move() {
	x1 += xd;
	y1 += yd;
	x2 += xd;
	y2 += yd;
}
