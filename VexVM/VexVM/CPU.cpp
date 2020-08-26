#include "CPU.h"
#include <string>
#include <iostream>

CPU::CPU() {
	std::cout << "CPU missing; default to demo rom" << std::endl;
}

void CPU::Connect(Scheduler* scheduler, GPU* gpu, APU* apu, Input* input) {
	this->scheduler = scheduler;
	this->gpu = gpu;
	this->apu = apu;
	this->input = input;
}

void CPU::LoadCode(Code* code) {
	this->code = code;
}

void CPU::Boot() {

	run();

}

void CPU::OnUpdate(float deltaTime) {
	// run handler queue first

	// 'put deltatime on stack'
	// 'jump to onUpdate handler'
}

void CPU::OnInput(int input) {
	// add to the handler queue:
	// 'put input on stack'
	// 'jump to onInput handler'
}

void CPU::OnMIDI(Note* note) {

}

void CPU::Stop() {

}

// any Executor can 'wait' and be tagged with a wake time
// cpu.onUpdate checks for executors ready to wake


void CPU::stackDump() {

}

void CPU::run() {

	this->ip = code->code;
	this->stacktop = stack;

	for (;;) {
		uint8_t ins;
		int adr;
		switch (ins = READ_BYTE()) {

			// System

		case OP_UNDEF:
			throw "op_undef";

		case OP_NOP:
			break;

		case OP_EXIT:
			return;

		case OP_DEBUG:
			stackDump();
			break;

			// Values

		case OP_VAR:    // arg2: index of var
			push(code->variables[READ_I16()]);
			break;

		case OP_CONST:   // arg2: index of constant
			push(code->constants[READ_I16()]);
			break;

		case OP_RANDF:
			break;

		case OP_RANDI:
			break;

		case OP_SETVAR:   // arg2: index of var
			code->variables[READ_I16()] = pop();
			break;

			// Math

		case OP_ADDI:
			push(INT_VAL(AS_INT(pop()) + AS_INT(pop())));
			break;
		case OP_SUBI:
			push(INT_VAL(AS_INT(pop()) - AS_INT(pop())));
			break;
		case OP_MULTI:
			push(INT_VAL(AS_INT(pop()) * AS_INT(pop())));
			break;
		case OP_DIVI:
			push(INT_VAL(AS_INT(pop()) / AS_INT(pop())));
			break;
		case OP_NEGI:
			push(INT_VAL(-AS_INT(pop())));
			break;
		case OP_ADDF:
			push(FLOAT_VAL(AS_FLOAT(pop()) + AS_FLOAT(pop())));
			break;
		case OP_SUBF:
			push(FLOAT_VAL(AS_FLOAT(pop()) - AS_FLOAT(pop())));
			break;
		case OP_MULTF:
			push(FLOAT_VAL(AS_FLOAT(pop()) * AS_FLOAT(pop())));
			break;
		case OP_DIVF:
			push(FLOAT_VAL(AS_FLOAT(pop()) / AS_FLOAT(pop())));
			break;
		case OP_NEGF:
			push(FLOAT_VAL(-AS_FLOAT(pop())));
			break;
		case OP_I2F:
			push(FLOAT_VAL((float)AS_INT(pop())));
			break;
		case OP_F2I:
			push(INT_VAL((int)AS_FLOAT(pop())));
			break;

			// Vector math

		case OP_VADDV:
			
			break;

			// Logic

		case OP_NOT:
			push(BOOL_VAL(!AS_BOOL(pop())));
			break;
		case OP_GT:
			push(BOOL_VAL(AS_INT(pop()) > AS_INT(pop())));
			break;
		case OP_GE:
			push(BOOL_VAL(AS_INT(pop()) >= AS_INT(pop())));
			break;
		case OP_LT:
			push(BOOL_VAL(AS_INT(pop()) < AS_INT(pop())));
			break;
		case OP_LE:
			push(BOOL_VAL(AS_INT(pop()) <= AS_INT(pop())));
			break;
		case OP_EQ:
			push(BOOL_VAL(AS_INT(pop()) == AS_INT(pop())));
			break;

			// Flow control

		case OP_JUMP:   // arg2: index of jump
			ip = code->jumps[READ_I16()];
			break;

		case OP_IF:   // arg2: index of jump
			adr = READ_I16();
			if (!AS_BOOL(pop())) {
				ip = code->jumps[adr];
			}
			break;

			// I/O

		case OP_SONG:
			apu->PlaySong(AS_INT(pop()), 1.0, 0.5, false);
			break;

		case OP_INPUT:
			push(BOOL_VAL(input->isPressed(AS_INT(pop()))));
			break;

		case OP_BUTTON:
			push(BOOL_VAL(input->wasPressed(AS_INT(pop()))));
		}
	}
}
