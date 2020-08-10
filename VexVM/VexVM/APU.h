#pragma once

#include "RtAudio.h"

struct APU{
	APU();

	RtAudio* adac;

	void Setup();
	void Reset();
};

