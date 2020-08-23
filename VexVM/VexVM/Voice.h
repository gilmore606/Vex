#pragma once

#include <iostream>

enum Waveform { TRIANGLE, SAWTOOTH, PULSE, SINE, NOISE };
enum Filter { NO_FILTER, LOWPASS_12, HIGHPASS_12, BANDPASS_12, LOWPASS_24, HIGHPASS_24, BANDPASS_24 };
enum ModTarget { M_PITCH, M_VOLUME, M_PW1, M_PW2, M_OSCMIX, M_DIST, M_FILTER, M_LFO_AMT, M_LFO_FREQ };

const static double transposeTable[13] = { 0.0, 0.59463, 0.122462, 0.189207, 0.259921, 0.334839, 0.414213,
	0.498307, 0.587401, 0.681792, 0.781797, 0.887748, 1.0 };

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
	int transpose;  // -12/+12 semis

	double bendFactor;
	double bufpwidth;
	void reset();
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

inline void OSC::reset() {
	level = 0.0;
	progress = 0.0;
	noiseProgress = 0.0;
}

inline void OSC::setFreq(double freq) {
	this->freq = freq + ((transpose > 0) ? (transposeTable[transpose] * freq)
		: (0 - (transposeTable[-transpose] + 1.0) * freq * 0.5));
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
	void setA(double rate);
	void setD(double rate);
	void setS(double level);
	void setR(double rate);
	void set(double a, double d, double s, double r);
	void Trigger();
	void Release();
	double* nextLevel();

private:
	double a, d, s, r;
	double acoef, dcoef, rcoef;
	double abase, dbase, rbase;
	const double targetRatioA = 0.3;
	const double targetRatioDR = 0.0001;
	double level;
	unsigned int stage;

	double calcCoef(double rate, double targetRatio);
};

