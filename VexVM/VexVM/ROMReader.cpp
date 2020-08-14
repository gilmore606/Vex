#include "ROMReader.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

ROMReader::ROMReader(const char* filename) {
	this->filename = filename;
}

void ROMReader::Read(std::vector<Sprite> sprites, std::vector<VEXSong> songs, Input input) {
	std::cout << "reading ROM " << filename << std::endl;

	std::streampos fileSize = 0;
	PLARGE_INTEGER fs = 0;
	HANDLE hFile = CreateFile(LPCWSTR(filename), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	GetFileSizeEx(hFile, fs);
	CloseHandle(hFile);
	fileSize = fs->QuadPart;


	std::ifstream file(filename, std::ios_base::binary);
	file.seekg(0, std::ios::beg);
	std::cout << fileSize << " bytes to read";
	std::vector<BYTE> fileData(fileSize);
	file.read((char*)&data[0], fileSize);
	file.close();
	data = fileData;
	data.begin();
	std::cout << fileSize << " bytes read";


	if (!expectMarker("VEXO")) throw "file format error";
	std::string gameName = getMarker();
	std::cout << gameName << std::endl;

	bool eof = false;
	while (!eof) {
		std::string marker = getMarker();
		std::cout << "MARKER " << marker << std::endl;
		std::string resourceName = nullptr;
		if (marker.compare("EOF")) {
			eof = true;
		} else {
			resourceName = getMarker();
		}
		if (marker.compare("SONG")) {
			readSong(songs);
		} else if (marker.compare("CODE")) {
			readCode();
		} else if (marker.compare("CONTROLS")) {
			readControls(input);
		} else if (marker.compare("SPRITE")) {
			readSprite(sprites);
		} else if (marker.compare("DATA")) {
			readData();
		}
	}
}

BYTE ROMReader::next() {
	BYTE b = data[cursor];
	cursor++;
	return b;
}

bool ROMReader::expectMarker(std::string expected) {
	std::string marker = getMarker();
	bool match = marker.compare(expected);
	std::cout << marker << std::endl;
	return match;
}

std::string ROMReader::getMarker() {
	unsigned char b1, b2;
	std::string marker = std::string();
	marker = "";
	b1 = next();
	b2 = next();
	std::cout << "looks like " << (int)b1 << std::endl;
	throw "FUCK";
	if (b1 != 255) std::cout << "file format error";
	while (b2 != 255) {
		marker.push_back(b2);
		b2 = next();
	}
	return marker;
}

void ROMReader::readSong(std::vector<VEXSong> songs) {
	std::cout << "reading song...";
}

void ROMReader::readCode() {
	std::cout << "reading code...";
}

void ROMReader::readControls(Input input) {
	std::cout << "reading controls...";
}

void ROMReader::readInstr() {
	std::cout << "reading instr...";
}

void ROMReader::readSprite(std::vector<Sprite> sprites) {
	std::cout << "reading sprite...";
}

void ROMReader::readData() {
	std::cout << "reading data...";
}
