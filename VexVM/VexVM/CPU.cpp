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

void CPU::LoadCode(std::string name) {

}

void CPU::Boot() {
	// start the first executor, set scene to start, run onstart
}

void CPU::OnUpdate(float deltaTime) {
	// if my current executor is still running, throw it on the running queue, get a new executor
	// run onUpdate in my executor
}

void CPU::OnInput(int input) {
	// if my current executor is still running, throw it on the running queue, get a new executor
	// run onInput in my executor
}

void CPU::OnMIDI(Note* note) {

}

void CPU::Stop() {

}

// any Executor can 'wait' and be tagged with a wake time
// cpu.onUpdate checks for executors ready to wake


void CPU::stackDump() {

}

void CPU::run(Codechunk* chunk) {
	this->chunk = chunk;
	this->ip = chunk->code;
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
			push(chunk->variables[READ_I16()]);
			break;

		case OP_CONST:   // arg2: index of constant
			push(chunk->constants[READ_I16()]);
			break;

		case OP_RANDF:
			push(NUMBER_VAL(rand()));
			break;

		case OP_RANDI:
			push(NUMBER_VAL((int)(rand() * (double)AS_NUMBER(pop()))));
			break;

		case OP_SETVAR:   // arg2: index of var
			chunk->variables[READ_I16()] = pop();
			break;

			// Math

		case OP_ADD:
			push(NUMBER_VAL(AS_NUMBER(pop()) + AS_NUMBER(pop())));
			break;
		case OP_SUB:
			push(NUMBER_VAL(AS_NUMBER(pop()) - AS_NUMBER(pop())));
			break;
		case OP_MULT:
			push(NUMBER_VAL(AS_NUMBER(pop()) * AS_NUMBER(pop())));
			break;
		case OP_DIV:
			push(NUMBER_VAL(AS_NUMBER(pop()) / AS_NUMBER(pop())));
			break;
		case OP_NEG:
			push(NUMBER_VAL(-AS_NUMBER(pop())));
			break;

			// Logic

		case OP_NOT:
			push(BOOL_VAL(!AS_BOOL(pop())));
			break;
		case OP_GT:
			push(BOOL_VAL(AS_NUMBER(pop()) > AS_NUMBER(pop())));
			break;
		case OP_GE:
			push(BOOL_VAL(AS_NUMBER(pop()) >= AS_NUMBER(pop())));
			break;
		case OP_LT:
			push(BOOL_VAL(AS_NUMBER(pop()) < AS_NUMBER(pop())));
			break;
		case OP_LE:
			push(BOOL_VAL(AS_NUMBER(pop()) <= AS_NUMBER(pop())));
			break;
		case OP_EQ:
			push(BOOL_VAL(AS_NUMBER(pop()) == AS_NUMBER(pop())));
			break;

			// Flow control

		case OP_JUMP:   // arg2: index of jump
			ip = chunk->jumps[READ_I16()];
			break;

		case OP_IF:   // arg2: index of jump
			adr = READ_I16();
			if (!AS_BOOL(pop())) {
				ip = chunk->jumps[adr];
			}
			break;

			// I/O

		case OP_SONG:
			apu->PlaySong(AS_NUMBER(pop()), AS_NUMBER(pop()), AS_NUMBER(pop()), AS_BOOL(pop()));
			break;

		case OP_INPUT:
			push(BOOL_VAL(input->isPressed(AS_NUMBER(pop()))));
			break;

		}
	}
}
