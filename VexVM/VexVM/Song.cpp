#include "Song.h"
#include "ROMReader.h"

Song::Song() { 
	speed = 1.0;
	resolution = 192.0;
}

void Song::Read(ROMReader* rom) {
	notecount = rom->next3Int();
	resolution = rom->next2Int();
	voicecount = rom->next();
	for (int i = 0; i < notecount; i++) {
		Note* note = new Note();
		note->tick = rom->next3Int();
		note->channel = rom->next();
		switch (rom->next()) {
			case 0: note->type = NOTE_OFF; break;
			case 1: note->type = NOTE_ON; break;
			case 2: note->type = POLY_AFTER; break;
			case 3: note->type = CONTROL_CHANGE; break;
			case 4: note->type = PROGRAM_CHANGE; break;
			case 5: note->type = CHANNEL_AFTER; break;
			case 6: note->type = PITCH_BEND; break;
			case 7: note->type = TEMPO; break;
			default: note->type = SYSEX; break;
		}
		note->data1 = rom->next();
		note->data2 = rom->next();
		notes.push_back(*note);
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
		instruments.push_back(Instrument(w1, w2, a, d, s, r, dt, ph, v, p));
	}
	std::cout << "  read " << voicecount << " instruments" << std::endl;
}

void Song::Reset() {
	notecursor = 0;
	setTempo(600);
	tick = 0.0;
}

void Song::setTempo(int tempo) {
	this->tempo = tempo;
	ticksPerSecond = ((double)tempo / 1000.0) * resolution * speed;
	std::cout << "song tps " << ticksPerSecond << std::endl;
}

// Advance our ticks
void Song::advanceTime(double sec) {
	tick += (sec * ticksPerSecond);
}

Note* Song::getNote() {
	if (notecursor > notecount) return nullptr;
	if (notes[notecursor].tick <= tick) {
		notecursor++;
		return &notes[notecursor - 1];
	}
	return nullptr;
}