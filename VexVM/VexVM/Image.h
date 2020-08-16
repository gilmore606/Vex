#pragma once

#include "Line.h"
#include <vector>

class ROMReader;

class Image
{
public:
	Image();
	void Read(ROMReader* rom);

private:
	std::vector<Line> lines;
};
