#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "VEXSong.h"
#include "Sprite.h"
#include "Input.h"
#include "APU.h"
#include "GPU.h"


typedef unsigned char BYTE;

class ROMReader {
public:
	ROMReader(const char* filename);

	void Read(GPU* gpu, APU* apu, Input* input);
	unsigned char next();
	int nextInt();
	int next2Int();
	int next3Int();
	float next2Float();
	bool expectMarker(std::string expected);
	std::string getMarker();

private:
	
	VEXSong* readSong();
	void readCode();
	void readControls(Input* input);
	void readInstr();
	Image* readImage(int id);
	void readData();

	const char* filename;
	std::vector<BYTE> data;
	long cursor = 0;
};
