#pragma once

#include <glad/glad.h>
#include <GLFW\glfw3.h>

enum VEXInputType { BUTTON, SWITCH };

typedef struct {
	int id;
	VEXInputType type;
	GLuint key;
	bool isPressed;
} VEXControl;

class Input
{
public:
	Input();

	void Setup(GLFWwindow* window, 
		void (*proxyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods),
		void buttonCallback (int), 
		void switchCallback (int, bool));
	void Add(int id, VEXInputType type, GLuint defaultKey);
	void HandleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	bool isPressed(int id);
	bool wasPressed(int id);

	bool isShift, isAlt, isCtrl;

private:
	VEXControl* controls;
	bool* status;

	void (*buttonCallback)(int);
	void (*switchCallback)(int, bool);
};
