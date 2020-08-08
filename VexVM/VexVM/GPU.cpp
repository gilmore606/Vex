#include "GPU.h"
#include "Framebuffer.h"
#include <iostream>

GPU::GPU() : w(0), h(0), window(0) { }

GPU::GPU(int w, int h, GLFWwindow* window) {
	this->w = w;
	this->h = h;
	this->window = window;

	int pointCount = 20000;

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
	glGenVertexArrays(1, &linesVAO);
	glGenBuffers(1, &linesVBO);
	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screendata), &screendata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	drawShader = Shader("./data/shaders/draw.vert", "./data/shaders/draw.frag");
	drawShader.Load();
	blitShader = Shader("./data/shaders/blit.vert", "./data/shaders/blit.frag");
	blitShader.Load();
	blitShader.setInt("screenTexture", 0);

	screenBuffer = Framebuffer(w, h);
	screenBuffer.Create();

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glDepthMask(false);
	glLineWidth(0.8f);
	glDisable(GL_DEPTH_TEST);

	std::cout << "GPU initialized" << std::endl;
}

void GPU::Assemble() {
	// TODO: Update all sprite data and copy lines into linedata for rendering
	for (int i = 0; i < linedataCount; i++) {
		linedata[i] = linedata[i] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.001f;
	}
}

void GPU::Render() {

	// Draw lines to screenbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer.ID());
	glViewport(0, 0, w, h);
	glClearColor(0.1f, 0.1f, 0.3f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT);

	drawShader.Use();
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linedata), linedata, GL_STREAM_DRAW);
	glBindVertexArray(linesVAO);
	glDrawArrays(GL_LINES, 0, linedataCount);

	// Draw screenbuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	blitShader.Use();
	glBindVertexArray(screenVAO);
	glBindTexture(GL_TEXTURE_2D, screenBuffer.texID());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
