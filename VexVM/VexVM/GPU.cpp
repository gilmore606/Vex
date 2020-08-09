#include "GPU.h"
#include <glad/glad.h>
#include <GLFW\glfw3.h>
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
	Reset();
	std::cout << "GPU created" << std::endl;
}

void GPU::Reset() {
	pointc = 800;
	for (int i = 0; i < pointc; i++) {
		float x = (randFloat() - 0.5f) * 2.0f;
		float y = (randFloat() - 0.5f) * 2.0f;
		float r = randFloat();
		float g = randFloat();
		float b = randFloat();
		float size = randFloat() * 3.5f + 1.0f;
		points[i] = Point(x, y, r, g, b, size);
	}
	linec = 50;
	for (int i = 0; i < linec; i++) {
		float x1 = (randFloat() - 0.5f) * 2.0f;
		float y1 = (randFloat() - 0.5f) * 2.0f;
		float x2 = x1 + (randFloat() - 0.5f) * 0.3f;
		float y2 = y1 + (randFloat() - 0.5f) * 0.3f;
		float r = randFloat();
		float g = randFloat();
		float b = randFloat();
		lines[i] = Line(x1, y1, x2, y2, r, g, b);
	}
}

void GPU::Setup() {
	glGenVertexArrays(1, &pointsVAO);
	glGenBuffers(1, &pointsVBO);
	glBindVertexArray(pointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);  // pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);  // color
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);  // size
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);

	std::cout << "Compiling shaders..." << std::endl;

	pointShader = Shader("./data/shaders/drawpoint.vert", "./data/shaders/drawpoint.frag");
	pointShader.Load();
	lineShader = Shader("./data/shaders/drawline.vert", "./data/shaders/drawline.frag");
	lineShader.Load();
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
	blurShader = Shader("./data/shaders/blur.vert", "./data/shaders/blur.frag");
	blurShader.Load();
	blurShader.setInt("texture", 0);

	screenBuffer = Framebuffer(w, h);
	screenBuffer.Create();
	trailBuffer = Framebuffer(w, h);
	trailBuffer.Create();
	glowBuffer = Framebuffer(w, h);
	glowBuffer.Create();
	glowDestBuffer = Framebuffer(w, h);
	glowDestBuffer.Create();

	std::cout << "GPU initialized" << std::endl;
}

void GPU::Assemble(bool inMotion) {
	pointdatac = 0;
	for (int i = 0; i < pointc; i++) {
		if (inMotion) points[i].move();
		points[i].pushData(pointdata, &pointdatac);
	}
	linedatac = 0;
	for (int i = 0; i < linec; i++) {
		if (inMotion) lines[i].move();
		lines[i].pushData(linedata, &linedatac);
	}
	for (int i = 0; i < spritec; i++) {
		sprites[i].pushData(linedata, &linedatac);
	}
}

void GPU::DrawPrims(float pointBright, float lineBright) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	pointShader.Use("brightness", pointBright);
	glBindVertexArray(pointsVAO);
	glDrawArrays(GL_POINTS, 0, pointdatac);

	lineShader.Use("brightness", lineBright);
	glBindVertexArray(linesVAO);
	glDrawArrays(GL_LINES, 0, linedatac);

	glDisable(GL_BLEND);
}

void GPU::Render() {
	
	// Buffer prim data into their VBOs
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointdata), pointdata, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linedata), linedata, GL_STREAM_DRAW);

	// Draw to trailbuffer
	glLineWidth(3.0f);
	trailBuffer.DrawTo();
	DrawPrims(1.0f, 0.3f);

	// Blur trailbuffer
	blurShader.Use();
	trailBuffer.BindTexture(GL_TEXTURE0);
	blurShader.setInt("texture", 0);
	blurShader.Blur(screenVAO, w, 0.5f, 1.0f, 0.0f);
	blurShader.Blur(screenVAO, h, 0.5f, 0.0f, 1.0f);

	// Draw to glowbuffer
	glLineWidth(2.4f);
	glowBuffer.DrawTo();
	glowBuffer.Clear(0.0f, 0.0f, 0.0f, 0.0f);
	DrawPrims(1.0f, 0.7f);

	// Blur glowbuffer
	glowDestBuffer.DrawTo();
	blurShader.Use();
	glowBuffer.BindTexture(GL_TEXTURE0);
	blurShader.setInt("texture", 0);
	blurShader.Blur(screenVAO, w, 1.0f, 1.0f, 0.0f);
	blurShader.Blur(screenVAO, h, 1.0f, 0.0f, 1.0f);
	blurShader.Blur(screenVAO, w, 2.0f, 1.0f, 0.0f);
	blurShader.Blur(screenVAO, h, 2.0f, 0.0f, 1.0f);

	// Fade trailbuffer
	trailBuffer.DrawTo();
	trailBuffer.BindTexture(GL_TEXTURE0);
	trailBuffer.Blit(fadeShader, screenVAO);

	// Blit trailbuffer to screenbuffer
	screenBuffer.DrawTo();
	screenBuffer.Clear(0.0f, 0.0f, 0.0f, 1.0f);
	trailBuffer.Blit(blitShader, screenVAO);

	// Draw to screenbuffer
	glLineWidth(0.8f);
	screenBuffer.DrawTo();
	DrawPrims(1.5f, 1.5f);

	// Compose to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(screenVAO);
	composeShader.Use();
	screenBuffer.BindTexture(GL_TEXTURE0);
	composeShader.setInt("screenTex", 0);
	glowDestBuffer.BindTexture(GL_TEXTURE1);
	composeShader.setInt("glowTex", 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
