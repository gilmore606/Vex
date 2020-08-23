#include "Song.h"
#include "ROMReader.h"

Song::Song(int id) { 
	this->id = id;
	speed = 4.0;
	resolution = 192.0;
}

void Song::Read(ROMReader* rom) {
	notecount = rom->next3Int();
	resolution = rom->next2Int();
	int voicecount = rom->next();
	notes.reserve(notecount);
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
		Voice voice;
		voice.Read(rom);
		addVoice(voice);
	}
	std::cout << "  read " << voicecount << " instruments" << std::endl;
}

void Song::addVoice(Voice voice) {
	voices.push_back(voice);
}

void Song::Reset() {
	notecursor = 0;
	setTempo(600); // 120bpm default tempo
	tick = 0.0;
	done = false;
}

void Song::setTempo(int tempo) {
	this->tempo = tempo;
	ticksPerSecond = ((double)tempo / 1000.0) * resolution * speed;
	std::cout << "song tps " << ticksPerSecond << std::endl;
}
