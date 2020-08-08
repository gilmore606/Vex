#pragma once

#include <glad/glad.h>

struct Framebuffer {
	Framebuffer();
	Framebuffer(int w, int h);

	GLuint ID();
	GLuint texID();

	void Create();
	void Clear(float r, float g, float b, float a);

private:
	GLuint id;
	GLuint texid;
	int w;
	int h;
};

