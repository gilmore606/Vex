#pragma once
#include <string>
#include "Scheduler.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
class Note;

class CPU {
public:
	CPU();

	void Connect(Scheduler* scheduler, GPU* gpu, APU* apu, Input* input);
	void LoadCode(std::string name);
	virtual void Boot();

	virtual void OnUpdate(float deltaTime);
	virtual void OnInput(int input);
	virtual void OnMIDI(Note* note);

	virtual void Stop();

protected:
	Scheduler* scheduler;
	GPU* gpu;
	APU* apu;
	Input* input;

private:
};
