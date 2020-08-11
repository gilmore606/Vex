#pragma once

#include "RtAudio.h"

struct APU{
	APU();

	void Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData));
	void Reset();
	void Stop();
	int genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData);

private:
	RtAudio* adac;
	unsigned int bufferSize = 256;

	double data[2];

};
