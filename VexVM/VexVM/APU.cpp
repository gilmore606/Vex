#include "APU.h"
#include "RtAudio.h"
#include <iostream>

APU::APU() { 
	started = false;
	data = new double[2];
}

void APU::Reset() { }

int APU::genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {

	unsigned int i, j;
	double* buffer = (double*)outBuffer;
	double* lastValues = (double*)userData;
	if (status) std::cout << "APU stream underflow detected" << std::endl;

	// Write test sawtooths.
	for (i = 0; i < nFrames; i++) {
		for (j = 0; j < 2; j++) {
			*buffer++ = lastValues[j];
			lastValues[j] += 0.005 * (j + 1.0 + (j * 0.1));
			if (lastValues[j] >= 1.0) lastValues[j] -= 2.0;
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
		adac->openStream(&params, nullptr, RTAUDIO_FLOAT64, 44100, &bufferSize, proxyCallback, (void*)&data);
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

