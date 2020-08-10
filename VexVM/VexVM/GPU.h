#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "Shader.h"
#include "Framebuffer.h"
#include "Point.h"
#include "Line.h"
#include "Sprite.h"

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

private:
	void makeShaders();
	void makeBuffers();
	void DrawPrims(float pointBright, float lineBright);
	void BindTex(GLuint texid, int texunit);
	float pointdata[20000];
	int pointdatac = 0;
	float linedata[120000];
	int linedatac = 0;

	Point* points = new Point[2048];
	int pointc = 0;
	Line* lines = new Line[8192];
	int linec = 0;
	Sprite* sprites = new Sprite[2048];
	int spritec = 0;

	int w, h;
	GLFWwindow* window;
	GLuint pointsVBO, pointsVAO;
	GLuint linesVBO, linesVAO;
	GLuint screenVBO, screenVAO;
	Shader pointShader, lineShader, blitShader, fadeShader, composeShader, blurShader;
	Framebuffer trailBuffer, screenBuffer, glowBuffer, glowDestBuffer;

	float screendata[24] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

};
