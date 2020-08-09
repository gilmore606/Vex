#pragma once

#include <glad/glad.h>
#include <string>

struct Shader {
	Shader();
	Shader(const char* vertex_path, const char* fragment_path);
	~Shader();

	GLuint ID();

	void Load();
	void Use();
	void Use(const std::string& name, float paramValue);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
private:
	GLuint id;
	const char* vertex_path;
	const char* fragment_path;
};