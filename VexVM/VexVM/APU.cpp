#include "APU.h"
#include "RtAudio.h"
#include <iostream>
#include "util.cpp"

APU::APU() { 
	started = false;

	voices = new APUVoice[MAX_VOICES];
	voicedata = new double[MAX_VOICES];
}

void APU::Reset() { 
	for (int i = 0; i < MAX_VOICES; i++) {
		voices[i].Reset();
	}
}

int APU::genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {

	unsigned int i, j, v;
	double* buffer = (double*)outBuffer;
	if (status) std::cout << "APU stream underflow detected" << std::endl;
	double sample;

	for (i = 0; i < nFrames; i++) {
		for (v = 0; v < MAX_VOICES; v++) {
			voicedata[v] = 0.0;
			if (voices[v].enabled) voicedata[v] = *voices[v].nextSample();
		}
		for (j = 0; j < 2; j++) {
			sample = 0.0;
			for (v = 0; v < MAX_VOICES; v++) {
				if (voices[v].enabled) {
					if (j == 0) sample += voicedata[v] * (1.0 - voices[v].pan) * voices[v].volume;
					else sample += voicedata[v] * voices[v].pan * voices[v].volume;
				}
			}
			*buffer++ = sample;
		}
	}

	return 0;
}

void APU::PlaySong(VEXSong* song) {
	this->song = song;
	song->cursor = 0;
}

// Process the passage of time -- advance songs, etc.
void APU::Process(float delta) {

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
