#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Song.h"
#include "Sprite.h"
#include "Input.h"
#include "CPU.h"
#include "Code.h"
#include "APU.h"
#include "GPU.h"
#include "Font.h"


typedef unsigned char BYTE;

class ROMReader {
public:
	ROMReader(const char* filename);

	void Read(CPU* cpu, GPU* gpu, APU* apu, Input* input);
	unsigned char next();
	int nextInt();
	int next2Int();
	int next3Int();
	int next4Int();
	float next2Float();
	float next4Float();
	std::string nextString();
	bool expectMarker(std::string expected);
	std::string getMarker();

	long cursor = 0;
private:
	
	Song* readSong(int id);
	Code* readCode();
	void readControls(Input* input);
	Image* readImage(int id);
	ParticleDef readParticle(int id);
	Font* readFont(int id);
	void readData();

	const char* filename;
	std::vector<BYTE> data;
};
