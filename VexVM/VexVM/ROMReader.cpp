#include "ROMReader.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

ROMReader::ROMReader(const char* filename) {
	this->filename = filename;
}

void ROMReader::Read(CPU* cpu, GPU* gpu, APU* apu, Input* input) {
	std::cout << "reading ROM " << filename << std::endl;

	std::basic_ifstream<BYTE> file(filename, std::ios::binary);
	data = std::vector<BYTE>((std::istreambuf_iterator<BYTE>(file)), std::istreambuf_iterator<BYTE>());
	std::cout << "read " << data.size() << " bytes" << std::endl;
	file.close();

	if (!expectMarker("VEX")) throw "file format error";
	std::cout << "  got header" << std::endl;
	std::string gameName = getMarker();
	float aspectRatio = (float)next() / (float)next();
	std::cout << "reading game " << gameName << std::endl;
	std::cout << "  screen aspect ratio " << aspectRatio << std::endl;

	bool eof = false;
	while (!eof) {
		std::string marker = getMarker();
		std::string resourceName = "";

		if (marker.compare("EOF") == 0) {
			std::cout << "read EOF" << std::endl;
			eof = true;
		} else {
			resourceName = getMarker();
			std::cout << "read segment " << marker << ": " << resourceName << std::endl;
			if (marker.compare("SNG") == 0) {
				apu->LoadSong(readSong(std::stoi(resourceName)));
			} else if (marker.compare("COD") == 0) {
				cpu->LoadCode(readCode());
			} else if (marker.compare("CTR") == 0) {
				readControls(input);
			} else if (marker.compare("IMG") == 0) {
				gpu->loadImage(readImage(std::stoi(resourceName)));
			} else if (marker.compare("FNT") == 0) {
				gpu->loadFont(readFont(std::stoi(resourceName)));
			} else if (marker.compare("DAT") == 0) {
				readData();
			}
		}
	}
	std::cout << "ROMReader parsed " << data.size() << " bytes from " << filename << std::endl;
}

BYTE ROMReader::next() {
	BYTE b = data[cursor];
	cursor++;
	return b;
}
int ROMReader::nextInt() {
	BYTE b = data[cursor];
	cursor++;
	return b * 1;
}
int ROMReader::next2Int() {
	BYTE b1 = data[cursor];
	BYTE b2 = data[cursor + 1];
	cursor += 2;
	return b1 + b2 * 256;
}
int ROMReader::next3Int() {
	BYTE b1 = data[cursor];
	BYTE b2 = data[cursor + 1];
	BYTE b3 = data[cursor + 2];
	cursor += 3;
	return b1 + b2 * 256 + b3 * 65536;
}
float ROMReader::next2Float() {
	int i = next2Int();
	return ((float)i / 65536.0f) * 2.0f - 1.0f;
}

std::string ROMReader::nextString() {
	BYTE b1, b2;
	std::string s = std::string();
	b1 = next();
	b2 = next();
	if (b1 != 254) { throw "file format error"; }
	while (b2 != 254) {
		s.push_back(b2);
		b2 = next();
	}
	return s;
}

bool ROMReader::expectMarker(std::string expected) {
	std::string marker = getMarker();
	return marker.compare(expected) == 0;
}

std::string ROMReader::getMarker() {
	BYTE b1, b2;
	std::string marker = std::string();
	marker = "";
	b1 = next();
	b2 = next();
	if (b1 != 255) { throw "file format error"; }
	while (b2 != 255) {
		marker.push_back(b2);
		b2 = next();
	}
	return marker;
}

Song* ROMReader::readSong(int id) {
	Song* song = new Song(id);
	song->Read(this);
	return song;
}

Code* ROMReader::readCode() {
	Code* code = new Code();
	code->Read(this);
	return code;
}

void ROMReader::readControls(Input* input) {
	BYTE controlCount = next();
	for (int i = 0; i < controlCount; i++) {
		int glkey = next2Int();
		BYTE typeB = next();
		VEXInputType type = SWITCH;
		if (typeB == 1) {
			type = BUTTON;
		}
		int debounce = next2Int();
		input->Add(i, type, glkey);
		std::cout << "  mapped " << (int)glkey << " to control " << i << std::endl;
	}
}

void ROMReader::readInstr() {
	std::cout << "reading instr..." << std::endl;
}

Image* ROMReader::readImage(int id) {
	Image* image = new Image();
	image->Read(this);
	return image;
}

Font* ROMReader::readFont(int id) {
	Font* font = new Font();
	font->Read(this);
	return font;
}

void ROMReader::readData() {
	std::cout << "reading data..." << std::endl;
}
