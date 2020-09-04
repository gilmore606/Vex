#include "CPU.h"
#include <string>
#include <iostream>
#include <map>

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
	run(code->entryState);
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

	// TODO: implement
	//if (countPressed > 0) {
	//	CLEAR_STACK();
	//	push(INT_VAL(countPressed));
	//	for (int i = 0; i < 32; i++) {
	//		if (buttonPressed[i]) {
	//			push(INT_VAL(i));
	//			buttonPressed[i] = false;
	//		}
	//	}
	//	countPressed = 0;
	//	run(code->entryInput);
	//}

	// CLEAR_STACK();
	// push(FLOAT_VAL(deltaTime));
	// run(code->entryUpdate);
}

void CPU::OnInput(int input) {
	buttonPressed[input] = true;
	countPressed++;
}

void CPU::OnMIDI(Note* note) { }

void CPU::Stop() { }


void CPU::logValue(Value v) {
	std::cout << "CPU DEBUG : ";
	switch (v.type) {
	case VAL_NIL:
		std::cout << "nil";
	case VAL_BOOL:
		std::cout << v.as.boolean ? "true" : "false";
		break;
	case VAL_INT:
		std::cout << v.as.integer;
		break;
	case VAL_FLOAT:
		std::cout << v.as.fp;
		break;
	case VAL_VECTOR:
		std::cout << "V(" << v.as.vector[0] << "," << v.as.vector[1] << ")";
		break;
	case VAL_COLOR:
		std::cout << "C(" << v.as.color[0] << "," << v.as.color[1] << "," << v.as.color[2] << ")";
		break;
	case VAL_STRING:
		std::cout << v.as.string->s;
		break;
	}
	std::cout << std::endl;
}

