#pragma once

#include <iostream>

enum Waveform { TRIANGLE, SAWTOOTH, PULSE, SINE, NOISE };

inline static double noise() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

class OSC {
public:
	bool enabled;
	Waveform waveform;
	double detune;
	double phase;
	double bendFactor;
	void setFreq(double freq);
	void setBend(double bend);
	double* nextSample();

private:
	double freq;
	double level = 0.0;
	double step;
	double progress = 0.0;
	double lastNoiseLatch = 0.0;
	void setStep();
};

inline void OSC::setFreq(double freq) {
	this->freq = freq;
	level = phase;
	setStep();
}

inline void OSC::setBend(double bend) {
	bendFactor = bend;
	setStep();
}

inline void OSC::setStep() {
	this->step = ((this->freq * bendFactor) + detune) / 44100.0;
}

inline double* OSC::nextSample() {
	progress += step;
	if (progress > 1.0) progress -= 1.0;
	if (waveform == NOISE) {
		if (progress > (lastNoiseLatch + step * 0.25)) {
			level = noise();
			lastNoiseLatch = progress;
		}
	}
	else if (waveform == TRIANGLE) {
		if (progress < 0.5) {
			level = (progress * 4.0) - 1.0;
		} else {
			level = ((1.0 - progress) * 4.0) - 1.0;
		}
	} else if (waveform == SAWTOOTH) {
		level = (progress * 2.0) - 1.0;
	} else if (waveform == PULSE) {
		if (progress > 0.5) {
			level = 1.0;
		} else {
			level = -1.0;
		}
	} else if (waveform == SINE) {
		level = std::sin(progress * 6.28318);
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


class Voice {

public:
	Voice();

	void genSample();
	void Reset();
	void Trigger();
	void Trigger(double freq, int vel, double volume, double pan);
	void Release();
	void setADSR(double a, double d, double s, double r);
	void Patch(double pan, double volume, double a, double d, double s, double r, Waveform wave1, Waveform wave2, double detune, double phase);
	void Patch(int pan, int volume, int a, int d, int s, int r, int wave1, int wave2, int detune, int phase);
	void PitchBend(int bend);

	double volume;    // 0.0 to 1.0
	double pan;       // 0.0 to 1.0, 0.5 = center
	int pitchBend;    // 0 to 16384, 8192 = on-pitch
	double ccMod, ccVol, ccPan, ccExp;
	ADSR* adsrMain;
	OSC* osc1;
	OSC* osc2;

	double songPan;
	double outsample;

private:
	double songVolume;

	double samplebuf;
	double filtermem;
	double velocity;

	double intToDouble(int b, double max);
};

inline void Voice::genSample() {
	if (osc1->enabled && !osc2->enabled) samplebuf = *osc1->nextSample();
	else if (osc2->enabled && !osc1->enabled) samplebuf = *osc2->nextSample();
	else if (!osc1->enabled && !osc2->enabled) samplebuf = 0.0;
	else samplebuf = (*osc1->nextSample() + *osc2->nextSample()) / 2.0;

	samplebuf *= *(*adsrMain).nextLevel() * velocity * ccVol * volume * songVolume;
	samplebuf = (samplebuf + filtermem) / 2.0;  // cheap grungy filter
	filtermem = samplebuf;
	outsample = samplebuf;
}
inline void Voice::Trigger(double freq, int vel, double volume, double pan) {
	songVolume = volume;
	songPan = pan;
	osc1->setFreq(freq);
	osc2->setFreq(freq);
	velocity = ((double)vel + 158.0) / 285.0;
	Trigger();
}
inline void Voice::Trigger() {
	adsrMain->Trigger();
}
inline void Voice::Release() {
	adsrMain->Release();
}
