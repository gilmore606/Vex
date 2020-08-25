#pragma once
#include <string>
#include "Scheduler.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Codechunk.h"
#include "Opcodes.h"
#include "Values.h"

#define READ_BYTE() (*ip++)
#define READ_INT() (*ip++ + ((*ip++)<<8))

const static int STACK_MAX = 256;

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

	Codechunk* chunk;
	uint8_t* ip;

	Value stack[STACK_MAX];
	Value* stacktop;

	void run(Codechunk* code);

private:
	inline void push(Value value);
	inline Value pop();
};


inline void CPU::push(Value value) {
	*stacktop = value;
	stacktop++;
}
inline Value CPU::pop() {
	stacktop--;
	return *stacktop;
}

