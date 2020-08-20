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

void CPU::Stop() {

}

// any Executor can 'wait' and be tagged with a wake time
// cpu.onUpdate checks for executors ready to wake
