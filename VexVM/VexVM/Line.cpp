#include "Line.h"
#include <iostream>

Line::Line() : x1(0.0f), y1(0.0f), x2(0.0f), y2(0.0f) { }

Line::Line(float x1, float y1, float x2, float y2, float r, float g, float b) {
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	this->r = r;
	this->g = g;
	this->b = b;
}

