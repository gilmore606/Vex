#include "ROMReader.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

ROMReader::ROMReader(const char* filename) {
	this->filename = filename;
}

void ROMReader::Read(std::vector<Sprite> sprites, std::vector<VEXSong> songs, Input* input) {
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
				readSong(songs, resourceName);
			} else if (marker.compare("COD") == 0) {
				readCode();
			} else if (marker.compare("CTR") == 0) {
				readControls(input);
			} else if (marker.compare("SPR") == 0) {
				readSprite(sprites);
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
	if (b1 != 255) {
		std::cout << b1 << std::endl;
		throw "file format error";
	}
	while (b2 != 255) {
		marker.push_back(b2);
		b2 = next();
	}
	return marker;
}

void ROMReader::readSong(std::vector<VEXSong> songs, std::string title) {
	VEXSong* song = new VEXSong(this);
	songs.push_back(*song);
}

void ROMReader::readCode() {
	std::cout << "reading code [PLACEHOLDER]..." << std::endl;
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

void ROMReader::readSprite(std::vector<Sprite> sprites) {
	std::cout << "reading sprite..." << std::endl;
}

void ROMReader::readData() {
	std::cout << "reading data..." << std::endl;
}
