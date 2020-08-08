#pragma once

#include <glad/glad.h>

struct Buffer {
	Buffer();
	Buffer(int w, int h);

	GLuint ID();

	void Create();
private:
	GLuint id;
	GLuint texid;
	int w;
	int h;
};

