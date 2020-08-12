#include "APU.h"
#include "RtAudio.h"
#include <iostream>
#include "util.cpp"

APU::APU() { 
	started = false;

	data = new double[2];
	data[0] = 0.0;
	data[1] = 0.0;

	voices = new APUVoice[MAX_VOICES];
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
		sample = 0.0;
		for (j = 0; j < 2; j++) {
			for (v = 0; v < MAX_VOICES; v++) {
				sample += voices[v].nextSample(j);
			}
			*buffer++ = sample;
		}
	}

	return 0;
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
		adac->openStream(&params, nullptr, RTAUDIO_FLOAT64, 44100, &bufferSize, proxyCallback, nullptr);
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
