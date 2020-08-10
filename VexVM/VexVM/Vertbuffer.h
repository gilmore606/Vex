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
	Vertbuffer(GLuint drawtype, int maxsize, GLuint drawmode);

	void addAttrib(int size, GLuint type);
	void Create();
	void Reset();
	void bulkLoad(float data[], int count);
	void push(float f);
	void Update();
	void Draw();

	float* vertdata;
	int vertdatac = 0;

private:
	GLuint drawmode;
	int maxsize;
	GLuint vao, vbo;
	Vattrib attributes[8];
	int attribc = 0;
	GLuint drawtype;

};
