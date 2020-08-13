#include "APUVoice.h"
#include <cmath>

APUVoice::APUVoice() { 
	volume = 1.0f;
	pan = 0.5f;
	enabled = true;
	testTone = false;
	adsrMain = new ADSR();
	osc1 = new OSC();
	osc2 = new OSC();
	osc1->enabled = true;
	osc2->enabled = true;
	pitchBend = 8192;
	osc1->setBend(1.0);
	osc2->setBend(1.0);
}

void APUVoice::Reset() {

}

void APUVoice::setADSR(double a, double d, double s, double r) {
	adsrMain->a = a;
	adsrMain->d = d;
	adsrMain->s = s;
	adsrMain->r = r;
}

void APUVoice::Patch(double pan, double volume, double a, double d, double s, double r, Waveform wave1, Waveform wave2, double detune, double phase) {
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

void APUVoice::PitchBend(int bend) {
	pitchBend = bend;
	double b = (double)(bend - 8192) / 8192.0;
	b *= 2.0;  // 2 semitone bend range
	double fac = std::pow(2.0, b / 12.0);
	osc1->setBend(fac);
	osc2->setBend(fac);
}
