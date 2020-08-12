#pragma once

#include <iostream>

enum Waveform { TRIANGLE, SAWTOOTH, PULSE, NOISE };

inline static double noise() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

class OSC {
public:
	bool enabled;
	Waveform waveform = TRIANGLE;
	double detune;
	void setFreq(double freq);
	double* nextSample();

private:
	double freq;
	double level = 0.0;
	double step;
};

inline void OSC::setFreq(double freq) {
	this->freq = freq + detune;
	this->step = (this->freq / 44100.0);
}

inline double* OSC::nextSample() {
	if (waveform == NOISE) {
		level = noise();
	} else if (waveform == TRIANGLE) {
		level += step;
		if (level >= 1.0) {
			level = 1.0;
			step = -step;
		} else if (level <= -1.0) {
			level = -1.0;
			step = -step;
		}
	} else if (waveform == SAWTOOTH) {
		level += step;
		if (level >= 1.0) {
			level = -1.0;
		}
	} else if (waveform == PULSE) {

	}
	return &level;
}

class ADSR {
public:
	bool active;
	double a, d, s, r;
	void Trigger();
	void Release();
	double* nextLevel();

private:
	double astep, dstep, rstep;
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
	void Trigger(double freq);
	void Release();
	void setADSR(double a, double d, double s, double r);

	bool enabled;
	double volume;    // 0.0 to 1.0
	double pan;       // 0.0 to 1.0, 0.5 = center
	ADSR* adsrMain;
	OSC* osc1;
	OSC* osc2;

	bool testTone;

private:
	double samplebuf;
};

inline double* APUVoice::nextSample() {
	if (testTone) {
		samplebuf = noise();
		return &samplebuf;
	}
	
	if (osc1->enabled && !osc2->enabled) samplebuf = *osc1->nextSample();
	else if (osc2->enabled && !osc1->enabled) samplebuf = *osc2->nextSample();
	else if (!osc1->enabled && !osc2->enabled) samplebuf = 0.0;
	else samplebuf = (*osc1->nextSample() + *osc2->nextSample()) / 2.0;

	samplebuf *= *(*adsrMain).nextLevel();

	return &samplebuf;
}
inline void APUVoice::Trigger(double freq) {
	osc1->setFreq(freq);
	osc2->setFreq(freq);
	Trigger();
}
inline void APUVoice::Trigger() {
	adsrMain->Trigger();
}
inline void APUVoice::Release() {
	adsrMain->Release();
}
