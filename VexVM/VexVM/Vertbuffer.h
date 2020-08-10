#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>

struct Vattrib {
	int size;
	GLuint type;
	int bytes;
};

struct Vertbuffer{
	Vertbuffer();
	Vertbuffer(GLuint drawtype);

	void addAttrib(int size, GLuint type);
	void Create();
	void loadVertices(float data[], int count, GLuint mode);
	void Draw();

	GLuint vao, vbo;
	Vattrib attributes[8];
	int attribc = 0;
	GLuint drawtype;
	int vertc = 0;
};

