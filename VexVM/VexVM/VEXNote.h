#pragma once

enum VEXNoteType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, PITCH_BEND, TEMPO, SYSEX };

class VEXNote
{
public:
	long tick;
	int channel;
	VEXNoteType type;
	int data1;
	int data2;
	VEXNote();
	VEXNote(long tick, int channel, VEXNoteType type, int data1, int data2);
private:
};
