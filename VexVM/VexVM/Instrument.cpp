#include "Instrument.h"
#include <iostream>

Instrument::Instrument(int wave1, int wave2, int a, int d, int s, int r, int detune, int phase, int volume, int pan) {
	this->wave1 = intToWave(wave1);
	this->wave2 = intToWave(wave2);
	this->a = intToDouble(a, 1.0);
	this->d = intToDouble(d, 2.0);
	this->s = intToDouble(s, 1.0);
	this->r = intToDouble(r, 2.0);
	this->detune = intToDouble(detune, 10.0);
	this->phase = intToDouble(phase, 0.5);
	this->volume = intToDouble(volume, 1.0);
	this->pan = intToDouble(pan, 1.0);
}

Waveform Instrument::intToWave(int wavei) {
	if (wavei == 0) return TRIANGLE;
	if (wavei == 1) return SAWTOOTH;
	if (wavei == 2) return PULSE;
	if (wavei == 3) return SINE;
	if (wavei == 4) return NOISE;
	throw "illegal waveform";
}

double Instrument::intToDouble(int b, double max) {
	return ((double)b / 255.0) * max;
}
