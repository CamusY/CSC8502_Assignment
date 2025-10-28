#include "GLFWWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


GLFWWindow::GLFWWindow() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
	}
	window = nullptr;
}

GLFWWindow::~GLFWWindow() {
	if (window) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

bool GLFWWindow::createWindow(int width, int height, const char* title) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		return false;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}
bool GLFWWindow::shouldCloseWindow() {
	return glfwWindowShouldClose(window);
}
void GLFWWindow::swapBuffers() {
	glfwSwapBuffers(window);
}
void GLFWWindow::pollEvents() {
	glfwPollEvents();
}
bool GLFWWindow::isKeyPressed(int keyCode) {
	return glfwGetKey(window, keyCode) == GLFW_PRESS;
}
bool GLFWWindow::isMouseButtonPressed(int button) {
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}
void GLFWWindow::getMousePos(double& x, double& y) {
	glfwGetCursorPos(window, &x, &y);
}
