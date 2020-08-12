#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"

#include "util.cpp"


constexpr auto DEMO_POINTS = 500;
constexpr auto DEMO_LINES = 500;

GLFWwindow* window;
int windowWidth = 1300;
int windowHeight = 1300;
GPU gpu;
APU apu;
Input input;

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
Sprite* demoSprite = new Sprite();
struct DemoShip {
	float x = 0.0f;
	float y = 0.0f;
	float xd = 0.0f;
	float yd = 0.0f;
};
DemoShip demoShip;



void makeDemoPrims();
void doReset() {
	gpu.Reset();
	makeDemoPrims();
}

void onResize(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;
	gpu.Resize(w, h);
}

bool makeWindow(int w, int h) {
	std::cout << "GLFW initializing" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	window = glfwCreateWindow(w, h, "VEX", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, onResize);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	std::cout << "GLFW window initialized" << std::endl;
	return true;
}

void makeDemoPrims() {
	demoSprite->add(0.0f, 0.1f, 0.05f, -0.05f, 0.3f, 0.7f, 1.0f);
	demoSprite->add(0.05f, -0.05f, 0.0f, -0.02f, 0.7f, 0.3f, 0.0f);
	demoSprite->add(0.0f, -0.02f, -0.05f, -0.05f, 0.7f, 0.3f, 0.0f);
	demoSprite->add(-0.05f, -0.05f, 0.0f, 0.1f, 0.3f, 0.7f, 1.0f);
	demoSprite->xs = 0.35f;
	demoSprite->ys = 0.35f;

	for (int i = 0; i < DEMO_POINTS; i++) {
		float x = (randFloat() - 0.5f) * 2.0f;
		float y = (randFloat() - 0.5f) * 2.0f;
		float r = randFloat();
		float g = randFloat();
		float b = randFloat();
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
		float r = randFloat();
		float g = randFloat();
		float b = randFloat();
		demoLines[i].gpuline = gpu.addLine(x1, y1, x2, y2, r, g, b);
		demoLines[i].dx = (randFloat() - 0.5f) * 0.5f;
		demoLines[i].dy = (randFloat() - 0.5f) * 0.5f;
	}
}

void moveDemoPrims(float delta) {

	if (input.isPressed(1)) demoSprite->rotate(2.5f * delta);
	if (input.isPressed(2)) demoSprite->rotate(-2.5f * delta);
	if (input.isPressed(3)) {
		demoShip.xd += std::cos(demoSprite->rot + 1.570796) * delta * 1.0f;
		demoShip.yd += std::sin(demoSprite->rot + 1.570796) * delta * 1.0f;
	}
	demoShip.x += demoShip.xd * delta;
	demoShip.y += demoShip.yd * delta;
	demoSprite->moveTo(demoShip.x, demoShip.y);

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
	apu.voices[1].Trigger();
}

void handleButton(int input) {
	if (input == 0) glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (input == 4) fireDemoShot();
	if (input == 11) gpu.toggleLayer(0);
	if (input == 12) gpu.toggleLayer(1);
	if (input == 13) gpu.toggleLayer(2);
	if (input == 14) apu.voices[0].testTone = !apu.voices[0].testTone;
}
void handleSwitch(int input, bool isDown) { }



int main() {

	float currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();

	// Setup window
	if (!makeWindow(windowWidth, windowHeight)) {
		glfwTerminate();
		return 0;
	}

	// Setup output
	gpu = GPU(windowWidth, windowHeight, window);
	gpu.Setup();
	apu = APU();
	apu.Setup(handleAudio);

	// Setup input
	input = Input();
	input.Add(0, VEXInputType::BUTTON, GLFW_KEY_ESCAPE);
	input.Add(1, VEXInputType::SWITCH, GLFW_KEY_A);
	input.Add(2, VEXInputType::SWITCH, GLFW_KEY_D);
	input.Add(3, VEXInputType::SWITCH, GLFW_KEY_W);
	input.Add(4, VEXInputType::BUTTON, GLFW_KEY_SPACE);
	input.Add(11, VEXInputType::BUTTON, GLFW_KEY_1);
	input.Add(12, VEXInputType::BUTTON, GLFW_KEY_2);
	input.Add(13, VEXInputType::BUTTON, GLFW_KEY_3);
	input.Add(14, VEXInputType::BUTTON, GLFW_KEY_4);
	input.Setup(window, handleKey, handleButton, handleSwitch);


	// Make demo shit
	makeDemoPrims();
	gpu.addSprite(demoSprite);

	apu.voices[0].pan = 0.2;
	apu.voices[0].volume = 1.0;

	apu.voices[1].pan = 0.5;          // demo shooty sound
	apu.voices[1].envMain->a = 0.03;
	apu.voices[1].envMain->d = 0.3;
	apu.voices[1].envMain->s = 0.0;
	apu.voices[1].envMain->r = 0.0;


	// MAIN LOOP

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		moveDemoPrims(deltaTime);

		gpu.Assemble();
		gpu.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	apu.Stop();
	glfwTerminate();
	return 0;
}
