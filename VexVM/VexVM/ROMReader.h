#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Sprite.h"
#include "VEXSong.h"
#include "Input.h"

typedef unsigned char BYTE;

class ROMReader
{
public:
	ROMReader(const char* filename);

	void Read(std::vector<Sprite> sprites, std::vector<VEXSong> songs, Input input);

private:
	unsigned char next();
	bool expectMarker(std::string expected);
	std::string getMarker();
	void readSong(std::vector<VEXSong> songs);
	void readCode();
	void readControls(Input input);
	void readInstr();
	void readSprite(std::vector<Sprite> sprites);
	void readData();

	const char* filename;
	std::vector<BYTE> data;
	long cursor = 0;
};

