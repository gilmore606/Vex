#pragma once

#include <iostream>

enum Waveform { TRIANGLE, SAWTOOTH, PULSE };

inline static double noise() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

class ADSR {
public:
	bool active;
	double a, d, s, r;
	double astep, dstep, rstep;
	void Trigger();
	void Release();
	double* nextLevel();

private:
	double level;
	unsigned int stage;
};

inline void ADSR::Trigger() {
	active = true;
	stage = 0;
	level = 0.0;
	astep = (1.0 / a) * (1.0 / 44100.0);
	dstep = (1.0 / d) * (1.0 / 44100.0);
	rstep = (1.0 / r) * (1.0 / 44100.0);
}
inline void ADSR::Release() {
	if (active && (stage < 3)) stage = 3;
}
inline double* ADSR::nextLevel() {
	if (!active) {
		level = 0.0;
	} else if (stage == 0) { // a
		level += astep;
		if (level >= 1.0) {
			level = 1.0;
			stage = 1;
		}
	} else if (stage == 1) { // d
		level -= dstep;
		if (level <= s) {
			level = s;
			stage = 2;
		}
	} else if (stage == 2) { // s
		level = s;
		if (level <= 0.0000001) active = false;
	} else if (stage == 3) { // r
		level -= rstep;
		if (level <= 0.0000001) active = false;
	}
	return &level;
}


class APUVoice {

public:
	APUVoice();

	double* nextSample();
	void Reset();
	void Trigger();
	void Release();

	bool enabled;
	double volume;    // 0.0 to 1.0
	double pan;       // 0.0 to 1.0, 0.5 = center
	ADSR* envMain;
	Waveform waveMain;

	bool testTone;

private:
	double samplebuf;
};

inline double* APUVoice::nextSample() {
	samplebuf = 0.0;
	if (testTone) {
		samplebuf = noise();
		return &samplebuf;
	}
	// synthesis stuff happens here
	samplebuf = noise();
	samplebuf *= *(*envMain).nextLevel();


	return &samplebuf;
}

inline void APUVoice::Trigger() {
	envMain->Trigger();
}
inline void APUVoice::Release() {
	envMain->Release();
}
