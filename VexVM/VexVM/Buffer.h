#pragma once

#include <glad/glad.h>

struct Buffer {
	Buffer();
	Buffer(int w, int h);

	GLuint ID();
	GLuint texID();

	void Create();
private:
	GLuint id;
	GLuint texid;
	int w;
	int h;
};

