#include "GPU.h"
#include <iostream>

GPU::GPU() : w(0), h(0), window(0) { }

GPU::GPU(int w, int h, GLFWwindow* window) {
	this->w = w;
	this->h = h;
	this->window = window;

	int pointCount = 12000;

	for (int i = 0; i < pointCount / 4; i++) {
		linedata[i * 4] = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
		linedata[i * 4 + 1] = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
		linedata[i * 4 + 2] = linedata[i * 4] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.1f;
		linedata[i * 4 + 3] = linedata[i * 4 + 1] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.1f;
	}

	linedataCount = pointCount;

	std::cout << "GPU created" << std::endl;
}

void GPU::Setup() {
	drawShader = Shader("./data/shaders/draw.vert", "./data/shaders/draw.frag");
	drawShader.Load();

	glGenVertexArrays(1, &linesVAO);
	glGenBuffers(1, &linesVBO);
	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Buffer glowBuffer = Buffer(w, h);
	Buffer drawBuffer = Buffer(w, h);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glDepthMask(false);
	glLineWidth(0.8f);

	std::cout << "GPU initialized" << std::endl;
}

// Update all sprite data and copy lines into linedata for rendering
void GPU::Assemble() {
	for (int i = 0; i < linedataCount; i++) {
		linedata[i] = linedata[i] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.001f;
	}
}

void GPU::Render() {
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linedata), linedata, GL_STREAM_DRAW);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(drawShader.ID());
	glBindVertexArray(linesVAO);
	//glBindFramebuffer(GL_FRAMEBUFFER, glowBuffer.ID());
	//glDrawArrays(GL_LINES, 0, linedataCount);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawArrays(GL_LINES, 0, linedataCount);
}
