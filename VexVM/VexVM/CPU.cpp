#include "CPU.h"
#include <string>
#include <iostream>

CPU::CPU() {
	std::cout << "CPU missing; default to demo rom" << std::endl;
}

void CPU::Connect(GPU* gpu, APU* apu, Input* input) {
	this->gpu = gpu;
	this->apu = apu;
	this->input = input;
}

void CPU::LoadCode(std::string name) {

}

void CPU::Boot() {

}

void CPU::OnUpdate(float deltaTime) {

}

void CPU::OnInput(int input) {

}

void CPU::Stop() {

}
