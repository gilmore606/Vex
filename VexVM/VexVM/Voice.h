#pragma once

#include <iostream>

enum Waveform { TRIANGLE, SAWTOOTH, PULSE, SINE, NOISE };
enum Filter { NO_FILTER, LOWPASS_12, HIGHPASS_12, BANDPASS_12, LOWPASS_24, HIGHPASS_24, BANDPASS_24 };
enum ModTarget { M_PITCH, M_VOLUME, M_PW1, M_PW2, M_OSCMIX, M_DIST, M_FILTER, M_LFO_AMT, M_LFO_FREQ };

inline static double noise() {
	return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

class OSC {
public:
	bool enabled;
	Waveform waveform;
	double pwidth;
	double detune;
	double phase;
	double bendFactor;
	double bufpwidth;
	void setFreq(double freq);
	void setBend(double bend);
	double* nextSample();

private:
	double freq;
	double level = 0.0;
	double step;
	double progress = 0.0;
	double noiseProgress = 0.0;
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
		noiseProgress += step;
		if (noiseProgress > 0.2) {
			noiseProgress = 0.0;
			level = noise();
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
		if (progress > bufpwidth) {
			level = 1.0;
		} else {
			level = -1.0;
		}
	} else if (waveform == SINE) {
		level = std::sin(progress * 6.28318);
	}
	return &level;
}

class LFO {
public:
	OSC osc;
	double amount;
	ModTarget target;

private:

};

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
	void Trigger(double freq, int vel, double volume = 1.0, double pan = 0.5);
	void Release();
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
	double oscMix;      // 0.0 all osc1 1.0 all osc2
	double distortion;  // 0.0 for none, 2.0-5.0 mild, 7.0+ heavy
	Filter filter;
	double filterF, filterQ;
	ADSR* adsrFilter;
	double adsrFilterAmount;
	LFO* lfo;
	ADSR* adsrAux;


	double songPan;
	double outsample;

private:
	double songVolume;

	double samplebuf;
	double filtermem;
	double velocity;
	double freq;

	double filterbuf0, filterbuf1, filterbuf2, filterbuf3;

	double intToDouble(int b, double max);
};

inline void Voice::genSample() {

	double bufvol = velocity * ccVol * volume * songVolume;
	double pitchmod = 0.0;
	double bufoscmix = oscMix;
	double bufpwidth1 = osc1->pwidth;
	double bufpwidth2 = osc2->pwidth;
	double bufdist = distortion;
	double buffilter = filterF;

	// Apply LFO
	double* nextlfo = lfo->osc.nextSample();
	switch (lfo->target) {
		case M_PITCH:
			pitchmod = *nextlfo * freq * lfo->amount;
			osc1->setFreq(freq + pitchmod); osc2->setFreq(freq + pitchmod); break;
		case M_VOLUME:
			bufvol -= lfo->amount * (*nextlfo + 1.0); break;
		case M_PW1:
			bufpwidth1 += lfo->amount * (*nextlfo); break;
		case M_PW2:
			bufpwidth2 += lfo->amount * (*nextlfo); break;
		case M_OSCMIX:
			bufoscmix += lfo->amount * *nextlfo; break;
		case M_DIST:
			bufdist += lfo->amount * *nextlfo; break;
		case M_FILTER:
			buffilter += lfo->amount * *nextlfo;
			buffilter = buffilter > 0.99 ? 0.99 : (buffilter < 0.01 ? 0.01 : buffilter);
			break;
	}
	osc1->bufpwidth = bufpwidth1 > 0.95 ? 0.95 : (bufpwidth1 < 0.05 ? 0.05 : bufpwidth1);
	osc2->bufpwidth = bufpwidth2 > 0.95 ? 0.95 : (bufpwidth2 < 0.05 ? 0.05 : bufpwidth2);

	// Average the raw oscillators
	if (osc1->enabled && !osc2->enabled) samplebuf = *osc1->nextSample();
	else if (osc2->enabled && !osc1->enabled) samplebuf = *osc2->nextSample();
	else if (!osc1->enabled && !osc2->enabled) samplebuf = 0.0;
	else samplebuf = (*osc1->nextSample() * (1.0 - bufoscmix) + *osc2->nextSample() * bufoscmix) / 2.0;

	// Distortion
	// TODO: find a distortion algorithm!

	// Filter
	if (filter != NO_FILTER) {
		double F = buffilter + *(*adsrFilter).nextLevel() * adsrFilterAmount;
		double feedback = filterQ + filterQ / (1.0 - F);
		filterbuf0 += F * (samplebuf - filterbuf0 + feedback * (filterbuf0 - filterbuf1));
		filterbuf1 += F * (filterbuf0 - filterbuf1);
		filterbuf2 += F * (filterbuf1 - filterbuf2);
		filterbuf3 += F * (filterbuf2 - filterbuf3);
		switch (filter) {
			case LOWPASS_12: samplebuf = filterbuf1; break;
			case LOWPASS_24: samplebuf = filterbuf3; break;
			case HIGHPASS_12: samplebuf = samplebuf - filterbuf0; break;
			case HIGHPASS_24: samplebuf = samplebuf - filterbuf3; break;
			case BANDPASS_12: samplebuf = filterbuf0 - filterbuf1; break;
			case BANDPASS_24: samplebuf = filterbuf0 - filterbuf3; break;
		}
	}

	// Adjust volume
	samplebuf *= *(*adsrMain).nextLevel() * bufvol;

	// Apply final grungefilter
	samplebuf = (samplebuf + filtermem) / 2.0;
	filtermem = samplebuf;
	outsample = samplebuf;

	// Clamp
	samplebuf = samplebuf > 1.0 ? 1.0 : (samplebuf < -1.0 ? -1.0 : samplebuf);
}

inline void Voice::Trigger(double freq, int vel, double volume, double pan) {
	songVolume = volume;
	songPan = pan;
	this->freq = freq;
	osc1->setFreq(freq);
	osc2->setFreq(freq);
	velocity = ((double)vel + 158.0) / 285.0;
	Trigger();
}
inline void Voice::Trigger() {
	adsrMain->Trigger();
	adsrFilter->Trigger();
	adsrAux->Trigger();
}
inline void Voice::Release() {
	adsrMain->Release();
	adsrFilter->Release();
	adsrAux->Release();
}
