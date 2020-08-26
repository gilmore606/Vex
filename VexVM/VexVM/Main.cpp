#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <list>

#include "ROMReader.h"
#include "util.cpp"
#include "Scheduler.h"
#include "CPU.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Vector.h"
#include "DemoGame.h"
#include "DemoSynth.h"

CPU* cpu;
//DemoSynth* cpu;
//DemoSynth* cpu;
GPU* gpu;
APU* apu;
Input* input;
Scheduler* scheduler;
GLFWwindow* window;
int windowWidth = 1300;
int windowHeight = 1300;

// Proxy callback for GPU
void onResize(GLFWwindow * window, int w, int h) {
	windowWidth = w;
	windowHeight = h;
	gpu->Resize(w, h);
}

// Proxy callbacks for APU
int handleAudio(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {
	return apu->genSamples(outBuffer, inBuffer, nFrames, streamTime, status, userData);
}
void handleMIDI(double deltatime, std::vector<unsigned char> * message, void* userData) {
	apu->receiveMIDI(deltatime, message, userData);
}

// Proxy callback for Input
void handleKey(GLFWwindow * window, int key, int scancode, int action, int mods) {
	input->HandleKey(window, key, scancode, action, mods);
}
void handleSwitch(int input, bool isDown) { }
void handleButton(int input) {
	if (input == 999) { gpu->ToggleFullscreen(); }
	if (input == 50) { glfwSetWindowShouldClose(window, GLFW_TRUE); }
	if (input == 51) { gpu->toggleLayer(0); }
	if (input == 52) { gpu->toggleLayer(1); }
	if (input == 53) { gpu->toggleLayer(2); }

	cpu->OnInput(input);
}


int main() {

	// Setup devices

	scheduler = new Scheduler();
	cpu = new CPU();
	//cpu = new DemoSynth();
	//cpu = new DemoGame();
	gpu = new GPU(windowWidth, windowHeight);
	window = gpu->Setup(onResize);
	apu = new APU();
	apu->Setup(&handleAudio, handleMIDI);

	input = new Input();
	input->Setup(window, handleKey, handleButton, handleSwitch);
	input->Add(50, BUTTON, GLFW_KEY_ESCAPE);
	input->Add(51, BUTTON, GLFW_KEY_F1);
	input->Add(52, BUTTON, GLFW_KEY_F2);
	input->Add(53, BUTTON, GLFW_KEY_F3);
	input->Add(54, BUTTON, GLFW_KEY_F4);
	input->Add(55, BUTTON, GLFW_KEY_F5);

	cpu->Connect(scheduler, gpu, apu, input);
	apu->Connect(cpu, input);

	// Read ROM file
	ROMReader reader = ROMReader("data/boot.vexo");
	reader.Read(cpu, gpu, apu, input);

	// Boot
	cpu->Boot();
	scheduler->Start();

	// MAIN LOOP

	float currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();
	int framec = 0;
	float frametime = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		scheduler->OnUpdate(currentFrame);
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

	scheduler->Stop();
	apu->Stop();
	cpu->Stop();
	gpu->Stop();

	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	main();
}