void CPU::run(uint8_t* address) {
	if (address == nullptr) { return; }
	this->ip = address;

	// temps
	Value v, v1, v2;
	Value* vp;
	VStr* vs1;
	VStr* vs2;
	int vi, ji, fi;
	float f, f2;
	boolean b;

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
			logValue(pop());
			break;
		case OP_WAIT:
			tasks.push_back(Task(ip, (double)AS_INT(pop()) / 1000.0 + elapsed));
			return;
		case OP_INPUT:
			push(BOOL_VAL(input->isPressed(AS_INT(pop()))));
			break;
		case OP_BUTTON:
			push(BOOL_VAL(buttonPressed[AS_INT(pop())]));
			break;

			// Values

		case OP_VAR:   
			push(code->variables[READ_I16()]);
			break;
		case OP_CONST:   
			push(code->constants[READ_I16()]);
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
		case OP_C2VAR:
			v = code->constants[READ_I16()];
			code->variables[READ_I16()] = v;
			break;
		case OP_V2VAR:
			v = code->variables[READ_I16()];
			code->variables[READ_I16()] = v;
			break;
		case OP_ACCVAR:
			fi = pop().as.integer;
			code->variables[READ_I16()].as.integer += fi;
			break;
		case OP_ACCVARF:
			f = pop().as.fp;
			code->variables[READ_I16()].as.fp += f;
			break;
		case OP_VFLIP:
			vi = READ_I16();
			code->variables[vi].as.boolean = !code->variables[vi].as.boolean;
			break;
		case OP_SETSYS: 

			break;
		case OP_FUN:
			fi = READ_I16();

			break;
		case OP_SFUN:
			callSFUN(READ_I16());
			break;
		case OP_SMETH:
			callSMETH(READ_I16(), pop());
			break;
		case OP_STAT:
			fi = READ_I16();
			callSTAT(fi, READ_I16());
			break;
		case OP_LDI:
			push(INT_VAL(READ_I16()));
			break;


			// Stack math

		case OP_INC:
			TOPVAL().as.integer++;
			break;
		case OP_DEC:
			TOPVAL().as.integer--;
			break;
		case OP_ADDI:
			vi = pop().as.integer;
			TOPVAL().as.integer += vi;
			break;
		case OP_SUBI:
			vi = pop().as.integer;
			TOPVAL().as.integer -= vi;
			break;
		case OP_MULI:
			vi = pop().as.integer;
			TOPVAL().as.integer *= vi;
			break;
		case OP_DIVI:
			vi = pop().as.integer;
			TOPVAL().as.integer /= vi;
			break;
		case OP_MODI:
			vi = pop().as.integer;
			TOPVAL().as.integer = TOPVAL().as.integer % vi;
			break;
		case OP_POWI:
			vi = pop().as.integer;
			TOPVAL().as.integer = TOPVAL().as.integer ^ vi;
			break;
		case OP_NEGI:
			TOPVAL().as.integer = -TOPVAL().as.integer;
			break;
		case OP_ADDF:
			f = pop().as.fp;
			TOPVAL().as.fp += f;
			break;
		case OP_SUBF:
			f = pop().as.fp;
			TOPVAL().as.fp -= f;
			break;
		case OP_MULF:
			f = pop().as.fp;
			TOPVAL().as.fp *= f;
			break;
		case OP_DIVF:
			f = pop().as.fp;
			TOPVAL().as.fp /= f;
			break;
		case OP_MODF:
			f = pop().as.fp;
			TOPVAL().as.fp = std::fmod(TOPVAL().as.fp, f);
			break;
		case OP_POWF:
			f = pop().as.fp;
			TOPVAL().as.fp = std::pow(TOPVAL().as.fp, f);
			break;
		case OP_NEGF:
			TOPVAL().as.fp = -TOPVAL().as.fp;
			break;


			// Type conversion

		case OP_I2F:
			TOPVAL().as.fp = static_cast<float>(TOPVAL().as.integer);
			TOPVAL().type = VAL_FLOAT;
			break;
		case OP_F2I:
			TOPVAL().as.integer = static_cast<int>(TOPVAL().as.fp);
			TOPVAL().type = VAL_INT;
			break;
		case OP_V2F:
			TOPVAL().as.fp = std::sqrt(TOPVAL().as.vector[0] * TOPVAL().as.vector[0] + TOPVAL().as.vector[1] * TOPVAL().as.vector[1]);
			TOPVAL().type = VAL_FLOAT;
			break;
		case OP_C2F:
			TOPVAL().as.fp = (TOPVAL().as.color[0] + TOPVAL().as.color[1] + TOPVAL().as.color[2]) * 0.3333f;
			TOPVAL().type = VAL_FLOAT;
			break;
		case OP_B2I:
			TOPVAL().as.integer = (TOPVAL().as.boolean ? 1 : 0);
			TOPVAL().type = VAL_INT;
			break;
		case OP_N2I:
			TOPVAL().as.integer = 0;
			TOPVAL().type = VAL_INT;
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
			TOPVAL().as.vector[0] = -TOPVAL().as.vector[0];
			TOPVAL().as.vector[1] = -TOPVAL().as.vector[1];
			break;
		case OP_ADDV:
			v1 = pop();
			TOPVAL().as.vector[0] += v1.as.vector[0];
			TOPVAL().as.vector[1] += v1.as.vector[1];
			break;
		case OP_MULV:   // dot product
			v1 = pop();
			TOPVAL().as.fp = TOPVAL().as.vector[0] * v1.as.vector[0] + TOPVAL().as.vector[1] * v1.as.vector[1];
			TOPVAL().type = VAL_FLOAT;
			break;
		case OP_DIVV:    // ??? replace with something useful
			v1 = pop();
			v2 = pop();
			push(VECTOR_VAL(v1.as.vector[0] / v2.as.vector[0], v1.as.vector[1] / v2.as.vector[1]));
			break;
		case OP_ADDVF:    // lengthen vector by scalar amount
			f = pop().as.fp;
			f2 = std::sqrt(TOPVAL().as.vector[0] * TOPVAL().as.vector[0] + TOPVAL().as.vector[1] * TOPVAL().as.vector[1]);
			f2 = (f2 + f) / f2;
			TOPVAL().as.vector[0] *= f2;
			TOPVAL().as.vector[1] *= f2;
			break;
		case OP_MULVF:
			f = pop().as.fp;
			TOPVAL().as.vector[0] *= f;
			TOPVAL().as.vector[1] *= f;
			break;
		case OP_DIVVF:
			f = pop().as.fp;
			TOPVAL().as.vector[0] /= f;
			TOPVAL().as.vector[1] /= f;
			break;


			// Color math

		case OP_ADDC:
			v1 = pop();
			v2 = pop();
			v1.as.color[0] += v2.as.color[0];
			v1.as.color[1] += v2.as.color[1];
			v1.as.color[2] += v2.as.color[2];
			push(v1);
			break;
		case OP_MULC:
			v1 = pop();
			v2 = pop();
			v1.as.color[0] = (v1.as.color[0] + v2.as.color[0]) * 0.5;
			v1.as.color[1] = (v1.as.color[1] + v2.as.color[1]) * 0.5;
			v1.as.color[2] = (v1.as.color[2] + v2.as.color[2]) * 0.5;
			push(v1);
			break;
		case OP_ADDCF:
			v1 = pop();
			f = AS_FLOAT(pop());
			v1.as.color[0] += f;
			v1.as.color[1] += f;
			v1.as.color[2] += f;
			push(v1);
			break;
		case OP_MULCF:
			v1 = pop();
			f = AS_FLOAT(pop());
			v1.as.color[0] *= f;
			v1.as.color[1] *= f;
			v1.as.color[2] *= f;
			push(v1);
			break;


			// Logic

		case OP_NOT:
			TOPVAL().as.boolean = !TOPVAL().as.boolean;
			break;
		case OP_OR:
			b = pop().as.boolean;
			TOPVAL().as.boolean = TOPVAL().as.boolean || b;
			break;
		case OP_AND:
			b = pop().as.boolean;
			TOPVAL().as.boolean = TOPVAL().as.boolean && b;
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
		case OP_EQV:
			v1 = pop();
			v2 = pop();
			push(BOOL_VAL((v1.as.vector[0] == v2.as.vector[0]) && (v1.as.vector[1] == v2.as.vector[1])));
			break;
		case OP_EQC:
			v1 = pop();
			v2 = pop();
			push(BOOL_VAL((v1.as.color[0] == v2.as.color[0]) && (v1.as.color[1] == v2.as.color[1]) && (v1.as.color[2] == v2.as.color[2])));
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

		}
	}
}

