#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "Shader.h"
#include "Framebuffer.h"
#include "Vertbuffer.h"
#include "Point.h"
#include "Line.h"
#include "Sprite.h"

struct GPUsettings {
	float MAX_POINTS = 16000;
	float MAX_LINES = 64000;
	float MAX_SPRITES = 4000;

	float LINE_WIDTH = 0.8f;
	float GLOW_WIDTH = 6.0f;
	float BEAM_SUSTAIN = 0.92f;
	float BEAM_DROP = 0.6f;
	float POINT_SPREAD = 0.0015f;
	float LINE_SPREAD = 0.0025f;
	float POINT_STABILITY = 0.990f;
	float LINE_STABILITY = 0.996f;
	float POINT_BRIGHTNESS = 1.5f;
	float LINE_BRIGHTNESS = 1.5f;
	float POINT_GLOW_BRIGHTNESS = 1.0f;
	float LINE_GLOW_BRIGHTNESS = 0.8f;
	float TRAIL_DECAY = 0.89f;
	float TRAIL_BLUR = 0.4f;
};

struct GPU{
	GPU();
	GPU(int w, int h, GLFWwindow* window);

	void Setup();
	void Resize(int w, int h);
	void Reset();
	void Assemble();
	void Render();

	Point* addPoint(float x, float y, float r, float g, float b, float size);
	Line* addLine(float x1, float y1, float x2, float y2, float r, float g, float b);

	GPUsettings settings;

private:
	void makeShaders();
	void makeBuffers();
	void makeVBs();
	void DrawPrims(float lineThickness, float pointBright, float lineBright);

	Point* points = new Point[settings.MAX_POINTS];
	int pointc = 0;
	Line* lines = new Line[settings.MAX_LINES];
	int linec = 0;
	Sprite* sprites = new Sprite[settings.MAX_SPRITES];
	int spritec = 0;

	int w, h;
	GLFWwindow* window;
	Shader pointShader, lineShader, blitShader, fadeShader, composeShader, blurShader;
	Framebuffer trailBuffer, screenBuffer, glowBuffer;
	Vertbuffer pointsVB, linesVB, screenVB;

	float screendata[24] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
};
