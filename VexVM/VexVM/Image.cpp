#include "Image.h"
#include "ROMReader.h"
#include <iostream>

Image::Image() { 
	lines.reserve(16);
	points.reserve(8);
}

void Image::Read(ROMReader* rom) {
	int floatc = rom->next2Int();
	for (int i = 0; i < (floatc / 7); i++) {
		float x1 = rom->next2Float();
		float y1 = rom->next2Float();
		float x2 = rom->next2Float();
		float y2 = rom->next2Float();
		float r = rom->next2Float();
		float g = rom->next2Float();
		float b = rom->next2Float();
		lines.push_back(Line(x1, y1, x2, y2, r, g, b));
	}
	floatc = rom->next2Int();
	for (int i = 0; i < (floatc / 6); i++) {
		float x = rom->next2Float();
		float y = rom->next2Float();
		float size = rom->next2Float() * 4.0;
		float r = rom->next2Float();
		float g = rom->next2Float();
		float b = rom->next2Float();
		points.push_back(Point(x, y, r, g, b, size));
	}
}
