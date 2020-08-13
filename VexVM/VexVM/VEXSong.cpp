#include "VEXSong.h"

VEXSong::VEXSong(const char* filepath) {
	std::ifstream infile(filepath, std::ios_base::binary);
	unsigned char b1,b2,b3,b4,b5,b6,b7;
	infile >> std::noskipws >> b1;
	infile >> std::noskipws >> b2;
	infile >> std::noskipws >> b3;
	infile >> std::noskipws >> b4;
	infile >> std::noskipws >> b5;
	notecount = b1 + (b2 * 256) + (b3 * 65536);
	resolution = b4 + (b5 * 256);
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
			case 6: notes[i].type = PITCH_BEND; break;
			case 7: notes[i].type = TEMPO; break;
			default: notes[i].type = SYSEX; break;
		}
		notes[i].data1 = b6;
		notes[i].data2 = b7;
		//std::cout << notes[i].tick << ": " << notes[i].channel << "  " << notes[i].type << " (" << notes[i].data1 << "," << notes[i].data2 << ")" << std::endl;
	}
	infile.close();
	std::cout << "APU read song " << filepath << " (" << notecount << " notes)" << std::endl;
	Reset();
}

void VEXSong::Reset() {
	notecursor = 0;
	setTempo(600);
	tick = 0.0;
}

void VEXSong::setTempo(int tempo) {
	this->tempo = tempo;
	ticksPerSecond = ((double)tempo / 1000.0) * resolution * speed;
}

// Advance our ticks
void VEXSong::advanceTime(double sec) {
	tick += (sec * ticksPerSecond);
}

VEXNote* VEXSong::getNote() {
	if (notecursor > notecount) return nullptr;
	if (notes[notecursor].tick <= tick) {
		notecursor++;
		return &notes[notecursor - 1];
	}
	return nullptr;
}
