#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <vector>
#include "Shader.h"
#include "Framebuffer.h"
#include "Vertbuffer.h"
#include "Point.h"
#include "Line.h"
#include "Image.h"
#include "GPUSprite.h"
#include "Font.h"
#include "Collider.h"

struct GPUsettings {
	float MAX_POINTS = 16000;
	float MAX_LINES = 64000;
	float MAX_SPRITES = 1024;

	double ASPECT_RATIO = 4.0 / 4.0;
	bool DRAW_SCREEN = true;
	bool DRAW_GLOW = true;
	bool DRAW_TRAILS = true;

	float BEAM_SUSTAIN = 0.92f;
	float BEAM_DROP = 0.6f;
	float POINT_SPREAD = 0.0015f;
	float LINE_SPREAD = 0.0025f;
	float POINT_STABILITY = 0.980f;
	float LINE_STABILITY = 0.996f;
	float POINT_BRIGHTNESS = 1.5f;
	float LINE_BRIGHTNESS = 1.5f;
	float POINT_THICKNESS = 1.0f;
	float LINE_THICKNESS = 0.8f;
	float POINT_GLOW_THICKNESS = 2.5f;
	float LINE_GLOW_THICKNESS = 5.0f;
	float POINT_GLOW_BRIGHTNESS = 1.0f;
	float LINE_GLOW_BRIGHTNESS = 0.9f;
	float POINT_TRAIL_BRIGHTNESS = 0.8f;
	float LINE_TRAIL_BRIGHTNESS = 0.3f;
	float TRAIL_DECAY = 0.89f;
	float TRAIL_BLUR = 0.4f;
};


class GPU{
public:
	GPU();
	GPU(int w, int h);

	GLFWwindow* Setup(void (*onResize)(GLFWwindow* window, int neww, int newh));
	void Resize(int w, int h);
	void Reset();
	void Assemble();
	void Render();
	void Collide();
	void Stop();
	void loadImage(Image* image);
	void loadFont(Font* font);
	GPUSpriteTicket createSprite(int image);
	GPUSpriteTicket createText(int font, std::string* text, float r, float g, float b, float charSpacing, float lineSpacing);
	void destroySprite(int id);

	Point* addPoint(float x, float y, float r, float g, float b, float size);
	Line* addLine(float x1, float y1, float x2, float y2, float r, float g, float b);
	
	void toggleLayer(int layer);

	GPUsettings settings;

private:
	void makeShaders();
	void makeBuffers();
	void makeVBs();
	void drawPrims(float pointThick, float lineThick, float pointBright, float lineBright);
	int reserveSprite();

	Point* points;
	int pointc = 0;
	Line* lines;
	int linec = 0;
	GPUSprite* sprites;
	int spritec = 0;

	std::vector<Image*> images;
	std::vector<Font*> fonts;

	int w, h, screenw, screenh;
	GLFWwindow* window;
	Shader pointShader, lineShader, blitShader, fadeShader, composeShader, blurShader;
	Framebuffer trailBuffer, screenBuffer, glowBuffer;
	Vertbuffer pointsVB, linesVB, screenVB, bufferVB;

	float screendata[24] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	float* scaledscreen;

	short** collideMap;
	int colW, colH;
};
