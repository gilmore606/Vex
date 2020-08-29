#include "Input.h"
#include <iostream>

Input::Input() { 
	controls = new VEXControl[64];
	status = new bool[64];
	isAlt = false;
	isCtrl = false;
	isShift = false;
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
	if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
		if (action == GLFW_PRESS) {
			isAlt = true;
		}
		else if (action == GLFW_RELEASE) {
			isAlt = false;
		}
	}
	if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
		if (action == GLFW_PRESS) {
			isCtrl = true;
		}
		else if (action == GLFW_RELEASE) {
			isCtrl = false;
		}
	}
	if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
		if (action == GLFW_PRESS) {
			isShift = true;
		}
		else if (action == GLFW_RELEASE) {
			isShift = false;
		}
	}
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && isAlt) {
		buttonCallback(999);
		return;
	}
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
