#include "APU.h"
#include "RtAudio.h"
#include <iostream>
#include "util.cpp"
#include <GLFW\glfw3.h>

APU::APU() { 
	started = false;
	song = nullptr;
	voices = new APUVoice[MAX_VOICES];
	voicedata = new double[MAX_VOICES];
	lastTime = glfwGetTime();
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
					if (j == 0) sample += voicedata[v] * (1.0 - voices[v].pan) * voices[v].volume * 0.2;
					else sample += voicedata[v] * voices[v].pan * voices[v].volume * 0.2;
				}
			}
			*buffer++ = sample;
		}
	}
	processTime();
	return 0;
}

void APU::PlaySong(VEXSong* song) {
	this->song = song;
	song->Reset();
}

void APU::PlayNote(VEXNote* note) {
	if (note->channel >= MAX_VOICES) return;
	std::cout << note->channel << "  " << note->type << "  " << note->data1 << "," << note->data2 << std::endl;
	if (note->type == NOTE_ON) {
		voices[note->channel].Trigger(notefreqs[note->data1] * 2.0f);  // TODO: this 2.0 correction means osc sux
	} else if (note->type == NOTE_OFF) {
		voices[note->channel].Release();
	} else if (note->type == PITCH_BEND) {
		voices[note->channel].PitchBend(note->data1 + note->data2 * 128);
	}
}

// Process the passage of time -- advance songs, etc.
void APU::processTime() {
	time = glfwGetTime();
	deltaTime = time - lastTime;
	lastTime = time;
	if (song != nullptr) {
		song->advanceTime(deltaTime);
		VEXNote* note = song->getNote();
		while (note != nullptr) {
			PlayNote(note);
			note = song->getNote();
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
