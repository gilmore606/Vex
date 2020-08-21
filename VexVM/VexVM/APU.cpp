#include "APU.h"
#include "RtAudio.h"
#include <iostream>
#include "util.cpp"
#include <GLFW\glfw3.h>

APU::APU() { 
	started = false;
	lastTime = glfwGetTime();
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
			*buffer++ = sample;
		}
	}
	processTime();
	return 0;
}

void APU::receiveMIDI(double deltatime, std::vector<unsigned char>* message, void* userData) {
	unsigned int nBytes = message->size();
	for (unsigned int i = 0; i < nBytes; i++)
		std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
	if (nBytes > 0)
		std::cout << "stamp = " << deltatime << std::endl;
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
		std::cout << "APU detected " << ports << " MIDI in, opened port 0 for listen";
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