void CPU::resume(Task task) {
	CLEAR_STACK();
	run(task.ip);
}

void CPU::callSFUN(int fi) {
	float f1;
	int v1;
	int v2;
	VStr* text;
	switch (fi) {
	case 0: // vector = V(f,f)
		push(VECTOR_VAL(AS_FLOAT(pop()), AS_FLOAT(pop())));
		break;
	case 1: // color = C(f,f,f)
		push(COLOR_VAL(AS_FLOAT(pop()), AS_FLOAT(pop()), AS_FLOAT(pop())));
		break;
	case 2: // float = RAND(f,f)
		f1 = AS_FLOAT(pop());
		push(FLOAT_VAL(f1 + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (AS_FLOAT(pop()) - f1)));
		break;
	case 3: // float = RAND(f)
		push(FLOAT_VAL((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * AS_FLOAT(pop())));
		break;
	case 4: // int = RAND(i,i)
		v1 = AS_INT(pop());
		v2 = AS_INT(pop());
		push(INT_VAL(v1 + (rand() % (v2 - v1))));
		break;
	case 5: // int = RAND(i)
		push(INT_VAL(rand() % AS_INT(pop())));
		break;
	case 6: // float = RAND()
		push(FLOAT_VAL(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)));
		break;
	case 7: // sprite = TEXT("text")
		text = pop().as.string;

		break;

	case 8: // sprite = SPRITE(imageid)
		v1 = pop().as.integer;

		break;
	}
}

void CPU::callSMETH(int fi, Value vthis) {

}

void CPU::callSTAT(int fi, int paramc) {
	std::map<int, Value> params;
	for (int i = 0; i < paramc; i++) {
		int pi = AS_INT(pop());
		params[pi] = pop();
	}
	switch (fi) {
	case 0: { // PARTICLE
		int particleID = AS_INT(pop());
		ParticleDef* def = gpu->fetchParticleDef(particleID);
		Pos p =			 (params.count(0) > 0) ? LIFT_POS(params[0]) : Pos(0.0f, 0.0f);
		Vec v =			 (params.count(1) > 0) ? LIFT_VECTOR(params[1]) : LIFT_VECTOR(def->v);
		Vec vv =		 (params.count(2) > 0) ? LIFT_VECTOR(params[2]) : LIFT_VECTOR(def->vv);
		Color c0 =		 (params.count(3) > 0) ? LIFT_COLOR(params[3]) : LIFT_COLOR(def->color0);
		Color c1 =		 (params.count(4) > 0) ? LIFT_COLOR(params[4]) : LIFT_COLOR(def->color1);
		Vec sc1 =		 (params.count(5) > 0) ? LIFT_VECTOR(params[5]) : LIFT_VECTOR(def->scale0);
		Vec sc2 =		 (params.count(6) > 0) ? LIFT_VECTOR(params[6]) : LIFT_VECTOR(def->scale1);
		float rot =		 (params.count(7) > 0) ? AS_FLOAT(params[7]) : (def->rot);
		float rotv =	 (params.count(8) > 0) ? AS_FLOAT(params[8]) : (def->rotv);
		float lifetime = (params.count(9) > 0) ? AS_FLOAT(params[9]) : (def->lifetime);
		gpu->spawnParticle(def->image, p, v, vv, c0, c1, sc1, sc2, rot, rotv, lifetime);
		break;
	}
	case 1: { // SONG
		int songID = AS_INT(pop());
		float vol =			(params.count(0) > 0) ? AS_FLOAT(params[0]) : 1.0f;
		float pan =			(params.count(1) > 0) ? AS_FLOAT(params[1]) : 0.5f;
		int transpose =		(params.count(2) > 0) ? AS_INT(params[2]) : 0;
		float timescale =	(params.count(3) > 0) ? AS_FLOAT(params[3]) : 1.0f;
		bool loop =			(params.count(4) > 0) ? AS_BOOL(params[4]) : false;
		apu->PlaySong(songID, vol, pan, loop);
		break;
	}
	}
}
