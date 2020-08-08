#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "Buffer.h"

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
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
