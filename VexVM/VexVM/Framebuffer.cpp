#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Framebuffer.h"
#include "Shader.h"

Framebuffer::Framebuffer() : w(0), h(0), id(0) { }

Framebuffer::Framebuffer(int w, int h) : id(0) {
	this->w = w;
	this->h = h;
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
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Framebuffer create failed!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::Blit(Shader shader, Vertbuffer vb) {
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texid);
	vb.Draw();
}

void Framebuffer::Target() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::BindAsTexture(GLuint texunit, Shader shader, const std::string& attribName, int attribId) {
	glActiveTexture(texunit);
	glBindTexture(GL_TEXTURE_2D, texid);
	shader.SetUniform(attribName, attribId);
}
