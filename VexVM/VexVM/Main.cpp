#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"
#include "GPU.h"
#include "APU.h"

GLFWwindow* window;
int windowWidth = 1800;
int windowHeight = 1300;
GPU gpu;
APU apu;
bool inMotion = true;

const static int demoPointC = 1600;
struct DemoPoint {
	Point* gpupoint;
	float dx, dy;
};
const static int demoLineC = 2000;
DemoPoint demoPoints[1600];
struct DemoLine {
	Line* gpuline;
	float dx, dy;
};
DemoLine demoLines[2000];

static float randFloat() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void makeDemoPrims();
void doReset() {
	gpu.Reset();
	makeDemoPrims();
}

void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) return;
	if (key == GLFW_KEY_TAB) {
		doReset();
	} else if (key == GLFW_KEY_SPACE) {
		inMotion = !inMotion;
	} else if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
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
	for (int i = 0; i < demoPointC; i++) {
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
	for (int i = 0; i < demoLineC; i++) {
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
	for (int i = 0; i < demoPointC; i++) {
		demoPoints[i].gpupoint->x += demoPoints[i].dx * delta;
		demoPoints[i].gpupoint->y += demoPoints[i].dy * delta;
		demoPoints[i].dy -= 0.2f * delta;
		if (demoPoints[i].gpupoint->y < -0.95f || demoPoints[i].gpupoint->y > 0.95f) demoPoints[i].dy = -demoPoints[i].dy;
		if (demoPoints[i].gpupoint->x < -0.95f || demoPoints[i].gpupoint->x > 0.95f) demoPoints[i].dx = -demoPoints[i].dx;
	}
	for (int i = 0; i < demoLineC; i++) {
		demoLines[i].gpuline->x1 += demoLines[i].dx * delta;
		demoLines[i].gpuline->x2 += demoLines[i].dx * delta;
		demoLines[i].gpuline->y1 += demoLines[i].dy * delta;
		demoLines[i].gpuline->y2 += demoLines[i].dy * delta;
	}
}

int main() {

	float currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();

	// Setup window
	if (!makeWindow(windowWidth, windowHeight)) {
		glfwTerminate();
		return 0;
	}

	// Setup devices
	gpu = GPU(windowWidth, windowHeight, window);
	gpu.Setup();
	apu = APU();
	apu.Setup();

	glfwSetKeyCallback(window, handleKey);

	makeDemoPrims();

	// MAIN LOOP

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (inMotion) moveDemoPrims(deltaTime);

		gpu.Assemble();
		gpu.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
