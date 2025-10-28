#pragma once
#include "IWindow.h"
#include "IInputHandler.h"
class GLFWWindow : public IWindow, public IInputHandler {
public:
	GLFWWindow();
	virtual ~GLFWWindow();
	bool createWindow(int width, int height, const char* title) override;
	bool shouldCloseWindow() override;
	void swapBuffers() override;
	void pollEvents() override;
	bool isKeyPressed(int keyCode) override;
	bool isMouseButtonPressed(int button) override;
	void getMousePos(double& x, double& y) override;

private:
	struct GLFWwindow* window;
};