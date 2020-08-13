#pragma once

#include <iostream>
#include <fstream>
#include <string>


enum VEXNoteType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, PITCH_BEND, TEMPO, SYSEX };

class VEXNote {
public:
	long tick;
	int channel;
	VEXNoteType type;
	int data1;
	int data2;
	VEXNote() { };
	VEXNote(long tick, int channel, VEXNoteType type, int data1, int data2) {
		this->tick = tick;
		this->channel = channel;
		this->type = type;
		this->data1 = data1;
		this->data2 = data2;
	}
private:
};

class VEXSong {
public:
	bool loop;
	long notecount;
	int resolution;

	VEXSong(const char* filepath);
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

