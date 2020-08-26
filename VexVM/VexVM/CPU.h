#pragma once
#include <string>
#include "Scheduler.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Code.h"
#include "Opcodes.h"
#include "Values.h"

#define READ_BYTE() (*ip++)
#define READ_I16() (*ip++ + ((*ip++)>>8))

const static int STACK_MAX = 256;

class Note;

class CPU {
public:
	CPU();

	void Connect(Scheduler* scheduler, GPU* gpu, APU* apu, Input* input);
	void LoadCode(Code* code);
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

	Code* code;
	uint8_t* ip;

	Value stack[STACK_MAX];
	Value* stacktop;

	void run(uint8_t* address);
	
	void stackDump();

private:

	inline void push(Value value) {
		*stacktop = value;
		stacktop++;
	}
	inline Value pop() {
		stacktop--;
		return *stacktop;
	}
};
