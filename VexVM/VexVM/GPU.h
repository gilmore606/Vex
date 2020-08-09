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
	void Reset();
	void Assemble(bool inMotion);
	void Render();

private:
	void DrawPrims();
	void BindTex(GLuint texid, int texunit);
	float pointdata[7000];
	int pointdatac;
	float linedata[100000];
	int linedatac;

	Point* points = new Point[1000];
	int pointc = 0;
	Line* lines = new Line[10000];
	int linec = 0;
	Sprite* sprites = new Sprite[1000];
	int spritec = 0;

	int w, h;
	GLFWwindow* window;
	GLuint linesVBO, linesVAO;
	GLuint screenVBO, screenVAO;
	Shader drawShader, blitShader, fadeShader, composeShader, blurShader;
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

