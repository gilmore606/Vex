#pragma once
#include <string>
#include "GPU.h"
#include "APU.h"
#include "Input.h"

class CPU {
public:
	CPU();

	void Connect(GPU* gpu, APU* apu, Input* input);
	void LoadCode(std::string name);
	void Boot();

	void OnUpdate(float deltaTime);
	void OnInput(int input);

	void Stop();

private:
	GPU* gpu;
	APU* apu;
	Input* input;
};
