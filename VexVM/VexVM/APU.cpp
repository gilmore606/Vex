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
		// left + right channels
		for (j = 0; j < 2; j++) {
			sample = 0.0;
			// add up panned samples of every voice of every song
			for (auto const& s : playing) {
				for (int i = 0; i < s->voices.size(); i++) {
					Voice* v = &s->voices[i];
					if (j == 0) {
						sample += v->outsample * (1.0 - (v->pan + v->ccPan) * 0.5);
					} else {
						sample += v->outsample * ((v->pan + v->ccPan) * 0.5);
					}
				}
			}
			*buffer++ = sample;
		}
	}
	processTime();
	return 0;
}

void APU::LoadSong(Song* song) {
	songs.push_back(song);
}

void APU::PlaySong(int songid) {
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
	song->Reset();
	if (!found) {
		playing.push_back(song);
	}
	std::cout << "APU play song " << songid << " (" << song->notecount << " notes)" << std::endl;
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

// The given proxyCallback func should call APU::genSamples()
void APU::Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData)) {
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
}

void APU::Stop() {
	if (!started) return;
	adac->stopStream();
	adac->closeStream();
	started = false;
	std::cout << "APU shutdown" << std::endl;
}
