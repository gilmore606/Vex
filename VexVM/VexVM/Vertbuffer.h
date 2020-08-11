#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>

struct Vattrib {
	int size;
	GLuint type;
	int bytes;
};

class Vertbuffer{
public:
	Vertbuffer();
	Vertbuffer(GLuint drawtype, int maxsize, GLuint drawmode);

	void addAttrib(int size, GLuint type);
	void Create();
	void Reset();
	void bulkLoad(float data[], int count);
	void Update();
	void Draw();

	float* vertdata;
	int vertdatac = 0;
	int maxsize;

private:
	GLuint drawmode;
	GLuint drawtype;
	GLuint vao, vbo;
	Vattrib attributes[8];
	int attribc = 0;
};

inline void Vertbuffer::Draw() {
	glBindVertexArray(vao);
	glDrawArrays(drawtype, 0, vertdatac);
}

inline void Vertbuffer::Reset() {
	vertdatac = 0;
}

// Load the completed buffer into the VBO for render
inline void Vertbuffer::Update() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertdatac * sizeof(float), vertdata, drawmode);
}
