#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "VEXNote.h"
#include "VEXInstrument.h"

class ROMReader;

class VEXSong {
public:
	bool loop;
	double speed;
	long notecount;
	int resolution;
	int voicecount;
	std::vector<VEXInstrument> instruments;

	VEXSong();
	void Read(ROMReader* rom);
	void Reset();
	void setTempo(int tempo);
	void advanceTime(double delta);
	VEXNote* getNote();

private:
	std::vector<VEXNote> notes;
	long tick;
	long notecursor;
	int tempo;
	double ticksPerSecond;
};
