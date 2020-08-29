#include "CPU.h"
#include <string>
#include <iostream>

CPU::CPU() {
	this->elapsed = 0.0;
	buttonPressed = new bool[32];
	countPressed = 0;
}

void CPU::Connect(GPU* gpu, APU* apu, Input* input) {
	this->gpu = gpu;
	this->apu = apu;
	this->input = input;
	std::cout << "CPU core online" << std::endl;
}

void CPU::LoadCode(Code* code) {
	this->code = code;
}

void CPU::Boot() {
	srand(static_cast <unsigned> (glfwGetTime())); // seed rng

	CLEAR_STACK();
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

	if (countPressed > 0) {
		CLEAR_STACK();
		push(INT_VAL(countPressed));
		for (int i = 0; i < 32; i++) {
			if (buttonPressed[i]) {
				push(INT_VAL(i));
				buttonPressed[i] = false;
			}
		}
		countPressed = 0;
		run(code->entryInput);
	}

	CLEAR_STACK();
	push(FLOAT_VAL(deltaTime));
	run(code->entryUpdate);
}

void CPU::OnInput(int input) {
	buttonPressed[input] = true;
	countPressed++;
}

void CPU::OnMIDI(Note* note) { }

void CPU::Stop() { }


void CPU::stackDump() {

}

void CPU::run(uint8_t* address) {
	if (address == nullptr) { return; }
	this->ip = address;

	// temps
	Value v, v1, v2;
	Value* vp;
	VStr* vs1;
	VStr* vs2;
	int vi, ji;
	float f, f2;

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

		case OP_VAR:   
			push(code->variables[READ_I16()]);
			break;

		case OP_CONST:   
			push(code->constants[READ_I16()]);
			break;

		case OP_RANDF:
			push(FLOAT_VAL(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)));
			break;

		case OP_RANDI:
			push(INT_VAL(rand() % AS_INT(pop())));
			break;

		case OP_INCVAR:  
			vp = &code->variables[READ_I16()];
			vp->as.integer += 1;
			break;

		case OP_DECVAR:   
			vp = &code->variables[READ_I16()];
			vp->as.integer -= 1;
			break;

		case OP_SETVAR:  
			code->variables[READ_I16()] = pop();
			break;

		case OP_SETSYS: 

			break;


			// Stack math

		case OP_INC:
			stacktop->as.integer = ++((*stacktop).as.integer);
			break;
		case OP_DEC:
			stacktop->as.integer = --((*stacktop).as.integer);
			break;
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
		case OP_MODI:
			push(INT_VAL(AS_INT(pop()) % AS_INT(pop())));
			break;
		case OP_POWI:
			push(INT_VAL(AS_INT(pop()) ^ AS_INT(pop())));
			break;
		case OP_NEGI:
			stacktop->as.integer = -((*stacktop).as.integer);
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
		case OP_MODF:
			push(FLOAT_VAL(std::fmod(pop().as.fp, pop().as.fp)));
			break;
		case OP_POWF:
			push(FLOAT_VAL(std::pow(pop().as.fp, pop().as.fp)));
			break;
		case OP_NEGF:
			stacktop->as.fp = -((*stacktop).as.fp);
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


			// Strings

		case OP_I2S:
			vs1 = new VStr{ std::to_string(AS_INT(pop())) };
			push(STRING_VAL(vs1));
			break;
		case OP_F2S:
			vs1 = new VStr{ std::to_string(AS_FLOAT(pop())) };
			push(STRING_VAL(vs1));
			break;
		case OP_CAT:
			vs1 = pop().as.string;
			vs2 = pop().as.string;
			push(STRING_VAL(new VStr{ vs1->s + vs2->s }));
			break;
		case OP_EQS:
			vs1 = pop().as.string;
			vs2 = pop().as.string;
			push(BOOL_VAL(vs1->s == vs2->s));
			break;


			// Vector math

		case OP_NEGV:
			stacktop->as.vector[0] = -((*stacktop).as.vector[0]);
			stacktop->as.vector[1] = -((*stacktop).as.vector[1]);
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
			f2 = (std::sqrt(v.as.vector[0] * v.as.vector[0] + v.as.vector[1] * v.as.vector[1]));
			f2 = (f2 + f) / f2;
			push(VECTOR_VAL(v.as.vector[0] * f2, v.as.vector[1] * f2));
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
			stacktop->as.boolean = !((*stacktop).as.boolean);
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
			ip = code->jumps[READ_I16()];
			break;

		case OP_JUMPZ:   
			vi = READ_I16();
			ji = READ_I16();
			if (AS_INT(code->variables[vi]) == 0) {
				ip = code->jumps[ji];
			}
			break;

		case OP_JUMPNZ:
			vi = READ_I16();
			ji = READ_I16();
			if (AS_INT(code->variables[vi]) != 0) {
				ip = code->jumps[ji];
			}
			break;

		case OP_IF:   
			ji = READ_I16();
			if (!AS_BOOL(pop())) {
				ip = code->jumps[ji];
			}
			break;


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
	CLEAR_STACK();
	run(task.ip);
}
