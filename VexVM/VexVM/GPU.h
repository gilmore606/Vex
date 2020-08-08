#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "Shader.h"
#include "Framebuffer.h"

struct GPU{
	GPU();
	GPU(int w, int h, GLFWwindow* window);

	void Setup();
	void Assemble();
	void Render();

private:
	int w, h;
	GLFWwindow* window;
	GLuint linesVBO, linesVAO;
	GLuint screenVBO, screenVAO;
	Shader drawShader, blitShader, postShader;
	Framebuffer screenBuffer;

	float linedata[40000];
	int linedataCount;
	float screendata[24] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
};

