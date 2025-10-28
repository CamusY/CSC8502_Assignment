#pragma once

class IWindow{
public:
	IWindow() {}
	virtual ~IWindow() {}
	bool virtual createWindow(int width, int height, const char* title) = 0;
	bool virtual shouldCloseWindow() = 0;
	void virtual swapBuffers() = 0;
	void virtual pollEvents() = 0;

};

