#include "Voice.h"
#include <cmath>

Voice::Voice() { 
	volume = 1.0f;
	pan = 0.5f;
	adsrMain = new ADSR();
	osc1 = new OSC();
	osc2 = new OSC();
	osc1->enabled = true;
	osc2->enabled = true;
	osc1->waveform = TRIANGLE;
	osc2->waveform = TRIANGLE;
	pitchBend = 8192;
	osc1->setBend(1.0);
	osc2->setBend(1.0);
	ccPan = 0.5;
	ccMod = 0.0;
	ccVol = 1.0;
	ccExp = 0.0;

	filtermem = 0.0;
}

void Voice::Reset() {

}

void Voice::setADSR(double a, double d, double s, double r) {
	adsrMain->a = a;
	adsrMain->d = d;
	adsrMain->s = s;
	adsrMain->r = r;
}

void Voice::Patch(double pan, double volume, double a, double d, double s, double r, Waveform wave1, Waveform wave2, double detune, double phase) {
	adsrMain->a = a;
	adsrMain->d = d;
	adsrMain->s = s;
	adsrMain->r = r;
	this->pan = pan;
	this->volume = volume;
	this->osc1->waveform = wave1;
	this->osc2->waveform = wave2;
	this->osc2->detune = detune;
	this->osc2->phase = phase;
}

// Patch using integer values from ROM
void Voice::Patch(int pan, int volume, int a, int d, int s, int r, int wave1, int wave2, int detune, int phase) {
	Patch(
		intToDouble(pan, 1.0), intToDouble(volume, 1.0),
		intToDouble(a, 2.0), intToDouble(d, 3.0), intToDouble(s, 1.0), intToDouble(r, 3.0),
		(Waveform)wave1, (Waveform)wave2, intToDouble(detune, 20.0), intToDouble(phase, 0.5)
	);
}

void Voice::PitchBend(int bend) {
	pitchBend = bend;
	double b = (double)(bend - 8192) / 8192.0;
	b *= 2.0;  // 2 semitone bend range
	double fac = std::pow(2.0, b / 12.0);
	osc1->setBend(fac);
	osc2->setBend(fac);
}

double Voice::intToDouble(int b, double max) {
	return ((double)b / 255.0) * max;
}