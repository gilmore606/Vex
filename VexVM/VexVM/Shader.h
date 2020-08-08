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
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void SendLine(GLuint vao, float x0, float y0, float x1, float y1);
private:
	GLuint id;
	const char* vertex_path;
	const char* fragment_path;
};