#include "Input.h"



Input::Input() { }

void Input::Setup(GLFWwindow* window, void buttonCallback (int), void switchCallback (int, bool)) {
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
				controls[i].isPressed = true;
				if (controls[i].type == VEXInputType::BUTTON) {
					buttonCallback(controls[i].id);
				}
			}
			if (action == GLFW_RELEASE) controls[i].isPressed = false;
			if (controls[i].type == VEXInputType::SWITCH) {
				switchCallback(controls[i].id, controls[i].isPressed);
			}
		}
	}
}

bool Input::isPressed(int id) {
	return controls[id].isPressed;
}
