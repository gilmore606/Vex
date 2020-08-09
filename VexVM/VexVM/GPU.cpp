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
		float r = randFloat();
		float g = randFloat();
		float b = randFloat();
		lines[i] = Line(x1, y1, x2, y2, r, g, b);
	}

	std::cout << "GPU created" << std::endl;
}

void GPU::Setup() {
	glGenVertexArrays(1, &linesVAO);
	glGenBuffers(1, &linesVBO);
	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  // pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);  // color
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screendata), &screendata, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  // pos
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);  // UV
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
	composeShader = Shader("./data/shaders/compose.vert", "./data/shaders/compose.frag");
	composeShader.Load();
	composeShader.setInt("screenTex", 0);
	composeShader.setInt("glowTex", 1);

	screenBuffer = Framebuffer(w, h);
	screenBuffer.Create();
	trailBuffer = Framebuffer(w, h);
	trailBuffer.Create();
	glowBuffer = Framebuffer(w, h);
	glowBuffer.Create();

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

	// Draw lines to glowbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, glowBuffer.ID());
	glowBuffer.Clear(0.0f, 0.0f, 0.0f, 0.0f);
	drawShader.Use("brightness", 0.7f);
	glBindVertexArray(linesVAO);
	//glDrawArrays(GL_LINES, 0, linedataCount);

	// Fade trailbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, trailBuffer.ID());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trailBuffer.texID());
	trailBuffer.Blit(fadeShader, screenVAO);

	// Blit trailbuffer to screenbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer.ID());
	screenBuffer.Clear(0.0f, 0.0f, 0.0f, 1.0f);
	trailBuffer.Blit(blitShader, screenVAO);

	// Draw lines to screenbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer.ID());
	drawShader.Use("brightness", 1.0f);
	glBindVertexArray(linesVAO);
	glDrawArrays(GL_LINES, 0, linedataCount);

	// Compose to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(screenVAO);
	composeShader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenBuffer.texID());
	composeShader.setInt("screenTex", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glowBuffer.texID());
	composeShader.setInt("glowTex", 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
