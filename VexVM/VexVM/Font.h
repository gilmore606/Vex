#pragma once

#include <unordered_map>
#include "Line.h"

class ROMReader;

class Font
{
public:
	Font();
	void Read(ROMReader* rom);

private:
	std::unordered_map<int, std::vector<Line>*> glyphs;
};
