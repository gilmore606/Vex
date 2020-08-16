#pragma once

#include "Line.h"
#include <vector>

class ROMReader;

class Image
{
public:
	Image();
	void Read(ROMReader* rom);

	std::vector<Line> lines;
};
