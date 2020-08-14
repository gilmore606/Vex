#include "ROMReader.h"

ROMReader::ROMReader(const char* filename) {
	this->filename = filename;
}

void ROMReader::Read() {
	std::cout << "reading ROM " << filename << std::endl;
	file = std::ifstream(filename, std::ios_base::binary);

	if (!expectMarker("VEXO")) throw "file format error";
	std::string gameName = getMarker();

	bool eof = false;
	while (!eof) {
		std::string marker = getMarker();
		std::string resourceName = nullptr;
		if (marker.compare("EOF")) {
			eof = true;
		} else {
			resourceName = getMarker();
		}
		if (marker.compare("SONG")) readSong();
		if (marker.compare("CODE")) readCode();
		if (marker.compare("INSTR")) readInstr();
		if (marker.compare("SPRITE")) readSprite();
		if (marker.compare("DATA")) readData();
	}
}

bool ROMReader::expectMarker(std::string marker) {
	return getMarker().compare(marker);
}

std::string ROMReader::getMarker() {
	unsigned char b;
	std::string marker = std::string();
	file >> std::noskipws >> b;
	if (b != 255) throw "file format error";
	file >> std::noskipws >> b;
	while (b != 255) {
		//std::cout << b;
		marker.push_back(b);
		file >> std::noskipws >> b;
	}
	return marker;
}

void ROMReader::readSong() {

}

void ROMReader::readCode() {
	
}

void ROMReader::readInstr() {

}

void ROMReader::readSprite() {

}

void ROMReader::readData() {

}
