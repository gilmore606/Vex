#pragma once
#include "CPU.h"
class DemoSynth : public CPU {
public:

	void Boot();
	void OnUpdate(float deltaTime);
	void OnInput(int input);
	void OnMIDI(Note* note);
	void Stop();

private:
	Voice* voice;
	Song* livesong;
	int lastLiveNote;
};

