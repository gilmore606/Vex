#include "ROMReader.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

ROMReader::ROMReader(const char* filename) {
	this->filename = filename;
}

void ROMReader::Read(std::vector<Sprite> sprites, std::vector<VEXSong> songs, Input input) {
	std::cout << "reading ROM " << filename << std::endl;

	std::basic_ifstream<BYTE> file(filename, std::ios::binary);
	data = std::vector<BYTE>((std::istreambuf_iterator<BYTE>(file)), std::istreambuf_iterator<BYTE>());
	std::cout << "read " << data.size() << " bytes" << std::endl;
	file.close();

	if (!expectMarker("VEXO")) throw "file format error";
	std::cout << "got header" << std::endl;
	std::string gameName = getMarker();
	std::cout << "reading game " << gameName << std::endl;

	bool eof = false;
	while (!eof) {
		std::string marker = getMarker();
		std::string resourceName = "";
		if (marker.compare("EOF") == 0) {
			eof = true;
		} else {
			resourceName = getMarker();
			std::cout << "read segment " << marker << ":" << resourceName << std::endl;
		}
		if (marker.compare("SONG") == 0) {
			readSong(songs);
		} else if (marker.compare("CODE") == 0) {
			readCode();
		} else if (marker.compare("CONTROLS") == 0) {
			readControls(input);
		} else if (marker.compare("SPRITE") == 0) {
			readSprite(sprites);
		} else if (marker.compare("DATA") == 0) {
			readData();
		}
	}
	std::cout << "ROMReader parsed " << data.size() << " bytes from " << filename << std::endl;
	delete &data;
}

BYTE ROMReader::next() {
	BYTE b = data[cursor];
	cursor++;
	return b;
}

bool ROMReader::expectMarker(std::string expected) {
	std::string marker = getMarker();
	bool match = marker.compare(expected) == 0;
	return match;
}

std::string ROMReader::getMarker() {
	unsigned char b1, b2;
	std::string marker = std::string();
	marker = "";
	b1 = next();
	b2 = next();
	if (b1 != 255) std::cout << "file format error";
	while (b2 != 255) {
		marker.push_back(b2);
		b2 = next();
	}
	return marker;
}

void ROMReader::readSong(std::vector<VEXSong> songs) {
	std::cout << "reading song..." << std::endl;
}

void ROMReader::readCode() {
	std::cout << "reading code..." << std::endl;
}

void ROMReader::readControls(Input input) {
	std::cout << "reading controls..." << std::endl;
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
