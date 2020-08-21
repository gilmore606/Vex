#pragma once

constexpr auto SAMPLE_RATE = 44100;

#include "RtAudio.h"
#include "Voice.h"
#include "Song.h"
#include <vector>
#include <list>

class APU{
public:
	APU();

	void Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData));
	void Reset();
	void LoadSong(Song* song);
	void PlaySong(int songid, float volume, float pan, bool loop);
	void Stop();
	int genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData);


private:
	bool started;
	std::vector<Song*> songs;
	std::list<Song*> playing;

	RtAudio* adac;
	unsigned int bufferSize = 512;

	double time, lastTime, deltaTime;

	void processTime();
};
