#pragma once

#include <iostream>
#include <fstream>
#include <string>



enum VEXNoteType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, BEND_RANGE, SYSEX };

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
	VEXNote* notes;

	int cint(unsigned char c) {
		return (int)c;
	}

	VEXSong(const char* filepath) {
		std::ifstream infile(filepath, std::ios_base::binary);
		unsigned char b1,b2,b3,b4,b5,b6,b7;
		infile >> std::noskipws >> b1;
		infile >> std::noskipws >> b2;
		infile >> std::noskipws >> b3;
		int notecount = b1 + (b2 * 256) + (b3 * 65536);
		notes = new VEXNote[notecount];
		for (int i = 0; i < notecount; i++) {
			infile >> std::noskipws >> b1;
			infile >> std::noskipws >> b2;
			infile >> std::noskipws >> b3;
			infile >> std::noskipws >> b4;
			infile >> std::noskipws >> b5;
			infile >> std::noskipws >> b6;
			infile >> std::noskipws >> b7;
			notes[i].tick = b1 + (b2 * 256) + (b3 * 65536);
			notes[i].channel = b4;
			switch (b5) {
				case 0: notes[i].type = NOTE_OFF; break;
				case 1: notes[i].type = NOTE_ON; break;
				case 2: notes[i].type = POLY_AFTER; break;
				case 3: notes[i].type = CONTROL_CHANGE; break;
				case 4: notes[i].type = PROGRAM_CHANGE; break;
				case 5: notes[i].type = CHANNEL_AFTER; break;
				case 6: notes[i].type = BEND_RANGE; break;
				default: notes[i].type = SYSEX; break;
			}
			notes[i].data1 = b6;
			notes[i].data2 = b7;
			//std::cout << notes[i].tick << ": " << notes[i].channel << "  " << notes[i].type << " (" << notes[i].data1 << "," << notes[i].data2 << ")" << std::endl;
		}
		infile.close();
		std::cout << "APU read song " << filepath << " (" << notecount << " notes)" << std::endl;
	}
private:
};

