#pragma once

enum NoteType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, PITCH_BEND, TEMPO, SYSEX };

class Note
{
public:
	long tick;
	int channel;
	NoteType type;
	int data1;
	int data2;
	Note();
	Note(long tick, int channel, NoteType type, int data1, int data2);
private:
};
