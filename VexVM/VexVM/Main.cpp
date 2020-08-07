#include <GLFW\glfw3.h>
#include <string>

GLFWwindow* window;

void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {

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

	glfwSetKeyCallback(window, handleKey);

	while (true) {
		glfwPollEvents();
	}
}


int main() {
	AppWindowCreate(800, 600, "hello asteroids");
	while (true) {

	}
}
