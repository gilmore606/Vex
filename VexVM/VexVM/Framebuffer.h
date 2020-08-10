#pragma once

#include <glad/glad.h>
#include "Shader.h"
#include "Vertbuffer.h"

struct Framebuffer {
	Framebuffer();
	Framebuffer(int w, int h);

	void Create();
	void Clear(float r, float g, float b, float a);
	void Blit(Shader shader, Vertbuffer vb);
	void Target();
	void BindAsTexture(GLuint texunit, Shader shader, const std::string& attribName, int attribId);

private:
	GLuint id;
	GLuint texid;
	int w;
	int h;
};

