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
	osc1->pwidth = 0.5;
	osc2->pwidth = 0.5;
	oscMix = 0.5;
	pitchBend = 8192;
	osc1->setBend(1.0);
	osc2->setBend(1.0);
	osc1->transpose = 0;
	osc2->transpose = 0;
	ccPan = 0.5;
	ccMod = 0.0;
	ccVol = 1.0;
	ccExp = 0.0;

	distortion = 0.0;
	filter = NO_FILTER;
	filterF = 0.8;
	filterQ = 0.1;
	adsrFilter = new ADSR();
	adsrFilterAmount = 0.0;

	lfo = new LFO();
	lfo->osc.setBend(1.0);
	lfo->osc.pwidth = 0.5;
	lfo->osc.enabled = true;
	adsrAux = new ADSR();
	adsrAuxAmt = 0.0;

	filtermem = 0.0;
	filterbuf0 = 0.0;
	filterbuf1 = 0.0;
	filterbuf2 = 0.0;
	filterbuf3 = 0.0;

	echobuf = new double[echosize];
	for (int i = 0; i < echosize; i++) { echobuf[i] = 0.0; }
	echoc = 0;
}

void Voice::Reset() {

}

void Voice::Patch(double pan, double volume, double a, double d, double s, double r, Waveform wave1, Waveform wave2, double detune, double phase) {
	adsrMain->set(a, d, s, r);
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
		intToDouble(a, 3.0), intToDouble(d, 3.0), intToDouble(s, 1.0), intToDouble(r, 3.0),
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
