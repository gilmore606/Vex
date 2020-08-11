#pragma once

#include <glad/glad.h>
#include <string>
#include "Vertbuffer.h"

struct Shader {
	Shader();
	Shader(const char* vertex_path, const char* fragment_path);

	void Load();
	void Use();
	void Use(const std::string& name, float paramValue);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void Blur(Vertbuffer vb, float resw, float resh, float radius);

private:
	GLuint id;
	const char* vertex_path;
	const char* fragment_path;
};
