#pragma once

#include "APUVoice.h"


class VEXInstrument {
public:
	Waveform wave1, wave2;
	double a, d, s, r;
	double detune, phase;
	double volume, pan;

	VEXInstrument(int wave1, int wave2, int a, int d, int s, int r, int detune, int phase, int volume, int pan);

private:
	Waveform intToWave(int wavei);
	double intToDouble(int b, double max);
};
