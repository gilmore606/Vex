#include "ROMReader.h"

ROMReader::ROMReader(const char* filename) {
	this->filename = filename;
}

void ROMReader::Read() {
	std::cout << "reading ROM " << filename << std::endl;
	file = std::ifstream(filename, std::ios_base::binary);

	if (!expectTag("VEXO")) throw "file format error";
	std::string gameName = getTag();

	bool eof = false;
	while (!eof) {
		std::string tag = getTag();
		std::string resourceName = nullptr;
		if (tag.compare("EOF")) {
			eof = true;
		} else {
			resourceName = getTag();
		}
		if (tag.compare("SONG")) readSong();
		if (tag.compare("CODE")) readCode();
		if (tag.compare("INSTR")) readInstr();
		if (tag.compare("SPRITE")) readSprite();
		if (tag.compare("DATA")) readData();
	}
}

bool ROMReader::expectTag(std::string tag) {
	return getTag().compare(tag);
}

std::string ROMReader::getTag() {
	unsigned char b;
	std::string tag = std::string();
	file >> std::noskipws >> b;
	if (b != 255) throw "file format error";
	file >> std::noskipws >> b;
	while (b != 255) {
		//std::cout << b;
		tag.push_back(b);
		file >> std::noskipws >> b;
	}
	return tag;
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