inline double ADSR::calcCoef(double rate, double targetRatio) {
	return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

inline void ADSR::setA(double rate) {
	a = rate;
	acoef = calcCoef(rate * 44100.0, targetRatioA);
	abase = (1.0 + targetRatioA) * (1.0 - acoef);
}
inline void ADSR::setD(double rate) {
	d = rate;
	dcoef = calcCoef(rate * 44100.0, targetRatioDR);
	dbase = (s - targetRatioDR) * (1.0 - dcoef);
}
inline void ADSR::setS(double level) {
	s = level;
	dbase = (s - targetRatioDR) * (1.0 - dcoef);
}
inline void ADSR::setR(double rate) {
	r = rate;
	rcoef = calcCoef(rate * 44100.0, targetRatioDR);
	rbase = -targetRatioDR * (1.0 - rcoef);
}
inline void ADSR::set(double a, double d, double s, double r) {
	setA(a);
	setD(d);
	setS(s);
	setR(r);
}

inline void ADSR::Trigger() {
	active = true;
	stage = 0;
}
inline void ADSR::Release() {
	if (active && (stage < 3)) stage = 3;
}
inline double* ADSR::nextLevel() {
	if (!active) { level = 0.0; return &level; }
	switch (stage) {
	case 0:
		level = abase + level * acoef;
		if (level >= 1.0) {
			level = 1.0;
			stage = 1;
		}
		break;
	case 1:
		level = dbase + level * dcoef;
		if (level <= s) {
			level = s;
			stage = 2;
		}
		break;
	case 2:
		break;
	case 3:
		level = rbase + level * rcoef;
		if (level <= 0.0) {
			level = 0.0;
			active = false;
		}
		break;
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
	double filterF, filterQ;    // F 0.0-1.0  Q 0.0-4.0+
	ADSR* adsrFilter;
	double adsrFilterAmount;
	LFO* lfo;
	ADSR* adsrAux;
	double adsrAuxAmt;
	ModTarget adsrAuxTarget;
	double echoLevel;  // 0.0-1.0 level
	double echoTime;   // 0.0-1.0 second
	double echoRegen;  // 0.0-1.0 regen

	double songPan;
	double outsample;

private:
	double songVolume;

	double samplebuf;
	double filtermem;
	double velocity;
	double freq;

	double filterbuf0, filterbuf1, filterbuf2, filterbuf3;
	double* echobuf;
	int echoc;
	const int echosize = 44100;

	double intToDouble(int b, double max);
};

inline void Voice::genSample() {

	double bufvol = velocity * ccVol * volume * songVolume;
	double bufpitch = freq;
	double bufoscmix = oscMix;
	double bufpwidth1 = osc1->pwidth;
	double bufpwidth2 = osc2->pwidth;
	double bufdist = distortion;
	double buffilter = filterF;
	double buflfoamt = lfo->amount;

	// Apply ADSR modulator
	double* nextadsr = (*adsrAux).nextLevel();
	switch (adsrAuxTarget) {
		case M_PW1:
			bufpwidth1 += adsrAuxAmt * (*nextadsr); break;
		case M_PW2:
			bufpwidth2 += adsrAuxAmt * (*nextadsr); break;
		case M_OSCMIX:
			bufoscmix += adsrAuxAmt * (*nextadsr); break;
		case M_DIST:
			bufdist += adsrAuxAmt * (*nextadsr); break;
		case M_LFO_AMT:
			buflfoamt = buflfoamt * (1.0 - adsrAuxAmt) + buflfoamt * adsrAuxAmt * (*nextadsr); break;
		case M_PITCH:
			bufpitch += adsrAuxAmt * (*nextadsr) * freq;
	}

	// Apply LFO modulator
	double* nextlfo = lfo->osc.nextSample();
	switch (lfo->target) {
		case M_PITCH:
			bufpitch += *nextlfo * freq * buflfoamt; break;
		case M_VOLUME:
			bufvol -= buflfoamt * (*nextlfo + 1.0); break;
		case M_PW1:
			bufpwidth1 += buflfoamt * (*nextlfo); break;
		case M_PW2:
			bufpwidth2 += buflfoamt * (*nextlfo); break;
		case M_OSCMIX:
			bufoscmix += buflfoamt * *nextlfo; break;
		case M_DIST:
			bufdist += buflfoamt * *nextlfo; break;
		case M_FILTER:
			buffilter += buflfoamt * *nextlfo;
			buffilter = buffilter > 0.99 ? 0.99 : (buffilter < 0.01 ? 0.01 : buffilter);
			break;
	}
	osc1->bufpwidth = bufpwidth1 > 0.95 ? 0.95 : (bufpwidth1 < 0.05 ? 0.05 : bufpwidth1);
	osc2->bufpwidth = bufpwidth2 > 0.95 ? 0.95 : (bufpwidth2 < 0.05 ? 0.05 : bufpwidth2);
	osc1->setFreq(bufpitch); 
	osc2->setFreq(bufpitch);

	// Mix the raw oscillators
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

	// Echo
	int returnc = echoc - (echoTime * echosize);
	returnc = returnc < 0 ? (returnc + echosize) : returnc;
	echobuf[echoc] = samplebuf * echoLevel + echobuf[returnc] * echoRegen;
	echoc++;
	if (echoc > echosize) echoc = 0;
	samplebuf += echobuf[returnc];

	// Clamp
	samplebuf = samplebuf > 1.0 ? 1.0 : (samplebuf < -1.0 ? -1.0 : samplebuf);

	// Apply final grungefilter
	samplebuf = (samplebuf + filtermem) / 2.0;
	filtermem = samplebuf;
	outsample = samplebuf;
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
	osc1->reset();
	osc2->reset();
	adsrMain->Trigger();
	adsrFilter->Trigger();
	adsrAux->Trigger();
}
inline void Voice::Release() {
	adsrMain->Release();
	adsrFilter->Release();
	adsrAux->Release();
}
