#include "VEXNote.h"

VEXNote::VEXNote() { }

VEXNote::VEXNote(long tick, int channel, VEXNoteType type, int data1, int data2) {
	this->tick = tick;
	this->channel = channel;
	this->type = type;
	this->data1 = data1;
	this->data2 = data2;
}
