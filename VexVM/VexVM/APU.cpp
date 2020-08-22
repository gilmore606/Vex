#include "APU.h"
#include "RtAudio.h"
#include <iostream>
#include "util.cpp"
#include "Input.h"

APU::APU() { 
	started = false;
	livesong = nullptr;
	midi = nullptr;
	adac = nullptr;
	lastTime = glfwGetTime();
	time = lastTime;
	deltaTime = 0.0;
}

void APU::Reset() { 
	playing.clear();
}

inline static double sumPans(double p1, double p2, double p3) {
	double s = (p1 + p2 + p3) - 1.5;
	return (s < 1.0) ? (s > 0.0 ? s : 0.0) : 1.0;
}

int APU::genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {

	unsigned int i, j, v;
	double* buffer = (double*)outBuffer;
	if (status) std::cout << "APU stream underflow detected" << std::endl;
	double sample;

	for (i = 0; i < nFrames; i++) {
		// make a sample for every voice of every song
		for (auto const& s : playing) {
			for (int i = 0; i < s->voices.size(); i++) {
				s->voices[i].genSample();
			}
		}
		// make a sample for every solo voice
		for (auto const& v : voices) {
			v->genSample();
		}
		// left + right channels
		for (j = 0; j < 2; j++) {
			sample = 0.0;
			// add up panned samples of every voice of every song
			for (auto const& s : playing) {
				for (int i = 0; i < s->voices.size(); i++) {
					Voice* v = &s->voices[i];
					if (j == 0) {
						sample += v->outsample * (1.0 - sumPans(v->pan, v->ccPan, v->songPan));
					} else {
						sample += v->outsample * sumPans(v->pan, v->ccPan, v->songPan);
					}
				}
			}
			// add solo voices
			for (auto const& v : voices) {
				if (j == 0) {
					sample += v->outsample * (1.0 - sumPans(v->pan, v->ccPan, v->songPan));
				} else {
					sample += v->outsample * sumPans(v->pan, v->ccPan, v->songPan);
				}
			}
			// add livesong voices
			if (livesong != nullptr) {
				for (int i = 0; i < livesong->voices.size(); i++) {
					livesong->voices[i].genSample();
					sample += livesong->voices[i].outsample;
				}
			}
			// clamp
			sample = sample > 1.0 ? 1.0 : (sample < -1.0 ? -1.0 : sample);
			*buffer++ = sample;
		}
	}
	processTime();
	return 0;
}

void APU::receiveMIDI(double deltatime, std::vector<unsigned char>* message, void* userData, Input* input) {
	Note* note = new Note();
	note->channel = 0;
	note->type = SYSEX;
	unsigned char type = message->at(0);
	if (type >= 128 && type <= 143) { note->type = NOTE_OFF; }
	if (type >= 144 && type <= 159) { note->type = NOTE_ON; }
	if (type >= 176 && type <= 191) { note->type = CONTROL_CHANGE; }
	if (type >= 224 && type <= 239) { note->type = PITCH_BEND; }
	note->data1 = (int)message->at(1);
	note->data2 = (int)message->at(2);
	if (note->type == NOTE_ON && note->data2 == 0 && note->data1 == lastLiveNote) {
		note->type = NOTE_OFF;
	}
	if (note->type == NOTE_ON && note->data2 > 0) {
		lastLiveNote = note->data1;
	}
	if (note->type == CONTROL_CHANGE) {
		ADSR* adsr = livesong->voices[0].adsrMain;
		if (input->isShift) {
			adsr = livesong->voices[0].adsrFilter;
		}
		if (note->data1 == 74) {
			adsr->a = (double)note->data2 / 64.0;
		}
		if (note->data1 == 71) {
			adsr->d = (double)note->data2 / 64.0;
		}
		if (note->data1 == 81) {
			adsr->s = (double)note->data2 / 128.0;
		}
		if (note->data1 == 91) {
			adsr->r = (double)note->data2 / 32.0;
		}
		if (note->data1 == 16) {
			livesong->voices[0].filterF = (double)note->data2 / 128.0;
		}
		if (note->data1 == 80) {
			livesong->voices[0].filterQ = (double)note->data2 / 128.0;
		}
		if (note->data1 == 19) {
			livesong->voices[0].distortion = (double)note->data2 / 16.0;
		}
	}

	livesong->playNote(note);
}


void APU::LoadSong(Song* song) {
	songs.push_back(song);
}

void APU::PlaySong(int songid, float volume, float pan, bool loop) {
	Song* song = nullptr;
	for (int i = 0; i < songs.size(); i++) {
		if (songs.at(i)->id == songid) {
			song = songs[i];
		}
	}
	if (song == nullptr) { throw "illegal songid"; }
	bool found = false;
	for (auto const& s : playing) {
		if (s->id == songid) {
			found = true;
		}
	}
	song->loop = loop;
	song->volume = volume;
	song->pan = pan;
	song->Reset();
	if (!found) {
		playing.push_back(song);
	}
	std::cout << "APU play song " << songid << " (" << song->notecount << " notes)" << std::endl;
}

Voice* APU::getVoice() {
	Voice* voice = new Voice();
	voices.push_back(voice);
	return voice;
}

void APU::processTime() {
	time = glfwGetTime();
	deltaTime = time - lastTime;
	lastTime = time;
	// Advance songs in time
	for (auto song = playing.begin(); song != playing.end();) {
		if ((*song)->done) { song = playing.erase(song); }
		else { 
			(*song)->advanceTime(deltaTime);
			++song; 
		}
	}
}

void APU::Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData),
	void (*midiCallback)(double deltatime, std::vector<unsigned char>* message, void* userData)) {

	RtAudio::StreamParameters params;
	try {
		adac = new RtAudio();
		params.deviceId = adac->getDefaultOutputDevice();
		params.nChannels = 2;
		params.firstChannel = 0;
		adac->openStream(&params, nullptr, RTAUDIO_FLOAT64, SAMPLE_RATE, &bufferSize, proxyCallback, nullptr);
		adac->startStream();
	}
	catch (RtAudioError &e) {
		e.printMessage();
		std::cout << "ERROR: APU failed" << std::endl;
		return;
	}
	started = true;
	std::cout << "APU initialized" << std::endl;

	midi = new RtMidiIn();
	unsigned int ports = midi->getPortCount();
	if (ports < 1) {
		std::cout << "APU detected no MIDI, aborting MIDI setup" << std::endl;
		delete midi;
		midi = nullptr;
	} else {
		midi->openPort(0);
		midi->setCallback(midiCallback);
		midi->ignoreTypes(true, true, true);
		livesong = new Song(999);
		livesong->volume = 1.0;
		livesong->pan = 0.5;
		Voice voice;
		voice.Patch(0.5, 1.0, 0.01, 0.3, 0.4, 1.0, PULSE, SINE, 0.1, 0.0);
		voice.lfo->osc.waveform = TRIANGLE;
		voice.lfo->osc.setFreq(2.0);
		voice.lfo->amount = 0.1;
		voice.lfo->target = M_FILTER;
		voice.echoLevel = 0.4;
		voice.echoTime = 0.3;
		voice.echoRegen = 0.7;

		livesong->addVoice(voice);
		std::cout << "APU detected " << ports << " MIDI in, opened port 0 for listen" << std::endl;
	}
}

void APU::Stop() {
	if (!started) return;
	adac->stopStream();
	adac->closeStream();
	started = false;
	std::cout << "APU shutdown" << std::endl;
	if (midi != nullptr) {
		delete midi;
		std::cout << "APU MIDI shutdown" << std::endl;
	}
}
