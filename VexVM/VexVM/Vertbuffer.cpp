#include "Vertbuffer.h"

Vertbuffer::Vertbuffer() { }

Vertbuffer::Vertbuffer(GLuint drawtype) { 
	this->drawtype = drawtype;
}

void Vertbuffer::addAttrib(int size, GLuint type) {
	attributes[attribc].size = size;
	attributes[attribc].type = type;
	int bytes = 0;
	if (type == GL_FLOAT) bytes = size * sizeof(float);
	if (type == GL_INT) bytes = size * sizeof(int);
	if (type == GL_DOUBLE) bytes = size * sizeof(double);
	attributes[attribc].bytes = bytes;
	attribc++;
}

void Vertbuffer::Create() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	int stride = 0;
	for (int i = 0; i < attribc; i++) {
		stride += attributes[i].bytes;
	}
	int offset = 0;
	for (int i = 0; i < attribc; i++) {
		glVertexAttribPointer(i, attributes[i].size, attributes[i].type, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(i);
		offset += attributes[i].bytes;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Vertbuffer::Draw() {
	glBindVertexArray(vao);
	glDrawArrays(drawtype, 0, vertc);
}

void Vertbuffer::loadVertices(float data[], int count, GLuint mode) {
	vertc = count;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, mode);
}
