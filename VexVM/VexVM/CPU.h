#pragma once
#include <string>
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Code.h"
#include "Opcodes.h"
#include "Values.h"
#include "Task.h"

#define READ_BYTE() (*ip++)
#define CLEAR_STACK() stacktop = stack

const static int STACK_MAX = 256;

class Note;

class CPU {
public:
	CPU();

	void Connect(GPU* gpu, APU* apu, Input* input);
	void LoadCode(Code* code);
	virtual void Boot();

	virtual void OnUpdate(float deltaTime);
	virtual void OnInput(int input);
	virtual void OnMIDI(Note* note);

	virtual void Stop();

protected:
	GPU* gpu;
	APU* apu;
	Input* input;

	Code* code;
	uint8_t* ip;

	Value stack[STACK_MAX];
	Value* stacktop;

	std::vector<Task> tasks;
	double elapsed;

	void run(uint8_t* address);
	void resume(Task task);
	void logValue(Value v);

private:

	bool* buttonPressed;
	int countPressed;

	void callSFUN(int fi);
	void callSMETH(int fi, Value vthis);
	void callSTAT(int fi, int paramc);

	inline int READ_I16() {
		int i = (int)*ip++;
		return i + ((int)(*ip++ & 0xFF) << 8);
	}
	inline void push(Value value) {
		*stacktop = value;
		stacktop++;
	}
	inline Value pop() {
		stacktop--;
		return *stacktop;
	}
};
