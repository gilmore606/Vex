#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "VEXNote.h"

class ROMReader;

class VEXSong {
public:
	bool loop;
	double speed;
	long notecount;
	int resolution;
	int voicecount;

	VEXSong(ROMReader* rom);
	void Reset();
	void setTempo(int tempo);
	void advanceTime(double delta);
	VEXNote* getNote();

private:
	VEXNote* notes;
	long tick;
	long notecursor;
	int tempo;
	double ticksPerSecond;
};
