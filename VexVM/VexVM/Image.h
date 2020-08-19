#pragma once

#include "Line.h"
#include "Point.h"
#include <vector>

class ROMReader;

class Image
{
public:
	Image();
	void Read(ROMReader* rom);

	std::vector<Line> lines;
	std::vector<Point> points;
};
