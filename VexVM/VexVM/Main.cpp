#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>

#include "VEXSong.h"
#include "ROMReader.h"
#include "util.cpp"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Sprite.h"

GPU gpu;
APU apu;
Input input;



GLFWwindow* window;
int windowWidth = 1300;
int windowHeight = 1300;

constexpr auto DEMO_POINTS = 200;

struct DemoPoint {
	Point* gpupoint;
	float dx, dy;
};
DemoPoint* demoPoints = new DemoPoint[DEMO_POINTS];

struct DemoRock {
	Sprite* sprite;
	float dx, dy, drot;
};
std::vector<DemoRock> demoRocks;
DemoRock demoShip;

void onResize(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;
	gpu.Resize(w, h);
}

void makeDemoPrims() {
	for (int i = 0; i < 16; i++) {
		Sprite* sp = new Sprite(1, &gpu);
		sp->moveTo(randCoord(), randCoord());
		float scale = (randFloat() * 0.05f) + 0.05f;
		sp->scale(scale, scale);
		sp->rotate(randFloat() * 2.7f);
		DemoRock rock;
		rock.sprite = sp;
		rock.dx = randCoord() * 0.2f;
		rock.dy = randCoord() * 0.2f;
		rock.drot = randFloat();
		demoRocks.push_back(rock);
	}
	Sprite* shipsp = new Sprite(0, &gpu);
	shipsp->scale(0.4f, 0.4f);
	demoShip.sprite = shipsp;
	demoShip.dx = 0.0f;
	demoShip.dy = 0.0f;
	demoShip.drot = 0.0f;
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
}

void moveDemoPrims(float delta) {
	for (int i = 0; i < demoRocks.size(); i++) {
		Sprite* sp = demoRocks[i].sprite;
		sp->moveTo(wrapCoord(sp->x() + demoRocks[i].dx * delta), wrapCoord(sp->y() + demoRocks[i].dy * delta));
		sp->rotate(demoRocks[i].drot * delta);
	}
}

void moveDemoClutter(float delta) {
	for (int i = 0; i < DEMO_POINTS; i++) {
		demoPoints[i].gpupoint->x += demoPoints[i].dx * delta;
		demoPoints[i].gpupoint->y += demoPoints[i].dy * delta;
		demoPoints[i].dy -= 0.2f * delta;
		if (demoPoints[i].gpupoint->y < -0.95f || demoPoints[i].gpupoint->y > 0.95f) demoPoints[i].dy = -demoPoints[i].dy;
		if (demoPoints[i].gpupoint->x < -0.95f || demoPoints[i].gpupoint->x > 0.95f) demoPoints[i].dx = -demoPoints[i].dx;
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
	// TODO: play an actual sound, make a point
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
	Sprite gridSprite = Sprite(2, &gpu);

	//apu.PlaySong(0);

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
