#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

static std::string ShaderSourceFromFile(const char* path) {
	std::ifstream file(path);
	std::string empty = "";
	if(!file.is_open()) {
		std::cout << "Failed to load shader\n" << std::endl;
		return empty;
	}
	std::ostringstream string_stream;
	string_stream << file.rdbuf();
	return string_stream.str();
}

Shader::Shader() : vertex_path(0), fragment_path(0), id(0) {

}

Shader::Shader(const char* vertex_path, const char* fragment_path) : id(0) {
	this->vertex_path = vertex_path;
	this->fragment_path = fragment_path;
}

Shader::~Shader() {
}

GLuint Shader::ID() {
	return id;
}

void Shader::Load() {
	int success;
	char infoLog[512];

	std::string vertex_src = ShaderSourceFromFile(vertex_path);
	std::string fragment_src = ShaderSourceFromFile(fragment_path);
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
	std::cout << "Shader loaded" << std::endl;
}

void Shader::Use() {
	glUseProgram(id);
}

void Shader::SendLine(GLuint vao, float x0, float y0, float x1, float y1) {
	// Generate a buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);

	// Bind VAO
	glBindVertexArray(vao);

	float verts[4] = { x0, y0, x1, y1 };

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Clean up
	glDeleteBuffers(1, &buffer);
}