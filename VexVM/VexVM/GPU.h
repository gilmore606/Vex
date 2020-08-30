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
#include "ParticleDef.h"
#include "GPUSprite.h"
#include "GPUParticle.h"
#include "Font.h"
#include "Collider.h"

struct GPUsettings {
	float MAX_POINTS = 16000;
	float MAX_LINES = 64000;
	float MAX_SPRITES = 1024;
	float MAX_PARTICLES = 1024;

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

	float LETTERBOX_R = 0.02f;
	float LETTERBOX_G = 0.02f;
	float LETTERBOX_B = 0.02f;

	int COLLIDEMAP_SCALE = 4;
};


class GPU{
public:
	GPU();
	GPU(int w, int h);

	void Setup(void (*onResize)(GLFWwindow* window, int neww, int newh));
	void Resize(int w, int h);
	void Reset();
	void ToggleFullscreen();
	void OnUpdate(float delta);
	void Assemble();
	void Render();
	void Collide(float delta);
	void Stop();
	void loadImage(Image* image);
	void loadParticle(ParticleDef particle);
	void loadFont(Font* font);
	GPUSpriteTicket createSprite(int image);
	GPUSpriteTicket createText(int font, std::string* text, Color color, float charSpacing, float lineSpacing);
	void drawText(GPUSprite* sprite, int font, std::string* text, Color color, float charSpacing = 0.2f, float lineSpacing = 1.1f);
	void destroySprite(int id);
	void spawnParticle(int image, Pos p, Vec v, Vec vv, Color color0, Color color1, 
		Vec scale0, Vec scale1, float rot, float rotv, float lifetime);

	Point* addPoint(float x, float y, float r, float g, float b, float size);
	Line* addLine(float x1, float y1, float x2, float y2, float r, float g, float b);
	
	void toggleLayer(int layer);

	GLFWwindow* window;
	GPUsettings settings;
	bool isFullscreen;
	int savedX, savedY;
	int savedWidth, savedHeight;

private:
	void makeShaders();
	void makeBuffers();
	void makeVBs();
	void drawPrims(float pointThick, float lineThick, float pointBright, float lineBright);
	int reserveSprite();
	inline void collideLine(short sp, short x1, short y1, short x2, short y2);

	Point* points;
	int pointc = 0;
	Line* lines;
	int linec = 0;
	GPUSprite* sprites;
	int spritec = 0;
	GPUParticle* particles;
	int particlec = 0;

	std::vector<Image*> images;
	std::vector<ParticleDef> particleDefs;
	std::vector<Font*> fonts;

	int w, h, screenw, screenh;
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
	float xscale, yscale;

	short** collideMap;
	int colW, colH;
};
