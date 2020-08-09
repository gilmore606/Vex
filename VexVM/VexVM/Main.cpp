#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"
#include "GPU.h"

GLFWwindow* window;
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
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) { glViewport(0, 0, w, h); });
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	std::cout << "GLFW window initialized" << std::endl;
	return true;
}

int main() {

	int w = 2000;
	int h = 1200;

	int currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();

	// Setup window
	if (!makeWindow(w, h)) {
		glfwTerminate();
		return 0;
	}

	// Setup devices
	gpu = GPU(w, h, window);
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

