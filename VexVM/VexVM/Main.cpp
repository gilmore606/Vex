#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"
#include "GPU.h"

GLFWwindow* window;
int windowWidth = 2000;
int windowHeight = 1300;
GPU gpu;
bool inMotion = true;


void doReset() {
	gpu.Reset();
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

int main() {

	int currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();

	// Setup window
	if (!makeWindow(windowWidth, windowHeight)) {
		glfwTerminate();
		return 0;
	}

	// Setup devices
	gpu = GPU(windowWidth, windowHeight, window);
	gpu.Setup();

	glfwSetKeyCallback(window, handleKey);


	// MAIN LOOP

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		gpu.Assemble(inMotion); // TODO move this test shit out of GPU!
		gpu.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
