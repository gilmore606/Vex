#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"
#include "Buffer.h"
#include "GPU.h"

GLFWwindow* window;
GPU gpu;
bool shouldQuit = false;


void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	shouldQuit = true;
}

int main() {

	int w = 2000;
	int h = 1200;

	// Setup window

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(w, h, "VEX", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) { glViewport(0, 0, w, h); });
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}
	std::cout << "GLFW window initialized" << std::endl;

	// Setup devices

	gpu = GPU(w, h, window);
	gpu.Setup();

	glfwSetKeyCallback(window, handleKey);


	// MAIN LOOP

	while (!shouldQuit) {
		gpu.Assemble();
		gpu.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

