#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"
#include "Buffer.h"

float vertices[40000];

GLFWwindow* window;
unsigned int VBO, VAO;
bool shouldQuit = false;

Shader drawShader;

void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	shouldQuit = true;
}


void AppWindowCreate(int w, int h, const char* title) {
	if (window) {
		return;
	}
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) { glViewport(0, 0, w, h); });
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	drawShader = Shader("./data/shaders/draw.vert", "./data/shaders/draw.frag");
	drawShader.Load();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}


int main() {
	AppWindowCreate(2400, 1300, "hello asteroids");
	Buffer glowBuffer = Buffer(2400, 1300);
	glfwSetKeyCallback(window, handleKey);

	int pointCount = 40000;

	for (int i = 0; i < pointCount / 4; i++) {
		vertices[i * 4] = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
		vertices[i * 4 + 1] = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
		vertices[i * 4 + 2] = vertices[i * 4] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.1f;
		vertices[i * 4 + 3] = vertices[i * 4 + 1] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.1f;
	}

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glDepthMask(false);
	glLineWidth(0.8f);

	while (!shouldQuit) {
		for (int i = 0; i < pointCount; i++) {
			vertices[i] = vertices[i] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.001f;
		}

		// send VBO to GPU
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(drawShader.ID());
		glBindVertexArray(VAO);
		glBindFramebuffer(GL_FRAMEBUFFER, glowBuffer.ID());
		glDrawArrays(GL_LINES, 0, pointCount);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_LINES, 0, pointCount);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

