#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Note.h"
#include "Instrument.h"

class ROMReader;

class Song {
public:
	bool loop;
	double speed;
	long notecount;
	int resolution;
	int voicecount;
	std::vector<Instrument> instruments;

	Song();
	void Read(ROMReader* rom);
	void Reset();
	void setTempo(int tempo);
	void advanceTime(double delta);
	Note* getNote();

private:
	std::vector<Note> notes;
	long tick;
	long notecursor;
	int tempo;
	double ticksPerSecond;
};
