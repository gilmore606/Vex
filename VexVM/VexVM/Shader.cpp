#include "Shader.h"
#include "util.cpp"
#include <iostream>
#include <fstream>
#include <sstream>


Shader::Shader() : vertex_path(0), fragment_path(0), id(0) { }

Shader::Shader(const char* vertex_path, const char* fragment_path) : id(0) {
	this->vertex_path = vertex_path;
	this->fragment_path = fragment_path;
}

void Shader::Load() {
	int success;
	char infoLog[512];

	std::string vertex_src = stringFromFile(vertex_path);
	std::string fragment_src = stringFromFile(fragment_path);
	const char* vertex_src_cstr = vertex_src.c_str();
	const char* fragment_src_cstr = fragment_src.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex_src_cstr, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_src_cstr, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	id = glCreateProgram();

	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if(!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Use() {
	glUseProgram(id);
}
void Shader::Use(const std::string& name, float paramValue) {
	glUseProgram(id);
	glUniform1f(glGetUniformLocation(id, name.c_str()), paramValue);
}

void Shader::SetUniform(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetUniform(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::Blur(Vertbuffer vb, float resw, float resh, float radius) {
	glViewport(0, 0, resw, resh);
	SetUniform("radius", radius);
	SetUniform("resolution", resw);
	SetUniform("dirx", 1.0f);
	SetUniform("diry", 0.0f);
	vb.Draw();
	SetUniform("resolution", resh);
	SetUniform("dirx", 0.0f);
	SetUniform("diry", 1.0f);
	vb.Draw();
}
