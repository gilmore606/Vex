#pragma once

#include <glad/glad.h>
#include "Shader.h"

struct Framebuffer {
	Framebuffer();
	Framebuffer(int w, int h);

	GLuint ID();
	GLuint texID();

	void Create();
	void Clear(float r, float g, float b, float a);
	void Blit(Shader shader, GLuint vao);
	void Target();
	void BindTexture(int texunit);

private:
	GLuint id;
	GLuint texid;
	int w;
	int h;
};

