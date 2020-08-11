#include "Vertbuffer.h"

Vertbuffer::Vertbuffer() { }

Vertbuffer::Vertbuffer(GLuint drawtype, int maxprims, GLuint drawmode) { 
	int maxsize = 0;
	if (drawtype == GL_POINTS) maxsize = maxprims * 6;
	if (drawtype == GL_LINES) maxsize = maxprims * 10;
	if (drawtype == GL_TRIANGLES) maxsize = maxprims * 12;
	this->drawtype = drawtype;
	this->maxsize = maxsize;
	this->drawmode = drawmode;
	vertdata = new float[maxsize];
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

void Vertbuffer::bulkLoad(float data[], int count) {
	vertdatac = count;
	for (int i = 0; i < vertdatac; i++) {
		vertdata[i] = data[i];
	}
	Update();
}


