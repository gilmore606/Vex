#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "Framebuffer.h"
#include <iostream>
#include "Shader.h"

Framebuffer::Framebuffer() : w(0), h(0), id(0) { }

Framebuffer::Framebuffer(int w, int h) : id(0) {
	this->w = w;
	this->h = h;
}

GLuint Framebuffer::ID() {
	return id;
}

GLuint Framebuffer::texID() {
	return texid;
}

void Framebuffer::Create() {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texid, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer created" << std::endl;
	}
	else {
		std::cout << "Framebuffer create failed!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::Blit(Shader shader, GLuint vao) {
	shader.Use();
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texid);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}