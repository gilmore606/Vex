#include "GPU.h"
#include "Framebuffer.h"
#include <iostream>

static float randFloat() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

GPU::GPU() : w(0), h(0), window(0) { }

GPU::GPU(int w, int h, GLFWwindow* window) {
	this->w = w;
	this->h = h;
	this->window = window;

	lineCount = 5000;
	for (int i = 0; i < lineCount; i++) {
		float x1 = (randFloat() - 0.5f) * 2.0f;
		float y1 = (randFloat() - 0.5f) * 2.0f;
		float x2 = x1 + (randFloat() - 0.5f) * 0.1f;
		float y2 = y1 + (randFloat() - 0.5f) * 0.1f;
		lines[i] = Line(x1, y1, x2, y2);
	}

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
	fadeShader = Shader("./data/shaders/fade.vert", "./data/shaders/fade.frag");
	fadeShader.Load();
	fadeShader.setInt("inTexture", 0);

	screenBuffer = Framebuffer(w, h);
	screenBuffer.Create();
	trailBuffer = Framebuffer(w, h);
	trailBuffer.Create();
	trailBuffer.Clear(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glDepthMask(false);
	glLineWidth(0.8f);
	glDisable(GL_DEPTH_TEST);

	std::cout << "GPU initialized" << std::endl;
}

void GPU::Assemble() {
	linedataCount = 0;
	for (int i = 0; i < lineCount; i++) {
		lines[i].move();
		lines[i].pushData(linedata, &linedataCount);
	}
}

void GPU::Render() {
	
	// Buffer linedata into its VBO
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linedata), linedata, GL_STREAM_DRAW);

	// Draw lines to trailbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, trailBuffer.ID());
	drawShader.Use("brightness", 0.4f);
	glBindVertexArray(linesVAO);
	glDrawArrays(GL_LINES, 0, linedataCount);

	// Fade trailbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, trailBuffer.ID());
	glBindTexture(GL_TEXTURE_2D, trailBuffer.texID());
	trailBuffer.Blit(fadeShader, screenVAO);

	// Blit trailbuffer to screenbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer.ID());
	trailBuffer.Blit(blitShader, screenVAO);

	// Draw lines to screenbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer.ID());
	drawShader.Use("brightness", 1.0f);
	glBindVertexArray(linesVAO);
	glDrawArrays(GL_LINES, 0, linedataCount);

	// Draw screenbuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	screenBuffer.Blit(blitShader, screenVAO);
}
