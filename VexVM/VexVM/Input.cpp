#include "Input.h"
#include <iostream>

Input::Input() { 
	controls = new VEXControl[64];
	status = new bool[64];
}

void Input::Setup(GLFWwindow* window, 
	void (*proxyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods), 
	void buttonCallback (int), void switchCallback (int, bool)) {

	for (int i = 0; i < 64; i++) { 
		status[i] = false;
		controls[i].id = 0;
		controls[i].type = BUTTON;
		controls[i].key = GLFW_KEY_UNKNOWN;
	}
	glfwSetKeyCallback(window, proxyCallback);
	this->buttonCallback = buttonCallback;
	this->switchCallback = switchCallback;
}

void Input::Add(int id, VEXInputType type, GLuint defaultKey) {
	if (id > 63) throw "Illegal control ID!";
	controls[id].id = id;
	controls[id].type = type;
	controls[id].key = defaultKey;
}

void Input::HandleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (int i = 0; i < 64; i++) {
		if (controls[i].key == key) {
			if (action == GLFW_PRESS) {
				status[controls[i].id] = true;
				if (controls[i].type == BUTTON) {
					buttonCallback(controls[i].id);
				}
			}
			if (action == GLFW_RELEASE) status[controls[i].id] = false;
			if (controls[i].type == SWITCH) {
				switchCallback(controls[i].id, controls[i].isPressed);
			}
		}
	}
}

bool Input::isPressed(int id) {
	return status[id];
}
