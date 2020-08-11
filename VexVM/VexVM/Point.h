#pragma once

#include "Vertbuffer.h"

class Point
{
public:
	Point();
	Point(float x, float y, float r, float g, float b, float size);

	void pushData(float dataArray[], int* counter);

	float x, y, r, g, b, size;
};

inline void Point::pushData(float dataArray[], int* counter) {
	dataArray[*counter] = x;
	dataArray[*counter + 1] = y;
	dataArray[*counter + 2] = r;
	dataArray[*counter + 3] = g;
	dataArray[*counter + 4] = b;
	dataArray[*counter + 5] = size;
	*counter += 6;
}
