#include "APU.h"
#include "RtAudio.h"
#include <iostream>

APU::APU() { }

void APU::Reset() {

}

int sendSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {

	unsigned int i, j;
	double* buffer = (double*)outBuffer;
	double* lastValues = (double*)userData;
	if (status) std::cout << "Stream underflow detected!" << std::endl;

	// Write interleaved audio data.
	for (i = 0; i < nFrames; i++) {
		for (j = 0; j < 2; j++) {
			*buffer++ = lastValues[j];
			lastValues[j] += 0.005 * (j + 1.0 + (j * 0.1));
			if (lastValues[j] >= 1.0) lastValues[j] -= 2.0;
		}
	}
	std::cout << ".";
	return 0;
}

void APU::Setup() {
	RtAudio::StreamParameters params;
	try {
		adac = new RtAudio();
		params.deviceId = adac->getDefaultOutputDevice();
		params.nChannels = 2;
		params.firstChannel = 0;
		adac->openStream(&params, NULL, RTAUDIO_FLOAT64, 44100, &bufferSize, sendSamples, (void*)&data);
		adac->startStream();
	}
	catch (RtAudioError &e) {
		e.printMessage();
		std::cout << "ERROR: APU failed" << std::endl;
		return;
	}
	std::cout << "APU initialized" << std::endl;
}

void APU::Stop() {
	adac->stopStream();
	adac->closeStream();
}

