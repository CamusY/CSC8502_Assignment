#include "GLFWWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


GLFWWindow::GLFWWindow() : window(nullptr){}

GLFWWindow::~GLFWWindow() {
	if (window) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
	std::cout << "GLFWWindow terminated" << std::endl;
}

bool GLFWWindow::createWindow(int width, int height, const char* title) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return false;
	}
	std::cout << "GLFW initialized." << std::endl;
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

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

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

bool GLFWWindow::isKeyHeld(int key) const {
	// "Held" (持续按住) 可以直接从 GLFW 查询。
	// 验收标准：实现 isKeyPressed (我们用 isKeyHeld 替代)
	return glfwGetKey(window, key) == GLFW_PRESS;
}


bool GLFWWindow::isKeyPressed(int key) const {
	// TODO: 这是 Day 2 的一个简化实现。
	// "Pressed" (按下瞬间) 需要状态跟踪 (比较前一帧和当前帧)。
	// 在我们实现状态跟踪之前，这个函数将不起作用。
	return false;
}

bool GLFWWindow::isKeyHeld(int key) const {
	// "Held" (持续按住) 可以直接从 GLFW 查询。
	// 验收标准：实现 isKeyPressed (我们用 isKeyHeld 替代)
	return glfwGetKey(window, key) == GLFW_PRESS;
}

bool GLFWWindow::isKeyReleased(int key) const {
	// TODO: "Released" (释放瞬间) 也需要状态跟踪。
	return false;
}

bool GLFWWindow::isMousePressed(int button) const {
	// "Pressed" (持续按住，GLFW 的术语)
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

MousePos GLFWWindow::getMousePosition() const {
	double xpos, ypos; // glfw 给我们 double
	glfwGetCursorPos(window, &xpos, &ypos);

	// 我们负责将 glfw 的 double 转换为接口承诺的 MousePos (float)
	return { static_cast<float>(xpos), static_cast<float>(ypos) };
}
