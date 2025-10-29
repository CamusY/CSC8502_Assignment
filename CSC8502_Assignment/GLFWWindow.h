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
	virtual bool isKeyPressed(int key) const override;
	virtual bool isKeyHeld(int key) const override;
	virtual bool isKeyReleased(int key) const override;
	virtual bool isMousePressed(int button) const override;

	virtual MousePos getMousePosition() const override;

private:
	struct GLFWwindow* window;
};