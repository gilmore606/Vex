#include "GPU.h"
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Framebuffer.h"
#include "util.cpp"


GPU::GPU() : w(0), h(0), window(0) { }

GPU::GPU(int w, int h, GLFWwindow* window) {
	this->w = w;
	this->h = h;
	this->window = window;
	std::cout << "GPU created" << std::endl;
}

void GPU::Resize(int w, int h) {
	this->w = w;
	this->h = h;
	glViewport(0, 0, w, h);
	makeBuffers();
}

Point* GPU::addPoint(float x, float y, float r, float g, float b, float size) {
	points[pointc] = Point(x, y, r, g, b, size);
	pointc++;
	return &points[pointc - 1];
}

Line* GPU::addLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
	lines[linec] = Line(x1, y1, x2, y2, r, g, b);
	linec++;
	return &lines[linec - 1];
}

// Remove all prims from draw lists
void GPU::Reset() {
	pointc = 0;
	linec = 0;
	spritec = 0;
}

void GPU::makeShaders() {
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
}

void GPU::makeBuffers() {
	std::cout << "Allocating framebuffers..." << std::endl;
	screenBuffer = Framebuffer(w, h);
	screenBuffer.Create();
	trailBuffer = Framebuffer(w, h);
	trailBuffer.Create();
	glowBuffer = Framebuffer(w, h);
	glowBuffer.Create();
	glowDestBuffer = Framebuffer(w, h);
	glowDestBuffer.Create();
}

void GPU::makeVBs() {
	std::cout << "Allocating vertex buffers..." << std::endl;
	pointsVB = Vertbuffer(GL_POINTS, 24000, GL_STREAM_DRAW);
	pointsVB.addAttrib(2, GL_FLOAT); // pos
	pointsVB.addAttrib(3, GL_FLOAT); // color
	pointsVB.addAttrib(1, GL_FLOAT); // size
	pointsVB.Create();
	linesVB = Vertbuffer(GL_LINES, 128000, GL_STREAM_DRAW);
	linesVB.addAttrib(2, GL_FLOAT); // pos
	linesVB.addAttrib(3, GL_FLOAT); // color
	linesVB.Create();
	screenVB = Vertbuffer(GL_TRIANGLES, 24, GL_STATIC_DRAW);
	screenVB.addAttrib(2, GL_FLOAT); // pos
	screenVB.addAttrib(2, GL_FLOAT); // uv
	screenVB.Create();
	screenVB.bulkLoad(screendata, 24);
}

void GPU::Setup() {
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);

	makeShaders();
	makeBuffers();
	makeVBs();

	Reset();
	std::cout << "GPU initialized" << std::endl;
}

// Copy all our abstract prims into vertex buffers, to prep for render
void GPU::Assemble() {
	pointsVB.Reset();
	linesVB.Reset();
	for (int i = 0; i < pointc; i++) {
		points[i].pushData(pointsVB.vertdata, &pointsVB.vertdatac);  // internals exposed for speed
	}
	for (int i = 0; i < linec; i++) {
		lines[i].pushData(linesVB.vertdata, &linesVB.vertdatac);
	}
	for (int i = 0; i < spritec; i++) {
		sprites[i].pushData(linesVB.vertdata, &linesVB.vertdatac);
	}
	pointsVB.Update();
	linesVB.Update();
}

void GPU::DrawPrims(float lineThickness, float pointBright, float lineBright) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glLineWidth(lineThickness);

	pointShader.Use("brightness", pointBright);
	pointShader.setFloat("time", glfwGetTime());
	pointShader.setFloat("spread", 0.001f);
	pointShader.setFloat("stability", 0.990f);
	pointsVB.Draw();

	lineShader.Use("brightness", lineBright);
	lineShader.setFloat("time", glfwGetTime());
	lineShader.setFloat("spread", 0.002f);
	lineShader.setFloat("stability", 0.997f);
	linesVB.Draw();

	glDisable(GL_BLEND);
}

void GPU::Render() {
	// Draw to trailbuffer
	trailBuffer.Target();
	DrawPrims(3.0f, 1.0f, 0.3f);

	// Blur trailbuffer
	blurShader.Use();
	trailBuffer.BindAsTexture(GL_TEXTURE0, blurShader, "texture", 0);
	blurShader.Blur(screenVB, w, h, 0.5f);

	// Draw to glowbuffer
	glowBuffer.Target();
	glowBuffer.Clear(0.0f, 0.0f, 0.0f, 0.0f);
	DrawPrims(2.4f, 1.0f, 0.7f);

	// Blur glowbuffer
	glowDestBuffer.Target();
	blurShader.Use();
	glowBuffer.BindAsTexture(GL_TEXTURE0, blurShader, "texture", 0);
	blurShader.Blur(screenVB, w, h, 1.0f);
	blurShader.Blur(screenVB, w, h, 2.0f);

	// Fade trailbuffer
	trailBuffer.Target();
	trailBuffer.BindAsTexture(GL_TEXTURE0, fadeShader, "texture", 0);
	trailBuffer.Blit(fadeShader, screenVB);

	// Blit trailbuffer to screenbuffer
	screenBuffer.Target();
	screenBuffer.Clear(0.0f, 0.0f, 0.0f, 1.0f);
	trailBuffer.Blit(blitShader, screenVB);

	// Draw to screenbuffer
	screenBuffer.Target();
	DrawPrims(0.8f, 1.5f, 1.5f);

	// Compose to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	composeShader.Use();
	screenBuffer.BindAsTexture(GL_TEXTURE0, composeShader, "screenTex", 0);
	glowDestBuffer.BindAsTexture(GL_TEXTURE1, composeShader, "glowTex", 1);
	screenVB.Draw();
}
