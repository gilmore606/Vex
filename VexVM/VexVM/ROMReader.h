#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Song.h"
#include "Sprite.h"
#include "Input.h"
#include "CPU.h"
#include "APU.h"
#include "GPU.h"


typedef unsigned char BYTE;

class ROMReader {
public:
	ROMReader(const char* filename);

	void Read(CPU* cpu, GPU* gpu, APU* apu, Input* input);
	unsigned char next();
	int nextInt();
	int next2Int();
	int next3Int();
	float next2Float();
	bool expectMarker(std::string expected);
	std::string getMarker();

private:
	
	Song* readSong();
	void readCode();
	void readControls(Input* input);
	void readInstr();
	Image* readImage(int id);
	void readData();

	const char* filename;
	std::vector<BYTE> data;
	long cursor = 0;
};
