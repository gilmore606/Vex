#pragma once

constexpr auto SAMPLE_RATE = 44100;

#include "RtAudio.h"
#include "RtMidi.h"
#include "Voice.h"
#include "Song.h"
#include <vector>
#include <list>

class Input;

class APU{
public:
	APU();

	void Setup(int (*proxyCallback)(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData),
		void (*midiCallback)(double deltatime, std::vector<unsigned char>* message, void* userData));
	void Reset();
	void LoadSong(Song* song);
	void PlaySong(int songid, float volume, float pan, bool loop);
	Voice* getVoice();
	void Stop();

	int genSamples(void* outBuffer, void* inBuffer, unsigned int nFrames,
		double streamTime, RtAudioStreamStatus status, void* userData);
	void receiveMIDI(double deltatime, std::vector<unsigned char>* message, void* userData, Input* input);


private:
	bool started;
	std::vector<Song*> songs;
	std::list<Song*> playing;
	std::list<Voice*> voices;
	Song* livesong;
	int lastLiveNote;

	RtAudio* adac;
	RtMidiIn* midi;
	unsigned int bufferSize = 512;

	double time, lastTime, deltaTime;

	void processTime();
};
