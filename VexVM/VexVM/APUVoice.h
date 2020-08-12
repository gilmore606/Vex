#pragma once

class APUVoice {

public:
	APUVoice();

	double nextSample();
	void Reset();

	bool enabled;
	double volume;    // 0.0 to 1.0
	double pan;       // 0.0 to 1.0, 0.5 = center

private:
	double lastSample;
};

inline double APUVoice::nextSample() {
	double sample = 0.0;
	// synthesis stuff happens here

	lastSample = sample;
	return sample;
}
