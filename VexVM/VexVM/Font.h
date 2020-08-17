#pragma once

#include <unordered_map>
#include "Line.h"
#include <string>

class ROMReader;

class Font
{
public:
	Font();
	void Read(ROMReader* rom);
	int countLinesInText(std::string* text);
	std::unordered_map<int, std::vector<Line>*> glyphs;

private:
};
