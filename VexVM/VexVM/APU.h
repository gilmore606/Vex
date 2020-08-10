#pragma once

#include "RtAudio.h"

struct APU{
	APU();

	void Setup();
	void Reset();
	void Stop();

	RtAudio* adac;
	unsigned int bufferSize = 256;

private:
	double data[2];

};
