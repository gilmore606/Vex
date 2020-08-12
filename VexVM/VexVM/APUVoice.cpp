#include "APUVoice.h"

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

}

void APUVoice::Reset() {

}

void APUVoice::setADSR(double a, double d, double s, double r) {
	adsrMain->a = a;
	adsrMain->d = d;
	adsrMain->s = s;
	adsrMain->r = r;
}
