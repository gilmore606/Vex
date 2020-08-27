#include "CPU.h"
#include <string>
#include <iostream>

CPU::CPU() {
	this->elapsed = 0.0;
	std::cout << "CPU core online" << std::endl;
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
	run(code->entryStart);
}

void CPU::OnUpdate(float deltaTime) {
	elapsed += deltaTime;
	std::vector<Task> undone;
	for (int i = 0; i < tasks.size(); i++) {
		Task task = tasks[i];
		if (task.waketime <= elapsed) {
			resume(task);
		} else {
			undone.push_back(task);
		}
	}
	tasks = undone;
	// put deltatime on stack and run entrypoint 'update'
	// run(code->entryUpdate);
}

void CPU::OnInput(int input) {
	// add to the handler queue:
	// 'put input on stack'
	// 'jump to onInput handler'
	run(code->entryInput);
}

void CPU::OnMIDI(Note* note) { }

void CPU::Stop() { }


void CPU::stackDump() {

}

void CPU::run(uint8_t* address) {
	if (address == nullptr) { return; }
	this->ip = address;
	this->stacktop = stack;  // clear the stack

	Value v, v1, v2;
	int adr, adr2;
	float f, vl, fac;
	for (;;) {
		uint8_t ins;
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

		case OP_WAIT:
			tasks.push_back(Task(ip, (double)AS_INT(pop()) / 1000.0 + elapsed));
			return;

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

		case OP_SETSYS:    // arg2: index of system setting

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

			// Type conversion

		case OP_I2F:
			push(FLOAT_VAL((float)AS_INT(pop())));
			break;
		case OP_F2I:
			push(INT_VAL((int)AS_FLOAT(pop())));
			break;
		case OP_V2F:
			v = pop();
			push(FLOAT_VAL(std::sqrt(v.as.vector[0] * v.as.vector[0] + v.as.vector[1] * v.as.vector[1])));
			break;
		case OP_B2I:
			push(INT_VAL(AS_BOOL(pop()) ? 1 : 0));
			break;
		case OP_N2I:
			pop();
			push(INT_VAL(0));
			break;

			// Vector math

		case OP_NEGV:
			v = pop();
			push(VECTOR_VAL(-v.as.vector[0], -v.as.vector[1]));
			break;
		case OP_ADDV:
			v1 = pop();
			v2 = pop();
			push(VECTOR_VAL(v1.as.vector[0] + v2.as.vector[0], v1.as.vector[1] + v2.as.vector[1]));
			break;
		case OP_MULTV:   // dot product
			v1 = pop();
			v2 = pop();
			push(FLOAT_VAL(v1.as.vector[0] * v2.as.vector[0] + v1.as.vector[1] * v2.as.vector[1]));
			break;
		case OP_DIVV:    // ??? replace with something useful
			v1 = pop();
			v2 = pop();
			push(VECTOR_VAL(v1.as.vector[0] / v2.as.vector[0], v1.as.vector[1] / v2.as.vector[1]));
			break;
		case OP_ADDVF:
			v = pop();
			f = AS_FLOAT(pop());
			vl = (std::sqrt(v.as.vector[0] * v.as.vector[0] + v.as.vector[1] * v.as.vector[1]));
			fac = (vl + f) / vl;
			push(VECTOR_VAL(v.as.vector[0] * fac, v.as.vector[1] * fac));
			break;
		case OP_MULTVF:  
			v = pop();
			f = AS_FLOAT(pop());
			push(VECTOR_VAL(v.as.vector[0] * f, v.as.vector[1] * f));
			break;
		case OP_DIVVF:
			v = pop();
			f = AS_FLOAT(pop());
			push(VECTOR_VAL(v.as.vector[0] / f, v.as.vector[1] / f));
			break;

			// Logic

		case OP_NOT:
			push(BOOL_VAL(!AS_BOOL(pop())));
			break;
		case OP_OR:
			push(BOOL_VAL(AS_BOOL(pop()) || AS_BOOL(pop())));
			break;
		case OP_AND:
			push(BOOL_VAL(AS_BOOL(pop()) && AS_BOOL(pop())));
			break;

			// Comparison

		case OP_GTI:
			push(BOOL_VAL(AS_INT(pop()) > AS_INT(pop())));
			break;
		case OP_GEI:
			push(BOOL_VAL(AS_INT(pop()) >= AS_INT(pop())));
			break;
		case OP_LTI:
			push(BOOL_VAL(AS_INT(pop()) < AS_INT(pop())));
			break;
		case OP_LEI:
			push(BOOL_VAL(AS_INT(pop()) <= AS_INT(pop())));
			break;
		case OP_EQI:
			push(BOOL_VAL(AS_INT(pop()) == AS_INT(pop())));
			break;
		case OP_GTF:
			push(BOOL_VAL(AS_FLOAT(pop()) > AS_FLOAT(pop())));
			break;
		case OP_GEF:
			push(BOOL_VAL(AS_FLOAT(pop()) >= AS_FLOAT(pop())));
			break;
		case OP_LTF:
			push(BOOL_VAL(AS_FLOAT(pop()) < AS_FLOAT(pop())));
			break;
		case OP_LEF:
			push(BOOL_VAL(AS_FLOAT(pop()) <= AS_FLOAT(pop())));
			break;
		case OP_EQF:
			push(BOOL_VAL(AS_FLOAT(pop()) == AS_FLOAT(pop())));
			break;

			// Flow control

		case OP_JUMP:   
			ip = code->jumps[READ_I16()];  // arg2: index of jump
			break;

		case OP_IF:   
			adr = READ_I16(); // arg2: index of jump
			if (!AS_BOOL(pop())) {
				ip = code->jumps[adr];
			}
			break;

		case OP_LOOP:   
			adr = READ_I16(); // arg2: index of var
			adr2 = READ_I16(); // arg2: index of jump
			if (AS_INT(code->variables[adr]) < AS_INT(pop())) {
				ip = code->jumps[adr2];
			}

			// I/O

		case OP_SONG:
			apu->PlaySong(AS_INT(pop()), 1.0, 0.5, false);
			break;

		case OP_SPRITE:

			break;

		case OP_PARTI:

			break;

		case OP_INPUT:
			push(BOOL_VAL(input->isPressed(AS_INT(pop()))));
			break;

		case OP_BUTTON:
			push(BOOL_VAL(input->wasPressed(AS_INT(pop()))));
		}
	}
}

void CPU::resume(Task task) {
	std::cout << "task resume" << std::endl;
	run(task.ip);
}
