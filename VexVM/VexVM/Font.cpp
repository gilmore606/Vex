#include "Font.h"
#include "ROMReader.h"
#include <vector>
#include <iostream>

Font::Font() {

}

void Font::Read(ROMReader* rom) {
	int glyphc = rom->nextInt();
	for (int i = 0; i < glyphc; i++) {
		int c = rom->nextInt();
		int datac = rom->next2Int();
		std::vector<Line>* lines = new std::vector<Line>;
		lines->reserve(datac / 4);
		for (int j = 0; j < (datac / 4); j++) {
			float x1 = rom->next2Float();
			float y1 = rom->next2Float();
			float x2 = rom->next2Float();
			float y2 = rom->next2Float();
			lines->push_back(Line(x1, y1, x2, y2, 1.0f, 1.0f, 1.0f));
		}
		std::cout << "  glyph " << c << " (" << lines->size() << " lines)" << std::endl;
		glyphs[c] = lines;
	}
}

int Font::countLinesInText(std::string* text) {
	int lc = 0;
	for (char& c : *text) {
		if (glyphs.count(c) > 0) {
			lc += glyphs[c]->size();
		}
	}
	std::cout << "counted " << lc << " lines in " << *text << std::endl;
	return lc;
}
