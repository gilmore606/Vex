#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <list>

#include "ROMReader.h"
#include "CPU.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Vector.h"
#include "DemoGame.h"
#include "DemoSynth.h"

CPU* cpu;
GPU* gpu;
APU* apu;
Input* input;

// proxy callback protos
void handleResize(GLFWwindow* window, int w, int h);
int handleAudio(void* outBuffer, void* inBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData);
void handleMIDI(double deltatime, std::vector<unsigned char>* message, void* userData);
void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleSwitch(int input, bool isDown);
void handleButton(int input);



int main() {

	// Setup devices

	cpu = new CPU();
	gpu = new GPU(1300, 1300);
	gpu->Setup(handleResize);
	apu = new APU();
	apu->Setup(handleAudio, handleMIDI);

	input = new Input();
	input->Setup(gpu->window, handleKey, handleButton, handleSwitch);
	input->Add(50, BUTTON, GLFW_KEY_ESCAPE);
	input->Add(51, BUTTON, GLFW_KEY_F1);
	input->Add(52, BUTTON, GLFW_KEY_F2);
	input->Add(53, BUTTON, GLFW_KEY_F3);
	input->Add(54, BUTTON, GLFW_KEY_F4);
	input->Add(55, BUTTON, GLFW_KEY_F5);

	cpu->Connect(gpu, apu, input);
	apu->Connect(cpu, input);


	// Boot

	ROMReader reader = ROMReader("data/boot.vexo");
	reader.Read(cpu, gpu, apu, input);

	cpu->Boot();


	// MAIN LOOP

	float currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();
	int framec = 0;
	float frametime = 0.0f;

	while (!glfwWindowShouldClose(gpu->window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		cpu->OnUpdate(deltaTime);
		gpu->OnUpdate(deltaTime);
		gpu->Assemble();
		gpu->Render();
		gpu->Collide(deltaTime);

		++framec;
		frametime += deltaTime;
		if (framec > 100) {
			std::cout << (int)((float)framec / frametime) << " fps" << std::endl;
			framec = 0;
			frametime = 0.0f;
		}
	}

	apu->Stop();
	cpu->Stop();
	gpu->Stop();

	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	main();
}


// Callbacks

// Proxy for GPU
void handleResize(GLFWwindow* window, int w, int h) {
	gpu->Resize(w, h);
}

// Proxy for APU
int handleAudio(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {
	return apu->genSamples(outBuffer, inBuffer, nFrames, streamTime, status, userData);
}
void handleMIDI(double deltatime, std::vector<unsigned char>* message, void* userData) {
	apu->receiveMIDI(deltatime, message, userData);
}

// Proxy for Input
void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	input->HandleKey(window, key, scancode, action, mods);
}
void handleSwitch(int input, bool isDown) { }
void handleButton(int input) {
	if (input == 999) { gpu->ToggleFullscreen(); }
	if (input == 50) { glfwSetWindowShouldClose(gpu->window, GLFW_TRUE); }
	if (input == 51) { gpu->toggleLayer(0); }
	if (input == 52) { gpu->toggleLayer(1); }
	if (input == 53) { gpu->toggleLayer(2); }

	cpu->OnInput(input);
}
