#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "VEXSong.h"
#include "Sprite.h"
#include "Input.h"


typedef unsigned char BYTE;

class ROMReader
{
public:
	ROMReader(const char* filename);

	void Read(std::vector<Sprite> sprites, std::vector<VEXSong> songs, Input* input);
	unsigned char next();
	int nextInt();
	int next2Int();
	int next3Int();
	bool expectMarker(std::string expected);
	std::string getMarker();

private:
	
	void readSong(std::vector<VEXSong> songs, std::string title);
	void readCode();
	void readControls(Input* input);
	void readInstr();
	void readSprite(std::vector<Sprite> sprites);
	void readData();

	const char* filename;
	std::vector<BYTE> data;
	long cursor = 0;
};
