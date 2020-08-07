#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>

#include "Shader.h"

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec2 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
	"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(0.8f, 0.9f, 1.0f, 1.0f);\n"
	"}\n\0";
float vertices[] = {
				-0.3f, -0.5f, 0.5f, -0.5f,
					0.5f, -0.5f, 0.0f, 0.5f,
					0.0f,  0.5f, -0.3f, -0.5f
};

GLFWwindow* window;
int shaderProgram;
unsigned int VBO, VAO;
unsigned int glowbuffer, texColorBuffer, rbo;
bool shouldQuit = false;

void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	shouldQuit = true;
}

void GlowbufferCreate(int w, int h) {
	glGenFramebuffers(1, &glowbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, glowbuffer);
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void AppWindowCreate(int w, int h, const char* title) {
	if (window) {
		return;
	}
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// Set up shaders
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

}


int main() {
	AppWindowCreate(2400, 1300, "hello asteroids");
	GlowbufferCreate(2400, 1300);
	glfwSetKeyCallback(window, handleKey);

	int pointCount = 40000;

	float vertices[40000];
	for (int i = 0; i < pointCount / 4; i++) {
		vertices[i * 4] = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
		vertices[i * 4 + 1] = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
		vertices[i * 4 + 2] = vertices[i * 4] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.1f;
		vertices[i * 4 + 3] = vertices[i * 4 + 1] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.1f;
	}

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glDepthMask(false);
	glLineWidth(0.8f);

	while (!shouldQuit) {
		for (int i = 0; i < pointCount; i++) {
			vertices[i] = vertices[i] + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 0.001f;
		}

		// send VBO to GPU
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glBindFramebuffer(GL_FRAMEBUFFER, glowbuffer);
		glDrawArrays(GL_LINES, 0, pointCount);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_LINES, 0, pointCount);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

