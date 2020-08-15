#include "VEXSong.h"
#include "ROMReader.h"
#include "VEXInstrument.h"

VEXSong::VEXSong(ROMReader* rom) {
	notecount = rom->next3Int();
	resolution = rom->next2Int();
	voicecount = rom->next();
	notes = new VEXNote[notecount];
	for (int i = 0; i < notecount; i++) {
		notes[i].tick = rom->next3Int();
		notes[i].channel = rom->next();
		switch (rom->next()) {
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
		notes[i].data1 = rom->next();
		notes[i].data2 = rom->next();
		//std::cout << notes[i].tick << ": " << notes[i].channel << "  " << notes[i].type << " (" << notes[i].data1 << "," << notes[i].data2 << ")" << std::endl;
	}
	std::cout << "  read " << notecount << " notes" << std::endl;
	Reset();

	if (!rom->expectMarker("INS")) throw "expected instruments after song";
	for (int i = 0; i < voicecount; i++) {
		int w1 = rom->nextInt();
		int w2 = rom->nextInt();
		int a = rom->nextInt();
		int d = rom->nextInt();
		int s = rom->nextInt();
		int r = rom->nextInt();
		int dt = rom->nextInt();
		int ph = rom->nextInt();
		int v = rom->nextInt();
		int p = rom->nextInt();
		VEXInstrument instrument = VEXInstrument(w1, w2, a, d, s, r, dt, ph, v, p);

	}
	std::cout << "  read " << voicecount << " instruments" << std::endl;
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
