#pragma once

#include "RtAudio.h"
#include "APUVoice.h"

constexpr auto MAX_VOICES = 6;

struct APU{
	APU();

	void Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData));
	void Reset();
	void Stop();
	int genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData);

private:
	bool started;
	RtAudio* adac;
	unsigned int bufferSize = 512;

	APUVoice* voices;

	double* voicedata;
};
