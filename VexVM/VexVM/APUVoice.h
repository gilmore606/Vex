#pragma once

class APUVoice {

public:
	APUVoice();

	double nextSample(int channel);
	void Reset();

	bool enabled;
	double volume;    // 0.0 to 1.0
	double pan;       // 0.0 to 1.0, 0.5 = center

private:
	double lastSample;
};

inline double APUVoice::nextSample(int channel) {
	double sample = 0.0;
	if (!enabled) return sample;

	// synthesis stuff happens here

	lastSample = sample;

	if (channel == 0) sample *= (1.0 - pan);
	if (channel == 1) sample *= pan;
	return sample * volume;
}
