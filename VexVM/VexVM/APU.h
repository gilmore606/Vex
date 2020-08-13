#pragma once

constexpr auto SAMPLE_RATE = 44100;
constexpr auto MAX_VOICES = 6;

#include "RtAudio.h"
#include "APUVoice.h"
#include "VEXSong.h"

struct APU{
	APU();

	void Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData));
	void Reset();
	void PlaySong(VEXSong* song);
	void Process(float delta);
	void Stop();
	int genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData);

	APUVoice* voices;

private:
	bool started;
	VEXSong* song;

	RtAudio* adac;
	unsigned int bufferSize = 512;
	double* voicedata;
};
