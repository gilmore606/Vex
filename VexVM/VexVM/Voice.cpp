#include "Voice.h"
#include <cmath>
#include "ROMReader.h"

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

void Voice::Read(ROMReader* rom) {
	int marker = rom->nextInt();
	if (marker != 254) {
		std::cout << "  expected instrument marker -- PANIC" << marker << std::endl;
		throw "instrument decoder fail";
	}
	std::cout << "  reading instrument" << std::endl;
	osc1->waveform = (Waveform)rom->nextInt();
	osc2->waveform = (Waveform)rom->nextInt();
	osc1->pwidth = rom->nextInt() * 0.5;
	osc2->pwidth = rom->nextInt() * 0.5;
	osc1->transpose = rom->nextInt() - 12;
	osc2->transpose = rom->nextInt() - 12;
	adsrMain->setA((rom->nextInt() / 255.0) * 3.0);
	adsrMain->setD((rom->nextInt() / 255.0) * 3.0);
	adsrMain->setS((rom->nextInt() / 255.0) * 1.0);
	adsrMain->setR((rom->nextInt() / 255.0) * 3.0);
	osc2->detune = (rom->nextInt() / 255.0) * 10.0;
	osc2->phase = (rom->nextInt() / 255.0) * 0.5;
	oscMix = (rom->nextInt() / 255.0) * 1.0;
	distortion = (rom->nextInt() / 255.0) * 1.0;
	filter = (Filter)rom->nextInt();
	filterF = (rom->nextInt() / 255.0) * 1.0;
	filterQ = (rom->nextInt() / 255.0) * 1.0;
	adsrFilter->setA((rom->nextInt() / 255.0) * 3.0);
	adsrFilter->setD((rom->nextInt() / 255.0) * 3.0);
	adsrFilter->setS((rom->nextInt() / 255.0) * 1.0);
	adsrFilter->setR((rom->nextInt() / 255.0) * 3.0);
	adsrFilterAmount = (rom->nextInt() / 255.0) * 1.0;
	echoLevel = (rom->nextInt() / 255.0) * 1.0;
	echoTime = (rom->nextInt() / 255.0) * 1.0;
	echoRegen = (rom->nextInt() / 255.0) * 1.0;
	lfo->osc.setFreq((rom->nextInt() / 255.0) * 10.0);
	lfo->osc.waveform = (Waveform)rom->nextInt();
	lfo->amount = (rom->nextInt() / 255.0) * 1.0;
	lfo->target = (ModTarget)rom->nextInt();
	adsrAux->setA((rom->nextInt() / 255.0) * 3.0);
	adsrAux->setD((rom->nextInt() / 255.0) * 3.0);
	adsrAux->setS((rom->nextInt() / 255.0) * 1.0);
	adsrAux->setR((rom->nextInt() / 255.0) * 3.0);
	adsrAuxAmt = ((rom->nextInt() - 127) / 128.0) * 1.0;
	adsrAuxTarget = (ModTarget)rom->nextInt();

	songVolume = (rom->nextInt() / 255.0) * 1.0;
	songPan = (rom->nextInt() / 255.0) * 1.0;
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
	this->osc1->reset();
	this->osc2->reset();
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
