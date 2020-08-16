#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>

#include "Shader.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "VEXSong.h"
#include "ROMReader.h"
#include "util.cpp"



GLFWwindow* window;
int windowWidth = 1300;
int windowHeight = 1300;
GPU gpu;
APU apu;
Input input;



constexpr auto DEMO_POINTS = 200;
constexpr auto DEMO_LINES = 50;

struct DemoPoint {
	Point* gpupoint;
	float dx, dy;
};
DemoPoint* demoPoints = new DemoPoint[DEMO_POINTS];
struct DemoLine {
	Line* gpuline;
	float dx, dy;
};
DemoLine* demoLines = new DemoLine[DEMO_LINES];


void onResize(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;
	gpu.Resize(w, h);
}

void makeDemoPrims() {

}

void makeDemoClutter() {
	for (int i = 0; i < DEMO_POINTS; i++) {
		float x = (randFloat() - 0.5f) * 2.0f;
		float y = (randFloat() - 0.5f) * 2.0f;
		float r = randFloat() * 0.4f;
		float g = randFloat() * 0.7f;
		float b = randFloat() * 1.9f;
		float size = randFloat() * 3.5f + 1.0f;
		demoPoints[i].gpupoint = gpu.addPoint(x,y,r,g,b,size);
		demoPoints[i].dx = (randFloat() - 0.5f) * 0.5f;
		demoPoints[i].dy = (randFloat() - 0.5f) * 0.5f;
	}
	for (int i = 0; i < DEMO_LINES; i++) {
		float x1 = (randFloat() - 0.5f) * 2.0f;
		float y1 = (randFloat() - 0.5f) * 2.0f;
		float x2 = x1 + (randFloat() - 0.5f) * 0.3f;
		float y2 = y1 + (randFloat() - 0.5f) * 0.3f;
		float r = randFloat() * 1.4f;
		float g = randFloat();
		float b = randFloat() * 0.5f;
		demoLines[i].gpuline = gpu.addLine(x1, y1, x2, y2, r, g, b);
		demoLines[i].dx = (randFloat() - 0.5f) * 0.5f;
		demoLines[i].dy = (randFloat() - 0.5f) * 0.5f;
	}
}

void moveDemoPrims(float delta) {

}

void moveDemoClutter(float delta) {
	for (int i = 0; i < DEMO_POINTS; i++) {
		demoPoints[i].gpupoint->x += demoPoints[i].dx * delta;
		demoPoints[i].gpupoint->y += demoPoints[i].dy * delta;
		demoPoints[i].dy -= 0.2f * delta;
		if (demoPoints[i].gpupoint->y < -0.95f || demoPoints[i].gpupoint->y > 0.95f) demoPoints[i].dy = -demoPoints[i].dy;
		if (demoPoints[i].gpupoint->x < -0.95f || demoPoints[i].gpupoint->x > 0.95f) demoPoints[i].dx = -demoPoints[i].dx;
	}
	for (int i = 0; i < DEMO_LINES; i++) {
		demoLines[i].gpuline->x1 += demoLines[i].dx * delta;
		demoLines[i].gpuline->x2 += demoLines[i].dx * delta;
		demoLines[i].gpuline->y1 += demoLines[i].dy * delta;
		demoLines[i].gpuline->y2 += demoLines[i].dy * delta;
	}
}

// Proxy callback for APU
int handleAudio(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {
	return apu.genSamples(outBuffer, inBuffer, nFrames, streamTime, status, userData);
}

// Proxy callback for Input
void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	input.HandleKey(window, key, scancode, action, mods);
}

void fireDemoShot() {
	apu.voices[1].Trigger(440, 127);
}

void handleButton(int input) {
	if (input == 3) fireDemoShot();

	if (input == 50) glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (input == 51) gpu.toggleLayer(0);
	if (input == 52) gpu.toggleLayer(1);
	if (input == 53) gpu.toggleLayer(2);
	if (input == 54) apu.voices[0].testTone = !apu.voices[0].testTone;
}
void handleSwitch(int input, bool isDown) { }



int main() {
	
	float currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();


	// Setup devices
	gpu = GPU(windowWidth, windowHeight);
	window = gpu.Setup(onResize);
	apu = APU();
	apu.Setup(handleAudio);
	input = Input();
	input.Setup(window, handleKey, handleButton, handleSwitch);
	input.Add(50, BUTTON, GLFW_KEY_ESCAPE);
	input.Add(51, BUTTON, GLFW_KEY_F1);
	input.Add(52, BUTTON, GLFW_KEY_F2);
	input.Add(53, BUTTON, GLFW_KEY_F3);
	input.Add(54, BUTTON, GLFW_KEY_F4);


	// Read ROM file
	ROMReader reader = ROMReader("data/demogame.vexo");
	reader.Read(&gpu, &apu, &input);

	
	// Make demo shit
	std::cout << "creating demo prims" << std::endl;
	makeDemoPrims();
	makeDemoClutter();
	
	apu.PlaySong(0);

	// MAIN LOOP

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		moveDemoPrims(deltaTime);
		moveDemoClutter(deltaTime);

		gpu.Assemble();
		gpu.Render();
	}

	apu.Stop();
	gpu.Stop();
	return 0;
}
