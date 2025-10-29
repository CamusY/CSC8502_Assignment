#include <iostream>
#include <memory>
#include "GLFWWindow.h"
#include "IWindow.h"
#include "IInputHandler.h"

int main() {
	std::unique_ptr<IWindow> window = std::make_unique<GLFWWindow>();
	IInputHandler* inputHandler = dynamic_cast<IInputHandler*>(window.get());
	if (!window->createWindow(800, 600, "CSC8502 Assignment")) {
		std::cerr << "Failed to create window!" << std::endl;
		return -1;
	}
	while (!window->shouldCloseWindow()) {
		window->pollEvents();
		window->swapBuffers();
	}
}