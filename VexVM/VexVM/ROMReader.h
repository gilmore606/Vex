#pragma once

#include <iostream>
#include <fstream>
#include <string>

class ROMReader
{
public:
	ROMReader(const char* filename);

	void Read();

private:
	bool expectMarker(std::string marker);
	std::string getMarker();
	void readSong();
	void readCode();
	void readInstr();
	void readSprite();
	void readData();

	const char* filename;
	std::ifstream file;
};

